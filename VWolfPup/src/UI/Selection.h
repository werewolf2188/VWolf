//
//  Selection.h
//  VWolfPup
//
//  Created by Enrique Moises on 7/10/26.
//

#pragma once

#include <string>
#include "VWolf.h"

namespace VWolfPup {
    class Selection {
    public:
        static VWolf::Ref<VWolf::Object> GetContext() { return _context; }
        static void SetContext(VWolf::Ref<VWolf::Object> context) { _context = context; }
    public:
        static VWolf::UUID GetActiveId();
        static VWolf::Ref<VWolf::GameObject> GetActiveGameObject();
    private:
        static VWolf::Ref<VWolf::Object> _context;
    };
}
