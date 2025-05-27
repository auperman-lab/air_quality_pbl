#include <Arduino.h>
#include <ESP8266WiFi.h>

#define BLYNK_TEMPLATE_ID "TMPL4c9VVLhf0"
#define BLYNK_TEMPLATE_NAME "WindowOpener"
#define BLYNK_AUTH_TOKEN "lrFUoJxSRn2mZTtUbXzf_c5-q0Oruyom"
// #define BLYNK_AUTH_TOKEN "GRkuD-KMx3VUA3uI6gvOjvU_vlOhLnXa"


#include <BlynkSimpleEsp8266.h>
#include <ArduinoOTA.h>

// ====== CONFIGURATION ======
#define BLYNK_PRINT Serial
#define RX_PIN 3


char auth[] = BLYNK_AUTH_TOKEN;        // Blynk auth token
char ssid[] = "SCArFAF";                                // WiFi SSID
char pass[] = "3224310100300";                         // WiFi password

float TimeOpen = 45.0;
float TimeOpenAlt = 15.0;
int TimeSleep = 10 * 60;
int StartAngle = 10, EndAngle = 170;

// Hall sensor pins
int EndPinO = D6, EndPinC = RX_PIN;
int EndPinAltO = D1, EndPinAltC = D5;

// Motor driver pins
int DrPinO = D5, DrPinC = 10;  // Change 10 to actual GPIO if needed
int DrPinAltO = D7, DrPinAltC = D8;

// ====== GLOBALS ======
BlynkTimer timer;
WidgetLED led1(V12);
WidgetLED led2(V13);

String MyTerm;
float lvl_curent = 0, lvl_curent_time = 0, lvl_sin_cor = 0, lvl_delta = 0;
float bat_lvl = 0, bat_volt = 0;

bool isFirstConnect = true;
bool MyEco = false;
int puls = 0;
int led_val = 0;

#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE
#define DEBUG(x) Serial.println(x)
#else
#define DEBUG(x)
#endif

// ====== FUNCTION PROTOTYPES ======
void MyGoSleep();
void ResetPosition();
bool CloseMainMotor();
bool OpenMainMotor();
bool CloseAltMotor();
bool OpenAltMotor();

void MoveAltMotor(float duration);
void MoveMainMotor(float duration);

float AngleToLvl(float Angle0);
float LvlToAngle(float New_lvl);
float LvlAngleTime(float New_lvl);
float TimeAnglelvl(float New_time);
float TimeTolvl (float New_WorkTime);
float LvlToTime (float New_lvl);

void SendMessage(String my_text, String my_text2);

void MyWork(int lvl);

// ====== SETUP ======
void setup() {
  #ifdef DEBUG_ENABLE
  Serial.begin(115200);
  #endif

  pinMode(DrPinO, OUTPUT);
  pinMode(DrPinC, OUTPUT);
  pinMode(DrPinAltO, OUTPUT);
  pinMode(DrPinAltC, OUTPUT);

  digitalWrite(DrPinO, LOW);
  digitalWrite(DrPinC, LOW);
  digitalWrite(DrPinAltO, LOW);
  digitalWrite(DrPinAltC, LOW);

  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(LED_BUILTIN, 900);

  pinMode(EndPinO, INPUT_PULLUP);
  pinMode(EndPinC, INPUT_PULLUP);
  pinMode(EndPinAltO, INPUT_PULLUP);
  pinMode(EndPinAltC, INPUT_PULLUP);

  Blynk.begin(auth, ssid, pass);
  ArduinoOTA.setHostname("WindowCloser2.0");
  ArduinoOTA.begin();

  #ifdef DEBUG_ENABLE
  timer.setInterval(1000L, []() {
    DEBUG("Uptime: " + String(millis() / 1000));
  });
  #endif
}

// ====== LOOP ======
void loop() {
  Blynk.run();
  ArduinoOTA.handle();
  timer.run();
}

// ====== BLYNK HANDLERS ======
BLYNK_CONNECTED() {
  if (isFirstConnect) {
    Blynk.syncVirtual(V0, V11);
    DEBUG("BLYNK_CONNECTED: Synced virtual pins");
    Blynk.syncVirtual(V2, V1);
    isFirstConnect = false;
  }
}

BLYNK_WRITE(V0) {
  MyEco = !param.asInt();
  DEBUG("Eco mode: " + String(MyEco));
  led_val = MyEco ? 900 : 100;
  analogWrite(LED_BUILTIN, led_val);
  if (MyTerm.endsWith("*\n")) {
    MyGoSleep();
  }
}

