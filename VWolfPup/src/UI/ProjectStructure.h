//
//  ProjectStructure.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/16/23.
//

#pragma once

#include "UICore.h"
#include "VWolf.h"

#include <mutex>

namespace VWolfPup {
    class ProjectTree;

    class ProjectStructure: public View {
    public:
        ProjectStructure();
        ~ProjectStructure();
    public:
        void OnGui() override;
        void SetInContainer() override;
    private:
        void Rebuild();
    private:
        float startingWidth = 200.0f;
        VWolf::Ref<ProjectTree> projectTree;

        std::mutex m_MainThreadQueueMutex;
    };
}
