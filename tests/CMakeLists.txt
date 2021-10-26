project(test
        VERSION ${VERSION}
        DESCRIPTION "tests - test apps for cxx17 library"
        LANGUAGES C CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

if (ENABLE_TESTS)
    # include(deps_inc_catch2)
    # include(deps_inc_fmt)
endif ()

find_package(Threads REQUIRED)

function(define_test_program name)
    #set(src_list )
    foreach (f ${ARGN})
        list(APPEND src_list ${f})
    endforeach ()

    # ensure_options_values(${PROJECT_MACRO_NAME} ${PROJECT_MACRO_PREFIX})

    add_executable(${PROJECT_NAME}-${name} ${src_list})
    #target_compile_features(${PROJECT_NAME}-${name} PRIVATE cxx_std_11)
    #target_compile_definitions(${PROJECT_NAME}-${name} PRIVATE)
    target_compile_definitions(${PROJECT_NAME}-${name} PRIVATE
            ${PROJECT_MACRO_PREFIX}_ENABLE_ASSERTIONS=${_${PROJECT_MACRO_NAME}_enable_assertions}
            ${PROJECT_MACRO_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJECT_MACRO_NAME}_enable_precondition_checks}
            ${PROJECT_MACRO_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${PROJECT_MACRO_NAME}_enable_thread_pool_ready_signal}
            ${PROJECT_MACRO_PREFIX}_ENABLE_VERBOSE_LOG=${_${PROJECT_MACRO_NAME}_enable_verbose_log}
            ${PROJECT_MACRO_PREFIX}_TEST_THREAD_POOL_DBGOUT=${_${PROJECT_MACRO_NAME}_enable_thread_pool_dbgout}
            ${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
            #${PROJECT_MACRO_PREFIX}_UNIT_TEST=1
            #UNIT_TESTING=1
            )
    target_include_directories(${PROJECT_NAME}-${name} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
            ${CMAKE_SOURCE_DIR}
            )
    target_link_libraries(${PROJECT_NAME}-${name}
            PRIVATE
            Threads::Threads
            ticker_cxx
            # undo_cxx
            # fsm_cxx
            # cmdr11::cmdr11
            # Catch2::Catch2
            # fmt::fmt-header-only
            )
    if (MSVC)
        target_compile_options(${PROJECT_NAME}-${name} PRIVATE /W4 /WX /utf-8
                #/D${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
                )
    else ()
        target_compile_options(${PROJECT_NAME}-${name} PRIVATE
                -pedantic -Wall -Wextra -Wshadow -Werror -pthread
                #-D${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
                )
        if (DEBUG)
            target_compile_options(${PROJECT_NAME}-${name} PRIVATE
                    -fsanitize=address -fno-omit-frame-pointer
                    # address(AddressSanitizer), leak(LeakSanitizer), thread(ThreadSanitizer), undefined(UndefinedBehaviorSanitizer), memory(MemorySanitizer) 
                    # [additional_options]: -fno-omit-frame-pointer, fsanitize-recover/fno-sanitize-recover, -fsanitize-blacklist, etc.
                    # [-g] [-OX]
                    )
            target_link_options(${PROJECT_NAME}-${name} PRIVATE -fsanitize=address)
        endif ()
    endif ()

    if (${ENABLE_AUTOMATE_TESTS})
        get_property(tmp GLOBAL PROPERTY UNIT_TEST_TARGETS)
        set(tmp ${UNIT_TEST_TARGETS} ${PROJECT_NAME}-${name})
        set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS "${tmp}")
        message(">> add_test(${PROJECT_NAME}-${name}) ...")
    endif ()

    if (ANDROID)
        add_test(NAME ${PROJECT_NAME}-${name}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMAND ${CMAKE_COMMAND} "-DANDROID_NDK=${ANDROID_NDK}"
                "-DTEST_RESOURCES_DIR=${CMAKE_SOURCE_DIR}"
                "-DTEST_RESOURCES=tests/data;tests/file_data.txt;Makefile"
                "-DUNITTEST=${PROJECT_NAME}-${name}"
                -P ${CMAKE_CURRENT_SOURCE_DIR}/ExecuteOnAndroid.cmake)
    else ()
        add_test(NAME ${PROJECT_NAME}-${name}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMAND $<TARGET_FILE:${PROJECT_NAME}-${name}>)
    endif ()
endfunction()


define_test_program(basic basic.cc)
define_test_program(ticker ticker.cc)


#


#define_test_program(cxx20-coroutine cxx20-coroutine.cc)
#add_cxx_20_to(test-cxx20-coroutine)

#define_test_program(cxx20-one
#        cxx20-one.cc
#        cxx20-one/v1.cc
#        cxx20-one/mod1/mod.cc
#        cxx20-one/mod2/mod.cc
#        cxx20-one/mod3/mod.cc
#        )
#add_cxx_20_to(test-cxx20-one)


message(STATUS "END of tests")