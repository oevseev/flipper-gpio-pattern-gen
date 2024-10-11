#pragma once

#include "app/app.h"

typedef struct MenuScene MenuScene;

MenuScene* MenuScene_alloc(App* app);
void MenuScene_free(MenuScene* this);

void MenuScene_handleSceneManagerEnter(MenuScene* this);
bool MenuScene_handleSceneManagerEvent(MenuScene* this, SceneManagerEvent event);
void MenuScene_handleSceneManagerExit(MenuScene* this);
