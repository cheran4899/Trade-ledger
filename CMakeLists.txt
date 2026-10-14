
cmake_minimum_required(VERSION 3.16)

project(TradeLedger
    VERSION 1.0.0
    DESCRIPTION "C++ trade ledger and portfolio P&L engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find all implementation files.
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

if(NOT PROJECT_SOURCES)
    message(FATAL_ERROR "No .cpp files found in src/")
endif()

# Build the executable.
add_executable(trade_ledger ${PROJECT_SOURCES})

# Locate header files.
target_include_directories(trade_ledger PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Compiler warnings.
if(MSVC)
    target_compile_options(trade_ledger PRIVATE /W4)
else()
    target_compile_options(trade_ledger PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
