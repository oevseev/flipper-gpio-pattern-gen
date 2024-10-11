#pragma once

#include <gui/gui.h>
#include <gui/modules/widget.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

typedef struct PatternGen PatternGen;
typedef struct MainView MainView;

typedef enum {
    AppEvent_Edit,
    AppEvent_Edit_Toggle,
    AppEvent_Run,
    AppEvent_Run_IncreaseFreq,
    AppEvent_Run_DecreaseFreq,
    AppEvent_Stop,
    AppEvent_OpenMenu
} AppEvent;

typedef struct App App;

App* App_alloc();
void App_free(App* this);

void App_run(App* this);

PatternGen* App_getPatternGen(App* this);
void App_setPatternGen(App* this, PatternGen* patternGen);
MainView* App_getMainView(App* this);

void App_sendEvent(App* this, AppEvent event);

void App_enterMainEditScene(App* this);
void App_enterMainRunScene(App* this);
void App_enterMenuScene(App* this);
void App_exitCurrentScene(App* this);
