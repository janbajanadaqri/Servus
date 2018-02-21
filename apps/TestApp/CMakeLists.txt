# Copyright (c) 2015-2017, Human Brain Project
#                          Daniel.Nachbaur@epfl.ch
#
# This file is part of Servus <https://github.com/HBPVIS/Servus>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

project(TestApp)

message("Test App config")

set(TESETAPP_SOURCES TestApp.cpp)

# video plugin
#link_directories("/home/dslocal/Work/CLion/Servus/cmake-build-debug/lib")
#include_directories (/home/dslocal/Work/CLion/Servus/cmake-build-debug/include)
#include_directories (/home/dslocal/Work/CLion/Servus/servus)

#link_directories("C:/Users/dslocal/Documents/Work/Git/Servus/cmake-build-debug/lib")
#include_directories (C:/Users/dslocal/Documents/Work/Git/Servus/cmake-build-debug/include)
#include_directories (C:/Users/dslocal/Documents/Work/Git/Servus/servus)

add_executable(TestApp
        ${TESETAPP_SOURCES}
        )

#set_target_properties(TestApp PROPERTIES
#        CXX_STANDARD 14
#        )

target_link_libraries(TestApp Servus)

#common_application(testApp GUI NOHELP)
