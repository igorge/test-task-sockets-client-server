cmake_minimum_required(VERSION 3.5)
project(objclient)


if(WIN32)
    set(SOCKSLINKLIB "Ws2_32.lib")
    set(THREADLIB "")

    add_definitions(-DNOMINMAX)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(SOCKSLINKLIB "")
    set(THREADLIB "pthread")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=float-cast-overflow")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


include_directories(
        ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../common/include)

set(SOURCE_FILES
        src/main.cpp)


add_executable(objclient ${SOURCE_FILES})

target_link_libraries(objclient ${SOCKSLINKLIB} ${THREADLIB})
