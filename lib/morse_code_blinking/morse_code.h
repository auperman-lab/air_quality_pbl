#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <Arduino.h>
#include "led.h"

class Morse_Code {
    private:
        Led led;
        int timeUnit = 500;
        int dotDuration = timeUnit;  
        int dashDuration = timeUnit * 3; 
        int pauseInLetter = timeUnit;
        int pauseInterLetter = timeUnit * 3;
        int pauseInterWords = timeUnit * 7;


        void printMorseSymbol(const char *symbol);
        void printLetter(char letter);
    
    
    public:
        Morse_Code(Led led);
        void print(char *text);
    };
    

#endif