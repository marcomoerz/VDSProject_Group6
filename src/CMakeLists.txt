if(CLASSPROJECT_TESTS)
    add_subdirectory(test)
endif()

add_library(Manager Manager.cpp)
target_include_directories(Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Manager PUBLIC ${boost_SOURCE_DIR})

# Visualization
if(CLASSPROJECT_VISUALIZE)
    # SET CLASSPROJECT_VISUALIZE to 1
    target_compile_definitions(Manager PUBLIC CLASSPROJECT_VISUALIZE=1)
    if(CLASSPROJECT_GRAPHVIZ)
        # SET CLASSPROJECT_GRAPHVIZ to 1
        target_compile_definitions(Manager PUBLIC CLASSPROJECT_GRAPHVIZ=1)
        target_include_directories(Manager PUBLIC ${GRAPHVIZ_INCLUDE_DIR})
        target_link_libraries(Manager PUBLIC ${GRAPHVIZ_LIBRARIES})
    else()
        # SET CLASSPROJECT_GRAPHVIZ to 0
        target_compile_definitions(Manager PUBLIC CLASSPROJECT_GRAPHVIZ=0)
    endif()
else()
    # SET CLASSPROJECT_VISUALIZE to 0 and CLASSPROJECT_GRAPHVIZ to 0
    target_compile_definitions(Manager PUBLIC CLASSPROJECT_VISUALIZE=0)
    target_compile_definitions(Manager PUBLIC CLASSPROJECT_GRAPHVIZ=0)
endif()