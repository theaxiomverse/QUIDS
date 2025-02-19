#include <gtest/gtest.h>
#include "blockchain/Address.hpp"

using namespace quids::blockchain;

TEST(AddressTest, ConstructorWithValidAddress) {
std::string valid_address = "qu_0x1234567890abcdef1234567890abcdef12345678";
Address address(valid_address);
EXPECT_EQ(address.getAddress(), valid_address);
}

TEST(AddressTest, ConstructorWithLocationHash) {
std::string valid_address = "qu_0x1234567890abcdef1234567890abcdef12345678";
std::array<uint8_t, 32> location_hash = {0};
Address address(valid_address, location_hash);
EXPECT_EQ(address.getAddress(), valid_address);
}

TEST(AddressTest, Serialize) {
std::string valid_address = "qu_0x1234567890abcdef1234567890abcdef12345678";
std::array<uint8_t, 32> location_hash = {0};
Address address(valid_address, location_hash);
auto serialized = address.serialize();
EXPECT_EQ(serialized.size(), valid_address.size() + location_hash.size());
}

TEST(AddressTest, DeserializeValidData) {
std::string valid_address = "qu_0x1234567890abcdef1234567890abcdef12345678";
std::array<uint8_t, 32> location_hash = {0};
Address address(valid_address, location_hash);
auto serialized = address.serialize();
auto deserialized = Address::deserialize(serialized);
ASSERT_TRUE(deserialized.has_value());
EXPECT_EQ(deserialized->getAddress(), valid_address);
}

TEST(AddressTest, DeserializeInvalidData) {
std::vector<uint8_t> invalid_data(10, 0);
auto deserialized = Address::deserialize(invalid_data);
EXPECT_FALSE(deserialized.has_value());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}