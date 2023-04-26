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
        void SetScene(VWolf::Scene* scene) { this->scene = scene; }
    public:
        void OnGui() override;
        void OnEvent(VWolf::Event& evt);
    private:
        bool OnMouseButtonReleasedEvent(VWolf::MouseButtonReleasedEvent& e);
    private:
        bool showDialog = false;
        VWolf::Scene *scene;
        std::function<void(VWolf::Ref<VWolf::GameObject>)> onTapped;
    };
}
