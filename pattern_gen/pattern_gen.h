#pragma once

#include "pattern.h"

#include <stdbool.h>
#include <stdint.h>

static const size_t PATTERN_GEN_MAX_SAMPLES = 32768;

typedef struct PatternGen PatternGen;

PatternGen* PatternGen_alloc();
void PatternGen_free(PatternGen* this);

const Pattern* PatternGen_getPattern(PatternGen* this);
size_t PatternGen_getIdx(PatternGen* this);

void PatternGen_resize(PatternGen* this, size_t size);
bool PatternGen_toggle(PatternGen* this, size_t sampleIdx, size_t pinIdx);

void PatternGen_reset(PatternGen* this);
size_t PatternGen_tick(PatternGen* this);
