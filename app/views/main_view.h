#pragma once

#include "app/app.h"

#include "pattern_gen/pattern.h"

typedef enum {
    MainViewState_Idle,
    MainViewState_Edit,
    MainViewState_Run
} MainViewState;

typedef struct MainView MainView;

MainView* MainView_alloc(App* app);
void MainView_free(MainView* this);

View* MainView_getView(MainView* this);

void MainView_setPattern(MainView* this, const Pattern* pattern);

void MainView_setState(MainView* this, MainViewState state);
size_t MainView_getSampleIdx(MainView* this);
size_t MainView_getPinIdx(MainView* this);

void MainView_scrollLeft(MainView* this, size_t delta);
void MainView_scrollRight(MainView* this, size_t delta);
void MainView_moveCursorUp(MainView* this, size_t delta);
void MainView_moveCursorDown(MainView* this, size_t delta);
void MainView_moveCursorLeft(MainView* this, size_t delta);
void MainView_moveCursorRight(MainView* this, size_t delta);
void MainView_scrollTo(MainView* this, size_t sampleIdx);

void MainView_updateGrid(MainView* this);
