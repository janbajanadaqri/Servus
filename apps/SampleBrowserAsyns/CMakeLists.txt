
project(SampleBrowserAsyns)

message("Test App config")

set(TESETAPP_SOURCES SampleBrowserAsyns.cpp)

add_executable(SampleBrowserAsyns
        ${TESETAPP_SOURCES}
        )

set_target_properties(SampleBrowserAsyns PROPERTIES
        CXX_STANDARD 14
        )

target_link_libraries(SampleBrowserAsyns Servus)