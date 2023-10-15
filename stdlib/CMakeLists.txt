
add_library(stdlib strfmt.c memcpy.c)
target_include_directories(stdlib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)
