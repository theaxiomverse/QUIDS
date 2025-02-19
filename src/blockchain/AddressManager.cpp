#include "blockchain/AddressManager.hpp"
#include "blockchain/Account.hpp"
#include <blake3.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace quids::blockchain {

    struct AddressManager::Impl {
        std::unordered_map<std::string, Account> accounts;
        std::unordered_map<std::string, quantum::QuantumState> quantum_states;
    };

    AddressManager::AddressManager() noexcept : impl_(std::make_unique<Impl>()) {}

    AddressManager::~AddressManager() = default;
    AddressManager::AddressManager(AddressManager&&) noexcept = default;
    AddressManager& AddressManager::operator=(AddressManager&&) noexcept = default;

    std::vector<uint8_t> AddressManager::LocationData::serialize() const {
        nlohmann::json j;
        j["country"] = country;
        j["city"] = city;
        j["latitude"] = latitude;
        j["longitude"] = longitude;

        std::string serialized = j.dump();
        return std::vector<uint8_t>(serialized.begin(), serialized.end());
    }

    std::optional<AddressManager::LocationData> AddressManager::LocationData::deserialize(
            const std::vector<uint8_t>& data
    ) {
        try {
            std::string json_str(data.begin(), data.end());
            auto j = nlohmann::json::parse(json_str);

            LocationData location;
            location.country = j["country"].get<std::string>();
            location.city = j["city"].get<std::string>();
            location.latitude = j["latitude"].get<double>();
            location.longitude = j["longitude"].get<double>();

            return location;
        } catch (const std::exception& e) {
            spdlog::error("Failed to deserialize location data: {}", e.what());
            return std::nullopt;
        }
    }

    std::optional<std::string> AddressManager::generateAddress(
            const LocationData& location,
            const std::string& purpose
    ) {
        try {
            auto location_vector = createLocationVector(location);
            auto location_hash = computeLocationHash(location);

            auto commitment = generateZKPCommitment(location_vector, location_hash);
            auto proof = generateZKPProof(location_vector, commitment);

            AddressComponents components{
                    location_hash,
                    commitment,
                    proof,
                    purpose
            };

            return encodeAddress(components);
        } catch (const std::exception& e) {
            spdlog::error("Failed to generate address: {}", e.what());
            return std::nullopt;
        }
    }

    bool AddressManager::verifyAddress(const std::string& address) {
        // 1. Check basic format requirements
        if (address.length() != ADDRESS_LENGTH ||
            !address.starts_with(ADDRESS_PREFIX)) {
            return false;
        }

        // 2. Decode address components
        auto components = decodeAddress(address);
        if (!components) {
            return false;
        }

        // 3. Validate component sizes
        if (components->location_hash.size() != 32 ||
            components->zkp_commitment.empty() ||
            components->zkp_proof.empty()) {
            return false;
        }

        // 4. Validate purpose string
        if (components->purpose != "EOA" &&
            components->purpose != "CONTRACT") {
            return false;
        }

        // 5. Verify hex encoding after prefix
        std::string hex_part = address.substr(strlen(ADDRESS_PREFIX));
        if (!std::all_of(hex_part.begin(), hex_part.end(), ::isxdigit)) {
            return false;
        }

        return true;
    }

    bool AddressManager::verifyLocation(
            const std::string& address,
            const LocationData& location
    ) {
        auto components = decodeAddress(address);
        if (!components) {
            return false;
        }

        auto computed_hash = computeLocationHash(location);
        return computed_hash == components->location_hash;
    }

    AddressManager::VSSScheme AddressManager::generateShares(
            const LocationData& location,
            size_t num_shares,
            size_t threshold
    ) {
        try {
            if (num_shares < MIN_SHARES || num_shares > MAX_SHARES || threshold > num_shares) {
                spdlog::error("Invalid VSS parameters: shares={}, threshold={}", num_shares, threshold);
                throw std::invalid_argument("Invalid share parameters");
            }

            // Convert location to quantum state vector
            auto location_vector = createLocationVector(location);
            if (location_vector.size() != LOCATION_VECTOR_SIZE) {
                spdlog::error("Invalid location vector size: {}", location_vector.size());
                throw std::runtime_error("Invalid location vector");
            }

            quantum::QuantumState state(LOCATION_VECTOR_SIZE);
            try {
                for (size_t i = 0; i < location_vector.size(); i++) {
                    state.setAmplitude(i, std::complex<double>(location_vector[i], 0.0));
                }
            } catch (const std::exception& e) {
                spdlog::error("Failed to set quantum state: {}", e.what());
                throw;
            }

            // Create polynomial coefficients
            std::vector<double> coefficients(POLYNOMIAL_DEGREE);
            coefficients[0] = location_vector[0]; // x-coordinate
            coefficients[1] = location_vector[1]; // y-coordinate

            // Generate random coefficients for higher degrees
            std::random_device rd;
            std::mt19937_64 gen(rd());
            std::uniform_real_distribution<double> dist(-1.0, 1.0);

            for (size_t i = 2; i < POLYNOMIAL_DEGREE; i++) {
                coefficients[i] = dist(gen);
            }

            // Create polynomial
            auto polynomial = createPolynomial(coefficients, threshold - 1);

            // Generate shares
            VSSScheme scheme;
            scheme.threshold = threshold;
            scheme.shares.reserve(num_shares);

            for (size_t i = 1; i <= num_shares; i++) {
                std::complex<double> x(static_cast<double>(i), 0.0);
                auto share_data = evaluatePolynomial(polynomial, x);

                VSSShare share;
                share.data = share_data;
                share.index = i;
                share.commitment = computeShareCommitment(share_data);

                scheme.shares.push_back(share);
            }

            // Compute root commitment
            scheme.root_commitment = computeShareCommitment(polynomial);

            spdlog::debug("Generated {} shares with threshold {}", num_shares, threshold);
            return scheme;
        } catch (const std::exception& e) {
            spdlog::error("Failed to generate shares: {}", e.what());
            throw;
        }
    }

    bool AddressManager::verifyShare(
            const VSSShare& share,
            const std::array<uint8_t, 32>& root_commitment
    ) {
        try {
            // Validate share data
            if (share.data.empty() || share.data.size() != LOCATION_VECTOR_SIZE) {
                spdlog::error("Invalid share data size: {}", share.data.size());
                return false;
            }

            if (share.index == 0) {
                spdlog::error("Invalid share index: 0");
                return false;
            }

            // Verify share commitment
            auto computed_commitment = computeShareCommitment(share.data);
            if (computed_commitment != share.commitment) {
                spdlog::warn("Share commitment verification failed");
                return false;
            }

            // Verify against root commitment using quantum state verification
            quantum::QuantumState share_state(share.data.size());
            for (size_t i = 0; i < share.data.size(); i++) {
                share_state.setAmplitude(i, share.data[i]);
            }

            // Use QZKP to verify the share against root commitment
            zkp::QZKPGenerator qzkp;
            return qzkp.verify_share(share_state, root_commitment);

            spdlog::debug("Share {} verified successfully", share.index);
            return true;
        } catch (const std::exception& e) {
            spdlog::error("Share verification failed: {}", e.what());
            return false;
        }
    }

    std::optional<AddressManager::LocationData> AddressManager::reconstructLocation(
            const std::vector<VSSShare>& shares,
            size_t threshold
    ) {
        try {
            if (shares.empty()) {
                spdlog::error("Empty shares vector");
                return std::nullopt;
            }

            if (shares.size() < threshold) {
                spdlog::error("Insufficient shares: {} < {}", shares.size(), threshold);
                return std::nullopt;
            }

            // Validate share data consistency
            for (const auto& share : shares) {
                if (share.data.size() != LOCATION_VECTOR_SIZE) {
                    spdlog::error("Invalid share data size: {}", share.data.size());
                    return std::nullopt;
                }
            }

            std::vector<std::complex<double>> reconstructed(LOCATION_VECTOR_SIZE, std::complex<double>(0.0, 0.0));

            // Pre-calculate denominators for Lagrange basis
            std::vector<std::complex<double>> denominators(threshold);
            for (size_t i = 0; i < threshold; i++) {
                std::complex<double> denom(1.0, 0.0);
                for (size_t j = 0; j < threshold; j++) {
                    if (i != j) {
                        denom *= std::complex<double>(
                                static_cast<double>(shares[i].index - shares[j].index), 0.0
                        );
                    }
                }
                denominators[i] = denom;
            }

            // Perform Lagrange interpolation
            for (size_t i = 0; i < threshold; i++) {
                std::complex<double> numerator(1.0, 0.0);
                for (size_t j = 0; j < threshold; j++) {
                    if (i != j) {
                        numerator *= std::complex<double>(-static_cast<double>(shares[j].index), 0.0);
                    }
                }

                std::complex<double> lagrange_basis = numerator / denominators[i];

                // Vectorized multiplication for better performance
                for (size_t k = 0; k < LOCATION_VECTOR_SIZE; k++) {
                    reconstructed[k] += shares[i].data[k] * lagrange_basis;
                }
            }

            // Convert reconstructed values back to LocationData
            LocationData location{
                    std::real(reconstructed[0]),
                    std::real(reconstructed[1])
            };

            // Verify reconstruction using quantum state comparison
            quantum::QuantumState reconstructed_state(LOCATION_VECTOR_SIZE);
            for (size_t i = 0; i < LOCATION_VECTOR_SIZE; i++) {
                reconstructed_state.setAmplitude(i, reconstructed[i]);
            }

            if (!reconstructed_state.isValid()) {
                spdlog::error("Invalid quantum state reconstruction");
                return std::nullopt;
            }

            // Validate reconstructed coordinates
            constexpr double MAX_LATITUDE = 90.0;
            constexpr double MAX_LONGITUDE = 180.0;

            if (std::isnan(location.latitude) || std::isnan(location.longitude) ||
                std::abs(location.latitude) > MAX_LATITUDE ||
                std::abs(location.longitude) > MAX_LONGITUDE) {
                spdlog::error("Invalid reconstructed coordinates: lat={}, lon={}",
                              location.latitude, location.longitude);
                return std::nullopt;
            }

            spdlog::debug("Location reconstructed successfully from {} shares", shares.size());
            return location;
        } catch (const std::exception& e) {
            spdlog::error("Location reconstruction failed: {}", e.what());
            return std::nullopt;
        }
    }

