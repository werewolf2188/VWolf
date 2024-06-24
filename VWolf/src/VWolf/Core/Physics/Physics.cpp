//
//  Physics.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/1/24.
//

#include "vwpch.h"
#include "Physics.h"

namespace VWolf {
    reactphysics3d::PhysicsCommon Physics::common;

    reactphysics3d::decimal Physics::timeStep = 1.0f / 60.0f;

    void Physics::SetTimeStep(float _timeStep) {
        timeStep = _timeStep;
    }
}
