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
        void SetScene(VWolf::Scene* scene) {
            this->scene = scene;
            selection = (int)scene->GetSceneBackground().GetType();
        }
    public:
        void OnGui() override;
    protected:
        virtual void SetInContainer() override;
    private:
        VWolf::Scene *scene;
        std::array<const char*, 2> items = { "Color", "Skybox" };
        int selection = 0;
    };
}
