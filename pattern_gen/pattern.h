#pragma once

#include <limits.h>
#include <stddef.h>

static const size_t PATTERN_NUM_PINS = CHAR_BIT;

typedef struct {
    const char* data;
    size_t numSamples;
} Pattern;
