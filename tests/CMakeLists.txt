add_executable(test_ieee754 test_ieee754.c)
purelibm_apply_options(test_ieee754)
target_link_libraries(test_ieee754 PRIVATE purelibm)
add_test(NAME ieee754_smoke COMMAND $<TARGET_FILE:test_ieee754>)
set_tests_properties(ieee754_smoke PROPERTIES LABELS "smoke")

add_executable(test_numeric_utils test_numeric_utils.c)
purelibm_apply_options(test_numeric_utils)
target_link_libraries(test_numeric_utils PRIVATE purelibm m)
add_test(NAME numeric_utils_smoke COMMAND $<TARGET_FILE:test_numeric_utils>)
set_tests_properties(numeric_utils_smoke PROPERTIES LABELS "smoke")

if(MPFR_FOUND)
  add_executable(test_mpfr_smoke test_mpfr_smoke.c)
  purelibm_apply_options(test_mpfr_smoke)
  target_link_libraries(test_mpfr_smoke PRIVATE purelibm PkgConfig::MPFR m)
  add_test(NAME mpfr_smoke COMMAND $<TARGET_FILE:test_mpfr_smoke>)
  set_tests_properties(mpfr_smoke PROPERTIES LABELS "smoke")
endif()

add_executable(test_exhaustive_harness test_exhaustive_harness.c)
purelibm_apply_options(test_exhaustive_harness)
target_link_libraries(test_exhaustive_harness PRIVATE purelibm)
target_include_directories(test_exhaustive_harness PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME exhaustive_harness_smoke COMMAND $<TARGET_FILE:test_exhaustive_harness>)
set_tests_properties(exhaustive_harness_smoke PROPERTIES LABELS "smoke")

# --- CORE-MATH consistency test infrastructure ---
if(PURELIBM_CORE_MATH_FOUND)
  # Build CORE-MATH's rsqrtf as a separate OBJECT library with relaxed
  # compile options (CORE-MATH uses type-punning unions, __builtin_expect,
  # etc. that would fail under PureLibm's strict flags).
  set(CORE_MATH_RSQRT_SRC
    "${CORE_MATH_BINARY32_DIR}/rsqrt/rsqrtf.c"
  )

  add_library(core_math_rsqrtf_obj OBJECT ${CORE_MATH_RSQRT_SRC})
  target_compile_options(core_math_rsqrtf_obj PRIVATE
    ${PURELIBM_CORE_MATH_COMPILE_OPTIONS}
  )
  target_include_directories(core_math_rsqrtf_obj PRIVATE
    "${CORE_MATH_BINARY32_DIR}/support"
  )

  add_executable(test_core_math_smoke
    test_core_math_smoke.c
  )
  purelibm_apply_options(test_core_math_smoke)
  target_link_libraries(test_core_math_smoke PRIVATE
    purelibm
    core_math_rsqrtf_obj
    m
  )
  add_test(NAME core_math_smoke COMMAND $<TARGET_FILE:test_core_math_smoke>)
  set_tests_properties(core_math_smoke PROPERTIES LABELS "smoke")
endif()

# --- Exhaustive driver infrastructure ---
if(MPFR_FOUND)
  add_executable(test_exhaustive_driver test_exhaustive_driver.c)
  purelibm_apply_options(test_exhaustive_driver)
  target_include_directories(test_exhaustive_driver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  target_link_libraries(test_exhaustive_driver PRIVATE purelibm PkgConfig::MPFR m)
  if(OpenMP_C_FOUND)
    target_link_libraries(test_exhaustive_driver PRIVATE OpenMP::OpenMP_C)
  endif()
  add_test(NAME exhaustive_driver_smoke COMMAND $<TARGET_FILE:test_exhaustive_driver>)
  set_tests_properties(exhaustive_driver_smoke PROPERTIES LABELS "smoke")
endif()
