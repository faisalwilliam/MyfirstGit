cmake_minimum_required(VERSION 3.5)

project(EmployeeManagementSystem LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Sql Widgets PrintSupport Charts REQUIRED)

# Find all source files recursively
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE UIS "*.ui")

# Exclude generated files to avoid conflicts
list(FILTER SOURCES EXCLUDE REGEX "/moc_.*\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX "/qrc_.*\\.cpp$")
# Exclude CMake internal files and conflicting main files
list(FILTER SOURCES EXCLUDE REGEX "/CMakeFiles/")
list(FILTER SOURCES EXCLUDE REGEX "/build/")
list(FILTER SOURCES EXCLUDE REGEX "_autogen/")
list(FILTER SOURCES EXCLUDE REGEX "/BoilerROOM44\\.cpp$")

add_executable(EmployeeManagementSystem
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    ${UIS}
)

target_link_libraries(EmployeeManagementSystem PRIVATE Qt5::Core Qt5::Gui Qt5::Sql Qt5::Widgets Qt5::PrintSupport Qt5::Charts)