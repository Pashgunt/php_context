cmake_minimum_required(VERSION 3.10)
project(context C)

set(CMAKE_C_STANDARD 99)

execute_process(
        COMMAND php-config --includes
        OUTPUT_VARIABLE PHP_INCLUDE_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "PHP includes raw: ${PHP_INCLUDE_RAW}")

separate_arguments(PHP_INCLUDE_DIRS UNIX_COMMAND "${PHP_INCLUDE_RAW}")

set(PHP_INCLUDE_PATHS "")
foreach(dir IN LISTS PHP_INCLUDE_DIRS)
    string(REPLACE "-I" "" clean_dir "${dir}")
    message(STATUS "PHP includes clear dir: ${clean_dir}")
    list(APPEND PHP_INCLUDE_PATHS "${clean_dir}")
endforeach()

add_library(context SHARED
        php_context.c
        context.c
)

target_include_directories(context PRIVATE ${PHP_INCLUDE_PATHS})
target_compile_definitions(context PRIVATE -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 -DCOMPILE_DL_CONTEXT)

set_target_properties(context PROPERTIES
        PREFIX ""
        OUTPUT_NAME "context"
)
