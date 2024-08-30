//
//  Vector4.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/22/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(Vector4Initializer)
{
    // Given

    // When
    VWolf::Vector4 vector4;

    // Then
    BOOST_TEST(vector4.x == 0);
    BOOST_TEST(vector4.y == 0);
    BOOST_TEST(vector4.z == 0);
    BOOST_TEST(vector4.w == 0);

    // When
    vector4 = VWolf::Vector4(10.5f, 10.5f, 10.5f, 10.5f);

    BOOST_TEST(vector4.x == 10.5f);
    BOOST_TEST(vector4.y == 10.5f);
    BOOST_TEST(vector4.z == 10.5f);
    BOOST_TEST(vector4.w == 10.5f);

    // When
    VWolf::Vector4 otherVector2(vector4);

    // Then
    BOOST_TEST(otherVector2.x == 10.5f);
    BOOST_TEST(otherVector2.y == 10.5f);
    BOOST_TEST(otherVector2.z == 10.5f);
    BOOST_TEST(otherVector2.w == 10.5f);

    // When
    VWolf::Vector4 moveVector2(std::move(vector4));

    // Then
    BOOST_TEST(moveVector2.x == 10.5f);
    BOOST_TEST(moveVector2.y == 10.5f);
    BOOST_TEST(moveVector2.z == 10.5f);
    BOOST_TEST(moveVector2.w == 10.5f);
    BOOST_TEST(vector4.x == 0);
    BOOST_TEST(vector4.y == 0);
    BOOST_TEST(vector4.z == 0);
    BOOST_TEST(vector4.w == 0);

//    otherVector2 = std::move(moveVector2);
//
//    BOOST_TEST(otherVector2.x == 10.5f);
//    BOOST_TEST(otherVector2.y == 10.5f);
//    BOOST_TEST(moveVector2.x == 0);
//    BOOST_TEST(moveVector2.y == 0);
}

BOOST_AUTO_TEST_CASE(Vector4Comparison) {
    // Given
    VWolf::Vector4 vector(10, 10, 10, 10);
    VWolf::Vector4 vector2(10, 10, 10, 10);
    VWolf::Vector4 vector3(20, 10, 10, 20);
    VWolf::Vector4 vector4(10, 20, 10, 20);
    VWolf::Vector4 vector5(30, 30, 30, 30);

    // When
    bool isSame = vector == vector2;

    // Then
    BOOST_TEST(isSame == true);

    // When
    isSame = vector == vector3;

    // Then
    BOOST_TEST(isSame == false);

    // When
    isSame = vector == vector4;

    // Then
    BOOST_TEST(isSame == false);

    // When
    bool isDifferent = vector != vector5;

    // Then
    BOOST_TEST(isDifferent == true);

    // When
    isDifferent = vector != vector3;

    // Then
    BOOST_TEST(isDifferent == true);

    // When
    isDifferent = vector != vector4;

    // Then
    BOOST_TEST(isDifferent == true);

    // When
    isDifferent = vector != vector2;

    // Then
    BOOST_TEST(isDifferent == false);

    // When
    isSame = vector == VWolf::Vector4(10, 10, 10, 10);

    // When
    BOOST_TEST(isSame == true);

    // Then
    isDifferent = vector !=  VWolf::Vector4(12, 12, 12, 12);

    // When
    BOOST_TEST(isDifferent == true);

    // When
    BOOST_TEST(vector == VWolf::Vector4(10, 10, 10, 10));
    BOOST_TEST(vector != VWolf::Vector4(12, 12, 12, 12));
}

BOOST_AUTO_TEST_CASE(Vector4Operations) {
    // Given
    VWolf::Vector4 vector(10, 10, 10, 10);
    VWolf::Vector4 vector2(14, 30, 84, 200);

    // When
    VWolf::Vector4 result = vector + vector2;

    // Then
    BOOST_TEST(result == VWolf::Vector4(24, 40, 94, 210));

    // When
    result = vector - vector2;

    // Then
    BOOST_TEST(result == VWolf::Vector4(-4, -20, -74, -190));

    // When/Then
    BOOST_TEST((vector + vector2) == VWolf::Vector4(24, 40, 94, 210));
    BOOST_TEST((vector - vector2) == VWolf::Vector4(-4, -20, -74, -190));

    // When
    result = vector * 3;

    // Then
    BOOST_TEST(result == VWolf::Vector4(30, 30, 30, 30));

    result = vector / 2;

    // Then
    BOOST_TEST(result == VWolf::Vector4(5, 5, 5, 5));

    // When/Then
    BOOST_TEST((vector * 3) == VWolf::Vector4(30, 30, 30, 30));
    BOOST_TEST((vector / 2) == VWolf::Vector4(5, 5, 5, 5));

    // When/Then
    BOOST_TEST(vector2[0] == 14);
    BOOST_TEST(vector2[1] == 30);
    BOOST_TEST(vector2[2] == 84);
    BOOST_TEST(vector2[3] == 200);
    BOOST_TEST(vector2[4] == std::numeric_limits<float>::max());
}

