//
//  Physics.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/1/24.
//

#pragma once

#ifdef VWOLF_CORE
#include <reactphysics3d/reactphysics3d.h>
#endif

namespace VWolf {
    class Physics {
    public:
#ifdef VWOLF_CORE
        static inline reactphysics3d::PhysicsCommon& GetCommon() { return common; }
        static inline reactphysics3d::decimal GetTimeStep() { return timeStep; }
#endif
        static void SetTimeStep(float _timeStep);
    private:
#ifdef VWOLF_CORE
        static reactphysics3d::decimal timeStep;
        static reactphysics3d::PhysicsCommon common;
#endif
    };
}
