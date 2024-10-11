#pragma once

#include "app/app.h"

typedef struct MenuView MenuView;

MenuView* MenuView_alloc(App* app);
void MenuView_free(MenuView* this);

View* MenuView_getView(MenuView* this);
