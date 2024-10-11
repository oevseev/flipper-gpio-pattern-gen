#include "app.h"

#include "scenes/main_idle_scene.h"
#include "scenes/main_edit_scene.h"
#include "scenes/main_run_scene.h"
#include "scenes/menu_scene.h"

#include "views/main_view.h"
#include "views/menu_view.h"

#include "pattern_gen/pattern_gen.h"

#include <gui/modules/empty_screen.h>
#include <gui/modules/number_input.h>

static const int32_t MIN_FREQ = 1;

typedef enum {
    AppView_Empty,
    AppView_Main,
    AppView_Menu,
    AppView_NumberInput
} AppView;

typedef enum {
    AppScene_Main_Idle,
    AppScene_Main_Edit,
    AppScene_Main_Run,
    AppScene_Menu,
    AppScene_SetPatternLength,
    AppScene_SetFrequency,
    AppScene_ScrollTo,
    AppScene_count
} AppScene;

struct App {
    Gui* gui;
    ViewDispatcher* viewDispatcher;
    SceneManager* sceneManager;
    EmptyScreen* emptyScreen;
    MainView* mainView;
    MenuView* menuView;
    NumberInput* numberInput;
    void* currentScene;
    PatternGen* patternGen;
    int32_t numberInputResult;
    int32_t freq;
};

static bool onViewDispatcherCustomEvent(void* context, uint32_t event) {
    App* this = (App*)context;

    return scene_manager_handle_custom_event(this->sceneManager, event);
}

static bool onViewDispatcherNavigationEvent(void* context) {
    App* this = (App*)context;

    return scene_manager_handle_back_event(this->sceneManager);
}

static void onViewDispatcherTickEvent(void* context) {
    App* this = (App*)context;

    return scene_manager_handle_tick_event(this->sceneManager);
}

static void numberInputCallback(void* context, int32_t number) {
    App* this = (App*)context;

    this->numberInputResult = number;
    App_sendEvent(this, AppEvent_NumberInput);
}

static void mainIdleScene_onEnter(void* context) {
    App* this = (App*)context;

    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Main);

    this->currentScene = MainIdleScene_alloc(this);
    MainIdleScene_handleSceneManagerEnter((MainIdleScene*)this->currentScene);
}

static bool mainIdleScene_onEvent(void* context, SceneManagerEvent event) {
    App* this = (App*)context;

    return MainIdleScene_handleSceneManagerEvent((MainIdleScene*)this->currentScene, event);
}

static void mainIdleScene_onExit(void* context) {
    App* this = (App*)context;

    MainIdleScene_handleSceneManagerExit((MainIdleScene*)this->currentScene);
    MainIdleScene_free((MainIdleScene*)this->currentScene);
    this->currentScene = NULL;
}

static void mainEditScene_onEnter(void* context) {
    App* this = (App*)context;

    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Main);

    this->currentScene = MainEditScene_alloc(this);
    MainEditScene_handleSceneManagerEnter((MainEditScene*)this->currentScene);
}

static bool mainEditScene_onEvent(void* context, SceneManagerEvent event) {
    App* this = (App*)context;

    return MainEditScene_handleSceneManagerEvent((MainEditScene*)this->currentScene, event);
}

static void mainEditScene_onExit(void* context) {
    App* this = (App*)context;

    MainEditScene_handleSceneManagerExit((MainEditScene*)this->currentScene);
    MainEditScene_free((MainEditScene*)this->currentScene);
    this->currentScene = NULL;
}

static void mainRunScene_onEnter(void* context) {
    App* this = (App*)context;

    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Main);

    this->currentScene = MainRunScene_alloc(this);
    MainRunScene_handleSceneManagerEnter((MainRunScene*)this->currentScene);
}

static bool mainRunScene_onEvent(void* context, SceneManagerEvent event) {
    App* this = (App*)context;

    return MainRunScene_handleSceneManagerEvent((MainRunScene*)this->currentScene, event);
}

static void mainRunScene_onExit(void* context) {
    App* this = (App*)context;

    MainRunScene_handleSceneManagerExit((MainRunScene*)this->currentScene);
    MainRunScene_free((MainRunScene*)this->currentScene);
    this->currentScene = NULL;
}

static void menuScene_onEnter(void* context) {
    App* this = (App*)context;

    this->menuView = MenuView_alloc(this);
    view_dispatcher_add_view(this->viewDispatcher, AppView_Menu, MenuView_getView(this->menuView));
    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Menu);

    this->currentScene = MenuScene_alloc(this);
    MenuScene_handleSceneManagerEnter((MenuScene*)this->currentScene);
}

