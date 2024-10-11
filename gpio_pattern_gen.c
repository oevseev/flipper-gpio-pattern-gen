#include "app/app.h"
#include "gpio/pin_controller.h"
#include "pattern_gen/pattern_gen.h"

int32_t gpio_pattern_gen_app(void* p) {
    UNUSED(p);

    PinController_init();
    PatternGen* patternGen = PatternGen_alloc();

    App* app = App_alloc();
    App_setPatternGen(app, patternGen);
    App_run(app);

    App_free(app);
    PatternGen_free(patternGen);

    return 0;
}
