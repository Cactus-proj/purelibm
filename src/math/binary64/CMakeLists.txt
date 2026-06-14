# Collect binary64 function sources.  Created now as a placeholder for
# M4 double-precision functions.  The parent src/CMakeLists.txt picks
# up the list.
file(GLOB PURELIBM_BINARY64_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
set(PURELIBM_BINARY64_SRCS ${PURELIBM_BINARY64_SRCS} PARENT_SCOPE)
