#pragma once

#include <limits.h>
#include <stddef.h>

static const size_t PATTERN_NUM_PINS = CHAR_BIT;

typedef struct {
    size_t numSamples;
    const char* data;
} Pattern;
