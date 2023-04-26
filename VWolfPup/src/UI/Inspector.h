//
//  Inspector.h
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "UICore.h"
#include "VWolf.h"

#include <functional>

namespace VWolfPup {
    class Inspector: public View {
    public:
        Inspector();
        ~Inspector();
    public:
        void OnGui() override;
    public:
        void SetGameObject(VWolf::Ref<VWolf::GameObject> gameObject);
    private:
        void DrawMaterial();
        void DrawComponentList();
    private:
        VWolf::Ref<VWolf::GameObject> gameObject;
    };
}
