cmake_minimum_required(VERSION 3.5)

project(linearHash)

add_compile_options(-std=c++11 -pthread -g  -ggdb -O0 -Wall   -fstack-protector-all -march=native -fnon-call-exceptions)


add_library(hash
    LinearHashing.cc
)


add_executable(linearHash
    main.cc
)

target_link_libraries(linearHash
    PRIVATE
        hash
)