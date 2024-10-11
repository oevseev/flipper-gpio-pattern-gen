#pragma once

#include <gui/gui.h>
#include <gui/modules/widget.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

typedef struct PatternGen PatternGen;
typedef struct MainView MainView;

static const int32_t TICK_FREQ = 1000;

typedef enum {
    AppEvent_MainView_MenuButtonInvoked,
    AppEvent_MainView_EditButtonInvoked,
    AppEvent_MainView_RunButtonInvoked,
    AppEvent_MainView_GridCellInvoked,
    AppEvent_SetPatternLengthRequested,
    AppEvent_SetFrequencyRequested,
    AppEvent_ScrollToRequested,
    AppEvent_NumberInput
} AppEvent;

typedef struct App App;

App* App_alloc();
void App_free(App* this);

void App_run(App* this);

PatternGen* App_getPatternGen(App* this);
void App_setPatternGen(App* this, PatternGen* patternGen);
MainView* App_getMainView(App* this);

int32_t App_getFreq(App* this);

void App_sendEvent(App* this, AppEvent event);

void App_enterMainEditScene(App* this);
void App_enterMainRunScene(App* this);
void App_enterMenuScene(App* this);
void App_enterSetPatternLengthScene(App* this);
void App_enterSetFrequencyScene(App* this);
void App_enterScrollToScene(App* this);
void App_exitCurrentScene(App* this);
void App_returnToMainScene(App* this);
