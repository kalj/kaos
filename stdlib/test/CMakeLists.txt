set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -O1")
set(CMAKE_EXE_LINKER_FLAGS "")
add_executable(test_strfmt test_strfmt.c ../strfmt.c)
target_include_directories(test_strfmt PRIVATE ${CMAKE_CURRENT_LIST_DIR}/..)

