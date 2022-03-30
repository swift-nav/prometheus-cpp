
add_executable(prometheus_pull_test
  exposer_test.cc
)

target_link_libraries(prometheus_pull_test
  PRIVATE
    ${PROJECT_NAME}::pull
    GTest::gmock_main
)

set_target_properties(prometheus_pull_test
  PROPERTIES
    CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,clang-analyzer-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-macro-usage,-google-readability-todo,cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cert-err58-cpp,-google-readability-avoid-underscore-in-googletest-name,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-avoid-goto,-cppcoreguidelines-non-private-member-variables-in-classes,-readability-function-cognitive-complexity"
)

add_test(
  NAME prometheus_pull_test
  COMMAND prometheus_pull_test
)
