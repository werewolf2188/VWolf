//
//  SceneSettings.h
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/17/23.
//

#pragma once

#include "UICore.h"
#include "VWolf.h"

namespace VWolfPup {
    class SceneSettings: public View {
    public:
        SceneSettings(VWolf::Scene *scene);
        ~SceneSettings();
    public:
        void OnGui() override;
    private:
        VWolf::Scene *scene;
    };
}
