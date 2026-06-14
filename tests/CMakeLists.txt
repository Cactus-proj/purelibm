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

# --- M2 Pilot functions ---
#
# Each function gets TWO test entries from the SAME executable:
#   test_smoke_<func>    — specials + ~1M subrange  (smoke label, ~1s)
#   test_exhaust_<func>  — full f32 sweep           (exhaustive label, ~10min)
#
# Smoke tests use a 1M-input subrange near zero where rounding is
# most demanding.  Exhaustive tests sweep all 2^32 bit patterns.
set(PURELIBM_SMOKE_RANGE_START 0x00000000)
set(PURELIBM_SMOKE_RANGE_END   0x000fffff)  # ~1M inputs

if(MPFR_FOUND)
  # --- cr_expf ---
  add_executable(test_exhaust_expf test_exhaust_expf.c)
  purelibm_apply_options(test_exhaust_expf)
  target_link_libraries(test_exhaust_expf PRIVATE purelibm PkgConfig::MPFR m)
  if(OpenMP_C_FOUND)
    target_link_libraries(test_exhaust_expf PRIVATE OpenMP::OpenMP_C)
  endif()

  add_test(NAME test_smoke_expf
    COMMAND $<TARGET_FILE:test_exhaust_expf>
            ${PURELIBM_SMOKE_RANGE_START} ${PURELIBM_SMOKE_RANGE_END})
  set_tests_properties(test_smoke_expf PROPERTIES LABELS "smoke")

  add_test(NAME test_exhaust_expf
    COMMAND $<TARGET_FILE:test_exhaust_expf>
            0x00000000 0xffffffff)
  set_tests_properties(test_exhaust_expf PROPERTIES LABELS "exhaustive")

  # --- cr_sqrtf ---
  add_executable(test_exhaust_sqrtf test_exhaust_sqrtf.c)
  purelibm_apply_options(test_exhaust_sqrtf)
  target_link_libraries(test_exhaust_sqrtf PRIVATE purelibm PkgConfig::MPFR m)
  if(OpenMP_C_FOUND)
    target_link_libraries(test_exhaust_sqrtf PRIVATE OpenMP::OpenMP_C)
  endif()

  add_test(NAME test_smoke_sqrtf
    COMMAND $<TARGET_FILE:test_exhaust_sqrtf>
            ${PURELIBM_SMOKE_RANGE_START} ${PURELIBM_SMOKE_RANGE_END})
  set_tests_properties(test_smoke_sqrtf PROPERTIES LABELS "smoke")

  add_test(NAME test_exhaust_sqrtf
    COMMAND $<TARGET_FILE:test_exhaust_sqrtf>
            0x00000000 0xffffffff)
  set_tests_properties(test_exhaust_sqrtf PROPERTIES LABELS "exhaustive")

  # --- cr_logf ---
  add_executable(test_exhaust_logf test_exhaust_logf.c)
  purelibm_apply_options(test_exhaust_logf)
  target_link_libraries(test_exhaust_logf PRIVATE purelibm PkgConfig::MPFR m)
  if(OpenMP_C_FOUND)
    target_link_libraries(test_exhaust_logf PRIVATE OpenMP::OpenMP_C)
  endif()

  add_test(NAME test_smoke_logf
    COMMAND $<TARGET_FILE:test_exhaust_logf>
            ${PURELIBM_SMOKE_RANGE_START} ${PURELIBM_SMOKE_RANGE_END})
  set_tests_properties(test_smoke_logf PROPERTIES LABELS "smoke")

  add_test(NAME test_exhaust_logf
    COMMAND $<TARGET_FILE:test_exhaust_logf>
            0x00000000 0xffffffff)
  set_tests_properties(test_exhaust_logf PROPERTIES LABELS "exhaustive")
endif()

# --- CORE-MATH consistency tests ---
if(PURELIBM_CORE_MATH_FOUND AND MPFR_FOUND)
  purelibm_add_core_math_obj(
    NAME expf
    CORE_MATH_SRC "${CORE_MATH_BINARY32_DIR}/exp/expf.c"
  )
  add_executable(test_consistency_expf test_consistency_expf.c)
  purelibm_apply_options(test_consistency_expf)
  target_link_libraries(test_consistency_expf PRIVATE purelibm core_math_expf_obj PkgConfig::MPFR m)
  add_test(NAME test_consistency_expf
    COMMAND $<TARGET_FILE:test_consistency_expf>)
  set_tests_properties(test_consistency_expf PROPERTIES LABELS "exhaustive")

  purelibm_add_core_math_obj(
    NAME logf
    CORE_MATH_SRC "${CORE_MATH_BINARY32_DIR}/log/logf.c"
  )
  add_executable(test_consistency_logf test_consistency_logf.c)
  purelibm_apply_options(test_consistency_logf)
  target_link_libraries(test_consistency_logf PRIVATE purelibm core_math_logf_obj PkgConfig::MPFR m)
  add_test(NAME test_consistency_logf
    COMMAND $<TARGET_FILE:test_consistency_logf>)
  set_tests_properties(test_consistency_logf PROPERTIES LABELS "exhaustive")
endif()
