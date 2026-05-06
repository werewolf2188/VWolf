//
//  Core.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/23/23.
//

#pragma once

#define VWOLF_VMATH_SERIALIZATION_FRIENDS(T) \
friend YAML::Emitter& operator<<(YAML::Emitter& out, T& v);\
friend YAML::Emitter& operator<<(YAML::Emitter& out, const T& v);\
friend YAML::convert<T>;

#define VWOLF_VMATH_SERIALIZATION_VECTOR_DECODER(T, glmT, Size) \
    template<> \
    struct convert<T> { \
        static bool decode(const Node& node, T& rhs) \
        { \
            rhs._vector##Size = node.as<glmT>(); \
            return true; \
        } \
    };

#define VWOLF_VMATH_SERIALIZATION_MATRIX_DECODER(T, glmT, columnSize, rowSize) \
    template<> \
    struct convert<T> { \
        static bool decode(const Node& node, T& rhs) \
        { \
            rhs._matrix##columnSize##x##rowSize = node.as<glmT>(); \
            return true; \
        } \
    };

#define VWOLF_VMATH_SERIALIZATION_VECTOR_EMITTER(T, Size) \
    YAML::Emitter& operator<<(YAML::Emitter& out, T& v) { \
        out << v._vector##Size; \
        return out; \
    } \
\
    YAML::Emitter& operator<<(YAML::Emitter& out, const T& v) { \
        out << v._vector##Size; \
        return out; \
    }

#define VWOLF_VMATH_SERIALIZATION_MATRIX_EMITTER(T, columnSize, rowSize) \
    YAML::Emitter& operator<<(YAML::Emitter& out, T& v) {\
        out << YAML::Block;\
        out << YAML::BeginSeq;\
        for (int index = 0; index < columnSize; index ++)\
            out << v._matrix##columnSize##x##rowSize[index];\
        out << YAML::EndSeq;\
        return out;\
    } \
\
    YAML::Emitter& operator<<(YAML::Emitter& out, const T& v) {\
        out << YAML::Block;\
        out << YAML::BeginSeq;\
        for (int index = 0; index < columnSize; index ++)\
            out << v._matrix##columnSize##x##rowSize[index];\
        out << YAML::EndSeq;\
        return out;\
    }
