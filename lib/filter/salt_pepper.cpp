#include "salt_pepper.h"
#include <string.h> 

SaltPepperFilter::SaltPepperFilter() {
    reset();
}

float SaltPepperFilter::apply(float new_value) {
    history[index++] = new_value;

    if (index >= HISTORY_SIZE) {
        index = 0;
        is_full = true;
    }

    if (!is_full) {
        return new_value;
    }

    float sorted[HISTORY_SIZE];
    memcpy(sorted, history, sizeof(sorted));

    for (uint8_t i = 0; i < HISTORY_SIZE - 1; ++i) {
        for (uint8_t j = i + 1; j < HISTORY_SIZE; ++j) {
            if (sorted[i] > sorted[j]) {
                float temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    float sum = 0;
    for (uint8_t i = 1; i < HISTORY_SIZE - 1; ++i) {
        sum += sorted[i];
    }

    return sum / (HISTORY_SIZE - 2);
}

void SaltPepperFilter::reset() {
    memset(history, 0, sizeof(history));
    index = 0;
    is_full = false;
}
