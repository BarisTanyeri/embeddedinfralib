if (EMIL_BUILD_WIN)
    add_library(services.network_win ${EMIL_EXCLUDE_FROM_ALL} STATIC)
    install(TARGETS services.network_win EXPORT emilServicesTargets)

    target_link_libraries(services.network_win PUBLIC
        services.network
        ws2_32.lib
        iphlpapi.lib
    )

    target_sources(services.network_win PRIVATE
        ConnectionWin.cpp
        ConnectionWin.hpp
        DatagramWin.cpp
        DatagramWin.hpp
        EventDispatcherWithNetwork.cpp
        EventDispatcherWithNetwork.hpp
        NameLookupWin.cpp
        NameLookupWin.hpp
    )
endif()
