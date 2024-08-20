//
//  IVector2.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/21/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(IVector2Initializer)
{
    // Given

    // When
    VWolf::IVector2 vector2;

    // Then
    BOOST_TEST(vector2.x == 0);
    BOOST_TEST(vector2.y == 0);

    // When
    vector2 = VWolf::IVector2(10, 10);

    BOOST_TEST(vector2.x == 10);
    BOOST_TEST(vector2.y == 10);

    // When
    VWolf::IVector2 otherVector2(vector2);

    // Then
    BOOST_TEST(otherVector2.x == 10);
    BOOST_TEST(otherVector2.y == 10);

    // When
    VWolf::IVector2 moveVector2(std::move(vector2));

    // Then
    BOOST_TEST(moveVector2.x == 10);
    BOOST_TEST(moveVector2.y == 10);
    BOOST_TEST(vector2.x == 0);
    BOOST_TEST(vector2.y == 0);

//    otherVector2 = std::move(moveVector2);
//
//    BOOST_TEST(otherVector2.x == 10.5f);
//    BOOST_TEST(otherVector2.y == 10.5f);
//    BOOST_TEST(moveVector2.x == 0);
//    BOOST_TEST(moveVector2.y == 0);

}

BOOST_AUTO_TEST_CASE(IVector2Comparison) {
    // Given
    VWolf::IVector2 vector(10, 10);
    VWolf::IVector2 vector2(10, 10);
    VWolf::IVector2 vector3(20, 10);
    VWolf::IVector2 vector4(10, 20);
    VWolf::IVector2 vector5(30, 30);

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
    isSame = vector == VWolf::IVector2(10, 10);

    // When
    BOOST_TEST(isSame == true);

    // Then
    isDifferent = vector !=  VWolf::IVector2(12, 12);

    // When
    BOOST_TEST(isDifferent == true);

    // When
    BOOST_TEST(vector == VWolf::IVector2(10, 10));
    BOOST_TEST(vector != VWolf::IVector2(12, 12));
}

BOOST_AUTO_TEST_CASE(IVector2Operations) {
    // Given
    VWolf::IVector2 vector(10, 10);
    VWolf::IVector2 vector2(14, 30);

    // When
    VWolf::IVector2 result = vector + vector2;

    // Then
    BOOST_TEST(result == VWolf::IVector2(24, 40));

    // When
    result = vector - vector2;

    // Then
    BOOST_TEST(result == VWolf::IVector2(-4, -20));

    // When/Then
    BOOST_TEST((vector + vector2) == VWolf::IVector2(24, 40));
    BOOST_TEST((vector - vector2) == VWolf::IVector2(-4, -20));

    // When
    result = vector * 3;

    // Then
    BOOST_TEST(result == VWolf::IVector2(30, 30));

    result = vector / 2;

    // Then
    BOOST_TEST(result == VWolf::IVector2(5, 5));

    // When/Then
    BOOST_TEST((vector * 3) == VWolf::IVector2(30, 30));
    BOOST_TEST((vector / 2) == VWolf::IVector2(5, 5));

    // When/Then
    BOOST_TEST(vector2[0] == 14);
    BOOST_TEST(vector2[1] == 30);
    BOOST_TEST(vector2[2] == std::numeric_limits<int32_t>::max());

    // When
    VWolf::Vector2 transformed = (VWolf::Vector2)vector;

    // Then
    BOOST_TEST(transformed == VWolf::Vector2(10, 10));
}

BOOST_AUTO_TEST_CASE(IVector2Constants) {
    BOOST_TEST(VWolf::IVector2::Up == VWolf::IVector2(0, 1));
    BOOST_TEST(VWolf::IVector2::Down == VWolf::IVector2(0, -1));
    BOOST_TEST(VWolf::IVector2::Left == VWolf::IVector2(-1, 0));
    BOOST_TEST(VWolf::IVector2::Right == VWolf::IVector2(1, 0));
    BOOST_TEST(VWolf::IVector2::Zero == VWolf::IVector2(0, 0));
    BOOST_TEST(VWolf::IVector2::One == VWolf::IVector2(1, 1));
}

BOOST_AUTO_TEST_CASE(IVector2MemberFunctions) {
    // Given
    VWolf::IVector2 vector(100, 100);

    // When
    float magnitude = vector.Magnitude();

    // Then
    BOOST_TEST(magnitude == 2);

    // When
    float sqrMagnitude = vector.SqrMagnitude();

    // Then
    BOOST_TEST(sqrMagnitude == 1);
}

BOOST_AUTO_TEST_CASE(IVector2StaticFunctions) {
    // Given
    VWolf::IVector2 vector(10, 10);
    VWolf::IVector2 vector2(14, 30);
    VWolf::Vector2 floatVector(1.2030402f, 14.500305f);

    // When/Then
    BOOST_TEST(VWolf::IVector2::Distance(vector, vector2) == 20.3960781f);

    // When/Then
    BOOST_TEST(VWolf::IVector2::Min(vector, vector2) == VWolf::IVector2(10, 10));

    // When/Then
    BOOST_TEST(VWolf::IVector2::Max(vector, vector2) == VWolf::IVector2(14, 30));

    // When/Then
    BOOST_TEST(VWolf::IVector2::Scale(vector, vector2) == VWolf::IVector2(140, 300));

    // When/Then
    BOOST_TEST(VWolf::IVector2::CeilToInt(floatVector) == VWolf::IVector2(2, 15));

    // When/Then
    BOOST_TEST(VWolf::IVector2::FloorToInt(floatVector) == VWolf::IVector2(1, 14));

    // When/Then
    BOOST_TEST(VWolf::IVector2::RoundToInt(floatVector) == VWolf::IVector2(1, 15));
}

BOOST_AUTO_TEST_CASE(IVector2Internals) {
    // Given
    VWolf::IVector2 vector(10, 10);

    // When
    vector.x = 15;
    vector.y = 30;

    // Then
    BOOST_TEST(vector.GetInternalVector().x == 15);
    BOOST_TEST(vector.GetInternalVector().y == 30);
}
