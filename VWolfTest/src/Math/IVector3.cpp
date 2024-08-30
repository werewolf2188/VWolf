//
//  IVector3.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/22/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(IVector3Initializer)
{
    // Given

    // When
    VWolf::IVector3 vector2;

    // Then
    BOOST_TEST(vector2.GetX() == 0);
    BOOST_TEST(vector2.GetY() == 0);
    BOOST_TEST(vector2.GetZ() == 0);

    // When
    vector2 = VWolf::IVector3(10, 10, 10);

    BOOST_TEST(vector2.GetX() == 10);
    BOOST_TEST(vector2.GetY() == 10);
    BOOST_TEST(vector2.GetZ() == 10);

    // When
    VWolf::IVector3 otherVector2(vector2);

    // Then
    BOOST_TEST(otherVector2.GetX() == 10);
    BOOST_TEST(otherVector2.GetY() == 10);
    BOOST_TEST(otherVector2.GetZ() == 10);

    // When
    VWolf::IVector3 moveVector2(std::move(vector2));

    // Then
    BOOST_TEST(moveVector2.GetX() == 10);
    BOOST_TEST(moveVector2.GetY() == 10);
    BOOST_TEST(moveVector2.GetZ() == 10);
    BOOST_TEST(vector2.GetX() == 0);
    BOOST_TEST(vector2.GetY() == 0);
    BOOST_TEST(vector2.GetZ() == 0);

//    otherVector2 = std::move(moveVector2);
//
//    BOOST_TEST(otherVector2.x == 10.5f);
//    BOOST_TEST(otherVector2.y == 10.5f);
//    BOOST_TEST(moveVector2.x == 0);
//    BOOST_TEST(moveVector2.y == 0);

}

BOOST_AUTO_TEST_CASE(IVector3Comparison) {
    // Given
    VWolf::IVector3 vector(10, 10, 10);
    VWolf::IVector3 vector2(10, 10, 10);
    VWolf::IVector3 vector3(20, 10, 10);
    VWolf::IVector3 vector4(10, 20, 10);
    VWolf::IVector3 vector5(30, 30, 30);

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
    isSame = vector == VWolf::IVector3(10, 10, 10);

    // When
    BOOST_TEST(isSame == true);

    // Then
    isDifferent = vector !=  VWolf::IVector3(12, 12, 12);

    // When
    BOOST_TEST(isDifferent == true);

    // When
    BOOST_TEST(vector == VWolf::IVector3(10, 10, 10));
    BOOST_TEST(vector != VWolf::IVector3(12, 12, 12));
}

BOOST_AUTO_TEST_CASE(IVector3Operations) {
    // Given
    VWolf::IVector3 vector(10, 10, 10);
    VWolf::IVector3 vector2(14, 30, 84);

    // When
    VWolf::IVector3 result = vector + vector2;

    // Then
    BOOST_TEST(result == VWolf::IVector3(24, 40, 94));

    // When
    result = vector - vector2;

    // Then
    BOOST_TEST(result == VWolf::IVector3(-4, -20, -74));

    // When/Then
    BOOST_TEST((vector + vector2) == VWolf::IVector3(24, 40, 94));
    BOOST_TEST((vector - vector2) == VWolf::IVector3(-4, -20, -74));

    // When
    result = vector * 3;

    // Then
    BOOST_TEST(result == VWolf::IVector3(30, 30, 30));

    result = vector / 2;

    // Then
    BOOST_TEST(result == VWolf::IVector3(5, 5, 5));

    // When/Then
    BOOST_TEST((vector * 3) == VWolf::IVector3(30, 30, 30));
    BOOST_TEST((vector / 2) == VWolf::IVector3(5, 5, 5));

    // When/Then
    BOOST_TEST(vector2[0] == 14);
    BOOST_TEST(vector2[1] == 30);
    BOOST_TEST(vector2[2] == 84);
    BOOST_TEST(vector2[3] == std::numeric_limits<int32_t>::max());

    // When
    VWolf::Vector3 transformed = (VWolf::Vector3)vector;

    // Then
    BOOST_TEST(transformed == VWolf::Vector3(10, 10, 10));
}

BOOST_AUTO_TEST_CASE(IVector3Constants) {
    BOOST_TEST(VWolf::IVector3::Up == VWolf::IVector3(0, 1, 0));
    BOOST_TEST(VWolf::IVector3::Down == VWolf::IVector3(0, -1, 0));
    BOOST_TEST(VWolf::IVector3::Left == VWolf::IVector3(-1, 0, 0));
    BOOST_TEST(VWolf::IVector3::Right == VWolf::IVector3(1, 0, 0));
    BOOST_TEST(VWolf::IVector3::Back == VWolf::IVector3(0, 0, -1));
    BOOST_TEST(VWolf::IVector3::Forward == VWolf::IVector3(0, 0, 1));
    BOOST_TEST(VWolf::IVector3::Zero == VWolf::IVector3(0, 0, 0));
    BOOST_TEST(VWolf::IVector3::One == VWolf::IVector3(1, 1, 1));
}

BOOST_AUTO_TEST_CASE(IVector3MemberFunctions) {
    // Given
    VWolf::IVector3 vector(100, 100, 100);

    // When
    float magnitude = vector.Magnitude();

    // Then
    BOOST_TEST(magnitude == 3);

    // When
    float sqrMagnitude = vector.SqrMagnitude();

    // Then
    BOOST_TEST(sqrMagnitude == 1.73205078f);
}

BOOST_AUTO_TEST_CASE(IVector3StaticFunctions) {
    // Given
    VWolf::IVector3 vector(10, 10, 10);
    VWolf::IVector3 vector2(14, 30, 84);
    VWolf::Vector3 floatVector(1.2030402f, 14.500305f, 20.93f);

    // When/Then
    BOOST_TEST(VWolf::IVector3::Distance(vector, vector2) == 76.7593613f);

    // When/Then
    BOOST_TEST(VWolf::IVector3::Min(vector, vector2) == VWolf::IVector3(10, 10, 10));

    // When/Then
    BOOST_TEST(VWolf::IVector3::Max(vector, vector2) == VWolf::IVector3(14, 30, 84));

    // When/Then
    BOOST_TEST(VWolf::IVector3::Scale(vector, vector2) == VWolf::IVector3(140, 300, 840));

    // When/Then
    BOOST_TEST(VWolf::IVector3::CeilToInt(floatVector) == VWolf::IVector3(2, 15, 21));

    // When/Then
    BOOST_TEST(VWolf::IVector3::FloorToInt(floatVector) == VWolf::IVector3(1, 14, 20));

    // When/Then
    BOOST_TEST(VWolf::IVector3::RoundToInt(floatVector) == VWolf::IVector3(1, 15, 21));
}

BOOST_AUTO_TEST_CASE(IVector3Internals) {
    // Given
    VWolf::IVector3 vector(10, 10, 10);

    // When
    vector.SetX(15);
    vector.SetY(30);
    vector.SetZ(84);

    // Then
    BOOST_TEST(vector.GetInternalVector().x == 15);
    BOOST_TEST(vector.GetInternalVector().y == 30);
    BOOST_TEST(vector.GetInternalVector().z == 84);

    BOOST_TEST(sizeof(VWolf::IVector3) == 12);
    // TODO: Remove when removing the old types
    BOOST_TEST(sizeof(VWolf::Vector3Int) == 12);
    BOOST_TEST(sizeof(VWolf::IVector3) == sizeof(VWolf::Vector3Int));
}

