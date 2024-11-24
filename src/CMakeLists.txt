add_subdirectory(test)

add_library(Manager Manager.cpp)
target_include_directories(Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Coverage
if(CLASSPROJECT_COVERAGE)
    target_compile_options(Manager PUBLIC --coverage)
    target_link_options(Manager PUBLIC --coverage)
endif()

# Visualization
if(CLASSPROJECT_VISUALIZE)
    # SET CLASSPROJECT_VISUALIZE to 1
    target_compile_definitions(Manager PUBLIC CLASSPROJECT_VISUALIZE=1)
    target_include_directories(Manager PUBLIC ${GRAPHVIZ_INCLUDE_DIR})
    target_link_libraries(Manager PUBLIC ${GRAPHVIZ_LIBRARIES})
else()
    # SET CLASSPROJECT_VISUALIZE to 0
    target_compile_definitions(Manager PUBLIC CLASSPROJECT_VISUALIZE=0)
endif()