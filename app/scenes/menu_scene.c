#include "menu_scene.h"

struct MenuScene {
    App* app;
};

MenuScene* MenuScene_alloc(App* app) {
    furi_assert(app != NULL);

    MenuScene* this = (MenuScene*)malloc(sizeof(MenuScene));
    furi_check(this != NULL);

    this->app = app;

    return this;
}

void MenuScene_free(MenuScene* this) {
    furi_assert(this != NULL);

    free(this);
}

void MenuScene_handleSceneManagerEnter(MenuScene* this) {
    furi_assert(this != NULL);
}

bool MenuScene_handleSceneManagerEvent(MenuScene* this, SceneManagerEvent event) {
    furi_assert(this != NULL);
    UNUSED(event);

    return false;
}

void MenuScene_handleSceneManagerExit(MenuScene* this) {
    furi_assert(this != NULL);
}
