cmake_minimum_required(VERSION 2.8)
project(summa)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
find_package(Qt4 REQUIRED)
find_package(Protobuf REQUIRED)
if(NOT HAS_PROTOC_C)
  message(STATUS "Looking for protoc-c")
  find_program(HAS_PROTOC_C "protoc-c")
  if(NOT HAS_PROTOC_C)
    message(FATAL_ERROR "protoc-c not found")
  else()
    message(STATUS "Looking for protoc-c - found")
  endif()
endif()

set(ui_SRCS src/summacontrolpanel.cpp src/summadeviceconfig.cpp src/main.cpp src/driverinstance.cpp src/drivermanager.cpp src/driverprovider.cpp)


add_custom_command(OUTPUT comm_summa.pb-c.h comm_summa.pb-c.c COMMAND protoc-c ARGS --c_out=${CMAKE_BINARY_DIR} --proto_path=${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/comm_summa.proto MAIN_DEPENDENCY src/comm_summa.proto)
add_library(summa src/drv_summa.c ${PROTO_SRCS} comm_summa.pb-c.c src/comm_summa.c src/summa_child.c)

add_executable(drv_summa src/summa.c)
target_link_libraries(drv_summa summa protobuf-c)


# qt4_wrap_cpp(MOCS src/summacontrolpanel.h)
qt4_wrap_ui(UIS src/summacontrolpanel.ui src/summadeviceconfig.ui)
# add_executable(summactl ${ui_SRCS} ${MOCS} ${UIS})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS src/comm_summa.proto)
add_executable(summactl ${ui_SRCS} ${UIS} ${PROTO_SRCS})
target_include_directories(summactl PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(summactl summa Qt4::QtGui protobuf)

add_subdirectory(src)