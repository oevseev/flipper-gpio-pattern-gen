#include "main_view.h"

#include "gpio_pattern_gen_icons.h"

#include <gui/elements.h>

#define GRID_NUM_COLS  19
#define MAX_STATUS_LEN 32

static const size_t BUTTON_HEIGHT = 12;
static const char editButtonStr[] = "Edit";

static const size_t STATUS_H_OFFSET = 3;

static const size_t GRID_NUM_ROWS = PATTERN_NUM_PINS;
static const size_t GRID_ROW_HEIGHT = 5;
static const size_t GRID_SEPARATOR_HEIGHT = 1;
static const size_t GRID_COL_WIDTH = 5;
static const size_t GRID_SEPARATOR_WIDTH = 1;
static const size_t GRID_LEFT_X = 13;
static const size_t GRID_HEIGHT = (GRID_ROW_HEIGHT + GRID_SEPARATOR_HEIGHT) * GRID_NUM_ROWS;

static const Icon* pinIcons[] = {&I_pa7, &I_pa6, &I_pa4, &I_pb3, &I_pb2, &I_pc3, &I_pc1, &I_pc0};

struct MainView {
    App* app;
    View* view;

    const Pattern* pattern;

    MainViewState state;
    size_t leftIdx;
    size_t sampleIdx;
    size_t pinIdx;
};

typedef struct {
    bool menuButtonVisible;
    bool editButtonVisible;
    bool runButtonVisible;
    bool runButtonOn;
    bool backButtonVisible;

    bool statusVisible;
    char status[MAX_STATUS_LEN];

    uint8_t grid[GRID_NUM_COLS];
    size_t numCols;

    bool cursorVisible;
    bool cursorColMode;
    size_t col;
    size_t row;
} Model;

void drawEditButton(Canvas* canvas) {
    // Compliant with the implementation in gui/elements.c

    const size_t verticalOffset = 3;
    const size_t horizontalOffset = 1;
    const size_t stringWidth = canvas_string_width(canvas, editButtonStr);
    const Icon* icon = &I_button_center;
    const int32_t iconHOffset = 3;
    const int32_t iconWidthWithOffset = icon_get_width(icon) + iconHOffset;
    const int32_t iconVOffset = icon_get_height(icon) + verticalOffset;
    const size_t buttonWidth = stringWidth + horizontalOffset * 2 + iconWidthWithOffset;

    const int32_t x = (canvas_width(canvas) - buttonWidth) / 2;
    const int32_t y = BUTTON_HEIGHT;

    int32_t lineY = y - BUTTON_HEIGHT;

    canvas_draw_box(canvas, x, y - BUTTON_HEIGHT, buttonWidth, BUTTON_HEIGHT);

    canvas_draw_line(canvas, x - 1, lineY, x - 1, y - 1);
    canvas_draw_line(canvas, x - 2, lineY, x - 2, y - 2);
    canvas_draw_line(canvas, x - 3, lineY, x - 3, y - 3);

    canvas_draw_line(canvas, x + buttonWidth + 0, lineY, x + buttonWidth + 0, y - 1);
    canvas_draw_line(canvas, x + buttonWidth + 1, lineY, x + buttonWidth + 1, y - 2);
    canvas_draw_line(canvas, x + buttonWidth + 2, lineY, x + buttonWidth + 2, y - 3);

    canvas_invert_color(canvas);
    canvas_draw_icon(canvas, x + horizontalOffset, y - iconVOffset, &I_button_center);
    canvas_draw_str(
        canvas, x + horizontalOffset + iconWidthWithOffset, y - verticalOffset, editButtonStr);
    canvas_invert_color(canvas);
}

