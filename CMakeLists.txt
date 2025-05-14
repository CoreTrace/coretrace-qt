cmake_minimum_required(VERSION 3.16)
project(coretrace-qt VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Find required packages
find_package(Qt5 COMPONENTS 
    Core 
    Widgets 
    Gui
    REQUIRED
)

# Set source files
set(SOURCES
    src/audit_cache.cpp
    src/audit_factory.cpp
    src/audit_result.cpp
    src/audit_results_view.cpp
    src/audit_service.cpp
    src/cli_options_panel.cpp
    src/ctrace_cli.cpp
    src/error_highlighter.cpp
    src/file_tree_view.cpp
    src/ide.cpp
    src/main.cpp
    src/main_window.cpp
    src/parser.cpp
    src/project.cpp
    src/project_manager.cpp
    src/output_display.cpp
    src/line_number_area.cpp
)

# Set header files
set(HEADERS
    includes/audit_cache.hpp
    includes/audit_factory.hpp
    includes/audit_result.hpp
    includes/audit_results_view.hpp
    includes/audit_service.hpp
    includes/cli_options_panel.hpp
    includes/ctrace_cli.hpp
    includes/error_highlighter.hpp
    includes/file_tree_view.hpp
    includes/ide.hpp
    includes/main_window.hpp
    includes/parser.hpp
    includes/project.hpp
    includes/project_manager.hpp
    includes/ui_component.hpp
    includes/output_display.hpp
    includes/line_number_area.hpp
)

# Set resource files
set(RESOURCES
    resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
) 