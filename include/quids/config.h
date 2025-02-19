#pragma once

// Compiler feature support
#define HAVE_CONCEPTS
#define HAVE_COROUTINES  
#define HAVE_RANGES

// Version information
#define QUIDS_VERSION "${VERSION_STRING}"
#define QUIDS_GIT_BRANCH "${GIT_BRANCH}"
#define QUIDS_GIT_COMMIT "${GIT_COMMIT_HASH}"

// Build configuration
/* #undef ENABLE_TESTS */
/* #undef ENABLE_BENCHMARKS */
/* #undef USE_SANITIZERS */
