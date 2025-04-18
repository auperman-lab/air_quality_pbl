#ifndef SALT_PEPPER_H
#define SALT_PEPPER_H

#include <Arduino.h>

class SaltPepperFilter {
public:
    SaltPepperFilter();

    // Applies the filter to a new value and returns the filtered result
    float apply(float new_value);

    // Resets the internal state of the filter
    void reset();

private:
    static const uint8_t HISTORY_SIZE = 50;
    float history[HISTORY_SIZE];
    uint8_t index;
    bool is_full;
};

#endif 
