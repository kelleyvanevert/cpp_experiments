cmake_minimum_required(VERSION 3.0.0)
project(exec VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

include(FetchContent)

FetchContent_Declare(lexy URL https://lexy.foonathan.net/download/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(robin_map
  GIT_REPOSITORY https://github.com/Tessil/robin-map.git
  GIT_TAG v1.2.1
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(robin_map)

set(BOOST_INCLUDE_LIBRARIES filesystem system container regex)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.84.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Boost)

add_executable(exec main.cpp)

target_link_libraries(exec PRIVATE foonathan::lexy)
target_link_libraries(exec PRIVATE fmt::fmt)
target_link_libraries(exec PRIVATE tsl::robin_map)
# target_link_libraries(exec PRIVATE Folly::folly)
target_link_libraries(exec PRIVATE Boost::container)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
