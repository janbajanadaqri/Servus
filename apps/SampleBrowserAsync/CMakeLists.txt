
project(SampleBrowserAsync)

message("Test App config")

set(TESETAPP_SOURCES SampleBrowserAsync.cpp)

add_executable(SampleBrowserAsync
        ${TESETAPP_SOURCES}
        )

set_target_properties(SampleBrowserAsync PROPERTIES
        CXX_STANDARD 14
        )

target_link_libraries(SampleBrowserAsync Servus)