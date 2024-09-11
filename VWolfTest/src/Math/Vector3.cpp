//
//  Vector3.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/22/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(Vector3Initializer)
{
    // Given

    // When
    VWolf::Vector3 vector3;

    // Then
    BOOST_TEST(vector3.GetX() == 0);
    BOOST_TEST(vector3.GetY() == 0);
    BOOST_TEST(vector3.GetZ() == 0);

    // When
    vector3 = VWolf::Vector3(10.5f, 10.5f, 10.5f);

    BOOST_TEST(vector3.GetX() == 10.5f);
    BOOST_TEST(vector3.GetY() == 10.5f);
    BOOST_TEST(vector3.GetZ() == 10.5f);

    // When
    VWolf::Vector3 otherVector2(vector3);

    // Then
    BOOST_TEST(otherVector2.GetX() == 10.5f);
    BOOST_TEST(otherVector2.GetY() == 10.5f);
    BOOST_TEST(otherVector2.GetZ() == 10.5f);

    // When
    VWolf::Vector3 moveVector2(std::move(vector3));

    // Then
    BOOST_TEST(moveVector2.GetX() == 10.5f);
    BOOST_TEST(moveVector2.GetY() == 10.5f);
    BOOST_TEST(moveVector2.GetY() == 10.5f);
    BOOST_TEST(vector3.GetX() == 0);
    BOOST_TEST(vector3.GetY() == 0);
    BOOST_TEST(vector3.GetZ() == 0);

//    otherVector2 = std::move(moveVector2);
//
//    BOOST_TEST(otherVector2.x == 10.5f);
//    BOOST_TEST(otherVector2.y == 10.5f);
//    BOOST_TEST(moveVector2.x == 0);
//    BOOST_TEST(moveVector2.y == 0);

}

BOOST_AUTO_TEST_CASE(Vector3Comparison) {
    // Given
    VWolf::Vector3 vector(10, 10, 10);
    VWolf::Vector3 vector2(10, 10, 10);
    VWolf::Vector3 vector3(20, 10, 10);
    VWolf::Vector3 vector4(10, 20, 10);
    VWolf::Vector3 vector5(30, 30, 30);

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
    isSame = vector == VWolf::Vector3(10, 10, 10);

    // When
    BOOST_TEST(isSame == true);

    // Then
    isDifferent = vector !=  VWolf::Vector3(12, 12, 12);

    // When
    BOOST_TEST(isDifferent == true);

    // When
    BOOST_TEST(vector == VWolf::Vector3(10, 10, 10));
    BOOST_TEST(vector != VWolf::Vector3(12, 12, 12));
}

BOOST_AUTO_TEST_CASE(Vector3Operations) {
    // Given
    VWolf::Vector3 vector(10, 10, 10);
    VWolf::Vector3 vector2(14, 30, 84);

    // When
    VWolf::Vector3 result = vector + vector2;

    // Then
    BOOST_TEST(result == VWolf::Vector3(24, 40, 94));

    // When
    result = vector - vector2;

    // Then
    BOOST_TEST(result == VWolf::Vector3(-4, -20, -74));

    // When/Then
    BOOST_TEST((vector + vector2) == VWolf::Vector3(24, 40, 94));
    BOOST_TEST((vector - vector2) == VWolf::Vector3(-4, -20, -74));

    // When
    result = vector * 3;

    // Then
    BOOST_TEST(result == VWolf::Vector3(30, 30, 30));

    result = vector / 2;

    // Then
    BOOST_TEST(result == VWolf::Vector3(5, 5, 5));

    // When/Then
    BOOST_TEST((vector * 3) == VWolf::Vector3(30, 30, 30));
    BOOST_TEST((vector / 2) == VWolf::Vector3(5, 5, 5));

    // When/Then
    BOOST_TEST(vector2[0] == 14);
    BOOST_TEST(vector2[1] == 30);
    BOOST_TEST(vector2[2] == 84);
    BOOST_TEST(vector2[3] == std::numeric_limits<float>::max());
}

BOOST_AUTO_TEST_CASE(Vector3Constants) {
    BOOST_TEST(VWolf::Vector3::Up == VWolf::Vector3(0, 1, 0));
    BOOST_TEST(VWolf::Vector3::Down == VWolf::Vector3(0, -1, 0));
    BOOST_TEST(VWolf::Vector3::Left == VWolf::Vector3(-1, 0, 0));
    BOOST_TEST(VWolf::Vector3::Right == VWolf::Vector3(1, 0, 0));
    BOOST_TEST(VWolf::Vector3::Back == VWolf::Vector3(0, 0, -1));
    BOOST_TEST(VWolf::Vector3::Forward == VWolf::Vector3(0, 0, 1));
    BOOST_TEST(VWolf::Vector3::Zero == VWolf::Vector3(0, 0, 0));
    BOOST_TEST(VWolf::Vector3::One == VWolf::Vector3(1, 1, 1));
    BOOST_TEST(VWolf::Vector3::NegativeInfinity == VWolf::Vector3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() ));
    BOOST_TEST(VWolf::Vector3::PositiveInfinity == VWolf::Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
}

