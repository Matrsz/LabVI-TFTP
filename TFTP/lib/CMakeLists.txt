
file(GLOB_RECURSE CPP_FILES "*.cpp")

add_library(tftp-lib STATIC ${CPP_FILES})

target_include_directories(tftp-lib PUBLIC .)