// Helper methods implementation
    std::vector<std::complex<double>> AddressManager::createPolynomial(
            const std::vector<double>& coefficients,
            size_t degree
    ) {
        std::vector<std::complex<double>> poly(degree + 1);
        for (size_t i = 0; i <= degree && i < coefficients.size(); i++) {
            poly[i] = std::complex<double>(coefficients[i], 0.0);
        }
        return poly;
    }

    std::vector<std::complex<double>> AddressManager::evaluatePolynomial(
            const std::vector<std::complex<double>>& poly,
            std::complex<double> x
    ) {
        std::vector<std::complex<double>> result(LOCATION_VECTOR_SIZE);
        std::complex<double> x_power(1.0, 0.0);

        for (const auto& coeff : poly) {
            for (size_t i = 0; i < LOCATION_VECTOR_SIZE; i++) {
                result[i] += coeff * x_power;
            }
            x_power *= x;
        }

        return result;
    }

    std::array<uint8_t, 32> AddressManager::computeShareCommitment(
            const std::vector<std::complex<double>>& share_data
    ) {
        std::array<uint8_t, 32> commitment;
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);

        // Hash real and imaginary parts of each complex number
        for (const auto& value : share_data) {
            double real_part = std::real(value);
            double imag_part = std::imag(value);
            blake3_hasher_update(&hasher, &real_part, sizeof(double));
            blake3_hasher_update(&hasher, &imag_part, sizeof(double));
        }

        blake3_hasher_finalize(&hasher, commitment.data(), commitment.size());
        return commitment;
    }

