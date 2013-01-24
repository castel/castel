project(castelc)

cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE CASTELC_STATIC_HEADERS_ORIGIN "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.hh" "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.def")
string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" CASTELC_STATIC_HEADERS "${CASTELC_STATIC_HEADERS_ORIGIN}")
file(GLOB_RECURSE CASTELC_STATIC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cc")
set(CASTELC_HEADERS ${CASTELC_STATIC_HEADERS})
set(CASTELC_SOURCES ${CASTELC_STATIC_SOURCES})

foreach(HEADER ${CASTELC_STATIC_HEADERS})
    file(RELATIVE_PATH RELATIVE_HEADER "${CMAKE_BINARY_DIR}" "${HEADER}")
    set(SOURCE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${RELATIVE_HEADER}")
    set(BINARY_HEADER "${CMAKE_BINARY_DIR}/${RELATIVE_HEADER}")
    get_filename_component(BINARY_PATH "${BINARY_HEADER}" PATH)
    add_custom_command(OUTPUT
        "${CMAKE_BINARY_DIR}/${RELATIVE_HEADER}"
        COMMAND mkdir -p "${BINARY_PATH}"
        COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/${RELATIVE_HEADER}" "${CMAKE_BINARY_DIR}/${RELATIVE_HEADER}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${RELATIVE_HEADER}"
    )
endforeach(HEADER)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(LLVM)

add_subdirectory(libcastel)

execute_process(COMMAND llvm-config --cppflags COMMAND tr -d "\\n" OUTPUT_VARIABLE LLVM_CPP_FLAGS)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-std=c++11 ${LLVM_CPP_FLAGS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include_directories("${CMAKE_BINARY_DIR}/includes")

add_executable(castelc ${CASTELC_HEADERS} ${CASTELC_SOURCES})
target_link_libraries(castelc -Wl,-whole-archive CastelRuntime -Wl,-no-whole-archive CastelCompile CastelParse ${LLVM_LIBS})
set_target_properties(castelc PROPERTIES LINK_FLAGS "-rdynamic ${LLVM_LDFLAGS}")
