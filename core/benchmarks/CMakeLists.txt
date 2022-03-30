
add_executable(benchmarks
  main.cc
  benchmark_helpers.cc
  benchmark_helpers.h
  counter_bench.cc
  gauge_bench.cc
  histogram_bench.cc
  registry_bench.cc
  summary_bench.cc
)

target_link_libraries(benchmarks
  PRIVATE
    ${PROJECT_NAME}::core
    benchmark::benchmark
)

add_test(
  NAME benchmarks
  COMMAND benchmarks
)

set_property(
  TEST benchmarks
  APPEND PROPERTY LABELS Benchmark
  CXX_CLANG_TIDY "clang-tidy-12;-header-filter=.;-checks=-*,bugprone-*,cert-*,concurrency-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*,-modernize-use-trailing-return-type,-misc-non-private-member-variables-in-classes,-modernize-pass-by-value,-cppcoreguidelines-macro-usage,-google-readability-todo"
)
