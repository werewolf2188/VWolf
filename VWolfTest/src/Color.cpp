//
//  Color.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 9/11/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(ColorInitializer)
{
    // Given

    // When
    VWolf::Color color;

    // Then
    BOOST_TEST(color.GetR() == 0);
    BOOST_TEST(color.GetG() == 0);
    BOOST_TEST(color.GetB() == 0);
    BOOST_TEST(color.GetA() == 0);

    // When
    color = VWolf::Color(10.5f, 10.5f, 10.5f, 10.5f);

    BOOST_TEST(color.GetR() == 10.5f);
    BOOST_TEST(color.GetG() == 10.5f);
    BOOST_TEST(color.GetB() == 10.5f);
    BOOST_TEST(color.GetA() == 10.5f);

    // When
    color = VWolf::Color(-10.5f, -10.5f, -10.5f, -10.5f);

    BOOST_TEST(color.GetR() == 0);
    BOOST_TEST(color.GetG() == 0);
    BOOST_TEST(color.GetB() == 0);
    BOOST_TEST(color.GetA() == 0);

    // When
    color = VWolf::Color(10.5f, 10.5f, 10.5f, 10.5f);
    VWolf::Color otherColor(color);

    // Then
    BOOST_TEST(otherColor.GetR() == 10.5f);
    BOOST_TEST(otherColor.GetG() == 10.5f);
    BOOST_TEST(otherColor.GetB() == 10.5f);
    BOOST_TEST(otherColor.GetA() == 10.5f);

    // When
    VWolf::Color moveColor(std::move(color));

    // Then
    BOOST_TEST(moveColor.GetR() == 10.5f);
    BOOST_TEST(moveColor.GetG() == 10.5f);
    BOOST_TEST(moveColor.GetB() == 10.5f);
    BOOST_TEST(moveColor.GetA() == 10.5f);
    BOOST_TEST(color.GetR() == 0);
    BOOST_TEST(color.GetG() == 0);
    BOOST_TEST(color.GetB() == 0);
    BOOST_TEST(color.GetA() == 0);
}

BOOST_AUTO_TEST_CASE(ColorComparison) {
    // Given
    VWolf::Color color(10, 10, 10, 10);
    VWolf::Color color2(10, 10, 10, 10);
    VWolf::Color color3(20, 10, 10, 20);
    VWolf::Color Color(10, 20, 10, 20);
    VWolf::Color color5(30, 30, 30, 30);

    // When
    bool isSame = color == color2;

    // Then
    BOOST_TEST(isSame == true);

    // When
    isSame = color == color3;

    // Then
    BOOST_TEST(isSame == false);

    // When
    isSame = color == Color;

    // Then
    BOOST_TEST(isSame == false);

    // When
    bool isDifferent = color != color5;

    // Then
    BOOST_TEST(isDifferent == true);

    // When
    isDifferent = color != color3;

    // Then
    BOOST_TEST(isDifferent == true);

    // When
    isDifferent = color != Color;

    // Then
    BOOST_TEST(isDifferent == true);

    // When
    isDifferent = color != color2;

    // Then
    BOOST_TEST(isDifferent == false);

    // When
    isSame = color == VWolf::Color(10, 10, 10, 10);

    // When
    BOOST_TEST(isSame == true);

    // Then
    isDifferent = color !=  VWolf::Color(12, 12, 12, 12);

    // When
    BOOST_TEST(isDifferent == true);

    // When
    BOOST_TEST(color == VWolf::Color(10, 10, 10, 10));
    BOOST_TEST(color != VWolf::Color(12, 12, 12, 12));
}

BOOST_AUTO_TEST_CASE(ColorOperations) {
    // Given
    VWolf::Color color(10, 10, 10, 10);
    VWolf::Color color2(14, 30, 84, 200);

    // When
    VWolf::Color result = color + color2;

    // Then
    BOOST_TEST(result == VWolf::Color(24, 40, 94, 210));

    // When
    result = color - color2;

    // Then
    BOOST_TEST(result == VWolf::Color(-4, -20, -74, -190));

    // When/Then
    BOOST_TEST((color + color2) == VWolf::Color(24, 40, 94, 210));
    BOOST_TEST((color - color2) == VWolf::Color(-4, -20, -74, -190));

    // When
    result = color * color2;

    // Then
    BOOST_TEST(result == VWolf::Color(140, 300, 840, 2000));

    result = result / color2;

    // Then
    BOOST_TEST(result == VWolf::Color(10, 10, 10, 10));

    // When/Then
    BOOST_TEST((color * color2) == VWolf::Color(140, 300, 840, 2000));
    BOOST_TEST((color2 / VWolf::Color(2, 2, 2, 2)) == VWolf::Color(7, 15, 42, 100));

    // When/Then
    BOOST_TEST(color2[0] == 14);
    BOOST_TEST(color2[1] == 30);
    BOOST_TEST(color2[2] == 84);
    BOOST_TEST(color2[3] == 200);
    BOOST_TEST(color2[4] == std::numeric_limits<float>::max());
}

