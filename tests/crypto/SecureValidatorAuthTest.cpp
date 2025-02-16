#include "crypto/auth/SecureValidatorAuth.hpp"
#include <gtest/gtest.h>

namespace quids {
namespace crypto {
namespace test {

class SecureValidatorAuthTest : public ::testing::Test {
protected:
    void SetUp() override {
        client_auth = std::make_unique<SecureValidatorAuth>();
        server_auth = std::make_unique<SecureValidatorAuth>();
    }

    std::unique_ptr<SecureValidatorAuth> client_auth;
    std::unique_ptr<SecureValidatorAuth> server_auth;
    
    const std::string test_identifier = "validator1";
    const std::string test_password = "secure_password123";
};

TEST_F(SecureValidatorAuthTest, GenerateVerifier) {
    auto verifier = client_auth->generateVerifier(test_identifier, test_password);
    
    EXPECT_FALSE(verifier.salt.empty());
    EXPECT_FALSE(verifier.verifier.empty());
    EXPECT_EQ(verifier.salt.size(), 32); // Check salt size
}

TEST_F(SecureValidatorAuthTest, SuccessfulAuthentication) {
    // Generate verifier
    auto verifier = client_auth->generateVerifier(test_identifier, test_password);
    
    // Start client authentication
    auto client_session = client_auth->startClientAuth(test_identifier, test_password);
    EXPECT_FALSE(client_session.public_ephemeral.empty());
    
    // Start server authentication
    auto server_session = server_auth->startServerAuth(test_identifier, verifier, client_session.public_ephemeral);
    EXPECT_FALSE(server_session.public_ephemeral.empty());
    EXPECT_FALSE(server_session.proof.empty());
    
    // Verify server's proof on client side
    EXPECT_TRUE(client_auth->verifyServerProof(server_session.proof));
    
    // Get session keys
    auto client_key = client_auth->getSessionKey();
    auto server_key = server_auth->getSessionKey();
    
    ASSERT_TRUE(client_key.has_value());
    ASSERT_TRUE(server_key.has_value());
    EXPECT_EQ(client_key.value(), server_key.value());
}

TEST_F(SecureValidatorAuthTest, FailedAuthenticationWrongPassword) {
    // Generate verifier with correct password
    auto verifier = client_auth->generateVerifier(test_identifier, test_password);
    
    // Start client authentication with wrong password
    auto client_session = client_auth->startClientAuth(test_identifier, "wrong_password");
    
    // Start server authentication
    auto server_session = server_auth->startServerAuth(test_identifier, verifier, client_session.public_ephemeral);
    
    // Verification should fail
    EXPECT_FALSE(client_auth->verifyServerProof(server_session.proof));
}

TEST_F(SecureValidatorAuthTest, FailedAuthenticationModifiedEphemeral) {
    // Generate verifier
    auto verifier = client_auth->generateVerifier(test_identifier, test_password);
    
    // Start client authentication
    auto client_session = client_auth->startClientAuth(test_identifier, test_password);
    
    // Modify client's public ephemeral
    if (!client_session.public_ephemeral.empty()) {
        client_session.public_ephemeral[0] ^= 0x01;
    }
    
    // Start server authentication
    auto server_session = server_auth->startServerAuth(test_identifier, verifier, client_session.public_ephemeral);
    
    // Verification should fail
    EXPECT_FALSE(client_auth->verifyServerProof(server_session.proof));
}

TEST_F(SecureValidatorAuthTest, SessionKeyAvailability) {
    // Before authentication, session key should not be available
    EXPECT_FALSE(client_auth->getSessionKey().has_value());
    EXPECT_FALSE(server_auth->getSessionKey().has_value());
    
    // After successful authentication, session key should be available
    auto verifier = client_auth->generateVerifier(test_identifier, test_password);
    auto client_session = client_auth->startClientAuth(test_identifier, test_password);
    auto server_session = server_auth->startServerAuth(test_identifier, verifier, client_session.public_ephemeral);
    
    EXPECT_TRUE(client_auth->verifyServerProof(server_session.proof));
    
    EXPECT_TRUE(client_auth->getSessionKey().has_value());
    EXPECT_TRUE(server_auth->getSessionKey().has_value());
}

} // namespace test
} // namespace crypto
} // namespace quids 