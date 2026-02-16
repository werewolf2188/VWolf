//
//  VMath.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 9/18/24.
//

#include "vwpch.h"
#include "MathClass.h"

#include "VWolf/Core/Color.h"

#include <glm/glm.hpp>

namespace VWolf {
    // MARK: Constants
    float Mathf::PI = M_PI;

    float Mathf::Rad2Deg = 360 / (M_PI * 2);

    float Mathf::Deg2Rad = (M_PI * 2) / 360;

    float Mathf::Infinity = std::numeric_limits<float>::max();

    float Mathf::NegativeInfinity = std::numeric_limits<float>::min();

    float Mathf::Epsilon = std::numeric_limits<float>::epsilon();

    // MARK: Static functions
    float Mathf::Abs(float t) {
        return std::abs(t);
    }

    float Mathf::Acos(float t) {
        return std::acos(t);
    }

    bool Mathf::Approximately(float a, float b) {
        return std::abs(a - b) <= Epsilon;
    }

    float Mathf::Asin(float t) {
        return std::asin(t);
    }

    float Mathf::Atan(float t) {
        return std::atan(t);
    }

    float Mathf::Atan2(float x, float y) {
        return std::atan2(x, y);
    }

    float Mathf::Ceil(float t) {
        return std::ceil(t);
    }

    int Mathf::CeilToInt(float t) {
        return static_cast<int>(Ceil(t));
    }

    float Mathf::Clamp(float value, float min, float max) {
        return Min(Max(value, min), max);
    }

    float Mathf::Clamp01(float t) {
        return Clamp(t, 0, 1);
    }

    int Mathf::ClosestPowerOfTwo(int t) {
        int power = 1;
        while (t >>= 1) power <<= 1;
        return power;
    }

