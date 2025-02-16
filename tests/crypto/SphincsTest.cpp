#include "crypto/signature/Sphincs.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace quids {
namespace crypto {
namespace test {

class SphincsTest : public ::testing::Test {
protected:
    void SetUp() override {
        signer = std::make_unique<SphincsPlus>();
    }

    std::unique_ptr<SphincsPlus> signer;
};

TEST_F(SphincsTest, GenerateKeyPair) {
    ASSERT_NO_THROW(signer->generateKeyPair());
    
    auto publicKey = signer->getPublicKey();
    auto privateKey = signer->getPrivateKey();
    
    EXPECT_FALSE(publicKey.empty());
    EXPECT_FALSE(privateKey.empty());
    EXPECT_EQ(publicKey.size(), signer->getPublicKeySize());
    EXPECT_EQ(privateKey.size(), signer->getPrivateKeySize());
}

TEST_F(SphincsTest, SignAndVerify) {
    signer->generateKeyPair();
    auto publicKey = signer->getPublicKey();
    
    std::vector<uint8_t> message = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
    
    // Sign the message
    auto signature = signer->sign(message);
    EXPECT_FALSE(signature.empty());
    EXPECT_EQ(signature.size(), signer->getSignatureSize());
    
    // Verify the signature
    bool isValid = signer->verify(message, signature, publicKey);
    EXPECT_TRUE(isValid);
}

TEST_F(SphincsTest, VerifyModifiedMessageFails) {
    signer->generateKeyPair();
    auto publicKey = signer->getPublicKey();
    
    std::vector<uint8_t> message = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
    auto signature = signer->sign(message);
    
    // Modify the message
    message[0] = 'h';
    
    // Verification should fail
    bool isValid = signer->verify(message, signature, publicKey);
    EXPECT_FALSE(isValid);
}

TEST_F(SphincsTest, VerifyModifiedSignatureFails) {
    signer->generateKeyPair();
    auto publicKey = signer->getPublicKey();
    
    std::vector<uint8_t> message = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
    auto signature = signer->sign(message);
    
    // Modify the signature
    if (!signature.empty()) {
        signature[0] ^= 0x01;
    }
    
    // Verification should fail
    bool isValid = signer->verify(message, signature, publicKey);
    EXPECT_FALSE(isValid);
}

TEST_F(SphincsTest, CheckAlgorithmInfo) {
    EXPECT_EQ(signer->getName(), "SPHINCS+");
    EXPECT_EQ(signer->getSecurityLevel(), 256);
    EXPECT_EQ(signer->getVariant(), "SHA2-256f-256");
}

} // namespace test
} // namespace crypto
} // namespace quids 