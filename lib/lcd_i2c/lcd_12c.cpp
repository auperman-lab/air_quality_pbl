#include "lcd_i2c.h"
#include <stdio.h>

LCDService* LCDService::instance = nullptr;

LCDService::LCDService(uint8_t address, uint8_t cols, uint8_t rows)
    : lcd(address, cols, rows),
      colPos(0),
      rowPos(0),
      cols(cols),
      rows(rows),
      lcd_output(nullptr) // Initialize pointer to null
{
    instance = this;
}

void LCDService::init() {

    lcd_output = &lcd_output_struct;

    lcd.init();
    backlightOn();

    fdev_setup_stream(lcd_output, putcharRedirect, nullptr, _FDEV_SETUP_WRITE);
    stdout = lcd_output;
}

void LCDService::print(const char* message, uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
    lcd.print(message);
}

void LCDService::clear() {
    lcd.clear();
    colPos = 0;
    rowPos = 0;
    lcd.setCursor(0, 0);
}

void LCDService::backlightOn() {
    lcd.backlight();
}

void LCDService::backlightOff() {
    lcd.noBacklight();
}

void LCDService::setCursor(int x, int y){
    lcd.setCursor(x,y);
}

int LCDService::putcharRedirect(char c, FILE* stream) {
    if (!instance) return -1;

    if (c == '\n') {
        instance->rowPos = (instance->rowPos + 1) % instance->rows;
        instance->colPos = 0;
        instance->lcd.setCursor(instance->colPos, instance->rowPos);
    } else {
        instance->lcd.setCursor(instance->colPos, instance->rowPos);
        instance->lcd.print(c);
        instance->colPos++;
        if (instance->colPos >= instance->cols) {
            instance->colPos = 0;
            instance->rowPos = (instance->rowPos + 1) % instance->rows;
            instance->lcd.setCursor(instance->colPos, instance->rowPos);
        }
    }
    return 0;
}