BLYNK_WRITE(V1) {
  MyTerm = param.asStr();
  DEBUG("Terminal input: " + MyTerm);
  if (!MyTerm.endsWith("*\n")) {
    MyWork(MyTerm.toInt());
  } else {
    MyGoSleep();
  }
}

BLYNK_WRITE(V2) {
  lvl_curent = param.asFloat();
  DEBUG("Level updated: " + String(lvl_curent));
}

// ======  FUNCTIONS ======
// These must be defined with logic, copied or re-implemented as needed

void MyWork(int lvl) {
  // Описание: от 100 до 10% окно закрывается основным приводом, от 10 до 0% запирается на рукоятку.
  
  Serial.println("Концевки:" + String(digitalRead(EndPinAltC)) + String(digitalRead(EndPinAltO)) + 
                 String(digitalRead(EndPinC)) + String(digitalRead(EndPinO)));

  if (!(MyTerm.endsWith("*\n") || lvl == 0)) {
      float need_lvl = constrain(lvl_curent + lvl, 0, 100);
      Serial.println("MyWork: lvl_curent=" + String(lvl_curent) + "; need_lvl=" + String(need_lvl));
      Blynk.virtualWrite(V1,"MyWork: lvl_curent=" + String(lvl_curent) + "; need_lvl=" + String(need_lvl));
      Blynk.virtualWrite(V1, "...");
      
      if (digitalRead(EndPinO) == 0 && lvl > 0) {
          Blynk.virtualWrite(V1, "Window full Open");
          lvl = 0;
          Blynk.virtualWrite(V2, 100);
          lvl_curent = 100;
          goto bailout;
      }

      if (digitalRead(EndPinAltC) == 0 && lvl < 0) {
          Blynk.virtualWrite(V1, "Window locked");
          lvl = 0;
          Blynk.virtualWrite(V2, 0);
          lvl_curent = 0;
          goto bailout;
      }

      if (lvl_curent <= 100 && lvl_curent >= 10) {
          if (digitalRead(EndPinAltO) != 0) {
              Blynk.virtualWrite(V1, "Err...");
              ResetPosition();
              if (!MyTerm.endsWith("CrErr")) {
                  Blynk.virtualWrite(V1, "...");
                  lvl = need_lvl - 10.0;
              }
          }
      }

      if (lvl_curent < 10 && lvl_curent >= 0) {
          if (digitalRead(EndPinC) != 0) {
              Blynk.virtualWrite(V1, "Err...");
              ResetPosition();
              if (!MyTerm.endsWith("CrErr")) {
                  Blynk.virtualWrite(V1, "...");
                  lvl = need_lvl - 10.0;
              }
          }
      }

      if (lvl > 0) {
          float lvl_delta = lvl - constrain(10.0 - lvl_curent, 0, 10);
          Serial.println("Если Команда на открытие: lvl_delta=" + String(lvl_delta) + 
                         "; lvl_curent=" + String(lvl_curent) + "; lvl=" + String(lvl));
          Blynk.virtualWrite(V1,"Если Команда на открытие: lvl_delta=" + String(lvl_delta) + 
          "; lvl_curent=" + String(lvl_curent) + "; lvl=" + String(lvl));

          if (lvl_curent < 10) {
              if (lvl_delta > 0) {
                  OpenAltMotor();
                  Blynk.virtualWrite(V1, "...");
                  MoveMainMotor(LvlAngleTime(lvl_curent + lvl_delta) - LvlAngleTime(lvl_curent));
              } else {
                  MoveAltMotor(LvlToTime(lvl));
              }
          } else {
            MoveMainMotor(LvlAngleTime(lvl_curent + lvl) - LvlAngleTime(lvl_curent));
          }

      } else {
          float lvl_delta = constrain(10.0 - lvl - lvl_curent, 0, 10);
          Serial.println("Если Команда на закрытие: lvl_delta=" + String(lvl_delta) + 
                         "; lvl_curent=" + String(lvl_curent) + "; lvl=" + String(lvl));
          Blynk.virtualWrite(V1,"Если Команда на закрытие: lvl_delta=" + String(lvl_delta) + 
          "; lvl_curent=" + String(lvl_curent) + "; lvl=" + String(lvl));

          if (lvl_curent <= 10) {
              MoveAltMotor(LvlToTime(lvl));
          } else {
              if (lvl_delta > 0) {
                  CloseMainMotor();
                  Blynk.virtualWrite(V1, "...");
                  MoveAltMotor(LvlToTime(-1 * lvl_delta));
              } else {
                  MoveMainMotor(LvlAngleTime(lvl_curent + lvl) - LvlAngleTime(lvl_curent));
              }
          }
      }

      Blynk.virtualWrite(V1, "*\n");
  }

bailout:
  MyGoSleep();
}


