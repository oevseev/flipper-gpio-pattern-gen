#include "main_run_scene.h"

#include "app/views/main_view.h"

#include "pattern_gen/pattern_gen.h"

struct MainRunScene {
    App* app;
};

bool MainRunScene_handleTick(MainRunScene* this) {
    PatternGen* patternGen = App_getPatternGen(this->app);
    if(patternGen == NULL) {
        return false;
    }

    size_t curSample = PatternGen_tick(patternGen);

    MainView* mainView = App_getMainView(this->app);
    if(mainView != NULL) {
        MainView_scrollTo(mainView, curSample);
    }

    return true;
}

MainRunScene* MainRunScene_alloc(App* app) {
    furi_assert(app != NULL);

    MainRunScene* this = (MainRunScene*)malloc(sizeof(MainRunScene));
    furi_check(this != NULL);

    this->app = app;

    return this;
}

void MainRunScene_free(MainRunScene* this) {
    furi_assert(this != NULL);

    free(this);
}

void MainRunScene_handleSceneManagerEnter(MainRunScene* this) {
    furi_assert(this != NULL);

    PatternGen* patternGen = App_getPatternGen(this->app);
    if(patternGen != NULL) {
        PatternGen_reset(patternGen);
    }

    MainView* mainView = App_getMainView(this->app);
    if(mainView != NULL) {
        MainView_setState(mainView, MainViewState_Run);
        MainView_scrollTo(mainView, 0);
    }
}

bool MainRunScene_handleSceneManagerEvent(MainRunScene* this, SceneManagerEvent event) {
    furi_assert(this != NULL);

    if(event.type == SceneManagerEventTypeTick) {
        return MainRunScene_handleTick(this);
    } else if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case AppEvent_Stop:
            App_exitCurrentScene(this->app);
            return true;
        default:
            return false;
        }
    } else {
        return false;
    }
}

void MainRunScene_handleSceneManagerExit(MainRunScene* this) {
    furi_assert(this != NULL);
}