    Color Mathf::CorrelatedColorTemperatureToRGB(float kelvin) {
        if (kelvin < 1000 || kelvin > 40000) throw std::logic_error("Incorrect value");

        float temperature = kelvin / 100;
        float r = 0, g = 0, b = 0;
        r = Clamp(temperature <= 66.0f ? 255.0f : (329.698727446f * Pow(temperature - 60, -0.1332047592f)), 0.0f, 255.0f);
        g = Clamp(temperature <= 66.0f ? (99.4708025861f * Log(temperature) - 161.1195681661f) : 288.1221695283f * Pow(temperature, -0.0755148492), 0.0f, 255.0f);
        b = Clamp(temperature >= 66.0f ? 255.0f : (temperature <= 19.0f ? 0 : 138.5177312231f * Log(temperature - 10.0f) - 305.0447927307f), 0.0f, 255.0f);
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0);
    }

    float Mathf::Cos(float t) {
        return std::cos(t);
    }

    float Mathf::DeltaAngle(float current, float target) {
        float result = Repeat((target - current), 360.0f);
        result = std::fmodf((result + 180.0f), 360.0f) - 180.0f;
        return result;
    }

    float Mathf::Exp(float power) {
        return std::exp(power);
    }

    uint16_t Mathf::FloatToHalf(float t) {
        uint32_t ia;
        std::memcpy(&ia, &t, sizeof(ia));
        uint16_t ir;

        ir = (ia >> 16) & 0x8000;
        if ((ia & 0x7f800000) == 0x7f800000) {
            if ((ia & 0x7fffffff) == 0x7f800000) {
                ir |= 0x7c00; /* infinity */
            } else {
                ir |= 0x7e00 | ((ia >> (24 - 11)) & 0x1ff); /* NaN, quietened */
            }
        } else if ((ia & 0x7f800000) >= 0x33000000) {
            int shift = (int)((ia >> 23) & 0xff) - 127;
            if (shift > 15) {
                ir |= 0x7c00; /* infinity */
            } else {
                ia = (ia & 0x007fffff) | 0x00800000; /* extract mantissa */
                if (shift < -14) { /* denormal */
                    ir |= ia >> (-1 - shift);
                    ia = ia << (32 - (-1 - shift));
                } else { /* normal */
                    ir |= ia >> (24 - 11);
                    ia = ia << (32 - (24 - 11));
                    ir = ir + ((14 + shift) << 10);
                }
                /* IEEE-754 round to nearest of even */
                if ((ia > 0x80000000) || ((ia == 0x80000000) && (ir & 1))) {
                    ir++;
                }
            }
        }
        return ir;
    }

    float Mathf::Floor(float t) {
        return std::floor(t);
    }

    int Mathf::FloorToInt(float t) {
        return static_cast<int>(Floor(t));
    }

    float Mathf::GammaToLinearSpace(float t) {
        return 255.0f * Pow(t / 255.0f, 2.2f);
    }

    float Mathf::HalfToFloat(uint16_t t) {
        uint32_t s = (t >> 15) & 0x00000001;
        uint32_t e = (t >> 10) & 0x0000001f;
        uint32_t m =  t        & 0x000003ff;
        uint32_t res = 0;

        if (e == 0)
        {
            if (m == 0)
            {
                //
                // Plus or minus zero
                //

                res = s << 31;
            }
            else
            {
                //
                // Denormalized number -- renormalize it
                //

                while (!(m & 0x00000400))
                {
                m <<= 1;
                e -=  1;
                }

                e += 1;
                m &= ~0x00000400;
            }
        }
        else if (e == 31)
        {
            if (m == 0)
            {
                //
                // Positive or negative infinity
                //

                res = (s << 31) | 0x7f800000;
            }
            else
            {
                //
                // Nan -- preserve sign and significand bits
                //

                res = (s << 31) | 0x7f800000 | (m << 13);
            }
        }

        //
        // Normalized number
        //

        e = e + (127 - 15);
        m = m << 13;

        //
        // Assemble s, e and m.
        //

        res = (s << 31) | (e << 23) | m;

        float ia;
        std::memcpy(&ia, &res, sizeof(ia));
        return ia;
    }

    float Mathf::InverseLerp(float a, float b, float value) {
        if (a != b)
            return Clamp01((value - a) / (b - a));
        else
            return 0.0f;
    }

    bool Mathf::IsPowerOfTwo(int t) {
        if (t <= 0) return false;
        uint32_t count = 0;

        int x = t;
        while (x > 0) {
            count += (x & 1) == 1 ? 1: 0;
            x >>= 1;
        }
        return count == 1;
    }

    float Mathf::Lerp(float a, float b, float t) {
        return (a + (b - a) * Clamp01(t));
    }

    float Mathf::LerpAngle(float a, float b, float t) {
        float delta = Repeat((b - a), 360);
        if (delta > 180)
            delta -= 360;
        return a + delta * Clamp01(t);
    }

    float Mathf::LerpUnclamped(float a, float b, float t) {
        return (a + (b - a) * t);
    }

    float Mathf::LinearToGammaSpace(float t) {
        float p = Pow(t, 1.0f / 2.2f);
        return p * 255.0f;
    }

    float Mathf::Log(float f, float p) {
        return std::log10(f) / std::log10(p);
    }

    float Mathf::Log(float f) {
        return std::log(f);
    }

    float Mathf::Log10(float t) {
        return std::log10(t);
    }

    float Mathf::Max(float a, float b) {
        return std::max(a, b);
    }

    float Mathf::Max(const std::vector<float>& values) {
        float current = 0;
        for (auto value: values)
            current = Max(current, value);
        return current;
    }

    float Mathf::Min(float a, float b) {
        return std::min(a, b);
    }

    float Mathf::Min(const std::vector<float>& values) {
        float current = 0;
        for (auto value: values)
            current = Min(current, value);
        return current;
    }

    float Mathf::MoveTowards(float current, float target, float maxDelta) {
        if (Abs(target - current) <= maxDelta)
            return target;
        return current + Sign(target - current) * maxDelta;
    }
    float Mathf::MoveTowardsAngle(float current, float target, float maxDelta) {
        float deltaAngle = DeltaAngle(current, target);
        if (-maxDelta < deltaAngle && deltaAngle < maxDelta)
            return target;
        target = current + deltaAngle;
        return MoveTowards(current, target, maxDelta);
    }

    int Mathf::NextPowerOfTwo(int t) {
        int x = t;
        int power = 2;
        x--;    // <<-- UPDATED
        while (x >>= 1) power <<= 1;
        return power;
//        return std::pow(2, std::ceil(std::log(t)/std::log(2)));
    }

    float Mathf::PerlinNoise(float x, float y) {
        int numOctaves = 7;
        int primeIndex = 0;

        double persistence = 0.5;

        int primes[10][3] = {
          { 995615039, 600173719, 701464987 },
          { 831731269, 162318869, 136250887 },
          { 174329291, 946737083, 245679977 },
          { 362489573, 795918041, 350777237 },
          { 457025711, 880830799, 909678923 },
          { 787070341, 177340217, 593320781 },
          { 405493717, 291031019, 391950901 },
          { 458904767, 676625681, 424452397 },
          { 531736441, 939683957, 810651871 },
          { 997169939, 842027887, 423882827 }
        };

        std::function<double(int, int, int)> Noise = [primes] (int i, int x, int y) {
          int n = x + y * 57;
          n = (n << 13) ^ n;
          int a = primes[i][0], b = primes[i][1], c = primes[i][2];
          int t = (n * (n * n * a + b) + c) & 0x7fffffff;
          return 1.0 - (double)(t)/1073741824.0;
        };

        std::function<double(int, int, int)> SmoothedNoise = [Noise](int i, int x, int y) {
          double corners = (Noise(i, x-1, y-1) + Noise(i, x+1, y-1) +
                            Noise(i, x-1, y+1) + Noise(i, x+1, y+1)) / 16,
                 sides = (Noise(i, x-1, y) + Noise(i, x+1, y) + Noise(i, x, y-1) +
                          Noise(i, x, y+1)) / 8,
                 center = Noise(i, x, y) / 4;
          return corners + sides + center;
        };

        std::function<double(double, double, double)> Interpolate = [](double a, double b, double x) {  // cosine interpolation
          double ft = x * 3.1415927,
                 f = (1 - cos(ft)) * 0.5;
          return  a*(1-f) + b*f;
        };

        std::function<double(int, double, double)> InterpolatedNoise = [SmoothedNoise, Interpolate](int i, double x, double y) {
          int integer_X = x;
          double fractional_X = x - integer_X;
          int integer_Y = y;
          double fractional_Y = y - integer_Y;

          double v1 = SmoothedNoise(i, integer_X, integer_Y),
                 v2 = SmoothedNoise(i, integer_X + 1, integer_Y),
                 v3 = SmoothedNoise(i, integer_X, integer_Y + 1),
                 v4 = SmoothedNoise(i, integer_X + 1, integer_Y + 1),
                 i1 = Interpolate(v1, v2, fractional_X),
                 i2 = Interpolate(v3, v4, fractional_X);
          return Interpolate(i1, i2, fractional_Y);
        };

        double total = 0,
               frequency = pow(2, numOctaves),
               amplitude = 1;
        for (int i = 0; i < numOctaves; ++i) {
            frequency /= 2;
            amplitude *= persistence;
            total += InterpolatedNoise((primeIndex + i) % 10, x / frequency, y / frequency) * amplitude;
        }
        return total / frequency;
    }

    float Mathf::PerlinNoise1D(float x) {
        return PerlinNoise(x, 0);
    }

    float Mathf::PingPong(float t, float length) {
        t = Repeat(t, length * 2.0f);
        return length - Abs(t - length);
    }

    float Mathf::Pow(float f, float p) {
        return std::pow(f, p);
    }

    float Mathf::Repeat(float t, float length) {
        return Clamp(t - Floor(t / length) * length, 0.0f, length);
    }

    float Mathf::Round(float t) {
        return std::round(t);
    }
    int Mathf::RoundToInt(float t) {
        return static_cast<int>(Round(t));
    }

    float Mathf::Sign(float t) {
        return std::signbit(t) ? -1: 1;
    }

    float Mathf::Sin(float t) {
        return std::sin(t);
    }

    float Mathf::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        smoothTime = Max(0.0001F, smoothTime);
        float omega = 2.0f / smoothTime;

        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48F * x * x + 0.235F * x * x * x);
        float change = current - target;
        float originalTo = target;

        // Clamp maximum speed
        float maxChange = maxSpeed * smoothTime;
        change = Clamp(change, -maxChange, maxChange);
        target = current - change;

        float temp = (currentVelocity + omega * change) * deltaTime;
        currentVelocity = (currentVelocity - omega * temp) * exp;
        float output = target + (change + temp) * exp;

        // Prevent overshooting
        if (originalTo - current > 0.0F == output > originalTo)
        {
            output = originalTo;
            currentVelocity = (output - originalTo) / deltaTime;
        }

        return output;
    }
    
    float Mathf::SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        target = current + DeltaAngle(current, target);
        return SmoothDamp(current, target, currentVelocity, smoothTime, maxSpeed, deltaTime);
    }
    
    float Mathf::SmoothStep(float from, float to, float t) {
        t = Clamp01(t);
        t = -2.0f * t * t * t + 3.0f * t * t;
        return to * t + from * (1.0f - t);
    }

    float Mathf::Sqrt(float t) {
        return std::sqrt(t);
    }

    float Mathf::Tan(float t) {
        return std::tan(t);
    }
}