// Convert complex vector to bytes
    std::vector<uint8_t> serialize_complex_vector(const std::vector<std::complex<double>>& vec) {
        std::vector<uint8_t> bytes;
        bytes.reserve(vec.size() * sizeof(std::complex<double>));

        for (const auto& c : vec) {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(&c);
            bytes.insert(bytes.end(), data, data + sizeof(std::complex<double>));
        }

        return bytes;
    }

    bool AddressManager::create_account(const std::string& address, uint64_t initial_balance) {
        if (impl_->accounts.find(address) != impl_->accounts.end()) {
            return false;
        }
        impl_->accounts.emplace(address, Account(address, initial_balance));
        return true;
    }




bool AddressManager::delete_account(const std::string& address) {
    return impl_->accounts.erase(address) > 0;
}

bool AddressManager::transfer(const std::string& from, const std::string& to, uint64_t amount) {
    auto from_it = impl_->accounts.find(from);
    auto to_it = impl_->accounts.find(to);
    
    if (from_it == impl_->accounts.end() || to_it == impl_->accounts.end()) {
        return false;
    }
    
    if (from_it->second.balance < amount) {
        return false;
    }
    
    from_it->second.balance -= amount;
    to_it->second.balance += amount;
    return true;
}

uint64_t AddressManager::get_balance(const std::string& address) const {
    auto it = impl_->accounts.find(address);
    if (it == impl_->accounts.end()) {
        return 0;
    }
    return it->second.balance;
}

