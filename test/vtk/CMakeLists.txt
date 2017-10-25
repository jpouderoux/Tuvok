cmake_minimum_required(VERSION 2.8)

project(vtkTuvok)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(Tuvok REQUIRED)

add_executable(vtkTuvok MACOSX_BUNDLE
  main.cxx)

target_link_libraries(vtkTuvok ${VTK_LIBRARIES} Tuvok)
