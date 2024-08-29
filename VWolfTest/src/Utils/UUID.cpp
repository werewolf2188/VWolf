//
//  UUID.cpp
//  VWolfTest
//
//  Created by Enrique Ricalde on 8/29/24.
//

#include <boost/test/unit_test.hpp>
#include <VWolf.h>

BOOST_AUTO_TEST_CASE(UUIDInitializer)
{
    VWolf::UUID uuid = VWolf::UUID::Empty;

    BOOST_TEST(uuid.IsEmpty());

    uuid = VWolf::UUID::NewUUID();

    BOOST_TEST(uuid.IsEmpty() == false);

    uuid = VWolf::UUID::Parse("81a130d2-502f-4cf1-a376-63edeb000e9f");

    BOOST_TEST(uuid.IsEmpty() == false);
    BOOST_TEST(uuid.ToString() == std::string("81a130d2-502f-4cf1-a376-63edeb000e9f"));

    bool parsed = VWolf::UUID::TryParse("81a130d1-504f-4cf1-a366-63edeb000e9f", uuid);

    BOOST_TEST(parsed);
    BOOST_TEST(uuid.ToString() == std::string("81a130d1-504f-4cf1-a366-63edeb000e9f"));

    bool failed = false;

    try {
        uuid = VWolf::UUID::Parse("81a13Nd2-502f-4cf1-a376-63edeb000e9f");
    } catch (std::exception& e) { failed = true; }

    BOOST_TEST(failed);
    BOOST_TEST(uuid.ToString() == std::string("81a130d1-504f-4cf1-a366-63edeb000e9f"));

    parsed = VWolf::UUID::TryParse("81a13Nd2-502f-4cf1-a376-63edeb000e9f", uuid);

    BOOST_TEST(parsed == false);
    BOOST_TEST(uuid.ToString() == std::string("81a130d1-504f-4cf1-a366-63edeb000e9f"));
}

BOOST_AUTO_TEST_CASE(UUIDComparison)
{
    VWolf::UUID uuid1 = VWolf::UUID::Empty;
    VWolf::UUID uuid2 = VWolf::UUID::Parse("81a130d2-502f-4cf1-a376-63edeb000e9f");
    VWolf::UUID uuid3 = VWolf::UUID::Parse("81a130d2-502f-4cf1-a376-63edeb000e9f");
    VWolf::UUID uuid4 = VWolf::UUID::Parse("81a130d1-504f-4cf1-a366-63edeb000e9f");
    VWolf::UUID uuid5 = uuid3;

    BOOST_TEST(uuid1 == VWolf::UUID::Empty);
    BOOST_TEST(uuid2 != VWolf::UUID::Empty);
    BOOST_TEST(uuid2 == uuid3);
    BOOST_TEST(uuid2 != uuid4);
    BOOST_TEST(uuid3 == uuid5);
}

BOOST_AUTO_TEST_CASE(UUIDGetData)
{
    VWolf::UUID uuid2 = VWolf::UUID::Parse("81a130d2-502f-4cf1-a376-63edeb000e9f");
    std::array<uint8_t, 16> array = uuid2.GetArray();
    const uint8_t* data = uuid2.GetData();

    for (uint32_t index = 0; index < 16; index++) {
        BOOST_TEST(array[index] == data[index]);
    }
}