bool AddressManager::set_balance(const std::string& address, uint64_t balance) {
    auto it = impl_->accounts.find(address);
    if (it == impl_->accounts.end()) {
        return false;
    }
    it->second.balance = balance;
    return true;
}

bool AddressManager::deploy_code(const std::string& address, const std::vector<uint8_t>& code) {
    auto it = impl_->accounts.find(address);
    if (it == impl_->accounts.end()) {
        return false;
    }
    it->second.code = code;
    return true;
}

std::vector<uint8_t> AddressManager::get_code(const std::string& address) const {
    auto it = impl_->accounts.find(address);
    if (it == impl_->accounts.end()) {
        return std::vector<uint8_t>();
    }
    return it->second.code;
}

uint64_t AddressManager::get_nonce(const std::string& address) const {
    auto it = impl_->accounts.find(address);
    if (it == impl_->accounts.end()) {
        return 0;
    }
    return it->second.nonce;
}

bool AddressManager::increment_nonce(const std::string& address) {
    auto it = impl_->accounts.find(address);
    if (it == impl_->accounts.end()) {
        return false;
    }
    it->second.nonce++;
    return true;
}

bool AddressManager::account_exists(const std::string& address) const {
    return impl_->accounts.find(address) != impl_->accounts.end();
}

bool AddressManager::is_contract_account(const std::string& address) const {
    auto it = impl_->accounts.find(address);
    if (it == impl_->accounts.end()) {
        return false;
    }
    return !it->second.code.empty();
}

bool AddressManager::register_quantum_state(
    const std::string& address,
    const quantum::QuantumState& state
) {
    if (!account_exists(address)) {
        return false;
    }
    impl_->quantum_states[address] = state;
    return true;
}

bool AddressManager::verify_quantum_state(
    const std::string& address,
    const quantum::QuantumState& state
) const {
    auto it = impl_->quantum_states.find(address);
    if (it == impl_->quantum_states.end()) {
        return false;
    }
    return it->second == state;
}

bool AddressManager::store_proof(
    const std::string& address,
    const zkp::QZKPGenerator::Proof& proof
) {
    if (!account_exists(address)) {
        return false;
    }
    impl_->stored_proofs[address] = proof;
    return true;
}

bool AddressManager::verify_proof(
    const quantum::QuantumState& state,
    const zkp::QZKPGenerator::Proof& proof
) const {
    return impl_->qzkp.verify_proof(proof, state);
}

// Private helper methods
std::array<uint8_t, 32> AddressManager::computeLocationHash(const LocationData& location) {
    std::array<uint8_t, 32> hash;
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    
    auto data = location.serialize();
    blake3_hasher_update(&hasher, data.data(), data.size());
    blake3_hasher_finalize(&hasher, hash.data(), hash.size());
    
    return hash;
}

std::vector<double> AddressManager::createLocationVector(const LocationData& location) {
    std::vector<double> vec(LOCATION_VECTOR_SIZE);
    vec[0] = location.latitude;
    vec[1] = location.longitude;
    // Additional location features can be added here
    return vec;
}

