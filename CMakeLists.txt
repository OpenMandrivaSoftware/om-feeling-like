cmake_minimum_required(VERSION 3.0)
project(om-feeling-like CXX)

install(PROGRAMS om-feeling-like DESTINATION bin)
install(DIRECTORY presets DESTINATION share/om-feeling-like)
