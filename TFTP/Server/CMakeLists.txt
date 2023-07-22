

add_executable(server server.cpp users.cpp)

target_link_libraries(
  server
  PUBLIC tftp-lib)
