//
//  MathClass.h
//  VWolf
//
//  Created by Enrique Ricalde on 9/18/24.
//

#pragma once

namespace VWolf {

    struct Color;

    struct Mathf {
    private:
        Mathf() = delete;
        Mathf(Mathf&) = delete;
        Mathf(const Mathf&) = delete;
        Mathf(Mathf&&) = delete;
        ~Mathf() = delete;
    public:
        static float PI;
        static float Deg2Rad;
        static float Rad2Deg;
        static float Infinity;
        static float NegativeInfinity;
        static float Epsilon;
    public:
        static float Abs(float t);
        static float Acos(float t);
        static bool Approximately(float a, float b);
        static float Asin(float t);
        static float Atan(float t);
        static float Atan2(float x, float y);
        static float Ceil(float t);
        static int CeilToInt(float t);
        static float Clamp(float value, float min, float max);
        static float Clamp01(float t);
        static int ClosestPowerOfTwo(int t);
        static Color CorrelatedColorTemperatureToRGB(float kelvin);
        static float Cos(float t);
        static float DeltaAngle(float current, float target);
        static float Exp(float power);
        static uint16_t FloatToHalf(float t);
        static float Floor(float t);
        static int FloorToInt(float t);
        static float GammaToLinearSpace(float t);
        static float HalfToFloat(uint16_t t);
        static float InverseLerp(float a, float b, float value);
        static bool IsPowerOfTwo(int t);
        static float Lerp(float a, float b, float t);
        static float LerpAngle(float a, float b, float t);
        static float LerpUnclamped(float a, float b, float t);
        static float LinearToGammaSpace(float t);
        static float Log(float f, float p);
        static float Log(float f);
        static float Log10(float t);
        static float Max(float a, float b);
        static float Max(const std::vector<float>& values);
        static float Min(float a, float b);
        static float Min(const std::vector<float>& values);
        static float MoveTowards(float current, float target, float maxDelta);
        static float MoveTowardsAngle(float current, float target, float maxDelta);
        static int NextPowerOfTwo(int t);
        static float PerlinNoise(float x, float y);
        static float PerlinNoise1D(float x);
        static float PingPong(float t, float length);
        static float Pow(float f, float p);
        static float Repeat(float t, float length);
        static float Round(float t);
        static int RoundToInt(float t);
        static float Sign(float t);
        static float Sin(float t);
        static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
        static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
        static float SmoothStep(float from, float to, float t);
        static float Sqrt(float t);
        static float Tan(float t);
    };
}
