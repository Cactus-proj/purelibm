cmake_minimum_required(VERSION 3.22)

project(
  purelibm
  VERSION 0.1.0
  DESCRIPTION "Readable correctly rounded libm reference implementation"
  LANGUAGES C
)

include(CheckCCompilerFlag)
include(CTest)
find_package(PkgConfig QUIET)

option(PURELIBM_REQUIRE_MPFR "Require MPFR for oracle tests" ON)
option(PURELIBM_REQUIRE_CORE_MATH "Require deps/core-math to be present" OFF)
set(PURELIBM_CORE_MATH_DIR "${PROJECT_SOURCE_DIR}/deps/core-math" CACHE PATH "Path to CORE-MATH source tree")

check_c_compiler_flag("-std=c23" PURELIBM_HAS_STD_C23)
if(PURELIBM_HAS_STD_C23)
  set(PURELIBM_C_STANDARD_FLAG "-std=c23")
else()
  check_c_compiler_flag("-std=c2x" PURELIBM_HAS_STD_C2X)
  if(PURELIBM_HAS_STD_C2X)
    set(PURELIBM_C_STANDARD_FLAG "-std=c2x")
    message(STATUS "Compiler does not accept -std=c23; using -std=c2x")
  else()
    message(FATAL_ERROR "Compiler must support -std=c23 or -std=c2x")
  endif()
endif()

set(PURELIBM_STRICT_COMPILE_OPTIONS
  "${PURELIBM_C_STANDARD_FLAG}"
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wdouble-promotion
  -fno-fast-math
  -ffp-contract=off
)

if(PkgConfig_FOUND)
  pkg_check_modules(MPFR IMPORTED_TARGET mpfr)
endif()

if(PURELIBM_REQUIRE_MPFR AND NOT MPFR_FOUND)
  message(FATAL_ERROR "MPFR is required. Install MPFR development files or set PURELIBM_REQUIRE_MPFR=OFF.")
endif()

if(EXISTS "${PURELIBM_CORE_MATH_DIR}/src")
  set(PURELIBM_CORE_MATH_FOUND TRUE)
else()
  set(PURELIBM_CORE_MATH_FOUND FALSE)
endif()

if(PURELIBM_REQUIRE_CORE_MATH AND NOT PURELIBM_CORE_MATH_FOUND)
  message(FATAL_ERROR "CORE-MATH is required. Initialize deps/core-math or set PURELIBM_CORE_MATH_DIR.")
endif()

if(PURELIBM_CORE_MATH_FOUND)
  message(STATUS "Found CORE-MATH: ${PURELIBM_CORE_MATH_DIR}")
endif()

function(purelibm_apply_options target)
  target_compile_options(${target} PRIVATE ${PURELIBM_STRICT_COMPILE_OPTIONS})
endfunction()

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
