//
//  BaseComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "vwpch.h"
#include "GameObject.h"
#include "BaseComponent.h"

namespace VWolf {
    Component::Component(std::string name): Object(UUID::NewUUID()), name(name) {}
    Component::Component(std::string name, UUID id): Object(id), name(name) {
    }
    Component::~Component() {}
}
