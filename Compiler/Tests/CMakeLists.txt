# add_executable( test-lemur )

# target_compile_features( test-lemur PRIVATE cxx_std_23 )
# target_compile_definitions( test-lemur PRIVATE UNIT_TESTING )

# target_link_libraries( test-lemur
#     PRIVATE
#         lemur-core
#         gtest_main
#         gmock
#         gmock_main
# )

# target_include_directories( test-lemur
#     PRIVATE ${CMAKE_SOURCE_DIR}/Include/*
# )

# include( GoogleTest )
# gtest_discover_tests( test-lemur )