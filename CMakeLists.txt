cmake_minimum_required(VERSION 3.0)
project(om-feeling-like CXX)

add_subdirectory(src)

install(PROGRAMS om-feeling-like DESTINATION bin)
install(FILES om-feeling-like.svg DESTINATION share/icons/hicolor/scalable/apps)
install(DIRECTORY presets DESTINATION share/om-feeling-like)
