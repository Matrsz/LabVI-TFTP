

add_executable(server server.cpp)

target_link_libraries(
  server
  PUBLIC tftp-lib)
