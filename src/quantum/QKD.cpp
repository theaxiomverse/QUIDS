#include "quantum/QKD.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace quantum {

// Constructor
QKD::QKD() : gen(rd()), dis(0, 1) {}

// Generate a random quantum key using the BB84 protocol
std::vector<bool> QKD::generateKey(size_t keyLength) {
    // Step 1: Alice generates random bits and bases
    std::vector<bool> aliceBits = generateRandomBits(keyLength);
    std::vector<bool> aliceBases = generateRandomBases(keyLength);

    // Step 2: Alice sends qubits to Bob using her bases
    std::vector<bool> bobBases = generateRandomBases(keyLength);
    std::vector<bool> bobBits = transmitQubits(aliceBits, bobBases);

    // Step 3: Alice and Bob sift their keys
    std::vector<bool> siftedKey = siftKeys(aliceBases, bobBases, bobBits);

    // Step 4: Perform error correction (simplified)
    std::vector<bool> correctedKey = correctErrors(siftedKey, 0.05); // Assume 5% error rate

    // Step 5: Perform privacy amplification
    std::vector<bool> finalKey = privacyAmplification(correctedKey, keyLength / 2);

    return finalKey;
}

// Simulate the transmission of qubits from Alice to Bob
std::vector<bool> QKD::transmitQubits(const std::vector<bool>& bits, const std::vector<bool>& bases) {
    std::vector<bool> measuredBits;
    for (size_t i = 0; i < bits.size(); ++i) {
        // If bases match, Bob measures the correct bit
        if (bases[i] == (i % 2 == 0)) { // Simplified basis matching
            measuredBits.push_back(bits[i]);
        } else {
            // If bases don't match, Bob's measurement is random
            measuredBits.push_back(dis(gen));
        }
    }
    return measuredBits;
}

// Perform sifting to discard mismatched bases
std::vector<bool> QKD::siftKeys(const std::vector<bool>& aliceBases, const std::vector<bool>& bobBases, const std::vector<bool>& rawKey) {
    std::vector<bool> siftedKey;
    for (size_t i = 0; i < aliceBases.size(); ++i) {
        if (aliceBases[i] == bobBases[i]) {
            siftedKey.push_back(rawKey[i]);
        }
    }
    return siftedKey;
}

// Perform error correction (simplified for demonstration)
std::vector<bool> QKD::correctErrors(const std::vector<bool>& siftedKey, double errorRate) {
    std::vector<bool> correctedKey = siftedKey;
    for (size_t i = 0; i < correctedKey.size(); ++i) {
        if (static_cast<double>(dis(gen)) / RAND_MAX < errorRate) {
            correctedKey[i] = !correctedKey[i]; // Flip the bit to simulate correction
        }
    }
    return correctedKey;
}

// Perform privacy amplification to reduce Eve's knowledge
std::vector<bool> QKD::privacyAmplification(const std::vector<bool>& correctedKey, size_t finalKeyLength) {
    std::vector<bool> finalKey;
    for (size_t i = 0; i < finalKeyLength; ++i) {
        finalKey.push_back(correctedKey[i % correctedKey.size()]); // Simplified amplification
    }
    return finalKey;
}

// Helper function to generate random bits
std::vector<bool> QKD::generateRandomBits(size_t length) {
    std::vector<bool> bits;
    for (size_t i = 0; i < length; ++i) {
        bits.push_back(dis(gen));
    }
    return bits;
}

// Helper function to generate random bases
std::vector<bool> QKD::generateRandomBases(size_t length) {
    std::vector<bool> bases;
    for (size_t i = 0; i < length; ++i) {
        bases.push_back(dis(gen));
    }
    return bases;
}

// Helper function to XOR two vectors
std::vector<bool> QKD::xorVectors(const std::vector<bool>& a, const std::vector<bool>& b) {
    std::vector<bool> result;
    for (size_t i = 0; i < a.size(); ++i) {
        result.push_back(a[i] ^ b[i]);
    }
    return result;
}

} // namespace quantum