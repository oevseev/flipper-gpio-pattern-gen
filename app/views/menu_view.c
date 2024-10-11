#include "menu_view.h"

#include <gui/modules/submenu.h>

struct MenuView {
    App* app;
    Submenu* menu;
};

typedef enum {
    MenuViewItem_SetFrequency,
    MenuViewItem_SetPatternLength,
    MenuViewItem_ScrollTo
} MenuViewItem;

static void onItem(void* context, uint32_t index) {
    MenuView* this = (MenuView*)context;

    switch(index) {
    case MenuViewItem_SetPatternLength:
        App_sendEvent(this->app, AppEvent_SetPatternLengthRequested);
        break;
    case MenuViewItem_SetFrequency:
        App_sendEvent(this->app, AppEvent_SetFrequencyRequested);
        break;
    case MenuViewItem_ScrollTo:
        App_sendEvent(this->app, AppEvent_ScrollToRequested);
    }
}

MenuView* MenuView_alloc(App* app) {
    MenuView* this = (MenuView*)malloc(sizeof(MenuView));
    furi_check(this != NULL);

    this->app = app;

    this->menu = submenu_alloc();
    submenu_add_item(
        this->menu, "Set Pattern Length...", MenuViewItem_SetPatternLength, onItem, this);
    submenu_add_item(this->menu, "Set Frequency...", MenuViewItem_SetFrequency, onItem, this);
    submenu_add_item(this->menu, "Scroll To...", MenuViewItem_ScrollTo, onItem, this);

    return this;
}

void MenuView_free(MenuView* this) {
    furi_assert(this != NULL);

    submenu_free(this->menu);

    free(this);
}

View* MenuView_getView(MenuView* this) {
    furi_assert(this != NULL);

    return submenu_get_view(this->menu);
}
