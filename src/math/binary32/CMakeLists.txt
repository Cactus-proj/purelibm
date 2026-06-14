# Collect binary32 function sources.  New functions: drop the .c file here
# and re-run cmake.  The parent src/CMakeLists.txt picks up the list.
file(GLOB PURELIBM_BINARY32_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
set(PURELIBM_BINARY32_SRCS ${PURELIBM_BINARY32_SRCS} PARENT_SCOPE)
