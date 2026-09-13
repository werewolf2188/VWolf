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
    Component::Component(std::string name): Object(UUID::NewUUID()) {
        this->name = name;
    }
    Component::Component(std::string name, UUID id): Object(id) {
        this->name = name;
    }
    Component::~Component() {
        gameObject.reset();
    }

    Component& Component::operator=(const Component& light) {
        this->gameObject = light.gameObject;
        this->name = light.name;
        this->id = light.id;
        
        return *this;
    }

    void Component::SetGameObject(Weak<GameObject> gameObject) {
        this->gameObject = gameObject;
    }
}
