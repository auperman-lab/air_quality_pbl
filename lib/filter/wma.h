#ifndef WMA_H
#define WMA_H

#include <Arduino.h>

class WeightedMovingAverageFilter {
public:
    WeightedMovingAverageFilter();

    // Applies the filter to a new value and returns the filtered result
    float apply(float new_value);

    // Resets internal state
    void reset();

private:
    static const uint8_t HISTORY_SIZE = 5;
    float history[HISTORY_SIZE];
    uint8_t index;
    bool is_full;

    // You can optionally define weights here if needed
    // E.g., recent values get more importance
    const float weights[HISTORY_SIZE] = { 0.05, 0.1, 0.2, 0.3, 0.35 };  // Must sum ~1
};

#endif // WEIGHTED_MOVING_AVERAGE_FILTER_H
