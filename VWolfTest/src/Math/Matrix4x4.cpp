//
//  Matrix4x4.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/27/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

bool ComparePerUnit(VWolf::Matrix4x4 matrix1, VWolf::Matrix4x4 matrix2) {
    bool result = true;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            result &= matrix1[c][r] == matrix2[c][r];

    return result;
}

BOOST_AUTO_TEST_CASE(Matrix4x4Initializer)
{
    BOOST_TEST(sizeof(VWolf::Matrix4x4) == 64);

    // Given
    VWolf::Matrix4x4 matrix;

    // When / Then
    BOOST_TEST(matrix == VWolf::Matrix4x4::Zero);

    // When
    matrix = VWolf::Matrix4x4(1.0f);

    // Then
    BOOST_TEST(matrix == VWolf::Matrix4x4::Identity);

    // When
    matrix = VWolf::Matrix4x4(4.0f, 3.0f, 2.0f, 1.0f,
                              3.0f, 2.0f, 1.0f, 4.0f,
                              2.0f, 1.0f, 4.0f, 3.0f,
                              1.0f, 2.0f, 3.0f, 4.0f);
    VWolf::Matrix4x4 matrix2 = VWolf::Matrix4x4(4.0f, 3.0f, 2.0f, 1.0f,
                                                3.0f, 2.0f, 1.0f, 4.0f,
                                                2.0f, 1.0f, 4.0f, 3.0f,
                                                1.0f, 2.0f, 3.0f, 4.0f);

    // Then
    BOOST_TEST(ComparePerUnit(matrix, matrix2));

    // When
    matrix = VWolf::Matrix4x4(VWolf::Vector4(4.0f, 3.0f, 2.0f, 1.0f),
                              VWolf::Vector4(3.0f, 2.0f, 1.0f, 4.0f),
                              VWolf::Vector4(2.0f, 1.0f, 4.0f, 3.0f),
                              VWolf::Vector4(1.0f, 2.0f, 3.0f, 4.0f));

    // Then
    BOOST_TEST(ComparePerUnit(matrix, matrix2));

    // When
    VWolf::Matrix4x4 otherMatrix(matrix);

    // Then
    BOOST_TEST(ComparePerUnit(otherMatrix, matrix2));

    // When
    VWolf::Matrix4x4 moveMatrix(std::move(matrix));

    // Then
    BOOST_TEST(ComparePerUnit(moveMatrix, matrix2));
    BOOST_TEST(matrix == VWolf::Matrix4x4::Zero);

    // When
    matrix = moveMatrix;

    // Then
    BOOST_TEST(ComparePerUnit(matrix, matrix2));
}

BOOST_AUTO_TEST_CASE(Matrix4x4Comparison)
{
    VWolf::Matrix4x4 matrix1 = VWolf::Matrix4x4(4.0f, 3.0f, 2.0f, 1.0f,
                                                3.0f, 2.0f, 1.0f, 4.0f,
                                                2.0f, 1.0f, 4.0f, 3.0f,
                                                1.0f, 2.0f, 3.0f, 4.0f);
    VWolf::Matrix4x4 matrix2 = VWolf::Matrix4x4(4.0f, 3.0f, 2.0f, 1.0f,
                                                3.0f, 2.0f, 1.0f, 4.0f,
                                                2.0f, 1.0f, 4.0f, 3.0f,
                                                1.0f, 2.0f, 3.0f, 4.0f);

    BOOST_TEST(matrix1 == matrix2);
    BOOST_TEST(matrix1 == VWolf::Matrix4x4(4.0f, 3.0f, 2.0f, 1.0f,
                                           3.0f, 2.0f, 1.0f, 4.0f,
                                           2.0f, 1.0f, 4.0f, 3.0f,
                                           1.0f, 2.0f, 3.0f, 4.0f));
}

BOOST_AUTO_TEST_CASE(Matrix4x4Constants) {
    BOOST_TEST(VWolf::Matrix4x4::Identity == VWolf::Matrix4x4(1.0f));
    BOOST_TEST(VWolf::Matrix4x4::Zero == VWolf::Matrix4x4(0));
}

BOOST_AUTO_TEST_CASE(Matrix4x4Operators)
{
    VWolf::Matrix4x4 matrix1 = VWolf::Matrix4x4(4.0f, 3.0f, 2.0f, 1.0f,
                                                3.0f, 2.0f, 1.0f, 4.0f,
                                                2.0f, 1.0f, 4.0f, 3.0f,
                                                1.0f, 2.0f, 3.0f, 4.0f);
    VWolf::Matrix4x4 matrix2 = VWolf::Matrix4x4(4.0f, 3.0f, 2.0f, 1.0f,
                                                3.0f, 2.0f, 1.0f, 4.0f,
                                                2.0f, 1.0f, 4.0f, 3.0f,
                                                1.0f, 2.0f, 3.0f, 4.0f);
    VWolf::Matrix4x4 result = VWolf::Matrix4x4(30, 22, 22, 26,
                                               24, 22, 24, 30,
                                               22, 18, 30, 30,
                                               20, 18, 28, 34);

    VWolf::Matrix4x4 result2 = matrix1 * matrix2;

    BOOST_TEST(result == result2);
}

