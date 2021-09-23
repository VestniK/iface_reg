macro(lib)
    set(opts "")
    set(oneval_args NAME STD)
    set(multyval_args LIBS TEST_LIBS)
    cmake_parse_arguments(LIB_PRJ
        "${opts}"
        "${oneval_args}"
        "${multyval_args}"
        ${ARGN}
    )
    if (NOT LIB_PRJ_STD)
        SET(LIB_PRJ_STD cxx_std_17)
    endif()

    file(GLOB GLOB_SRCS CONFIGURE_DEPENDS *.cpp)
    file(GLOB GLOB_HDRS CONFIGURE_DEPENDS *.hpp)


    # IMPL lib
    set(PRJ_IMPL_SRC ${GLOB_SRCS} ${GLOB_HDRS})
    list(FILTER PRJ_IMPL_SRC EXCLUDE REGEX main.cpp)
    list(FILTER PRJ_IMPL_SRC EXCLUDE REGEX "^.*\.test.[hc]pp$")

    if (PRJ_IMPL_SRC)
        add_library(${LIB_PRJ_NAME} OBJECT)
        target_sources(${LIB_PRJ_NAME} PRIVATE ${PRJ_IMPL_SRC})
        target_compile_features(${LIB_PRJ_NAME} PUBLIC ${LIB_PRJ_STD})
        target_include_directories(${LIB_PRJ_NAME}
          PUBLIC
            ${PROJECT_SOURCE_DIR}
            ${PROJECT_BINARY_DIR}
        )
        set_target_properties(${LIB_PRJ_NAME} PROPERTIES LINKER_LANGUAGE CXX)
        if (LIB_PRJ_LIBS)
            target_link_libraries(${LIB_PRJ_NAME} PUBLIC ${LIB_PRJ_LIBS})
        endif()
    endif()

    # TESTS
    set(PRJ_TEST_SRC ${GLOB_SRCS} ${GLOB_HDRS})
    list(FILTER PRJ_TEST_SRC EXCLUDE REGEX main.cpp)
    list(FILTER PRJ_TEST_SRC INCLUDE REGEX "^.*\.test.[hc]pp$")

    if (TARGET ${LIB_PRJ_NAME} AND PRJ_TEST_SRC)
        add_executable(${LIB_PRJ_NAME}.test)
        target_sources(${LIB_PRJ_NAME}.test PRIVATE ${PRJ_TEST_SRC})
        target_link_libraries(${LIB_PRJ_NAME}.test PRIVATE ${LIB_PRJ_NAME} ${LIB_PRJ_TEST_LIBS})
        add_test(NAME ${LIB_PRJ_NAME}-test COMMAND ${LIB_PRJ_NAME}.test)
    endif()
endmacro()