BOOST_AUTO_TEST_CASE(Vector3MemberFunctions) {
    // Given
    VWolf::Vector3 vector(100, 100, 200);

    // When
    VWolf::Vector3 normal = vector.Normalized();

    // Then
    BOOST_TEST(normal == VWolf::Vector3(0.408248276f, 0.408248276f, 0.816496551f));

    // When
    float magnitude = vector.Magnitude();

    // Then
    BOOST_TEST(magnitude == 3);

    // When
    float sqrMagnitude = vector.SqrMagnitude();

    // Then
    BOOST_TEST(sqrMagnitude == 1.73205078f);

    // When
    vector.Normalize();

    // Then
    BOOST_TEST(vector == VWolf::Vector3(0.408248276f, 0.408248276f, 0.816496551f));
}

BOOST_AUTO_TEST_CASE(Vector3StaticFunctions) {
    // Given
    VWolf::Vector3 vector(10, 10, 10);
    VWolf::Vector3 vector2(14, 30, 84);
    VWolf::Vector3 vector3(0.23f, 0.5f, 0.98f);

    // When/Then
    BOOST_TEST(VWolf::Vector3::Angle(vector, vector2) == 0);

    // When/Then
    BOOST_TEST(VWolf::Vector3::ClampMagnitude(vector2, 10) == VWolf::Vector3(10, 10, 10));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Cross(vector, vector2) == VWolf::Vector3(540, -700, 160));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Distance(vector, vector2) == 76.7593613f);

    // When/Then
    BOOST_TEST(VWolf::Vector3::Dot(vector, vector2) == 1280);

    // When/Then
    BOOST_TEST(VWolf::Vector3::Lerp(vector, vector2, 0.4) == VWolf::Vector3(1, 1, 1));

    // When/Then
    BOOST_TEST(VWolf::Vector3::LerpUnclamped(vector, vector2, 0.4) == VWolf::Vector3(11.6f, 18, 39.6f));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Min(vector, vector2) == VWolf::Vector3(10, 10, 10));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Max(vector, vector2) == VWolf::Vector3(14, 30, 84));

    // When/Then
    BOOST_TEST(VWolf::Vector3::MoveTowards(vector, vector2, 0.4) == VWolf::Vector3(0.4f, 0.4f, 0.4f));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Perpendicular(vector) == VWolf::Vector3(0, 1, 0));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Reflect(vector, vector2) == VWolf::Vector3(-35830, -76790, -215030));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Scale(vector, vector2) == VWolf::Vector3(140, 300, 840));

    // When/Then
    BOOST_TEST(VWolf::Vector3::SignedAngle(vector, vector2) == 0);

    // When/Then
    BOOST_TEST(VWolf::Vector3::Slerp(vector, vector2, 0.4) == VWolf::Vector3(10, 10, 10));

    // When/Then
    BOOST_TEST(VWolf::Vector3::Project(vector, vector3) == VWolf::Vector3(2.3f, 5, 9.8));

    // When/Then
    BOOST_TEST(VWolf::Vector3::ProjectOnPlane(vector, vector3) == VWolf::Vector3(3.11327457f, 6.7679882f, 13.2652569f));
}

BOOST_AUTO_TEST_CASE(Vector3Internals) {
    // Given
    VWolf::Vector3 vector(10, 10, 10);

    // When
    vector.SetX(15);
    vector.SetY(30);
    vector.SetZ(84);

    // Then
    BOOST_TEST(vector.GetInternalVector().x == 15);
    BOOST_TEST(vector.GetInternalVector().y == 30);
    BOOST_TEST(vector.GetInternalVector().z == 84);

    BOOST_TEST(sizeof(VWolf::Vector3) == 12);
}

BOOST_AUTO_TEST_CASE(Vector3Conversion) {
    // Given
    VWolf::Vector3 vector(10, 10, 10);

    // When
    VWolf::Vector2 vector2 = static_cast<VWolf::Vector2>(vector);

    // Then
    BOOST_TEST(vector2.GetX() == 10);
    BOOST_TEST(vector2.GetY() == 10);

    // When
    VWolf::Vector4 vector4 = static_cast<VWolf::Vector4>(vector);

    // Then
    BOOST_TEST(vector4.GetX() == 10);
    BOOST_TEST(vector4.GetY() == 10);
    BOOST_TEST(vector4.GetZ() == 10);
    BOOST_TEST(vector4.GetW() == 0);
}
