
set(HEADERS ${CONFIG_HEADER}
  cli.h
  lcab.h
  lcab_config.h
  cdata.h
  cfile.h
  cfolder.h
  cheader.h
  checksum.h
  cwrite.h
  cstruct.h
  mytypes.h
  utils.h
  parse_dff.h
  file_entry.h
  func.h)


add_library(cab STATIC
  cli.c
  lcab.c
  cdata.c
  cfile.c
  cfolder.c
  cheader.c
  checksum.c
  cwrite.c
  utils.c)

add_library(func STATIC ${HEADERS} func.c)
add_library(warn STATIC ${HEADERS} warn.c)
add_library(parse_dff STATIC ${HEADERS} parse_dff.c)

target_link_libraries(cab func warn)
target_link_libraries(func warn)

add_executable(lcab ${HEADERS} cli.c)
target_link_libraries(lcab cab parse_dff)

install(TARGETS lcab RUNTIME DESTINATION bin)
# LIBRARY DESTINATION lib
# ARCHIVE DESTINATION lib)

# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/money.h DESTINATION include)
