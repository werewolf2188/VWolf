//
//  Quaternion.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/25/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(QuaternionInitializer)
{
    // Given

    // When
    VWolf::Quaternion quat;

    // Then
    BOOST_TEST(quat.GetX() == 0);
    BOOST_TEST(quat.GetY() == 0);
    BOOST_TEST(quat.GetZ() == 0);
    BOOST_TEST(quat.GetW() == 0);

    // When
    quat = VWolf::Quaternion(10.5f, 10.5f, 10.5f, 10.5f);

    BOOST_TEST(quat.GetX() == 10.5f);
    BOOST_TEST(quat.GetY() == 10.5f);
    BOOST_TEST(quat.GetZ() == 10.5f);
    BOOST_TEST(quat.GetW() == 10.5f);

    // When
    VWolf::Quaternion otherQuat(quat);

    // Then
    BOOST_TEST(otherQuat.GetX() == 10.5f);
    BOOST_TEST(otherQuat.GetY() == 10.5f);
    BOOST_TEST(otherQuat.GetZ() == 10.5f);
    BOOST_TEST(otherQuat.GetW() == 10.5f);

    // When
    VWolf::Quaternion moveQuat(std::move(quat));

    // Then
    BOOST_TEST(moveQuat.GetX() == 10.5f);
    BOOST_TEST(moveQuat.GetY() == 10.5f);
    BOOST_TEST(moveQuat.GetZ() == 10.5f);
    BOOST_TEST(moveQuat.GetW() == 10.5f);
    BOOST_TEST(quat.GetX() == 0);
    BOOST_TEST(quat.GetY() == 0);
    BOOST_TEST(quat.GetZ() == 0);
    BOOST_TEST(quat.GetW() == 0);

//    otherVector2 = std::move(moveVector2);
//
//    BOOST_TEST(otherVector2.x == 10.5f);
//    BOOST_TEST(otherVector2.y == 10.5f);
//    BOOST_TEST(moveVector2.x == 0);
//    BOOST_TEST(moveVector2.y == 0);
}

BOOST_AUTO_TEST_CASE(QuaternionComparison) {
    // Given
    VWolf::Quaternion quat(10, 10, 10, 10);
    VWolf::Quaternion quat2(10, 10, 10, 10);
    VWolf::Quaternion quat3(20, 10, 10, 20);
    VWolf::Quaternion quat4(10, 20, 10, 20);

    // When
    bool isSame = quat == quat2;

    // Then
    BOOST_TEST(isSame == true);

    // When
    isSame = quat2 == quat3;

    // Then
    BOOST_TEST(isSame == false);

    // When
    isSame = quat2 == quat4;

    // Then
    BOOST_TEST(isSame == false);

    // When
    isSame = quat2 == VWolf::Quaternion(10, 10, 10, 10);

    // When
    BOOST_TEST(isSame == true);

    // When
    BOOST_TEST(quat2 == VWolf::Quaternion(10, 10, 10, 10));
}

BOOST_AUTO_TEST_CASE(QuaternionOperations) {
    // Given
    VWolf::Quaternion quat(10, 10, 10, 10);
    VWolf::Quaternion quat2(20, 10, 10, 20);

    // When
    VWolf::Quaternion result = quat * quat2;

    // Then
    BOOST_TEST(result == VWolf::Quaternion(-200, 400, 200, 400));

    // When
    BOOST_TEST((quat * VWolf::Quaternion(20, 10, 10, 20)) == VWolf::Quaternion(-200, 400, 200, 400));
}

