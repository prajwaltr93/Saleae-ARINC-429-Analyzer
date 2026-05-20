cmake_minimum_required (VERSION 3.13)

project(ARINC429Analyzer)

add_definitions( -DLOGIC2 )

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum supported MacOS version" FORCE)

# enable generation of compile_commands.json, helpful for IDEs to locate include files.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES 
src/ARINC429Analyzer.cpp
src/ARINC429Analyzer.h
src/ARINC429AnalyzerResults.cpp
src/ARINC429AnalyzerResults.h
src/ARINC429AnalyzerSettings.cpp
src/ARINC429AnalyzerSettings.h
src/ARINC429SimulationDataGenerator.cpp
src/ARINC429SimulationDataGenerator.h
)

add_analyzer_plugin(${PROJECT_NAME} SOURCES ${SOURCES})
