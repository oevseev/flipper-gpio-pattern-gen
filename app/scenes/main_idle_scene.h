#pragma once

#include "app/app.h"

typedef struct MainIdleScene MainIdleScene;

MainIdleScene* MainIdleScene_alloc(App* app);
void MainIdleScene_free(MainIdleScene* this);

void MainIdleScene_handleSceneManagerEnter(MainIdleScene* this);
bool MainIdleScene_handleSceneManagerEvent(MainIdleScene* this, SceneManagerEvent event);
void MainIdleScene_handleSceneManagerExit(MainIdleScene* this);
