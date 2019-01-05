cmake_minimum_required(VERSION 3.4.1)


set (CMAKE_CXX_STANDARD 14)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++abi")

project(meta-backend)

add_subdirectory(core)

option(BUILD_EXAMPLES "" OFF)
option(BUILD_STATIC_LIBS "" OFF)
option(BUILD_TESTS "" OFF)
option(BUILD_TOOLS "" OFF)
add_subdirectory(libs/amqpcpp)

add_executable(
    # Name
    meta-backend
    # source files (relative)
    main.cpp
    distributor.h
    distributor.cpp
    amqp/rabbitmqconfig.h
    amqp/rabbitmqconfig.cpp
    amqp/rabbitmqadapter.h
    amqp/rabbitmqadapter.cpp
    threadpool/blockingqueue.h
    threadpool/blockingqueue.cpp
    threadpool/worker.h
    threadpool/worker.cpp
    threadpool/workunit.h
    computeunit.o
)
set_target_properties(meta-backend PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(meta-backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

configure_file(rabbitMQ_conf.json ${CMAKE_CURRENT_BINARY_DIR}/rabbitMQ_conf.json COPYONLY)

target_link_libraries(meta-backend meta-core pthread amqpcpp)
