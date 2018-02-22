
project(SampleAnnounce)

message("Test App config")

set(TESETAPP_SOURCES SampleAnnounce.cpp)

# video plugin
#link_directories("/home/dslocal/Work/CLion/Servus/cmake-build-debug/lib")
#include_directories (/home/dslocal/Work/CLion/Servus/cmake-build-debug/include)
#include_directories (/home/dslocal/Work/CLion/Servus/servus)

#link_directories("C:/Users/dslocal/Documents/Work/Git/Servus/cmake-build-debug/lib")
#include_directories (C:/Users/dslocal/Documents/Work/Git/Servus/cmake-build-debug/include)
#include_directories (C:/Users/dslocal/Documents/Work/Git/Servus/servus)

add_executable(SampleAnnounce
        ${TESETAPP_SOURCES}
        )

#set_target_properties(SampleAnnounce PROPERTIES
#        CXX_STANDARD 14
#        )

target_link_libraries(SampleAnnounce Servus)

#common_application(SampleAnnounce GUI NOHELP)
