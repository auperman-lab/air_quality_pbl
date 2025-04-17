#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

class LCDService {
public:
    LCDService(uint8_t address = 0x27, uint8_t cols = 16, uint8_t rows = 2);

    void init();
    void print(const char* message, uint8_t col, uint8_t row);
    void clear();
    void backlightOn();
    void backlightOff();
    void setCursor(int, int);

private:
    LiquidCrystal_I2C lcd;

    FILE lcd_output_struct;
    FILE* lcd_output; // Just declaration here — no assignment!

    uint8_t colPos;
    uint8_t rowPos;
    uint8_t cols;
    uint8_t rows;

    static LCDService* instance;
    static int putcharRedirect(char c, FILE* stream);
};

#endif
