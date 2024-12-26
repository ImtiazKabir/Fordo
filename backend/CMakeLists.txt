cmake_minimum_required(VERSION 3.29)
project(Fordo C)

set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE MAIN_SOURCES "src/*.c")
add_executable(main ${MAIN_SOURCES})

target_compile_features(main PRIVATE c_std_90)
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build
)

target_include_directories(main PRIVATE include)
target_link_directories(main PRIVATE lib)

target_compile_options(main PRIVATE
  -g3 -ggdb
  -fsanitize=address,undefined,leak,null,alignment
  -Wall -Wextra -ansi -Werror -pedantic -Wparentheses -Wformat=2
  -Wshadow -Wwrite-strings -Wredundant-decls -Wmissing-include-dirs
  -pedantic-errors -Wuninitialized -Wmissing-declarations -Wconversion
  -Wdouble-promotion -Wfloat-equal -Wmain -Wundef
  -Wstrict-prototypes -Wnested-externs -Wold-style-definition
  -Wbad-function-cast -Wno-unknown-pragmas -Wno-unused-function
  -fno-strict-aliasing
)

target_link_options(main PRIVATE -fsanitize=address,undefined,leak,null,alignment)

target_link_libraries(main PRIVATE imlib)
target_link_libraries(main PRIVATE sqlite3)
target_link_libraries(main PRIVATE phr)
target_link_libraries(main PRIVATE cjson)

