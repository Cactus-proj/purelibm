add_executable(test_ieee754 test_ieee754.c)
purelibm_apply_options(test_ieee754)
target_link_libraries(test_ieee754 PRIVATE purelibm)
add_test(NAME ieee754_smoke COMMAND $<TARGET_FILE:test_ieee754>)
