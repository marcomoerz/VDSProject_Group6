add_subdirectory(test)

add_library(Manager Manager.cpp)
target_include_directories(Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Manager PUBLIC gvc cgraph cdt gvplugin_core gvplugin_dot_layout gvplugin_gd)
