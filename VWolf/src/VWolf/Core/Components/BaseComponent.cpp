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
    Component::Component(std::string name): name(name) {}
    Component::~Component() {}
}
