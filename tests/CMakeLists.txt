include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# If pkg-config is not installed on the system, then the
# CHECK_INSTALL_DIR variable must be set to the install
# location of Check. For example, on Windows, this may
# be: C:/Program Files/check
# set(CHECK_INSTALL_DIR "C:/Program Files/check")

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

function(add_my_test target)
  add_executable(check_${target} ${ARGN})
  target_link_libraries(check_${target} ${target} ${CHECK_LIBRARIES})
  add_test(NAME ${target} COMMAND check_${target})
endfunction()

add_my_test(func check_func.c)
add_my_test(warn check_warn.c)


# manual test addition since the pattern is slightly different
# than the macro above.
add_executable(check_lcab check_lcab.c util.c util.h)
target_link_libraries(check_lcab cab ${CHECK_LIBRARIES})
add_test(NAME lcab COMMAND check_lcab)


add_executable(check_parse_dff check_parse_dff.c util.c util.h)
target_link_libraries(check_parse_dff parse_dff cab ${CHECK_LIBRARIES})
add_test(NAME parse_dff COMMAND check_parse_dff)


# copy test.dff to the output directory so the
# tests can use it.
configure_file(test.dff test.dff COPYONLY)
configure_file(testcab.cab testcab.cab COPYONLY)



# add additonal files for cleaning
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "test.dff" "testcab" "out.cab")
