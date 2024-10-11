#pragma once

#include "app/app.h"

typedef struct MainRunScene MainRunScene;

MainRunScene* MainRunScene_alloc(App* app);
void MainRunScene_free(MainRunScene* this);

void MainRunScene_handleSceneManagerEnter(MainRunScene* this);
bool MainRunScene_handleSceneManagerEvent(MainRunScene* this, SceneManagerEvent event);
void MainRunScene_handleSceneManagerExit(MainRunScene* this);
