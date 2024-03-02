cmake_minimum_required(VERSION 3.28)

project(ClangBinder CXX)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_package(Clang REQUIRED CONFIG)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CLANG_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC
        clangBasic
        clangAST
        clangTooling
)
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