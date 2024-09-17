add_library(jml STATIC
  jml.c
  jml.h

  list.c
  list.h

  cwalk.c
  cwalk.h

  utils.c
  utils.h
)
target_include_directories(jml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(jml PUBLIC )

add_executable(justmake main.c)
target_link_libraries(justmake jml)
