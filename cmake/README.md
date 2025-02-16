# CMake Configuration

The CMake build system is configured using modern CMake practices with the following key features:

- Target-based configuration using modern CMake commands
- Proper dependency management through Find modules
- Consistent style across all Find modules
- Optional components properly handled (e.g. OQS provider)
- Platform-specific paths handled via Find modules
- Proper interface management with PRIVATE/PUBLIC visibility
- Organized target dependencies ensuring correct build order
- Standardized installation configuration

## Dependencies

All external dependencies are handled through Find modules or CMake's built-in package finding:

- System libraries (OpenSSL, ZLIB, etc.)
- Custom libraries (BLAKE3, RocksDB, etc.)
- Third-party packages (Boost, Eigen, etc.)
- Optional components (OQS provider)

## Build Targets 

The project is organized into several key library targets:

- quids_core: Main library 
- quids_network: Network functionality
- quids_blockchain: Blockchain components
- quids_evm: EVM implementation
- quids_quantum: Quantum operations
- quids_common: Shared utilities

## Testing

Test targets are configured to:
- Support unit tests via GTest
- Include benchmarking capabilities
- Handle optional OQS integration
- Provide optimized builds for benchmarks