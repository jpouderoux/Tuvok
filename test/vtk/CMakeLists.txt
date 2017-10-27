cmake_minimum_required(VERSION 2.8)

project(vtkTuvok)

find_package(GLEW REQUIRED)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(Tuvok REQUIRED)

add_executable(vtkTuvok MACOSX_BUNDLE
  vtkTuvokMapper.cxx
  main.cxx)

target_link_libraries(vtkTuvok Tuvok ${VTK_LIBRARIES} ${GLEW_LIBRARY})