static bool menuScene_onEvent(void* context, SceneManagerEvent event) {
    App* this = (App*)context;

    return MenuScene_handleSceneManagerEvent((MenuScene*)this->currentScene, event);
}

static void menuScene_onExit(void* context) {
    App* this = (App*)context;

    MenuScene_handleSceneManagerExit((MenuScene*)this->currentScene);
    MenuScene_free((MenuScene*)this->currentScene);
    this->currentScene = NULL;

    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Empty);
    view_dispatcher_remove_view(this->viewDispatcher, AppView_Menu);
    MenuView_free(this->menuView);
    this->menuView = NULL;
}

static void setPatternLengthScene_onEnter(void* context) {
    App* this = (App*)context;

    this->numberInput = number_input_alloc();
    number_input_set_header_text(this->numberInput, "Pattern Length");
    number_input_set_result_callback(
        this->numberInput,
        numberInputCallback,
        this,
        PatternGen_getPattern(this->patternGen)->numSamples,
        0,
        PATTERN_GEN_MAX_SAMPLES);

    view_dispatcher_add_view(
        this->viewDispatcher, AppView_NumberInput, number_input_get_view(this->numberInput));
    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_NumberInput);
}

static bool setPatternLengthScene_onEvent(void* context, SceneManagerEvent event) {
    App* this = (App*)context;

    if(event.type != SceneManagerEventTypeCustom) {
        return false;
    }
    if(event.event != AppEvent_NumberInput) {
        return false;
    }

    PatternGen_resize(this->patternGen, this->numberInputResult);
    MainView_updateGrid(this->mainView);
    App_returnToMainScene(this);

    return true;
}

static void setPatternLengthScene_onExit(void* context) {
    App* this = (App*)context;

    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Empty);
    view_dispatcher_remove_view(this->viewDispatcher, AppView_NumberInput);
}

static void setFrequencyScene_onEnter(void* context) {
    App* this = (App*)context;

    this->numberInput = number_input_alloc();
    number_input_set_header_text(this->numberInput, "Frequency (Hz)");
    number_input_set_result_callback(
        this->numberInput, numberInputCallback, this, this->freq, MIN_FREQ, TICK_FREQ);

    view_dispatcher_add_view(
        this->viewDispatcher, AppView_NumberInput, number_input_get_view(this->numberInput));
    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_NumberInput);
}

static bool setFrequencyScene_onEvent(void* context, SceneManagerEvent event) {
    App* this = (App*)context;

    if(event.type != SceneManagerEventTypeCustom) {
        return false;
    }
    if(event.event != AppEvent_NumberInput) {
        return false;
    }

    this->freq = this->numberInputResult;
    view_dispatcher_set_tick_event_callback(this->viewDispatcher, onViewDispatcherTickEvent, 1);
    App_returnToMainScene(this);

    return true;
}

static void setFrequencyScene_onExit(void* context) {
    App* this = (App*)context;

    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Empty);
    view_dispatcher_remove_view(this->viewDispatcher, AppView_NumberInput);
}

static void scrollToScene_onEnter(void* context) {
    App* this = (App*)context;

    this->numberInput = number_input_alloc();
    number_input_set_header_text(this->numberInput, "Scroll To...");
    number_input_set_result_callback(
        this->numberInput,
        numberInputCallback,
        this,
        PatternGen_getIdx(this->patternGen),
        0,
        PatternGen_getPattern(this->patternGen)->numSamples);

    view_dispatcher_add_view(
        this->viewDispatcher, AppView_NumberInput, number_input_get_view(this->numberInput));
    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_NumberInput);
}

static bool scrollToScene_onEvent(void* context, SceneManagerEvent event) {
    App* this = (App*)context;

    if(event.type != SceneManagerEventTypeCustom) {
        return false;
    }
    if(event.event != AppEvent_NumberInput) {
        return false;
    }

    MainView_scrollTo(this->mainView, this->numberInputResult);
    App_returnToMainScene(this);

    return false;
}

static void scrollToScene_onExit(void* context) {
    App* this = (App*)context;

    view_dispatcher_switch_to_view(this->viewDispatcher, AppView_Empty);
    view_dispatcher_remove_view(this->viewDispatcher, AppView_NumberInput);
}

static const AppSceneOnEnterCallback onEnterHandlers[] = {
    mainIdleScene_onEnter,
    mainEditScene_onEnter,
    mainRunScene_onEnter,
    menuScene_onEnter,
    setPatternLengthScene_onEnter,
    setFrequencyScene_onEnter,
    scrollToScene_onEnter};