void MyGoSleep() {

  if (MyEco==1) {
    digitalWrite(DrPinO,1); //1.01 - Исправлено самопроизольное закрывание в течении недели...
    delay(70); 
    digitalWrite(DrPinO,0);
    delay(1000); 
    DEBUG("I go to sleep"); 
    ESP.deepSleep(TimeSleep*1e4);
  }
  
}

void ResetPosition() {
  DEBUG("Going to home position...");
  Blynk.virtualWrite(V1, "Reset...");
  OpenAltMotor();
  Blynk.virtualWrite(V1, "...");
  CloseMainMotor();
}

bool CloseMainMotor() {
  if (!MyTerm.endsWith("CrErr")) {
    DEBUG("Closing main motor...");
    MoveMainMotor(-1.2 * TimeOpen);

    if (digitalRead(EndPinC) != LOW) {
      Blynk.virtualWrite(V1, "Check main motor - CrErr");
      MyTerm = "Check main motor - CrErr";
      SendMessage("Window actuator error", "There is a problem with the handle actuator. Check for obstructions and verify that timing is configured correctly.");
    }

    return digitalRead(EndPinC) == LOW;
  }
  return false;
}

bool OpenMainMotor() {
  DEBUG("Opening main motor...");
  MoveMainMotor(1.2 * TimeOpen);

  if (digitalRead(EndPinO) != LOW) {
    Blynk.virtualWrite(V1, "Check main motor - CrErr");
    MyTerm = "Check main motor - CrErr";
  }

  return digitalRead(EndPinO) == LOW;
}

bool CloseAltMotor() {
  DEBUG("Closing alternate motor...");
  Blynk.virtualWrite(V1, "Closing alternate motor...");

  MoveAltMotor(-1.2 * TimeOpenAlt);

  if (digitalRead(EndPinAltC) != LOW) {
    Blynk.virtualWrite(V1, "Check alt motor - CrErr");
    MyTerm = "Check alt motor - CrErr";
  }

  return digitalRead(EndPinAltC) == LOW;
}

bool OpenAltMotor() {
  DEBUG("Opening alternate motor...");
  Blynk.virtualWrite(V1, "Opening alternate motor...");
  MoveAltMotor(1.2 * TimeOpenAlt);

  if (digitalRead(EndPinAltO) != LOW) {
    Blynk.virtualWrite(V1, "Check alt motor - CrErr");
    MyTerm = "Check alt motor - CrErr";
    SendMessage("Window actuator error", "There is a problem with the main actuator. Check for obstructions and verify timing.");
  }

  return digitalRead(EndPinAltO) == LOW;
}

void MoveAltMotor(float duration) {
  if (duration == 0 || MyTerm.endsWith("CrErr")) return;

  DEBUG("Alt Motor command, s: " + String(duration) + "; lvl_current=" + String(lvl_curent));
  Blynk.virtualWrite(V1, "Alt Motor command, s: " + String(duration) + "; lvl_current=" + String(lvl_curent));


  if (abs(duration) > TimeOpenAlt * 1.2)
    duration = copysign(TimeOpenAlt * 1.2, duration);

  puls = led_val;
  lvl_curent_time = LvlToTime(lvl_curent);

  for (int i = 0; i < abs(duration * 10.0); i++) {
    int pulseSpeed = (1023 - led_val) / 10;
    puls = (puls < 1023) ? puls + pulseSpeed : led_val;
    analogWrite(D4, puls);

    if (duration > 0 && digitalRead(EndPinAltO) == LOW) {
      Blynk.virtualWrite(V1, "Window unlocked");
      Blynk.virtualWrite(V2, 10);
      lvl_curent = 10;
      lvl_curent_time = TimeOpenAlt;
      break;
    }

    if (duration < 0 && digitalRead(EndPinAltC) == LOW) {
      Blynk.virtualWrite(V1, "Window locked");
      Blynk.virtualWrite(V2, 0);
      lvl_curent = 0;
      lvl_curent_time = 0;
      break;
    }

    if (duration > 0) {
      digitalWrite(DrPinAltO, HIGH);
      lvl_curent_time += 0.1;
    }

    if (duration < 0) {
      digitalWrite(DrPinAltC, HIGH);
      lvl_curent_time -= 0.1;
    }

    if (i % 5 == 0) {
      lvl_curent = TimeTolvl(lvl_curent_time);
      Blynk.virtualWrite(V2, lvl_curent);
    }

    Blynk.run();
    delay(100);
  }

  lvl_curent = TimeTolvl(lvl_curent_time);
  Blynk.virtualWrite(V2, lvl_curent);

  digitalWrite(DrPinAltO, LOW);
  digitalWrite(DrPinAltC, LOW);
  analogWrite(D4, led_val);
  DEBUG("Alt Motor done. Final level: " + String(lvl_curent));

  if (abs(duration) <= TimeOpenAlt) {
    Blynk.virtualWrite(V1, "OK");
  }

  delay(100);
}

