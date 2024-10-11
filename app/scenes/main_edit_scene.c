#include "main_edit_scene.h"

#include "app/views/main_view.h"

#include "pattern_gen/pattern_gen.h"

struct MainEditScene {
    App* app;
};

bool MainEditScene_handleToggle(MainEditScene* this) {
    MainView* mainView = App_getMainView(this->app);
    PatternGen* patternGen = App_getPatternGen(this->app);
    if(mainView != NULL && patternGen != NULL) {
        PatternGen_toggle(
            patternGen, MainView_getSampleIdx(mainView), MainView_getPinIdx(mainView));
        MainView_updateGrid(mainView);
    }
    return true;
}

MainEditScene* MainEditScene_alloc(App* app) {
    furi_assert(app != NULL);

    MainEditScene* this = (MainEditScene*)malloc(sizeof(MainEditScene));
    furi_check(this != NULL);

    this->app = app;

    return this;
}

void MainEditScene_free(MainEditScene* this) {
    furi_assert(this != NULL);

    free(this);
}

void MainEditScene_handleSceneManagerEnter(MainEditScene* this) {
    furi_assert(this != NULL);

    MainView* mainView = App_getMainView(this->app);
    if(mainView != NULL) {
        MainView_setState(mainView, MainViewState_Edit);
    }
}

bool MainEditScene_handleSceneManagerEvent(MainEditScene* this, SceneManagerEvent event) {
    furi_assert(this != NULL);

    if(event.type != SceneManagerEventTypeCustom) {
        return false;
    }

    switch(event.event) {
    case AppEvent_MainView_GridCellInvoked:
        return MainEditScene_handleToggle(this);
    default:
        return false;
    }
}

void MainEditScene_handleSceneManagerExit(MainEditScene* this) {
    furi_assert(this != NULL);
}
