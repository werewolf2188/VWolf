//
//  Core.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/23/23.
//

#pragma once

#include "VWolf/Core/Math/Vector2.h"
#include "VWolf/Core/Math/Vector3.h"
#include "VWolf/Core/Math/Vector4.h"

namespace YAML {
    template<>
    struct convert<VWolf::Color>
    {
        static bool decode(const Node& node, VWolf::Color& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.SetR(node[0].as<float>());
            rhs.SetG(node[1].as<float>());
            rhs.SetB(node[2].as<float>());
            rhs.SetA(node[3].as<float>());
            return true;
        }
    };

    template<>
    struct convert<VWolf::Vector2> {
        static bool decode(const Node& node, VWolf::Vector2& rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            rhs.SetX(node[0].as<float>());
            rhs.SetY(node[1].as<float>());
            return true;
        }
    };

    template<>
    struct convert<VWolf::Vector3> {
        static bool decode(const Node& node, VWolf::Vector3& rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;
            rhs.SetX(node[0].as<float>());
            rhs.SetY(node[1].as<float>());
            rhs.SetZ(node[2].as<float>());
            return true;
        }
    };

    template<>
    struct convert<VWolf::Vector4> {
        static bool decode(const Node& node, VWolf::Vector4& rhs) {
            if (!node.IsSequence() || node.size() != 4)
                return false;
            rhs.SetX(node[0].as<float>());
            rhs.SetY(node[1].as<float>());
            rhs.SetZ(node[2].as<float>());
            rhs.SetW(node[3].as<float>());
            return true;
        }
    };

}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Color& v);

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector2& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector3& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector4& v);
}
