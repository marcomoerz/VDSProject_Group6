add_library(Benchmark
        BenchParser.cpp
        BenchmarkLib.cpp
        CircuitToBDD.cpp
        bench_grammar.hpp
        skip_parser.hpp
)
# Add Boost include directories
target_include_directories(Benchmark PRIVATE ${CMAKE_SOURCE_DIR}/lib/tqdm)
target_include_directories(Benchmark PUBLIC ${boost_SOURCE_DIR})

#Executable
add_executable(VDSProject_bench main_bench.cpp)
target_link_libraries(VDSProject_bench
        Manager
        Benchmark
)