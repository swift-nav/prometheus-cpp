
add_executable(sample_server
  sample_server.cc
)

target_link_libraries(sample_server
  PRIVATE
    ${PROJECT_NAME}::pull
)

set_target_properties(sample_server
  PROPERTIES
    CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,clang-analyzer-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-macro-usage,-google-readability-todo,cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cert-err58-cpp,-google-readability-avoid-underscore-in-googletest-name,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-avoid-goto,-cppcoreguidelines-non-private-member-variables-in-classes,-readability-function-cognitive-complexity"
)

add_executable(sample_server_multi
  sample_server_multi.cc
)

target_link_libraries(sample_server_multi
  PRIVATE
    ${PROJECT_NAME}::pull
)

set_target_properties(sample_server_multi
  PROPERTIES
    CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,clang-analyzer-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-macro-usage,-google-readability-todo,cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cert-err58-cpp,-google-readability-avoid-underscore-in-googletest-name,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-avoid-goto,-cppcoreguidelines-non-private-member-variables-in-classes,-readability-function-cognitive-complexity"
)

add_executable(sample_server_auth
  sample_server_auth.cc
)

target_link_libraries(sample_server_auth
  PRIVATE
     ${PROJECT_NAME}::pull
)

set_target_properties(sample_server_auth
  PROPERTIES
    CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,clang-analyzer-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-macro-usage,-google-readability-todo,cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cert-err58-cpp,-google-readability-avoid-underscore-in-googletest-name,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-avoid-goto,-cppcoreguidelines-non-private-member-variables-in-classes,-readability-function-cognitive-complexity"
)

find_package(CURL)

if(CURL_FOUND)
  add_executable(prometheus_pull_integration_test
    integration_test.cc
  )

  target_link_libraries(prometheus_pull_integration_test
    PRIVATE
      ${PROJECT_NAME}::pull
      CURL::libcurl
      GTest::gmock_main
  )

  set_target_properties(prometheus_pull_integration_test
    PROPERTIES
      CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,clang-analyzer-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-macro-usage,-google-readability-todo,cppcoreguidelines-pro-bounds-array-to-pointer-decay,-cert-err58-cpp,-google-readability-avoid-underscore-in-googletest-name,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-avoid-goto,-cppcoreguidelines-non-private-member-variables-in-classes,-readability-function-cognitive-complexity"
  )

  add_test(
    NAME prometheus_pull_integration_test
    COMMAND prometheus_pull_integration_test
  )
endif()
