# Copyright 2023 Pascal Schmid
#
# This file is part of guile-exprcad.
#
# guile-exprcad is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# guile-exprcad is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with guile-exprcad.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.25)

project(guile-exprcad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(guile-exprcad SHARED exprcad.cxx)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GUILE REQUIRED IMPORTED_TARGET guile-3.0)

target_link_libraries(guile-exprcad PRIVATE PkgConfig::GUILE)

find_package(OpenCASCADE REQUIRED)

if(TRUE)
    foreach(OCCT_LIB ${OpenCASCADE_LIBRARIES})
        if(TARGET ${OCCT_LIB})
            get_target_property(OCCT_PROP ${OCCT_LIB} INTERFACE_LINK_LIBRARIES)
            foreach(OCCT_TK IN LISTS OCCT_PROP)
                if(NOT "${OCCT_TK}" MATCHES "TK.+")
                    list(REMOVE_ITEM OCCT_PROP "${OCCT_TK}")
                endif()
            endforeach()
            set_target_properties(${OCCT_LIB} PROPERTIES INTERFACE_LINK_LIBRARIES "${OCCT_PROP}")
        endif()
    endforeach()
endif()

target_include_directories(guile-exprcad PRIVATE ${OpenCASCADE_INCLUDE_DIR})

target_link_libraries(guile-exprcad PRIVATE ${OpenCASCADE_DataExchange_LIBRARIES})
target_link_libraries(guile-exprcad PRIVATE ${OpenCASCADE_ApplicationFramework_LIBRARIES})
target_link_libraries(guile-exprcad PRIVATE ${OpenCASCADE_Visualization_LIBRARIES})
target_link_libraries(guile-exprcad PRIVATE ${OpenCASCADE_ModelingAlgorithms_LIBRARIES})
target_link_libraries(guile-exprcad PRIVATE ${OpenCASCADE_ModelingData_LIBRARIES})
target_link_libraries(guile-exprcad PRIVATE ${OpenCASCADE_FoundationClasses_LIBRARIES})

pkg_get_variable(GUILE_EXTENSIONDIR guile-3.0 extensiondir)
string(REGEX REPLACE "^\\/usr\\/" "" GUILE_EXTENSIONDIR "${GUILE_EXTENSIONDIR}")

install(TARGETS guile-exprcad DESTINATION "${GUILE_EXTENSIONDIR}")