void MoveMainMotor(float duration) {
  if (duration == 0 || MyTerm.endsWith("CrErr")) return;

  DEBUG("Main Motor command, s: " + String(duration) + "; lvl_current=" + String(lvl_curent));
  Blynk.virtualWrite(V1, "Main Motor command, s: " + String(duration) + "; lvl_current=" + String(lvl_curent));

  if (abs(duration) > TimeOpen * 1.2)
    duration = copysign(TimeOpen * 1.2, duration);

  puls = led_val;
  lvl_curent_time = LvlAngleTime(lvl_curent);

  for (int i = 0; i < abs(duration * 10.0); i++) {
    int pulseSpeed = (1023 - led_val) / 10;
    puls = (puls < 1023) ? puls + pulseSpeed : led_val;
    analogWrite(D4, puls);

    if (duration > 0 && digitalRead(EndPinO) == LOW) {
      Blynk.virtualWrite(V1, "Window fully open");
      Blynk.virtualWrite(V2, 100);
      lvl_curent = 100;
      lvl_curent_time = TimeOpen;
      break;
    }

    if (duration < 0 && digitalRead(EndPinC) == LOW) {
      Blynk.virtualWrite(V1, "Window closed");
      Blynk.virtualWrite(V2, 10);
      lvl_curent = 10;
      lvl_curent_time = 0;
      break;
    }

    if (duration > 0) {
      digitalWrite(DrPinO, HIGH);
      lvl_curent_time += 0.1;
    }

    if (duration < 0) {
      digitalWrite(DrPinC, HIGH);
      lvl_curent_time -= 0.1;
    }

    if (i % 5 == 0) {
      lvl_curent = TimeAnglelvl(constrain(lvl_curent_time, 0, TimeOpen));
      Blynk.virtualWrite(V2, lvl_curent);
    }

    Blynk.run();
    delay(100);
  }

  lvl_curent = TimeAnglelvl(constrain(lvl_curent_time, 0, TimeOpen));
  Blynk.virtualWrite(V2, lvl_curent);

  digitalWrite(DrPinO, LOW);
  digitalWrite(DrPinC, LOW);
  analogWrite(D4, led_val);
  DEBUG("Main Motor done. Final level: " + String(lvl_curent));
  Blynk.virtualWrite(V1, "Main Motor done. Final level: " + String(lvl_curent));


  if (abs(duration) <= TimeOpen) {
    Blynk.virtualWrite(V1, "OK");
  }

  delay(100);
}

float AngleToLvl(float Angle) {
  int mid_Angle=(StartAngle+(180-EndAngle))/2;
  return constrain(-45/cos(radians(mid_Angle))*cos(radians(Angle))+55,10,100);
}

float LvlToAngle(float New_lvl) {
  int mid_Angle=(StartAngle+(180-EndAngle))/2;
  return constrain(degrees(acos((New_lvl-55)*cos(radians(mid_Angle))/-45)),StartAngle,EndAngle);
}

float LvlAngleTime(float New_lvl) {
  float New_Angle = LvlToAngle(constrain(New_lvl,10,100));
  return constrain(map(New_Angle*300.0,StartAngle*300,EndAngle*300,0,TimeOpen*300),0,TimeOpen*300)/300.0;
}

float TimeAnglelvl(float New_time) {
  float New_Angle = map(New_time*300.0,0,TimeOpen*300,StartAngle*300,EndAngle*300)/300.0;
  return AngleToLvl(New_Angle);
}

float TimeTolvl (float New_WorkTime) {
  float inv;
  if (New_WorkTime<0) {inv=-1;} else {inv=1;}
  return inv*constrain(map(abs(New_WorkTime*300),0,TimeOpenAlt*300,0,10*300),0,10*300)/300.0; // Умножаем все на 300 потом делим на нее же, т.к. функция map работает с целыми чисолами и получаеться погрешность!
}


float LvlToTime (float New_lvl)  {

  float inv;
  if (New_lvl<0) {inv=-1;} else {inv=1;}
  return inv*constrain(map(abs(New_lvl*300),0,10*300,0,TimeOpenAlt*300),0,TimeOpenAlt*300)/300.0;
}

void SendMessage(String my_text, String my_text2) {
  Blynk.logEvent(my_text, my_text2);
  delay(1000);
} 
