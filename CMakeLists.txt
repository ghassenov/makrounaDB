cmake_minimum_required(VERSION 3.20)

project(makrounaDB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MAKROUNADB_ENABLE_SANITIZERS "Enable ASan/UBSan for debug-like builds" ON)

if(MAKROUNADB_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(makrounadb_core
    src/command/command_dispatcher.cpp
    src/common/nl_parser.cpp
    src/persistence/aof_persistence.cpp
    src/protocol/request_parser.cpp
    src/protocol/resp.cpp
    src/storage/key_value_store.cpp
)

target_include_directories(makrounadb_core
    PUBLIC
        include
)

target_compile_features(makrounadb_core PUBLIC cxx_std_20)

add_executable(makrounadb_server
    src/app/main.cpp
    src/network/server.cpp
)
set_target_properties(makrounadb_server PROPERTIES OUTPUT_NAME makrounaDB)

target_link_libraries(makrounadb_server
    PRIVATE
        makrounadb_core
)

enable_testing()

add_executable(unit_test_key_value_store
    tests/unit/test_key_value_store.cpp
)
target_link_libraries(unit_test_key_value_store PRIVATE makrounadb_core)
add_test(NAME unit.key_value_store COMMAND unit_test_key_value_store)

add_executable(unit_test_command_dispatcher
    tests/unit/test_command_dispatcher.cpp
)
target_link_libraries(unit_test_command_dispatcher PRIVATE makrounadb_core)
add_test(NAME unit.command_dispatcher COMMAND unit_test_command_dispatcher)

add_executable(unit_test_request_parser
    tests/unit/test_request_parser.cpp
)
target_link_libraries(unit_test_request_parser PRIVATE makrounadb_core)
add_test(NAME unit.request_parser COMMAND unit_test_request_parser)

add_executable(unit_test_aof_persistence
    tests/unit/test_aof_persistence.cpp
)
target_link_libraries(unit_test_aof_persistence PRIVATE makrounadb_core)
add_test(NAME unit.aof_persistence COMMAND unit_test_aof_persistence)

add_executable(unit_test_nl_parser
    tests/unit/test_nl_parser.cpp
)
target_link_libraries(unit_test_nl_parser PRIVATE makrounadb_core)
add_test(NAME unit.nl_parser COMMAND unit_test_nl_parser)

add_executable(integration_test_command_flow
    tests/integration/test_command_flow.cpp
)
target_link_libraries(integration_test_command_flow PRIVATE makrounadb_core)
add_test(NAME integration.command_flow COMMAND integration_test_command_flow)

add_executable(integration_test_socket_level
    tests/integration/test_socket_level.cpp
)
add_test(
    NAME integration.socket_level
    COMMAND integration_test_socket_level $<TARGET_FILE:makrounadb_server>
)

include(GNUInstallDirs)

install(
    TARGETS makrounadb_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(CPACK_PACKAGE_NAME "makrounadb")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "makrounaDB server")
set(CPACK_PACKAGE_VENDOR "makrounaDB")
set(CPACK_PACKAGE_CONTACT "maintainers@makrounadb.local")
if(PROJECT_VERSION)
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
else()
    set(CPACK_PACKAGE_VERSION "0.1.0")
endif()
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "makrounaDB maintainers")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

include(CPack)
