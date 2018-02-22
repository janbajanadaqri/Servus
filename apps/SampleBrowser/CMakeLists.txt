
project(SampleBrowser)

set(TESETAPP_SOURCES SampleBrowser.cpp)

# video plugin
#link_directories("/home/dslocal/Work/CLion/Servus/cmake-build-debug/lib")
#include_directories (/home/dslocal/Work/CLion/Servus/cmake-build-debug/include)
#include_directories (/home/dslocal/Work/CLion/Servus/servus)

#link_directories("C:/Users/dslocal/Documents/Work/Git/Servus/cmake-build-debug/lib")
#include_directories (C:/Users/dslocal/Documents/Work/Git/Servus/cmake-build-debug/include)
#include_directories (C:/Users/dslocal/Documents/Work/Git/Servus/servus)

add_executable(SampleBrowser
        ${TESETAPP_SOURCES}
        )

#set_target_properties(SampleBrowser PROPERTIES
#        CXX_STANDARD 14
#        )

target_link_libraries(SampleBrowser Servus)

#common_application(SampleBrowser GUI NOHELP)
