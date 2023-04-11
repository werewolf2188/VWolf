//
//  SceneHierarchy.h
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/13/23.
//

#pragma once

#include "UICore.h"
#include "VWolf.h"

#include <functional>

namespace VWolfPup {
    class SceneHierarchy: public View {
    public:
        SceneHierarchy(VWolf::Scene *scene, std::function<void(VWolf::Ref<VWolf::GameObject>)> onTapped);
        ~SceneHierarchy();
    public:
        void OnGui() override;
    private:
        VWolf::Scene *scene;
        std::function<void(VWolf::Ref<VWolf::GameObject>)> onTapped;
    };
}
