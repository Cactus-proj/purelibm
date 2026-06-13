add_library(purelibm INTERFACE)

target_include_directories(purelibm INTERFACE
  "${PROJECT_SOURCE_DIR}/src"
)

target_compile_options(purelibm INTERFACE
  ${PURELIBM_STRICT_COMPILE_OPTIONS}
)