void drawRunButton(Canvas* canvas, bool on) {
    // Compliant with the implementation in gui/elements.c

    const Icon* iconDown = &I_button_down;
    const Icon* iconSwitch = on ? &I_switch_on : &I_switch_off;

    const size_t verticalOffset = 3;
    const size_t horizontalOffset = 3;
    const int32_t iconDownHOffset = 3;
    const int32_t iconDownWidthWithOffset = icon_get_width(iconDown) + iconDownHOffset;
    const int32_t iconDownVOffset = icon_get_height(iconDown) + verticalOffset + 1;
    const int32_t iconSwitchRelativeOffset = -2;
    const int32_t iconSwitchVOffset =
        icon_get_height(iconSwitch) + verticalOffset + 1 + iconSwitchRelativeOffset;
    const size_t buttonWidth =
        icon_get_width(iconSwitch) + horizontalOffset * 2 + iconDownWidthWithOffset;

    const int32_t x = canvas_width(canvas);
    const int32_t y = BUTTON_HEIGHT;

    int32_t lineX = x - buttonWidth;
    int32_t lineY = y - BUTTON_HEIGHT;

    canvas_draw_box(canvas, lineX, lineY, buttonWidth, BUTTON_HEIGHT);
    canvas_draw_line(canvas, lineX - 1, lineY, lineX - 1, y - 1);
    canvas_draw_line(canvas, lineX - 2, lineY, lineX - 2, y - 2);
    canvas_draw_line(canvas, lineX - 3, lineY, lineX - 3, y - 3);

    canvas_invert_color(canvas);
    canvas_draw_icon(
        canvas, x - buttonWidth + horizontalOffset, y - iconSwitchVOffset, iconSwitch);
    canvas_draw_icon(
        canvas, x - horizontalOffset - icon_get_width(iconDown), y - iconDownVOffset, iconDown);
    canvas_invert_color(canvas);
}

static void drawGrid(Canvas* canvas, Model* model) {
    const size_t canvasHeight = canvas_height(canvas);

    const size_t gridRightX = GRID_LEFT_X +
                              (GRID_COL_WIDTH + GRID_SEPARATOR_WIDTH) * model->numCols +
                              GRID_SEPARATOR_WIDTH - 1;
    const size_t gridTopY = canvasHeight - GRID_HEIGHT - 1;

    canvas_set_color(canvas, ColorBlack);
    canvas_set_bitmap_mode(canvas, true);

    for(size_t row = 0; row <= GRID_NUM_ROWS; row++) {
        const size_t rowY = gridTopY + (GRID_ROW_HEIGHT + GRID_SEPARATOR_HEIGHT) * row;
        canvas_draw_line(canvas, GRID_LEFT_X, rowY, gridRightX, rowY);
        if(row < GRID_NUM_ROWS) {
            canvas_draw_icon(canvas, 0, rowY + GRID_SEPARATOR_HEIGHT, pinIcons[row]);
        }
    }

    for(size_t col = 0; col < model->numCols; col++) {
        const size_t colX = GRID_LEFT_X + (GRID_COL_WIDTH + GRID_SEPARATOR_WIDTH) * col;
        canvas_draw_line(canvas, colX, gridTopY, colX, canvasHeight - 1);
        for(size_t row = 0; row < GRID_NUM_ROWS; row++) {
            const size_t rowY = gridTopY + (GRID_ROW_HEIGHT + GRID_SEPARATOR_HEIGHT) * row;

            const bool isSet = ((model->grid[col] >> row) & 1) != 0;
            if(isSet) {
                canvas_draw_box(
                    canvas,
                    colX + GRID_SEPARATOR_WIDTH,
                    rowY + GRID_SEPARATOR_HEIGHT,
                    GRID_COL_WIDTH,
                    GRID_ROW_HEIGHT);
            }

            const bool isSelected = model->cursorVisible && col == model->col &&
                                    (model->cursorColMode || row == model->row);
            if(isSelected) {
                canvas_set_color(canvas, ColorXOR);
                canvas_draw_icon(
                    canvas, colX + GRID_SEPARATOR_WIDTH, rowY + GRID_SEPARATOR_HEIGHT, &I_cursor);
                canvas_set_color(canvas, ColorBlack);
            }
        }
    }

    canvas_draw_line(canvas, gridRightX, gridTopY, gridRightX, canvasHeight - 1);
}

static void drawStatus(Canvas* canvas, const char* str) {
    canvas_draw_str(canvas, STATUS_H_OFFSET, canvas_current_font_height(canvas) - 1, str);
}

static void drawCallback(Canvas* canvas, void* modelPtr) {
    Model* model = modelPtr;

    if(model->menuButtonVisible) {
        elements_button_up(canvas, "Menu");
    }
    if(model->editButtonVisible) {
        drawEditButton(canvas);
    }
    if(model->runButtonVisible) {
        drawRunButton(canvas, model->runButtonOn);
    }

    if(model->statusVisible) {
        drawStatus(canvas, model->status);
    }

    drawGrid(canvas, model);
}

