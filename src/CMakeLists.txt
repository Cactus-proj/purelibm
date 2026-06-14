# Collect all function sources.  GLOB is acceptable here because new .c
# files are infrequent (~40 total) and require a cmake re-run anyway.
add_subdirectory(math)

list(APPEND PURELIBM_SRCS ${PURELIBM_BINARY32_SRCS} ${PURELIBM_BINARY64_SRCS})

if(PURELIBM_SRCS)
  add_library(purelibm STATIC ${PURELIBM_SRCS})
  purelibm_apply_options(purelibm)
  target_include_directories(purelibm PUBLIC "${PROJECT_SOURCE_DIR}/src")
else()
  # No compiled functions yet — header-only INTERFACE library.
  add_library(purelibm INTERFACE)
  target_include_directories(purelibm INTERFACE "${PROJECT_SOURCE_DIR}/src")
endif()
