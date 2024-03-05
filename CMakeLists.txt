cmake_minimum_required(VERSION 3.28)

project(ClangBinder CXX C)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_package(Clang REQUIRED CONFIG)
find_package(fmt REQUIRED)
find_package(Boost REQUIRED)
find_package(range-v3 REQUIRED)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CLANG_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC
        clangBasic
        clangAST
        clangTooling

        fmt::fmt
        range-v3::range-v3
        Boost::headers
)

include(ExternalProject)
ExternalProject_Add(TestExport
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/test_export
        BINARY_DIR ${CMAKE_BINARY_DIR}/test_export
        BUILD_COMMAND "" # Disable build step.
        INSTALL_COMMAND "" # Disable install step.
)
add_dependencies(${PROJECT_NAME} TestExport)

if (CLANGBINDER_INSTALL)
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Targets
            RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    install(EXPORT ${PROJECT_NAME}Targets DESTINATION lib/cmake)

    include(CMakePackageConfigHelpers)
    configure_package_config_file(${PROJECT_NAME}Config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib/cmake
    )
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            VERSION 0.0.0
            COMPATIBILITY SameMajorVersion)
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            DESTINATION lib/cmake
    )
endif ()