static bool MainView_handleInputIdle(MainView* this, InputEvent* event) {
    if(event->type != InputTypeShort) {
        return false;
    }

    switch(event->key) {
    case InputKeyUp:
        App_sendEvent(this->app, AppEvent_OpenMenu);
        return true;
    case InputKeyDown:
        App_sendEvent(this->app, AppEvent_Run);
        return true;
    case InputKeyLeft:
        MainView_scrollLeft(this, 1);
        return true;
    case InputKeyRight:
        MainView_scrollRight(this, 1);
        return true;
    case InputKeyOk:
        App_sendEvent(this->app, AppEvent_Edit);
        return true;
    default:
        return false;
    }
}

static bool MainView_handleInputEdit(MainView* this, InputEvent* event) {
    if(event->type != InputTypeShort) {
        return false;
    }

    switch(event->key) {
    case InputKeyUp:
        MainView_moveCursorUp(this, 1);
        return true;
    case InputKeyDown:
        MainView_moveCursorDown(this, 1);
        return true;
    case InputKeyLeft:
        MainView_moveCursorLeft(this, 1);
        return true;
    case InputKeyRight:
        MainView_moveCursorRight(this, 1);
        return true;
    case InputKeyOk:
        App_sendEvent(this->app, AppEvent_Edit_Toggle);
        return true;
    default:
        return false;
    }
}

static bool MainView_handleInputRun(MainView* this, InputEvent* event) {
    if(event->type != InputTypeShort) {
        return false;
    }

    switch(event->key) {
    case InputKeyDown:
        App_sendEvent(this->app, AppEvent_Stop);
        return true;
    default:
        return false;
    }
}

static bool inputCallback(InputEvent* event, void* context) {
    MainView* this = (MainView*)context;

    switch(this->state) {
    case MainViewState_Idle:
        return MainView_handleInputIdle(this, event);
    case MainViewState_Edit:
        return MainView_handleInputEdit(this, event);
    case MainViewState_Run:
        return MainView_handleInputRun(this, event);
    default:
        return false;
    }
}

static inline size_t MainView_numCols(MainView* this) {
    if(this->pattern == NULL) {
        return 0;
    }

    size_t numCols = this->pattern->numSamples - this->leftIdx;
    if(numCols > GRID_NUM_COLS) {
        numCols = GRID_NUM_COLS;
    }

    return numCols;
}

static inline size_t MainView_numSamples(MainView* this) {
    if(this->pattern == NULL) {
        return 0;
    }

    return this->pattern->numSamples;
}

MainView* MainView_alloc(App* app) {
    MainView* this = (MainView*)malloc(sizeof(MainView));
    furi_check(this != NULL);

    this->app = app;

    this->view = view_alloc();
    view_set_context(this->view, this);
    view_set_draw_callback(this->view, drawCallback);
    view_set_input_callback(this->view, inputCallback);
    view_allocate_model(this->view, ViewModelTypeLocking, sizeof(Model));

    this->pattern = NULL;

    MainView_setState(this, MainViewState_Idle);

    this->leftIdx = 0;
    this->sampleIdx = 0;
    this->pinIdx = 0;

    MainView_updateGrid(this);

    return this;
}

void MainView_free(MainView* this) {
    furi_assert(this != NULL);

    view_free_model(this->view);
    view_free(this->view);

    free(this);
}

View* MainView_getView(MainView* this) {
    furi_assert(this != NULL);

    return this->view;
}

void MainView_setPattern(MainView* this, const Pattern* pattern) {
    furi_assert(this != NULL);

    this->pattern = pattern;

    MainView_updateGrid(this);
}

void MainView_setState(MainView* this, MainViewState state) {
    furi_assert(this != NULL);

    this->state = state;

    Model* model = (Model*)view_get_model(this->view);

    model->menuButtonVisible = state == MainViewState_Idle;
    model->editButtonVisible = state == MainViewState_Idle;
    model->runButtonVisible = state != MainViewState_Edit;
    model->runButtonOn = state == MainViewState_Run;
    model->backButtonVisible = state == MainViewState_Edit;

    model->statusVisible = state != MainViewState_Idle;

    model->cursorVisible = state != MainViewState_Idle;
    model->cursorColMode = state == MainViewState_Run;

    view_commit_model(this->view, true);
}

