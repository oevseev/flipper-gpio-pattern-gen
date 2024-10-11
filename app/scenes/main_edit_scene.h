#pragma once

#include "app/app.h"

typedef struct MainEditScene MainEditScene;

MainEditScene* MainEditScene_alloc(App* app);
void MainEditScene_free(MainEditScene* this);

void MainEditScene_handleSceneManagerEnter(MainEditScene* this);
bool MainEditScene_handleSceneManagerEvent(MainEditScene* this, SceneManagerEvent event);
void MainEditScene_handleSceneManagerExit(MainEditScene* this);
