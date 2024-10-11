#pragma once

#include "pattern.h"

#include <stdbool.h>

typedef struct PatternGen PatternGen;

PatternGen* PatternGen_alloc();
void PatternGen_free(PatternGen* this);

const Pattern* PatternGen_getPattern(PatternGen* this);

bool PatternGen_toggle(PatternGen* this, size_t sampleIdx, size_t pinIdx);
void PatternGen_resize(PatternGen* this, size_t size);

void PatternGen_reset(PatternGen* this);
size_t PatternGen_tick(PatternGen* this);
