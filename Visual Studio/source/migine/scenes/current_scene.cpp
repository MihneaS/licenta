#include "current_scene.h"
#include <migine/scenes/all_scenes.h>

namespace migine {
    Scene_base& migine::get_current_scene() {
        static Scene_06 instance;
        return instance;
        // // O: insert return statement here
    }
}
