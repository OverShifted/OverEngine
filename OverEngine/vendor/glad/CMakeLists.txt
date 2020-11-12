add_library(glad
    "include/glad/gl.h"
    "include/KHR/khrplatform.h"
    "src/gl.c"
)

include_directories("include")