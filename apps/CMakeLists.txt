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

add_subdirectory(SampleBrowserAsync)
add_subdirectory(SampleAnnounce)
add_subdirectory(SampleBrowser)

if(NOT TARGET ServusQt)
  return()
endif()

set(SERVUSBROWSER_SOURCES servusBrowser.cpp)
set(SERVUSBROWSER_LINK_LIBRARIES ServusQt Qt5::Widgets)

if(APPLE)
common_application(ServusBrowser GUI NOHELP)
else()
common_application(servusBrowser GUI NOHELP)
endif()
