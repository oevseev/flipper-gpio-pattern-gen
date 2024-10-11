#include "main_idle_scene.h"

#include "app/views/main_view.h"

struct MainIdleScene {
    App* app;
};

MainIdleScene* MainIdleScene_alloc(App* app) {
    furi_assert(app != NULL);

    MainIdleScene* this = (MainIdleScene*)malloc(sizeof(MainIdleScene));
    furi_check(this != NULL);

    this->app = app;

    return this;
}

void MainIdleScene_free(MainIdleScene* this) {
    furi_assert(this != NULL);

    free(this);
}

void MainIdleScene_handleSceneManagerEnter(MainIdleScene* this) {
    furi_assert(this != NULL);

    MainView* mainView = App_getMainView(this->app);
    if(mainView != NULL) {
        MainView_setState(mainView, MainViewState_Idle);
    }
}

bool MainIdleScene_handleSceneManagerEvent(MainIdleScene* this, SceneManagerEvent event) {
    furi_assert(this != NULL);

    if(event.type != SceneManagerEventTypeCustom) {
        return false;
    }

    switch(event.event) {
    case AppEvent_Edit:
        App_enterMainEditScene(this->app);
        return true;
    case AppEvent_Run:
        App_enterMainRunScene(this->app);
        return true;
    case AppEvent_OpenMenu:
        App_enterMenuScene(this->app);
        return true;
    default:
        return false;
    }
}

void MainIdleScene_handleSceneManagerExit(MainIdleScene* this) {
    furi_assert(this != NULL);
}
