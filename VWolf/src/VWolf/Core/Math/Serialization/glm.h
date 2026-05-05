//
//  glm.h
//  VWolf
//
//  Created by Enrique Moises on 5/4/26.
//

#pragma once
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "yaml-cpp/yaml.h"

namespace YAML {
    template<glm::length_t L, typename T, glm::qualifier Q>
    bool decodeVector(const Node& node, glm::vec<L, T, Q>& rhs, glm::length_t size) {
        if (size < 0) return true;
        
        glm::length_t index = size - 1;
        if (index < 0) return true;
        rhs[index] = node[index].as<T>();
        return decodeVector(node, rhs, index);
    }

    template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    bool decodeMatrix(const Node& node, glm::mat<C, R, T, Q>& rhs) {
        
        for (glm::length_t index; index < C; index++) {
            const Node& nodeIndex = node[index];
            if (!nodeIndex.IsSequence() || node.size() != R) return false;
            decodeVector(nodeIndex, rhs[index], R);
        }
        return true;
    }
}

namespace glm {
    template<length_t L, typename T, qualifier Q>
    YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec<L, T, Q>& v) {
        
        out << YAML::Flow;
        out << YAML::BeginSeq;
        for (length_t index = 0; index < L; index++) {
            out << v[index];
        }
        out << YAML::EndSeq;
        return out;
    }

    template<length_t L, typename T, qualifier Q>
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec<L, T, Q>& v) {
        
        out << YAML::Flow;
        out << YAML::BeginSeq;
        for (length_t index = 0; index < L; index++) {
            out << v[index];
        }
        out << YAML::EndSeq;
        return out;
    }
}

#define CREATE_GLM_VECTOR_DECODER(T, sizeT) \
template<>\
struct convert<T>\
{\
    static bool decode(const Node& node, T& rhs)\
    {\
        if (!node.IsSequence() || node.size() != sizeT) return false; \
        if (rhs.length() > sizeT) return false; \
        else if (rhs.length() < sizeT) return false; \
        return decodeVector(node, rhs, sizeT);\
    }\
};

#define CREATE_GLM_MATRIX_DECODER(T, columnSize) \
template<> \
struct convert<T> \
{ \
    static bool decode(const Node& node, T& rhs) \
    { \
        if (!node.IsSequence() || node.size() != columnSize) return false; \
        return decodeMatrix(node, rhs); \
    } \
};

namespace YAML {
    CREATE_GLM_VECTOR_DECODER(glm::vec2, 2)
    CREATE_GLM_VECTOR_DECODER(glm::vec3, 3)
    CREATE_GLM_VECTOR_DECODER(glm::vec4, 4)

    CREATE_GLM_VECTOR_DECODER(glm::ivec2, 2)
    CREATE_GLM_VECTOR_DECODER(glm::ivec3, 3)

    template<>
    struct convert<glm::quat>
    {
        static bool decode(const Node& node, glm::quat& rhs)\
        {
            if (!node.IsSequence() || node.size() != 4) return false;

            rhs.w = node[0].as<float>();
            rhs.x = node[1].as<float>();
            rhs.y = node[2].as<float>();
            rhs.z = node[3].as<float>();
            return true;
        }
    };

    CREATE_GLM_MATRIX_DECODER(glm::mat4x4, 4)
    CREATE_GLM_MATRIX_DECODER(glm::mat3x3, 3)
}
