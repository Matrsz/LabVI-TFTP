

add_executable(client client.cpp)
add_executable(test test.cpp)

target_link_libraries(
  client
  PUBLIC tftp-lib)
