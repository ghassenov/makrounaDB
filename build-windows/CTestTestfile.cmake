# CMake generated Testfile for 
# Source directory: /home/louay/personal/makrounaDB
# Build directory: /home/louay/personal/makrounaDB/build-windows
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[unit.key_value_store]=] "/home/louay/personal/makrounaDB/build-windows/unit_test_key_value_store.exe")
set_tests_properties([=[unit.key_value_store]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/louay/personal/makrounaDB/CMakeLists.txt;53;add_test;/home/louay/personal/makrounaDB/CMakeLists.txt;0;")
add_test([=[unit.command_dispatcher]=] "/home/louay/personal/makrounaDB/build-windows/unit_test_command_dispatcher.exe")
set_tests_properties([=[unit.command_dispatcher]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/louay/personal/makrounaDB/CMakeLists.txt;59;add_test;/home/louay/personal/makrounaDB/CMakeLists.txt;0;")
add_test([=[unit.request_parser]=] "/home/louay/personal/makrounaDB/build-windows/unit_test_request_parser.exe")
set_tests_properties([=[unit.request_parser]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/louay/personal/makrounaDB/CMakeLists.txt;65;add_test;/home/louay/personal/makrounaDB/CMakeLists.txt;0;")
add_test([=[unit.aof_persistence]=] "/home/louay/personal/makrounaDB/build-windows/unit_test_aof_persistence.exe")
set_tests_properties([=[unit.aof_persistence]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/louay/personal/makrounaDB/CMakeLists.txt;71;add_test;/home/louay/personal/makrounaDB/CMakeLists.txt;0;")
add_test([=[unit.nl_parser]=] "/home/louay/personal/makrounaDB/build-windows/unit_test_nl_parser.exe")
set_tests_properties([=[unit.nl_parser]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/louay/personal/makrounaDB/CMakeLists.txt;77;add_test;/home/louay/personal/makrounaDB/CMakeLists.txt;0;")
add_test([=[integration.command_flow]=] "/home/louay/personal/makrounaDB/build-windows/integration_test_command_flow.exe")
set_tests_properties([=[integration.command_flow]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/louay/personal/makrounaDB/CMakeLists.txt;83;add_test;/home/louay/personal/makrounaDB/CMakeLists.txt;0;")
add_test([=[integration.socket_level]=] "/home/louay/personal/makrounaDB/build-windows/integration_test_socket_level.exe" "/home/louay/personal/makrounaDB/build-windows/makrounaDB.exe")
set_tests_properties([=[integration.socket_level]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/louay/personal/makrounaDB/CMakeLists.txt;88;add_test;/home/louay/personal/makrounaDB/CMakeLists.txt;0;")
