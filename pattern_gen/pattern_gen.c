#include "pattern_gen.h"

#include "gpio/pin_controller.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct PatternGen {
    char* data;
    Pattern* pattern;
    size_t idx;
};

void PatternGen_allocPattern(PatternGen* this, size_t size) {
    if(size > 0) {
        this->data = (char*)malloc(size);
        memset(this->data, 0, size);
    }

    this->pattern = (Pattern*)malloc(sizeof(Pattern));
    this->pattern->data = this->data;
    this->pattern->numSamples = size;
}

void PatternGen_freePattern(PatternGen* this) {
    free(this->pattern);
    this->pattern = NULL;

    free(this->data);
    this->data = NULL;
}

PatternGen* PatternGen_alloc() {
    PatternGen* this = (PatternGen*)malloc(sizeof(PatternGen));
    assert(this != NULL);

    PatternGen_allocPattern(this, 0);
    PatternGen_reset(this);

    return this;
}

void PatternGen_free(PatternGen* this) {
    assert(this != NULL);

    PatternGen_freePattern(this);

    free(this);
}

const Pattern* PatternGen_getPattern(PatternGen* this) {
    assert(this != NULL);

    return this->pattern;
}

size_t PatternGen_getIdx(PatternGen* this) {
    assert(this != NULL);

    return this->idx;
}

void PatternGen_resize(PatternGen* this, size_t size) {
    assert(this != NULL);

    PatternGen_freePattern(this);
    PatternGen_allocPattern(this, size);
}

bool PatternGen_toggle(PatternGen* this, size_t sampleIdx, size_t pinIdx) {
    assert(this != NULL);

    if(sampleIdx >= this->pattern->numSamples) {
        return false;
    }
    if(pinIdx >= PATTERN_NUM_PINS) {
        return false;
    }

    this->data[sampleIdx] ^= (1 << pinIdx);
    return true;
}

void PatternGen_reset(PatternGen* this) {
    assert(this != NULL);

    this->idx = 0;
}

size_t PatternGen_tick(PatternGen* this) {
    assert(this != NULL);

    if(this->pattern->numSamples == 0) {
        return 0;
    }

    size_t curIdx = this->idx;
    PinController_setPins(this->data[curIdx]);

    this->idx++;
    if(this->idx >= this->pattern->numSamples) {
        this->idx -= this->pattern->numSamples;
    }

    return curIdx;
}
