//
//  GenericSerialization.h
//  VWolf
//
//  Created by Enrique Moises on 4/24/26.
//

#pragma once

#include "yaml-cpp/yaml.h"
#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/preprocessor.hpp>
#include <boost/algorithm/string.hpp>

#define VWOLF_SERIALIZATION_FRIENDS(T) \
friend YAML::Emitter& operator<<(YAML::Emitter& out, T& v);\
friend YAML::Emitter& operator<<(YAML::Emitter& out, const T& v);\
friend YAML::convert<T>;

#define VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(T)\
template<>\
struct convert<T> {\
    static bool decode(const Node& node, T& rhs)\
    {\
        return DeserializeFromBoostDescribe(node, rhs);\
    }\
};

#define VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER(T)\
YAML::Emitter& operator<<(YAML::Emitter& out, T& v) { \
    return SerializeFromBoostDescribe(out, v, ToLower(#T));\
}\
\
YAML::Emitter& operator<<(YAML::Emitter& out, const T& v) { \
    return SerializeFromBoostDescribe(out, v, ToLower(#T));\
}

#define VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(T, name)\
YAML::Emitter& operator<<(YAML::Emitter& out, T& v) { \
    return SerializeFromBoostDescribe(out, v, name);\
}\
\
YAML::Emitter& operator<<(YAML::Emitter& out, const T& v) { \
    return SerializeFromBoostDescribe(out, v, name);\
}

#define VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(T)\
YAML::Emitter& operator<<(YAML::Emitter& out, T& v) { \
    return SerializeFromBoostDescribeNoName(out, v);\
}\
\
YAML::Emitter& operator<<(YAML::Emitter& out, const T& v) { \
    return SerializeFromBoostDescribeNoName(out, v);\
}

// 1. The operation to apply to each argument
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER_CONDITIONAL_EXPAND(T, x) if (val == #x) { rhs = T::x; return true; }

#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER_CONDITIONAL_EXPAND(T, x) if (val == T::x) { out << #x; }

// 2. Helper to select the correct iteration macro
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_GET_MACRO(T, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, NAME, ...) NAME

// 3. Chain of macros for different counts
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_1(T, op, x)      op(T, x)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_2(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_1(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_3(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_2(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_4(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_3(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_5(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_4(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_6(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_5(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_7(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_6(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_8(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_7(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_9(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_8(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_10(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_9(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_11(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_10(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_12(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_11(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_13(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_12(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_14(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_13(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_15(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_14(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_16(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_15(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_17(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_16(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_18(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_17(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_19(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_18(T, op, __VA_ARGS__)
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_20(T, op, x, ...) op(T, x) VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_19(T, op, __VA_ARGS__)

// 4. Final FOR_EACH macro
#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH(T, op, ...) \
VWOLF_CREATE_CONVERT_GENERIC_ENUM_GET_MACRO(T, __VA_ARGS__, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_20, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_19, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_18, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_17, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_16, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_15, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_14, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_13, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_12, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_11, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_10, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_9, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_8, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_7, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_6, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_5, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_4, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_3, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_2, VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH_1)(T, op, __VA_ARGS__)

#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_LOOP_DECODER(T, ...)\
VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH(T, VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER_CONDITIONAL_EXPAND, __VA_ARGS__)

#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_LOOP_ENCODER(T, ...)\
VWOLF_CREATE_CONVERT_GENERIC_ENUM_FOR_EACH(T, VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER_CONDITIONAL_EXPAND, __VA_ARGS__)

#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(T, ...)\
template<>\
struct convert<T> {\
    static bool decode(const Node& node, T& rhs)\
    {\
\
        if (!node.IsScalar()) return false;\
\
        std::string val = node.as<std::string>();\
        VWOLF_CREATE_CONVERT_GENERIC_ENUM_LOOP_DECODER(T, __VA_ARGS__)\
        return false;\
    }\
};

#define VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(T, ...)\
YAML::Emitter& operator<<(YAML::Emitter& out, T& val) { \
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_LOOP_ENCODER(T, __VA_ARGS__)\
    return out;\
}\
\
YAML::Emitter& operator<<(YAML::Emitter& out, const T& val) { \
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_LOOP_ENCODER(T, __VA_ARGS__)\
    return out;\
}

namespace YAML {
    template<>
    struct convert<std::filesystem::path> {
        static bool decode(const Node& node, std::filesystem::path& rhs) {
            rhs = node.as<std::string>();
            return true;
        }
    };
}

namespace std::filesystem {
    static YAML::Emitter& operator<<(YAML::Emitter& out, std::filesystem::path& v) {
        out << v.string();
        return out;
    }
}

namespace VWolf {
    template<class T,
    class Md = boost::describe::describe_members<T,boost::describe::mod_any_access>>
    bool DeserializeFromBoostDescribe(const YAML::Node& node, T& rhs) {
        boost::mp11::mp_for_each<Md>([&](auto D){
            using propertType = std::remove_reference_t<decltype(rhs.*D.pointer)>;
            if (node[D.name]) {
                rhs.*D.pointer = node[D.name].template as<propertType>();
            }
        });
        return true;
    }

    template<class T,
    class Md = boost::describe::describe_members<T,boost::describe::mod_any_access>>
    YAML::Emitter& SerializeFromBoostDescribeNoName(YAML::Emitter& out, const T& v)
    {
        out << YAML::BeginMap;
        boost::mp11::mp_for_each<Md>([&](auto D){
            using propertType = std::remove_reference_t<decltype(v.*D.pointer)>;
            out << YAML::Key << D.name << YAML::Value << v.*D.pointer;
        });
        out << YAML::EndMap;
        return out;
    }

    template<class T,
    class Md = boost::describe::describe_members<T,boost::describe::mod_any_access>>
    YAML::Emitter& SerializeFromBoostDescribe(YAML::Emitter& out, T& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << typeid(T).name();
        SerializeFromBoostDescribeNoName(out, v);
        out << YAML::EndMap;
        return out;
    }

    template<class T,
    class Md = boost::describe::describe_members<T,boost::describe::mod_any_access>>
    YAML::Emitter& SerializeFromBoostDescribe(YAML::Emitter& out, const T& v, std::string name)
    {
        out << YAML::BeginMap;
        out << YAML::Key << name;
        SerializeFromBoostDescribeNoName(out, v);
        out << YAML::EndMap;
        return out;
    }

    static std::string ToLower(std::string name) {
        return boost::algorithm::to_lower_copy(name);
    }
}
