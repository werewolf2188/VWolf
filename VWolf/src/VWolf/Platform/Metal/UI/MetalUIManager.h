//
//  MetalUIManager.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#include "VWolf/Core/UI/UIManager.h"

namespace VWolf {
    class MetalUIManager: public UIManager {
    public:
        MetalUIManager();

        virtual void Initialize() override;
        virtual void Terminate() override;
        virtual void Render() override;
        virtual void NewFrame() override;
        virtual bool OnEvent(Event& evt) override;
    };
}
