#include "current_scene.h"

#include "Scene_01.h"
#include "Scene_02.h"
#include "Scene_03.h"
#include "Scene_04.h"
#include "Scene_05.h"
#include "Scene_06.h"
#include "Scene_07.h"
#include "Scene_08.h"
#include "Scene_09.h"
#include "Scene_10.h"
#include "Scene_11.h"
#include "Scene_12.h"
#include "Scene_13.h"
#include "Scene_14.h"
#include "Scene_15.h"
#include "Scene_16.h"
#include "Scene_17.h"

namespace migine {
    Scene_base& migine::get_current_scene() {
        static Scene_02 instance;
        return instance;
    }
}