static const AppSceneOnEventCallback onEventHandlers[] = {
    mainIdleScene_onEvent,
    mainEditScene_onEvent,
    mainRunScene_onEvent,
    menuScene_onEvent,
    setPatternLengthScene_onEvent,
    setFrequencyScene_onEvent,
    scrollToScene_onEvent};

static const AppSceneOnExitCallback onExitHandlers[] = {
    mainIdleScene_onExit,
    mainEditScene_onExit,
    mainRunScene_onExit,
    menuScene_onExit,
    setPatternLengthScene_onExit,
    setFrequencyScene_onExit,
    scrollToScene_onExit};

static const SceneManagerHandlers sceneManagerHandlers = {
    .on_enter_handlers = onEnterHandlers,
    .on_event_handlers = onEventHandlers,
    .on_exit_handlers = onExitHandlers,
    .scene_num = AppScene_count};

static const uint32_t mainScenes[] = {AppScene_Main_Idle, AppScene_Main_Edit, AppScene_Main_Run};

App* App_alloc() {
    App* this = (App*)malloc(sizeof(App));
    furi_check(this != NULL);

    this->gui = furi_record_open(RECORD_GUI);

    this->viewDispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(this->viewDispatcher, this->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(this->viewDispatcher, this);
    view_dispatcher_set_custom_event_callback(this->viewDispatcher, onViewDispatcherCustomEvent);
    view_dispatcher_set_navigation_event_callback(
        this->viewDispatcher, onViewDispatcherNavigationEvent);
    view_dispatcher_set_tick_event_callback(this->viewDispatcher, onViewDispatcherTickEvent, 1);

    this->sceneManager = scene_manager_alloc(&sceneManagerHandlers, this);

    this->emptyScreen = empty_screen_alloc();
    view_dispatcher_add_view(
        this->viewDispatcher, AppView_Empty, empty_screen_get_view(this->emptyScreen));

    this->mainView = MainView_alloc(this);
    view_dispatcher_add_view(this->viewDispatcher, AppView_Main, MainView_getView(this->mainView));

    this->menuView = NULL;
    this->numberInput = NULL;
    this->currentScene = NULL;
    this->patternGen = NULL;

    this->numberInputResult = 0;
    this->freq = MIN_FREQ;

    return this;
}

void App_free(App* this) {
    furi_assert(this != NULL);

    view_dispatcher_remove_view(this->viewDispatcher, AppView_Main);
    MainView_free(this->mainView);

    view_dispatcher_remove_view(this->viewDispatcher, AppView_Empty);
    empty_screen_free(this->emptyScreen);

    scene_manager_free(this->sceneManager);
    view_dispatcher_free(this->viewDispatcher);
    furi_record_close(RECORD_GUI);

    free(this);
}

void App_run(App* this) {
    furi_assert(this != NULL);

    scene_manager_next_scene(this->sceneManager, AppScene_Main_Idle);

    view_dispatcher_run(this->viewDispatcher);
}

PatternGen* App_getPatternGen(App* this) {
    furi_assert(this != NULL);

    return this->patternGen;
}

void App_setPatternGen(App* this, PatternGen* patternGen) {
    furi_assert(this != NULL);

    this->patternGen = patternGen;
    MainView_setPattern(
        this->mainView, patternGen != NULL ? PatternGen_getPattern(patternGen) : NULL);
}

MainView* App_getMainView(App* this) {
    furi_assert(this != NULL);

    return this->mainView;
}

int32_t App_getFreq(App* this) {
    furi_assert(this != NULL);

    return this->freq;
}

void App_sendEvent(App* this, AppEvent event) {
    furi_assert(this != NULL);

    view_dispatcher_send_custom_event(this->viewDispatcher, event);
}

void App_enterMainEditScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_next_scene(this->sceneManager, AppScene_Main_Edit);
}

void App_enterMainRunScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_next_scene(this->sceneManager, AppScene_Main_Run);
}

void App_enterMenuScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_next_scene(this->sceneManager, AppScene_Menu);
}

void App_enterSetPatternLengthScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_next_scene(this->sceneManager, AppScene_SetPatternLength);
}

void App_enterSetFrequencyScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_next_scene(this->sceneManager, AppScene_SetFrequency);
}

void App_enterScrollToScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_next_scene(this->sceneManager, AppScene_ScrollTo);
}

void App_exitCurrentScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_previous_scene(this->sceneManager);
}

void App_returnToMainScene(App* this) {
    furi_assert(this != NULL);

    scene_manager_search_and_switch_to_previous_scene_one_of(
        this->sceneManager, mainScenes, sizeof(mainScenes) / sizeof(mainScenes[0]));
}
