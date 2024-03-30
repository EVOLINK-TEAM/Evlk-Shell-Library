# Evolink Shell Library
# https://github.com/EVOLINK-TEAM/Evlk-Shell-Library
# BSD License

cmake_minimum_required(VERSION 3.5)
project(evlk_shell)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/ srcs)
set(include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(requires arduino)

set(srcs ${srcs} ${CMAKE_CURRENT_SOURCE_DIR}/src/evlk_dictman/evlk_dictman.cpp)
set(include_dirs ${include_dirs} ${CMAKE_CURRENT_SOURCE_DIR}/src/evlk_dictman/)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires})