project(test
        VERSION ${VERSION}
        DESCRIPTION "tests - test apps for cxx17 library"
        LANGUAGES C CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

if (ENABLE_TESTS)
    # include(deps_inc_catch2)
    # include(deps_inc_fmt)
endif ()


define_test_program(basic basic.cc LIBRARIES libs::ticker_cxx)


define_test_program(type_name type_name.cc LIBRARIES libs::ticker_cxx)
define_test_program(thread_basics thread_basics.cc LIBRARIES libs::ticker_cxx)
define_test_program(periodical_job periodical_job.cc LIBRARIES libs::ticker_cxx)


define_test_program(ztk-timer ztk-timer.cc LIBRARIES libs::ticker_cxx)
define_test_program(ztk-ticker ztk-ticker.cc LIBRARIES libs::ticker_cxx)
define_test_program(ztk-alarm ztk-alarm.cc LIBRARIES libs::ticker_cxx)


# simple tests
# need c++20
define_test_program(enum enum.cc
        CXXSTANDARD 20)

# based on c++11
define_test_program(mon-obj mon-obj.cc
        CXXSTANDARD 11)

define_test_program(big-decimal big-decimal.cc
        CXXSTANDARD 11)


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