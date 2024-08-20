//
//  Vector2.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/20/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(Vector2Initializer)
{
    // Given

    // When
    VWolf::Vector2 vector2;

    // Then
    BOOST_TEST(vector2.x == 0);
    BOOST_TEST(vector2.y == 0);

    // When
    vector2 = VWolf::Vector2(10.5f, 10.5f);

    BOOST_TEST(vector2.x == 10.5f);
    BOOST_TEST(vector2.y == 10.5f);

    // When
    VWolf::Vector2 otherVector2(vector2);

    // Then
    BOOST_TEST(otherVector2.x == 10.5f);
    BOOST_TEST(otherVector2.y == 10.5f);

    // When
    VWolf::Vector2 moveVector2(std::move(vector2));

    // Then
    BOOST_TEST(moveVector2.x == 10.5f);
    BOOST_TEST(moveVector2.y == 10.5f);
    BOOST_TEST(vector2.x == 0);
    BOOST_TEST(vector2.y == 0);

//    otherVector2 = std::move(moveVector2);
//
//    BOOST_TEST(otherVector2.x == 10.5f);
//    BOOST_TEST(otherVector2.y == 10.5f);
//    BOOST_TEST(moveVector2.x == 0);
//    BOOST_TEST(moveVector2.y == 0);

}

BOOST_AUTO_TEST_CASE(Vector2Comparison) {
    // Given
    VWolf::Vector2 vector(10, 10);
    VWolf::Vector2 vector2(10, 10);
    VWolf::Vector2 vector3(20, 10);
    VWolf::Vector2 vector4(10, 20);
    VWolf::Vector2 vector5(30, 30);

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
    isSame = vector == VWolf::Vector2(10, 10);

    // When
    BOOST_TEST(isSame == true);

    // Then
    isDifferent = vector !=  VWolf::Vector2(12, 12);

    // When
    BOOST_TEST(isDifferent == true);

    // When
    BOOST_TEST(vector == VWolf::Vector2(10, 10));
    BOOST_TEST(vector != VWolf::Vector2(12, 12));
}

BOOST_AUTO_TEST_CASE(Vector2Operations) {
    // Given
    VWolf::Vector2 vector(10, 10);
    VWolf::Vector2 vector2(14, 30);

    // When
    VWolf::Vector2 result = vector + vector2;

    // Then
    BOOST_TEST(result == VWolf::Vector2(24, 40));

    // When
    result = vector - vector2;

    // Then
    BOOST_TEST(result == VWolf::Vector2(-4, -20));

    // When/Then
    BOOST_TEST((vector + vector2) == VWolf::Vector2(24, 40));
    BOOST_TEST((vector - vector2) == VWolf::Vector2(-4, -20));

    // When
    result = vector * 3;

    // Then
    BOOST_TEST(result == VWolf::Vector2(30, 30));

    result = vector / 2;

    // Then
    BOOST_TEST(result == VWolf::Vector2(5, 5));

    // When/Then
    BOOST_TEST((vector * 3) == VWolf::Vector2(30, 30));
    BOOST_TEST((vector / 2) == VWolf::Vector2(5, 5));

    // When/Then
    BOOST_TEST(vector2[0] == 14);
    BOOST_TEST(vector2[1] == 30);
    BOOST_TEST(vector2[2] == std::numeric_limits<float>::max());
}

