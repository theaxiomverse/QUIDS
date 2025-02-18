# CPU features detection

include(CheckCXXSourceRuns)

# Check for AVX2 support
check_cxx_source_runs("
#include <immintrin.h>
int main() {
    __m256i a = _mm256_set1_epi32(1);
    __m256i b = _mm256_set1_epi32(2);
    __m256i c = _mm256_add_epi32(a, b);
    return 0;
}
" HAVE_AVX2)

# Check for FMA support
check_cxx_source_runs("
#include <immintrin.h>
int main() {
    __m256 a = _mm256_set1_ps(1.0f);
    __m256 b = _mm256_set1_ps(2.0f);
    __m256 c = _mm256_set1_ps(3.0f);
    __m256 result = _mm256_fmadd_ps(a, b, c);
    return 0;
}
" HAVE_FMA)

# Configure CPU feature flags
if(HAVE_AVX2)
    add_compile_definitions(HAVE_AVX2)
    if(NOT MSVC)
        add_compile_options(-mavx2)
    endif()
endif()

if(HAVE_FMA)
    add_compile_definitions(HAVE_FMA)
    if(NOT MSVC)
        add_compile_options(-mfma)
    endif()
endif()

# Print CPU feature status
message(STATUS "CPU Features:")
message(STATUS "  AVX2: ${HAVE_AVX2}")
message(STATUS "  FMA: ${HAVE_FMA}")