BOOST_AUTO_TEST_CASE(QuaternionConstants) {
    BOOST_TEST(VWolf::Quaternion::Identity == VWolf::Quaternion(1, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE(QuaternionMemberFunctions) {
    // Given
    VWolf::Quaternion quat(100, 100, 200, 200);

    // When
    VWolf::Quaternion normal = quat.Normalized();

    // Then
    BOOST_TEST(normal == VWolf::Quaternion(0.316227794f, 0.316227794f, 0.632455587f, 0.632455587f));

    // When
    VWolf::Vector3 euler = quat.EulerAngles();

    // Then
    BOOST_TEST(euler == VWolf::Vector3(1.57079637f, -0, 2.21429753f));

    // Given
    VWolf::Quaternion quat2;

    // When
    quat2.SetFromToRotation(VWolf::Vector3(10, 10, 10), VWolf::Vector3(20, 20, 20));

    // Then
    // TODO: Round to closest value
    BOOST_TEST(quat2 == VWolf::Quaternion(-0.858940601f, -0.337996632f, 0.183678418f, -0.337996662f));

    // When
    quat2.SetLookRotation(VWolf::Vector3(10, 10, 10));

    // Then
    BOOST_TEST(quat2 == VWolf::Quaternion(-1.05297542f, -0.695394278f, 2.54210758f, -1.67883015f));

    float f = 90.0f;
    VWolf::Vector3 v(10, 10, 10);

    // When
    quat2.ToAngleAxis(f, v);

    // Then
    BOOST_TEST(quat2 == VWolf::Quaternion(0.52532196f, 8.50903511f, 8.50903511f, 8.50903511f));
}

BOOST_AUTO_TEST_CASE(QuaternionStaticFunctions) {
    // Given
    VWolf::Quaternion quat(0.73f, 0.68f, 0.06f, 0.04f);
    VWolf::Quaternion quat2(0.69f, 0.58f, 0.31f, 0.30f);

    // When
    float result = VWolf::Quaternion::Angle(quat, quat2);

    // Then
    BOOST_TEST(result == 43.5338554f);

    // When
    VWolf::Quaternion quatResult = VWolf::Quaternion::AngleAxis(43.5338554f, VWolf::Vector3::Up);

    // Then
    // TODO: Round to closest value
    BOOST_TEST(quatResult == VWolf::Quaternion(-0.974967659f, 0, 0.222346783f, 0));

    // When
    result = VWolf::Quaternion::Dot(quat, quat2);

    // Then
    BOOST_TEST(result == 0.928700029f);

    // When
    quatResult = VWolf::Quaternion::Euler(0, 1, 0);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.87758255f, 0, 0.47942555f, 0));

    // When
    quatResult = VWolf::Quaternion::FromToRotation(VWolf::Vector3::Up, VWolf::Vector3::Right);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.770151138f, 0.420735508f, -0.420735508f, 0.229848862f));

    // When
    quatResult = VWolf::Quaternion::Inverse(quat);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.729635119f, -0.679660141f, -0.0599700101f, -0.0399800055f));

    // When
    quatResult = VWolf::Quaternion::Lerp(quat, quat2, 0.5f);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.710000038f, 0.629999995f, 0.185000002f, 0.170000002f));

    // When
    quatResult = VWolf::Quaternion::LerpUnclamped(quat, quat2, 0.5f);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.710000038f, 0.629999995f, 0.185000002f, 0.170000002f));

    // When
    quatResult = VWolf::Quaternion::Normalize(quat);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.729817569f, 0.679830074f, 0.0599850044f, 0.0399900004));

    // When
    quatResult = VWolf::Quaternion::Slerp(quat, quat2, 0.5f);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.72300446f, 0.641539216f, 0.188388482f, 0.173113763f));

    // When
    quatResult = VWolf::Quaternion::SlerpUnclamped(quat, quat2, 0.5f);

    // Then
    BOOST_TEST(quatResult == VWolf::Quaternion(0.72300446f, 0.641539216f, 0.188388482f, 0.173113763f));
}

BOOST_AUTO_TEST_CASE(QuaternionsInternals) {
    // Given
    VWolf::Quaternion quaternion(10, 10, 10, 10);

    // When
    quaternion.SetX(15);
    quaternion.SetY(30);
    quaternion.SetZ(84);
    quaternion.SetW(200);

    // Then
    BOOST_TEST(quaternion.GetInternalQuaternion().x == 15);
    BOOST_TEST(quaternion.GetInternalQuaternion().y == 30);
    BOOST_TEST(quaternion.GetInternalQuaternion().z == 84);
    BOOST_TEST(quaternion.GetInternalQuaternion().w == 200);

    BOOST_TEST(sizeof(VWolf::Quaternion) == 16);
}
