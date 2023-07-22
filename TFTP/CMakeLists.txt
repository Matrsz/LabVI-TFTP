
cmake_minimum_required(VERSION 3.16)

project(TFTP LANGUAGES CXX)

# add_compile_options(-Wall -Wextra -pedantic -Werror)
add_compile_options(-g -Wall)

add_subdirectory(lib)
add_subdirectory(Client)
add_subdirectory(Server)

