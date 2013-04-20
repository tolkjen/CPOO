project(cpoo)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
find_package( OpenCV REQUIRED )


add_subdirectory(src)


