cmake_minimum_required(VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

project("xcp")
add_library(${PROJECT_NAME} SHARED
    "src/misc.c"
    "src/server.c"
    "src/interface.c"
    "src/client.c"
    "src/dispatch.c"
    "src/mutex_win32.c"
    "src/mutex_pthread.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PUBLIC "XCPBUILD")
target_compile_options(${PROJECT_NAME} PUBLIC "-ansi")

project("xcp-test-client")
add_executable(${PROJECT_NAME}
    "src/test-client/main.c"
    "src/test-client/strconv_c.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(${PROJECT_NAME} PUBLIC "-ansi")
target_link_libraries(${PROJECT_NAME} "xcp")

project("xcp-test-server")
add_executable(${PROJECT_NAME}
    "src/test-server/main.c"
    "src/test-server/strconv_s.c"
    "src/test-server/strconv.c"
)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(${PROJECT_NAME} PUBLIC "-ansi")
target_link_libraries(${PROJECT_NAME} "xcp")

project("xcp-idl")
add_executable(${PROJECT_NAME} "src/idl/main.cpp")
target_compile_options(${PROJECT_NAME} PUBLIC "-ansi")