BOOST_AUTO_TEST_CASE(Vector4Constants) {
    BOOST_TEST(VWolf::Vector4::Zero == VWolf::Vector4(0, 0, 0, 0));
    BOOST_TEST(VWolf::Vector4::One == VWolf::Vector4(1, 1, 1, 1));
    BOOST_TEST(VWolf::Vector4::NegativeInfinity == VWolf::Vector4(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
    BOOST_TEST(VWolf::Vector4::PositiveInfinity == VWolf::Vector4(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
}

BOOST_AUTO_TEST_CASE(Vector4MemberFunctions) {
    // Given
    VWolf::Vector4 vector(100, 100, 200, 200);

    // When
    VWolf::Vector4 normal = vector.Normalized();

    // Then
    BOOST_TEST(normal == VWolf::Vector4(0.316227794f, 0.316227794f, 0.632455587f, 0.632455587f));

    // When
    float magnitude = vector.Magnitude();

    // Then
    BOOST_TEST(magnitude == 4);

    // When
    float sqrMagnitude = vector.SqrMagnitude();

    // Then
    BOOST_TEST(sqrMagnitude == 2);

    // When
    vector.Normalize();

    // Then
    BOOST_TEST(vector == VWolf::Vector4(0.316227794f, 0.316227794f, 0.632455587f, 0.632455587f));
}

BOOST_AUTO_TEST_CASE(Vector4StaticFunctions) {
    // Given
    VWolf::Vector4 vector(10, 10, 10, 10);
    VWolf::Vector4 vector2(14, 30, 84, 200);
    VWolf::Vector4 vector3(0.23f, 0.5f, 0.98f, 0.4f);

    // When/Then
    BOOST_TEST(VWolf::Vector4::Distance(vector, vector2) == 204.919495f);

    // When/Then
    BOOST_TEST(VWolf::Vector4::Dot(vector, vector2) == 3280);

    // When/Then
    BOOST_TEST(VWolf::Vector4::Lerp(vector, vector2, 0.4) == VWolf::Vector4(1, 1, 1, 1));

    // When/Then
    BOOST_TEST(VWolf::Vector4::LerpUnclamped(vector, vector2, 0.4) == VWolf::Vector4(11.6f, 18, 39.6f, 86));

    // When/Then
    BOOST_TEST(VWolf::Vector4::Min(vector, vector2) == VWolf::Vector4(10, 10, 10, 10));

    // When/Then
    BOOST_TEST(VWolf::Vector4::Max(vector, vector2) == VWolf::Vector4(14, 30, 84, 200));

    // When/Then
    BOOST_TEST(VWolf::Vector4::MoveTowards(vector, vector2, 0.4) == VWolf::Vector4(0.4f, 0.4f, 0.4f, 0.4f));

    // When/Then
    BOOST_TEST(VWolf::Vector4::Reflect(vector, vector3) == VWolf::Vector4(0.293999672f, -11.1000004f, -31.3560028f, -6.88000107));

    // When/Then
    BOOST_TEST(VWolf::Vector4::Scale(vector, vector2) == VWolf::Vector4(140, 300, 840, 2000));

    // When/Then
    BOOST_TEST(VWolf::Vector4::ProjectOnPlane(vector, vector3) == VWolf::Vector4(3.4096818f, 7.4123516f, 14.5282097f, 5.92988157f));
}

BOOST_AUTO_TEST_CASE(Vector4Internals) {
    // Given
    VWolf::Vector4 vector(10, 10, 10, 10);

    // When
    vector.x = 15;
    vector.y = 30;
    vector.z = 84;
    vector.w = 200;

    // Then
    BOOST_TEST(vector.GetInternalVector().x == 15);
    BOOST_TEST(vector.GetInternalVector().y == 30);
    BOOST_TEST(vector.GetInternalVector().z == 84);
    BOOST_TEST(vector.GetInternalVector().w == 200);
}

BOOST_AUTO_TEST_CASE(Vector4Conversion) {
    // Given
    VWolf::Vector4 vector(10, 10, 10, 10);

    // Given
    VWolf::Vector2 vector2 = static_cast<VWolf::Vector2>(vector);

    // Then
    BOOST_TEST(vector2.GetX() == 10);
    BOOST_TEST(vector2.GetY() == 10);

    // Given
    VWolf::Vector3 vector3 = static_cast<VWolf::Vector3>(vector);

    // Then
    BOOST_TEST(vector3.GetX() == 10);
    BOOST_TEST(vector3.GetY() == 10);
    BOOST_TEST(vector3.GetZ() == 10);
}
