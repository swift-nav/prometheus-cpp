
add_executable(prometheus_core_test
  builder_test.cc
  check_names_test.cc
  counter_test.cc
  family_test.cc
  gauge_test.cc
  histogram_test.cc
  registry_test.cc
  serializer_test.cc
  summary_test.cc
  text_serializer_test.cc
  utils_test.cc
)

target_link_libraries(prometheus_core_test
  PRIVATE
    ${PROJECT_NAME}::core
    GTest::gmock_main
)

set_target_properties(prometheus_core_test
  PROPERTIES
    CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-macro-usage,-google-readability-todo,-cert-err58-cpp,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-avoid-goto,-cppcoreguidelines-non-private-member-variables-in-classes,-readability-function-cognitive-complexity"
)

add_test(
  NAME prometheus_core_test
  COMMAND prometheus_core_test
)
