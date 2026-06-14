add_library(purelibm INTERFACE)

target_include_directories(purelibm INTERFACE
  "${PROJECT_SOURCE_DIR}/src"
)

# Strict compile options are applied per-target via purelibm_apply_options().
# We do NOT propagate them through the INTERFACE library — consumers should
# not be forced into -Werror, -Wconversion, etc.
