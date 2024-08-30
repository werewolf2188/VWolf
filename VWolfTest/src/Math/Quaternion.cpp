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
    BOOST_TEST(quat.x == 0);
    BOOST_TEST(quat.y == 0);
    BOOST_TEST(quat.z == 0);
    BOOST_TEST(quat.w == 0);

    // When
    quat = VWolf::Quaternion(10.5f, 10.5f, 10.5f, 10.5f);

    BOOST_TEST(quat.x == 10.5f);
    BOOST_TEST(quat.y == 10.5f);
    BOOST_TEST(quat.z == 10.5f);
    BOOST_TEST(quat.w == 10.5f);

    // When
    VWolf::Quaternion otherQuat(quat);

    // Then
    BOOST_TEST(otherQuat.x == 10.5f);
    BOOST_TEST(otherQuat.y == 10.5f);
    BOOST_TEST(otherQuat.z == 10.5f);
    BOOST_TEST(otherQuat.w == 10.5f);

    // When
    VWolf::Quaternion moveQuat(std::move(quat));

    // Then
    BOOST_TEST(moveQuat.x == 10.5f);
    BOOST_TEST(moveQuat.y == 10.5f);
    BOOST_TEST(moveQuat.z == 10.5f);
    BOOST_TEST(moveQuat.w == 10.5f);
    BOOST_TEST(quat.x == 0);
    BOOST_TEST(quat.y == 0);
    BOOST_TEST(quat.z == 0);
    BOOST_TEST(quat.w == 0);

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
    BOOST_TEST(quat2 == VWolf::Quaternion(-0.858940601f, -0.337996632f, 0.183678403f, -0.337996662f));

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
    BOOST_TEST(quatResult == VWolf::Quaternion(-0.974967658f, 0, 0.222346798f, 0));

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
    quaternion.x = 15;
    quaternion.y = 30;
    quaternion.z = 84;
    quaternion.w = 200;

    // Then
    BOOST_TEST(quaternion.GetInternalQuaternion().x == 15);
    BOOST_TEST(quaternion.GetInternalQuaternion().y == 30);
    BOOST_TEST(quaternion.GetInternalQuaternion().z == 84);
    BOOST_TEST(quaternion.GetInternalQuaternion().w == 200);
}
