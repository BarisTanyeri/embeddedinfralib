if (EMIL_HOST_BUILD)
    add_executable(protobuf.protoc_echo_plugin_csharp)
    install(TARGETS protobuf.protoc_echo_plugin_csharp EXPORT emilProtobufTargets)

    target_compile_options(protobuf.protoc_echo_plugin_csharp PRIVATE
        $<$<BOOL:${EMIL_BUILD_WIN}>:
            $<$<CXX_COMPILER_ID:MSVC>:
                /wd4273
            >
        >
    )

    target_link_libraries(protobuf.protoc_echo_plugin_csharp PUBLIC
        protobuf.protoc_echo_plugin_lib
    )

    target_sources(protobuf.protoc_echo_plugin_csharp PRIVATE
        Main.cpp
        ProtoCEchoPluginCSharp.cpp
        ProtoCEchoPluginCSharp.hpp
    )
endif()
