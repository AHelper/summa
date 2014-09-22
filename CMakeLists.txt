cmake_minimum_required(VERSION 2.8)
project(summa)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
find_package(Qt4 REQUIRED)

set(ui_SRCS src/summacontrolpanel.cpp src/summadeviceconfig.cpp src/main.cpp src/driverinstance.cpp src/drivermanager.cpp src/driverprovider.cpp)

add_library(summa src/drv_summa.c)

add_executable(drv_summa src/summa.c)
target_link_libraries(drv_summa summa)


# qt4_wrap_cpp(MOCS src/summacontrolpanel.h)
qt4_wrap_ui(UIS src/summacontrolpanel.ui src/summadeviceconfig.ui)
# add_executable(summactl ${ui_SRCS} ${MOCS} ${UIS})
add_executable(summactl ${ui_SRCS} ${UIS})
target_include_directories(summactl PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(summactl summa Qt4::QtGui)

add_subdirectory(src)