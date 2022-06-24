#include "current_scene.h"
#include "all_scenes.h"

namespace migine {
    Scene_base& migine::get_current_scene() {
        static Scene_21 instance;
        return instance;
    }
}