BOOST_AUTO_TEST_CASE(Matrix4x4GetMethods) {
    VWolf::Matrix4x4 matrix = VWolf::Matrix4x4::TRS(VWolf::Vector3::Zero, VWolf::Quaternion::Identity, VWolf::Vector3(5, 3, 2));

    BOOST_TEST(matrix.GetDeterminant() == 30);

    BOOST_TEST(matrix.GetInverse() == VWolf::Matrix4x4(0.200000018f, -0, 0, -0,
                                                       -0, 0.333333343f, -0, 0,
                                                       0, -0, 0.5f, 0,
                                                       -0, 0, -0, 1));

    BOOST_TEST(matrix.IsIdentity() == false);

    BOOST_TEST(matrix.GetLossyScale() == VWolf::Vector3(5, 3, 2));

    BOOST_TEST(matrix.GetRotation() == VWolf::Quaternion::Identity);

    BOOST_TEST(matrix.GetPosition() == VWolf::Vector3::Zero);

    BOOST_TEST(matrix.GetTranspose() == VWolf::Matrix4x4(5, 0, 0, 0,
                                                         0, 3, 0, 0,
                                                         0, 0, 2, 0,
                                                         0, 0, 0, 1));

    BOOST_TEST(matrix == VWolf::Matrix4x4(5, 0, 0, 0,
                                          0, 3, 0, 0,
                                          0, 0, 2, 0,
                                          0, 0, 0, 1));
}

BOOST_AUTO_TEST_CASE(Matrix4x4InstanceMethods) {
    // Given
    VWolf::Matrix4x4 matrix = VWolf::Matrix4x4::Identity;
    VWolf::Vector3 position(10, 10, 10);
    VWolf::Quaternion rotation = VWolf::Quaternion::Euler(0, 90, 0);
    VWolf::Vector3 scale(1, 1, 1);

    // When
    matrix.SetTRS(position, rotation, scale);

    // Then
    BOOST_TEST(matrix.GetPosition() == position);
    BOOST_TEST(static_cast<VWolf::Vector3>(matrix.GetColumn(3)) == position);
    BOOST_TEST(matrix.GetRow(3) == VWolf::Vector4(0, 0, 0, 1));

    // When
    VWolf::Vector4 result(matrix[0][0], matrix[1][1], matrix[2][2], matrix[3][3]);

    // Then
    BOOST_TEST(result == VWolf::Vector4(-0.448073685f, 1, -0.448073685f, 1));

    // When
    matrix.SetColumn(0, VWolf::Vector4(10, 20, 30, 40));

    // Then
    BOOST_TEST(matrix.GetColumn(0) == VWolf::Vector4(10, 20, 30, 40));

    // When
    matrix.SetRow(0, VWolf::Vector4(11, 21, 31, 41));

    // Then
    BOOST_TEST(matrix.GetRow(0) == VWolf::Vector4(11, 21, 31, 41));

    // When
    matrix = VWolf::Matrix4x4(10);
    VWolf::Vector3 vector = matrix.MultiplyPoint(VWolf::Vector3(10, 20, 30));

    // Then
    BOOST_TEST(vector == VWolf::Vector3(100, 200, 300));

    // When
    matrix = VWolf::Matrix4x4(10);
    vector = matrix.MultiplyVector(VWolf::Vector3(20, 40, 60));

    // Then
    BOOST_TEST(vector == VWolf::Vector3(200, 400, 600));
}

BOOST_AUTO_TEST_CASE(Matrix4x4StaticMethods) {
    // Given
    VWolf::Matrix4x4 matrix = VWolf::Matrix4x4::Identity;

    // When
    matrix = VWolf::Matrix4x4::Translate(VWolf::Vector3(10, 10, 10));

    // Then
    BOOST_TEST(matrix.GetPosition() == VWolf::Vector3(10, 10, 10));

    // When
    matrix = VWolf::Matrix4x4::Scale(VWolf::Vector3(10, 10, 10));

    // Then
    BOOST_TEST(VWolf::Vector3(matrix[0][0], matrix[1][1], matrix[2][2]) == VWolf::Vector3(10, 10, 10));

    // When
    matrix = VWolf::Matrix4x4::Rotate(VWolf::Quaternion::Euler(0, 90, 0));
    VWolf::Vector4 result(matrix[0][0], matrix[1][1], matrix[2][2], matrix[3][3]);

    // Then
    BOOST_TEST(result == VWolf::Vector4(-0.448073685f, 1, -0.448073685f, 1));

    // When
    VWolf::Vector3 position(10, 10, 10);
    VWolf::Quaternion rotation = VWolf::Quaternion::Euler(0, 90, 0);
    VWolf::Vector3 scale(1, 1, 1);

    // When
    matrix = VWolf::Matrix4x4::TRS(position, rotation, scale);

    // Then
    BOOST_TEST(matrix.GetPosition() == position);
    BOOST_TEST(static_cast<VWolf::Vector3>(matrix.GetColumn(3)) == position);
    BOOST_TEST(matrix.GetRow(3) == VWolf::Vector4(0, 0, 0, 1));

    // When
    matrix = VWolf::Matrix4x4::Frustum(-256, 256, -256, 256, 0.1f, 1000);

    // Then
    BOOST_TEST(matrix == VWolf::Matrix4x4(0.000390625006f, 0, 0, 0,
                                          0, 0.000390625006f, 0, 0,
                                          0, 0, -1.00019991f, -1,
                                          0, 0, -0.20002f, 0));

    // When
    matrix = VWolf::Matrix4x4::Ortho(-256, 256, -256, 256, 0.1f, 1000);

    // Then
    BOOST_TEST(matrix == VWolf::Matrix4x4(0.00390625f, 0, 0, 0,
                                          0, 0.00390625f, 0, 0,
                                          0, 0, -0.00200019986f, 0,
                                          0, 0, -1.00019991f, 1));

    // When
    matrix = VWolf::Matrix4x4::Perspective(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);

    // Then
    BOOST_TEST(matrix == VWolf::Matrix4x4(1.35799515f, 0, 0, 0,
                                          0, 2.41421342f, 0, 0,
                                          0, 0, -1.00019991, -1,
                                          0, 0, -0.20002f, 0));
}
