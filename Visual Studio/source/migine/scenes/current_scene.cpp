#include "current_scene.h"

#include "Scene_01.h"
#include "Scene_02.h"
#include "Scene_03.h"
#include "Scene_04.h"
#include "Scene_05.h"
#include "Scene_06.h"
#include "Scene_07.h"
#include "Scene_08.h"

namespace migine {
    Scene_base& migine::get_current_scene() {
        static Scene_02 instance;
        return instance;
    }
}
