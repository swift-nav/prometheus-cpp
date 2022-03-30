
add_executable(sample_client
  sample_client.cc
)

target_link_libraries(sample_client
  PRIVATE
    ${PROJECT_NAME}::push
)

set_target_properties(sample_client
  PROPERTIES
    CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-macro-usage,-google-readability-todo,-cert-err58-cpp,-google-readability-avoid-underscore-in-googletest-name,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-avoid-goto,-cppcoreguidelines-non-private-member-variables-in-classes,-readability-function-cognitive-complexity"
)
