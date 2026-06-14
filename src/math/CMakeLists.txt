add_subdirectory(binary32)
add_subdirectory(binary64)

# Propagate source lists to parent (src/CMakeLists.txt)
set(PURELIBM_BINARY32_SRCS ${PURELIBM_BINARY32_SRCS} PARENT_SCOPE)
set(PURELIBM_BINARY64_SRCS ${PURELIBM_BINARY64_SRCS} PARENT_SCOPE)