size_t MainView_getSampleIdx(MainView* this) {
    furi_assert(this != NULL);

    return this->sampleIdx;
}

size_t MainView_getPinIdx(MainView* this) {
    furi_assert(this != NULL);

    return this->pinIdx;
}

void MainView_scrollLeft(MainView* this, size_t delta) {
    furi_assert(this != NULL);

    if(delta <= this->leftIdx) {
        this->leftIdx -= delta;
    } else {
        this->leftIdx = 0;
    }

    size_t numCols = MainView_numCols(this);
    size_t rightIdx = this->leftIdx + numCols - 1;
    if(this->sampleIdx > rightIdx) {
        this->sampleIdx = rightIdx;
    }

    MainView_updateGrid(this);
}

void MainView_scrollRight(MainView* this, size_t delta) {
    furi_assert(this != NULL);

    size_t numCols = MainView_numCols(this);
    size_t rightIdx = this->leftIdx + numCols - 1;
    if(rightIdx + delta < MainView_numSamples(this)) {
        this->leftIdx += delta;
    } else {
        this->leftIdx = rightIdx - numCols + 1;
    }

    if(this->sampleIdx < this->leftIdx) {
        this->sampleIdx = this->leftIdx;
    }

    MainView_updateGrid(this);
}

void MainView_moveCursorUp(MainView* this, size_t delta) {
    furi_assert(this != NULL);

    if(delta <= this->pinIdx) {
        this->pinIdx -= delta;
    } else {
        this->pinIdx = 0;
    }

    MainView_updateGrid(this);
}

void MainView_moveCursorDown(MainView* this, size_t delta) {
    furi_assert(this != NULL);

    if(this->pinIdx + delta < GRID_NUM_ROWS) {
        this->pinIdx += delta;
    } else {
        this->pinIdx = GRID_NUM_ROWS - 1;
    }

    MainView_updateGrid(this);
}

void MainView_moveCursorLeft(MainView* this, size_t delta) {
    furi_assert(this != NULL);

    if(delta <= this->sampleIdx) {
        this->sampleIdx -= delta;
    } else {
        this->sampleIdx = 0;
    }

    if(this->leftIdx > this->sampleIdx) {
        this->leftIdx = this->sampleIdx;
    }

    MainView_updateGrid(this);
}

void MainView_moveCursorRight(MainView* this, size_t delta) {
    furi_assert(this != NULL);

    size_t numSamples = MainView_numSamples(this);
    if(this->sampleIdx + delta < numSamples) {
        this->sampleIdx += delta;
    } else {
        this->sampleIdx = numSamples - 1;
    }

    size_t numCols = MainView_numCols(this);
    size_t rightIdx = this->leftIdx + numCols - 1;
    if(rightIdx < this->sampleIdx) {
        size_t offset = this->sampleIdx - rightIdx;
        this->leftIdx += offset;
    }

    MainView_updateGrid(this);
}

void MainView_scrollTo(MainView* this, size_t sampleIdx) {
    furi_assert(this != NULL);

    this->sampleIdx = sampleIdx;
    this->leftIdx = sampleIdx;

    size_t numSamples = MainView_numSamples(this);
    size_t maxLeftIdx = numSamples >= GRID_NUM_COLS ? numSamples - GRID_NUM_COLS : 0;
    if(this->leftIdx > maxLeftIdx) {
        this->leftIdx = maxLeftIdx;
    }

    MainView_updateGrid(this);
}

void MainView_updateGrid(MainView* this) {
    furi_assert(this != NULL);

    Model* model = (Model*)view_get_model(this->view);

    model->row = this->pinIdx;
    model->col = this->sampleIdx - this->leftIdx;
    model->numCols = MainView_numCols(this);

    if(this->pattern != NULL) {
        memcpy(&model->grid, &this->pattern->data[this->leftIdx], model->numCols);
    }

    size_t numSamples = MainView_numSamples(this);
    snprintf(
        model->status,
        sizeof(model->status),
        "%d/%d",
        this->sampleIdx,
        numSamples > 0 ? numSamples - 1 : 0);

    view_commit_model(this->view, true);
}
