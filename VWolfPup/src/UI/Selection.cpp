//
//  Selection.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 7/10/26.
//

#include "Selection.h"

namespace VWolfPup {

    VWolf::Ref<VWolf::Object> Selection::_context = nullptr;

    VWolf::UUID Selection::GetActiveId() {
        if (_context == nullptr) return VWolf::UUID::Empty;
        return _context->GetID();
    }

    VWolf::Ref<VWolf::GameObject> Selection::GetActiveGameObject() {
        if (_context == nullptr) return nullptr;
        
        VWolf::Ref<VWolf::GameObject> result = std::dynamic_pointer_cast<VWolf::GameObject>(_context);
        return result;
    }
}