BOOST_AUTO_TEST_CASE(ColorConstants) {
    BOOST_TEST(VWolf::Color::black == VWolf::Color(0, 0, 0, 1));
    BOOST_TEST(VWolf::Color::blue == VWolf::Color(0, 0, 1, 1));
    BOOST_TEST(VWolf::Color::clear == VWolf::Color(0, 0, 0, 0));
    BOOST_TEST(VWolf::Color::cyan == VWolf::Color(0, 1, 1, 1));
    BOOST_TEST(VWolf::Color::gray == VWolf::Color(0.5f, 0.5f, 0.5f, 1));
    BOOST_TEST(VWolf::Color::green == VWolf::Color(0, 1, 0, 1));
    BOOST_TEST(VWolf::Color::grey == VWolf::Color(0.5f, 0.5f, 0.5f, 1));
    BOOST_TEST(VWolf::Color::magenta == VWolf::Color(1, 0, 1, 1));
    BOOST_TEST(VWolf::Color::red == VWolf::Color(1, 0, 0, 1));
    BOOST_TEST(VWolf::Color::white == VWolf::Color(1, 1, 1, 1));
    BOOST_TEST(VWolf::Color::yellow == VWolf::Color(1, 0.92f, 0.016f, 1));
}

BOOST_AUTO_TEST_CASE(ColorProperties) {
    VWolf::Color color = VWolf::Color(10.5f, 10.5f, 10.5f, 10.5f);

    BOOST_TEST(color.GetR() == 10.5f);
    BOOST_TEST(color.GetG() == 10.5f);
    BOOST_TEST(color.GetB() == 10.5f);
    BOOST_TEST(color.GetA() == 10.5f);

    color.SetR(11);
    color.SetG(12);
    color.SetB(13);
    color.SetA(14);

    BOOST_TEST(color.GetR() == 11);
    BOOST_TEST(color.GetG() == 12);
    BOOST_TEST(color.GetB() == 13);
    BOOST_TEST(color.GetA() == 14);

    float& r = color.GetR();
    float& g = color.GetG();
    float& b = color.GetB();
    float& a = color.GetA();

    r = 20;
    g = 21;
    b = 22;
    a = 23;

    BOOST_TEST(color.GetR() == 20);
    BOOST_TEST(color.GetG() == 21);
    BOOST_TEST(color.GetB() == 22);
    BOOST_TEST(color.GetA() == 23);
}

BOOST_AUTO_TEST_CASE(ColorInstanceMethods) {
    VWolf::Color color = VWolf::Color(0.2f, 0.5f, 0.1f, 1);

    BOOST_TEST(color.GetGrayScale() == 0.364700019f);
    BOOST_TEST(color.GetMaxColorComponent() == 0.5f);
    BOOST_TEST(color.GetGamma() == VWolf::Color(0.484529227f, 0.735356986f, 0.349190205f, 1));
    BOOST_TEST(color.GetLinear() == VWolf::Color(0.0331047624f, 0.214041144f, 0.0100228256f, 1));
}

BOOST_AUTO_TEST_CASE(ColorStaticMethods) {
    VWolf::Color color(10, 10, 10, 10);
    VWolf::Color color2(14, 30, 84, 200);
    // When/Then
    BOOST_TEST(VWolf::Color::Lerp(color, color2, 0.4) == VWolf::Color(1, 1, 1, 1));

    // When/Then
    BOOST_TEST(VWolf::Color::LerpUnclamped(color, color2, 0.4) == VWolf::Color(11.6f, 18, 39.6f, 86));

    // When/Then
    BOOST_TEST(VWolf::Color::HSVToRGB(60, 0.6f, 0.6f) == VWolf::Color(0.600000024f, 0.600000024f, 0.24000001f, 1));

    // When
    VWolf::Color color3 = VWolf::Color(0.600000024f, 0.600000024f, 0.24000001f, 1);
    float h, s, v;
    VWolf::Color::RGBToHSV(color3, h, s, v);
    BOOST_TEST(h == 60.0f);
    BOOST_TEST(s == 0.6f);
    BOOST_TEST(v == 0.6f);
}
