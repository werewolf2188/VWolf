//
//  MathClass.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 9/19/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>
#include <limits>

BOOST_AUTO_TEST_CASE(MathfConstants) {
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::PI, 3.14159265358979323846264338327950288));
    BOOST_TEST(VWolf::Mathf::Epsilon == std::numeric_limits<float>::epsilon());
    BOOST_TEST(VWolf::Mathf::Infinity == std::numeric_limits<float>::max());
    BOOST_TEST(VWolf::Mathf::NegativeInfinity == std::numeric_limits<float>::min());
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Rad2Deg, 360.0f / (3.14159265358979 * 2)));
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Deg2Rad, (3.14159265358979 * 2) / 360.0f));
}

BOOST_AUTO_TEST_CASE(MathfStaticFunctions) {
    BOOST_TEST(VWolf::Mathf::Abs(-10.0f) == 10.0f);
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Acos(0.5f), 1.04719758f));
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Asin(0.5f), 0.52359879f));
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Atan(0.5f), 0.463647604f));
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Atan2(0.5f, 0.5f), 0.785398185f));
    BOOST_TEST(VWolf::Mathf::Ceil(0.52359879f) == 1);
    BOOST_TEST(VWolf::Mathf::CeilToInt(0.52359879f) == 1);
    BOOST_TEST(VWolf::Mathf::Clamp(0.52359879f, 0, 2) == 0.52359879f);
    BOOST_TEST(VWolf::Mathf::Clamp(-0.52359879f, 0, 2) == 0);
    BOOST_TEST(VWolf::Mathf::Clamp(2.52359879f, 0, 2) == 2);
    BOOST_TEST(VWolf::Mathf::Clamp01(0.52359879f) == 0.52359879f);
    BOOST_TEST(VWolf::Mathf::Clamp01(-0.52359879f) == 0);
    BOOST_TEST(VWolf::Mathf::Clamp01(2.52359879f) == 1);
    BOOST_TEST(VWolf::Mathf::ClosestPowerOfTwo(7) == 4);
    BOOST_TEST(VWolf::Mathf::ClosestPowerOfTwo(19) == 16);
    BOOST_TEST(VWolf::Mathf::CorrelatedColorTemperatureToRGB(16700) == VWolf::Color(0.6938307216f, 0.767694451f, 1, 1));
    BOOST_TEST(VWolf::Mathf::Cos(0) == 1);
    BOOST_TEST(VWolf::Mathf::DeltaAngle(1080, 90) == 90);
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Exp(6), 403.428802f));
    BOOST_TEST(VWolf::Mathf::FloatToHalf(0.10304) == 11928);
    BOOST_TEST(VWolf::Mathf::Floor(0.52359879f) == 0);
    BOOST_TEST(VWolf::Mathf::FloorToInt(0.52359879f) == 0);
    BOOST_TEST(VWolf::Mathf::GammaToLinearSpace(190) == 133.478012f);
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::HalfToFloat(11928), 0.103027344f));
    BOOST_TEST(VWolf::Mathf::InverseLerp(20, 40, 22) == 0.1f);
    BOOST_TEST(!VWolf::Mathf::IsPowerOfTwo(7));
    BOOST_TEST(VWolf::Mathf::IsPowerOfTwo(32));
    BOOST_TEST(VWolf::Mathf::Lerp(20, 40, 0.5f) == 30);
    BOOST_TEST(VWolf::Mathf::LerpAngle(20, 40, 0.5f) == 30);
    BOOST_TEST(VWolf::Mathf::LerpUnclamped(20, 40, 2.5f) == 70);
    BOOST_TEST(VWolf::Mathf::LinearToGammaSpace(0.1f) == 89.5353928f);
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Log(6, 2), 2.58496237f));
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Log(100), 4.60517025f));
    BOOST_TEST(VWolf::Mathf::Min(6, 2) == 2);
    BOOST_TEST(VWolf::Mathf::Min({ 6, 2, 10, -2 }) == -2);
    BOOST_TEST(VWolf::Mathf::Max(6, 2) == 6);
    BOOST_TEST(VWolf::Mathf::Max({ 6, 2, 10, -2 }) == 10);
    BOOST_TEST(VWolf::Mathf::MoveTowards(20, 40, 0.5f) == 20.5f);
    BOOST_TEST(VWolf::Mathf::MoveTowardsAngle(20, 40, 0.5f) == 20.5f);
    BOOST_TEST(VWolf::Mathf::NextPowerOfTwo(7) == 8);
    BOOST_TEST(VWolf::Mathf::NextPowerOfTwo(19) == 32);
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::PerlinNoise(6, 2), 0.164425731f));
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::PerlinNoise1D(6), 0.15625377f));
    BOOST_TEST(VWolf::Mathf::PingPong(2, 5) == 2);
    BOOST_TEST(VWolf::Mathf::PingPong(7, 5) == 3);
    BOOST_TEST(VWolf::Mathf::PingPong(11, 5) == 1);
    BOOST_TEST(VWolf::Mathf::Pow(2, 3) == 8);
    BOOST_TEST(VWolf::Mathf::Repeat(-1, 5) == 4);
    BOOST_TEST(VWolf::Mathf::Repeat(-1, 5) == 4);
    BOOST_TEST(VWolf::Mathf::Repeat(0, 5) == 0);
    BOOST_TEST(VWolf::Mathf::Repeat(1, 5) == 1);
    BOOST_TEST(VWolf::Mathf::Round(0.52359879f) == 1);
    BOOST_TEST(VWolf::Mathf::RoundToInt(0.52359879f) == 1);
    BOOST_TEST(VWolf::Mathf::Sign(-10) == -1);
    BOOST_TEST(VWolf::Mathf::Sign(10) == 1);
    BOOST_TEST(VWolf::Mathf::Sin(0) == 0);
    BOOST_TEST(VWolf::Mathf::Sqrt(9) == 3);
    float yVelocity = 0.0f;
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::SmoothDamp(20, 40, yVelocity, 10.f, 0.3f, 0.12442f), 20.0008793f));
    yVelocity = 0.0f;
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::SmoothDampAngle(20, 40, yVelocity, 10.f, 0.3f, 0.12442f), 20.0008793f));
    BOOST_TEST(VWolf::Mathf::SmoothStep(20, 40, 0.5f) == 30);
    BOOST_TEST(VWolf::Mathf::Approximately(VWolf::Mathf::Tan(0.5f), 0.546302497f));
}
