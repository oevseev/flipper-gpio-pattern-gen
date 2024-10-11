#include "menu_view.h"

struct MenuView {
    App* app;
    View* view;
};

MenuView* MenuView_alloc(App* app) {
    MenuView* this = (MenuView*)malloc(sizeof(MenuView));
    furi_check(this != NULL);

    this->app = app;
    this->view = view_alloc();

    return this;
}

void MenuView_free(MenuView* this) {
    furi_assert(this != NULL);

    view_free(this->view);

    free(this);
}

View* MenuView_getView(MenuView* this) {
    furi_assert(this != NULL);

    return this->view;
}