BOOST_AUTO_TEST_CASE(Vector2Constants) {
    BOOST_TEST(VWolf::Vector2::Up == VWolf::Vector2(0, 1));
    BOOST_TEST(VWolf::Vector2::Down == VWolf::Vector2(0, -1));
    BOOST_TEST(VWolf::Vector2::Left == VWolf::Vector2(-1, 0));
    BOOST_TEST(VWolf::Vector2::Right == VWolf::Vector2(1, 0));
    BOOST_TEST(VWolf::Vector2::Zero == VWolf::Vector2(0, 0));
    BOOST_TEST(VWolf::Vector2::One == VWolf::Vector2(1, 1));
    BOOST_TEST(VWolf::Vector2::NegativeInfinity == VWolf::Vector2(std::numeric_limits<float>::min(), std::numeric_limits<float>::min()));
    BOOST_TEST(VWolf::Vector2::PositiveInfinity == VWolf::Vector2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
}

BOOST_AUTO_TEST_CASE(Vector2MemberFunctions) {
    // Given
    VWolf::Vector2 vector(100, 100);

    // When
    VWolf::Vector2 normal = vector.Normalized();

    // Then
    BOOST_TEST(normal == VWolf::Vector2(0.707106829f, 0.707106829f));

    // When
    float magnitude = vector.Magnitude();

    // Then
    BOOST_TEST(magnitude == 2);

    // When
    float sqrMagnitude = vector.SqrMagnitude();

    // Then
    BOOST_TEST(sqrMagnitude == 1.41421354f);

    // When
    vector.Normalize();

    // Then
    BOOST_TEST(vector == VWolf::Vector2(0.707106829f, 0.707106829f));
}

BOOST_AUTO_TEST_CASE(Vector2StaticFunctions) {
    // Given
    VWolf::Vector2 vector(10, 10);
    VWolf::Vector2 vector2(14, 30);

    // When/Then
    BOOST_TEST(VWolf::Vector2::Angle(vector, vector2) == 0);

    // When/Then
    BOOST_TEST(VWolf::Vector2::ClampMagnitude(vector2, 10) == VWolf::Vector2(10, 10));

    // When/Then
    BOOST_TEST(VWolf::Vector2::Distance(vector, vector2) == 20.3960781f);

    // When/Then
    BOOST_TEST(VWolf::Vector2::Dot(vector, vector2) == 440);

    // When/Then
    BOOST_TEST(VWolf::Vector2::Lerp(vector, vector2, 0.4) == VWolf::Vector2(1, 1));

    // When/Then
    BOOST_TEST(VWolf::Vector2::LerpUnclamped(vector, vector2, 0.4) == VWolf::Vector2(11.6f, 18));

    // When/Then
    BOOST_TEST(VWolf::Vector2::Min(vector, vector2) == VWolf::Vector2(10, 10));

    // When/Then
    BOOST_TEST(VWolf::Vector2::Max(vector, vector2) == VWolf::Vector2(14, 30));

    // When/Then
    BOOST_TEST(VWolf::Vector2::MoveTowards(vector, vector2, 0.4) == VWolf::Vector2(0.4f, 0.4f));

    // When/Then
    BOOST_TEST(VWolf::Vector2::Perpendicular(vector) == VWolf::Vector2(0, 1));

    // When/Then
    BOOST_TEST(VWolf::Vector2::Reflect(vector, vector2) == VWolf::Vector2(-12310, -26390));

    // When/Then
    BOOST_TEST(VWolf::Vector2::Scale(vector, vector2) == VWolf::Vector2(140, 300));

    // When/Then
    BOOST_TEST(VWolf::Vector2::SignedAngle(vector, vector2) == 0);
}

BOOST_AUTO_TEST_CASE(Vector2Internals) {
    // Given
    VWolf::Vector2 vector(10, 10);

    // When
    vector.x = 15;
    vector.y = 30;

    // Then
    BOOST_TEST(vector.GetInternalVector().x == 15);
    BOOST_TEST(vector.GetInternalVector().y == 30);
}

BOOST_AUTO_TEST_CASE(Vector2Conversion) {
    // Given
    VWolf::Vector2 vector(10, 10);

    // When
    VWolf::Vector3 vector3 = static_cast<VWolf::Vector3>(vector);

    // Then
    BOOST_TEST(vector3.x == 10);
    BOOST_TEST(vector3.y == 10);
    BOOST_TEST(vector3.z == 0);

    // When
    VWolf::Vector4 vector4 = static_cast<VWolf::Vector4>(vector);

    // Then
    BOOST_TEST(vector4.x == 10);
    BOOST_TEST(vector4.y == 10);
    BOOST_TEST(vector4.z == 0);
    BOOST_TEST(vector4.w == 0);
}
