add_subdirectory(parsers)

add_executable(test_sp3_parser
    test_sp3_parser.cpp
)

target_link_libraries(test_sp3_parser parsers)
