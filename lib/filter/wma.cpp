#include "wma.h"
#include <string.h>  // for memset

WeightedMovingAverageFilter::WeightedMovingAverageFilter() {
    reset();
}

float WeightedMovingAverageFilter::apply(float new_value) {
    // Insert new value into history
    history[index++] = new_value;

    if (index >= HISTORY_SIZE) {
        index = 0;
        is_full = true;
    }

    float result = 0.0;
    uint8_t count = is_full ? HISTORY_SIZE : index;

    // Apply weighted average only to the valid range
    float total_weight = 0.0;
    for (uint8_t i = 0; i < count; ++i) {
        result += history[i] * weights[HISTORY_SIZE - count + i];
        total_weight += weights[HISTORY_SIZE - count + i];
    }

    if (total_weight > 0) {
        result /= total_weight;
    }

    return result;
}

void WeightedMovingAverageFilter::reset() {
    memset(history, 0, sizeof(history));
    index = 0;
    is_full = false;
}