std::vector<uint8_t> AddressManager::generateZKPCommitment(
    const std::vector<double>& location_vector,
    const std::array<uint8_t, 32>& identifier
) {
    // Implement ZKP commitment generation
    return std::vector<uint8_t>(32); // Placeholder
}

    std::vector<uint8_t> AddressManager::generateZKProof(
            const Address& address,
            const LocationData& location,
            const std::vector<VSSShare>& shares
    ) {
        constexpr size_t PROOF_SIZE = 64;
        std::vector<uint8_t> proof(PROOF_SIZE);

        try {
            blake3_hasher hasher;
            blake3_hasher_init(&hasher);

            // Add address data
            auto address_str = address.toString();
            blake3_hasher_update(&hasher, address_str.data(), address_str.size());

            // Add location data
            blake3_hasher_update(&hasher, &location.latitude, sizeof(double));
            blake3_hasher_update(&hasher, &location.longitude, sizeof(double));

            // Add shares data
            for (const auto& share : shares) {
                blake3_hasher_update(&hasher, &share.index, sizeof(size_t));
                for (const auto& value : share.data) {
                    double real_part = std::real(value);
                    double imag_part = std::imag(value);
                    blake3_hasher_update(&hasher, &real_part, sizeof(double));
                    blake3_hasher_update(&hasher, &imag_part, sizeof(double));
                }
            }

            blake3_hasher_finalize(&hasher, proof.data(), PROOF_SIZE);
            return proof;
        } catch (const std::exception& e) {
            spdlog::error("Failed to generate ZKP proof: {}", e.what());
            return std::vector<uint8_t>(PROOF_SIZE);
        }
    }

    std::string AddressManager::encodeAddress(const AddressComponents& components) {
        try {
            std::string result = ADDRESS_PREFIX;
            result.reserve(std::strlen(ADDRESS_PREFIX) + 40);

            result += components.purpose;

            if (!components.location_hash.empty()) {
                for (uint8_t byte : components.location_hash) {
                    result.push_back(HEX_CHARS[byte >> 4]);
                    result.push_back(HEX_CHARS[byte & 0x0F]);
                }
            }

            // Add checksum
            blake3_hasher hasher;
            blake3_hasher_init(&hasher);
            blake3_hasher_update(&hasher, result.data(), result.size());
            std::array<uint8_t, 4> checksum;
            blake3_hasher_finalize(&hasher, checksum.data(), checksum.size());

            for (uint8_t byte : checksum) {
                result.push_back(HEX_CHARS[byte >> 4]);
                result.push_back(HEX_CHARS[byte & 0x0F]);
            }

            return result;
        } catch (const std::exception& e) {
            spdlog::error("Failed to encode address: {}", e.what());
            return std::string();
        }
    }

    std::optional<AddressManager::AddressComponents> AddressManager::decodeAddress(
            const std::string& address
    ) {
        try {
            // Validate address format
            if (!address.starts_with(ADDRESS_PREFIX)) {
                spdlog::error("Invalid address prefix");
                return std::nullopt;
            }

            // Minimum length check (prefix + purpose + checksum)
            constexpr size_t MIN_LENGTH = ADDRESS_PREFIX.length() + 3 + 8;
            if (address.length() < MIN_LENGTH) {
                spdlog::error("Address too short");
                return std::nullopt;
            }

            // Verify checksum
            const std::string address_without_checksum = address.substr(0, address.length() - 8);
            blake3_hasher hasher;
            blake3_hasher_init(&hasher);
            blake3_hasher_update(&hasher, address_without_checksum.data(), address_without_checksum.size());
            std::array<uint8_t, 4> computed_checksum;
            blake3_hasher_finalize(&hasher, computed_checksum.data(), computed_checksum.size());

            std::string checksum_str = address.substr(address.length() - 8);
            if (!verifyChecksum(checksum_str, computed_checksum)) {
                spdlog::error("Invalid address checksum");
                return std::nullopt;
            }

            // Extract components
            AddressComponents components;

            // Extract purpose (assuming it's the first 3 characters after prefix)
            components.purpose = address.substr(ADDRESS_PREFIX.length(), 3);

            // Extract location hash if present
            const size_t hash_start = ADDRESS_PREFIX.length() + 3;
            const size_t hash_length = address.length() - hash_start - 8; // -8 for checksum
            if (hash_length > 0) {
                components.location_hash = hexToBytes(address.substr(hash_start, hash_length));
            }

            return components;
        } catch (const std::exception& e) {
            spdlog::error("Failed to decode address: {}", e.what());
            return std::nullopt;
        }
    }

    void AddressManager::setQuantumState(const Address& address, quantum::QuantumState state) {
        try {
            if (!state.isValid()) {
                spdlog::error("Attempting to set invalid quantum state for address {}",
                              address.toString());
                return;
            }
            impl_->quantum_states[address.toString()] = std::move(state);
            spdlog::debug("Set quantum state for address {}", address.toString());
        } catch (const std::exception& e) {
            spdlog::error("Failed to set quantum state: {}", e.what());
        }
    }

    / Helper method implementations
    bool AddressManager::verifyChecksum(
            const std::string& checksum_str,
            const std::array<uint8_t, 4>& computed_checksum
    ) const {
        if (checksum_str.length() != 8) return false;

        for (size_t i = 0; i < 4; ++i) {
            if (checksum_str[i*2] != HEX_CHARS[computed_checksum[i] >> 4] ||
                checksum_str[i*2+1] != HEX_CHARS[computed_checksum[i] & 0x0F]) {
                return false;
            }
        }
        return true;
    }

    std::vector<uint8_t> AddressManager::hexToBytes(const std::string& hex) const {
        if (hex.length() % 2 != 0) {
            throw std::invalid_argument("Hex string length must be even");
        }

        std::vector<uint8_t> bytes(hex.length() / 2);
        for (size_t i = 0; i < bytes.size(); ++i) {
            bytes[i] = (hexCharToInt(hex[i*2]) << 4) | hexCharToInt(hex[i*2+1]);
        }
        return bytes;
    }

} // namespace quids::blockchain 