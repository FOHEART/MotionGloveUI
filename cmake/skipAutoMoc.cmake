# skipAutoMoc.cmake
# Helper to prevent Qt AUTOMOC from parsing headers in transitive dependency source trees.

function(skip_automoc_in_deps)
  # cserialport examples
  file(GLOB_RECURSE _cserialport_example_headers
    "${CMAKE_BINARY_DIR}/_deps/cserialport-src/examples/*.h"
    "${CMAKE_BINARY_DIR}/_deps/cserialport-src/examples/*.hpp"
    "${CMAKE_BINARY_DIR}/_deps/cserialport-src/examples/**/*.h"
    "${CMAKE_BINARY_DIR}/_deps/cserialport-src/examples/**/*.hpp"
  )
  if(_cserialport_example_headers)
    set_source_files_properties(${_cserialport_example_headers} PROPERTIES SKIP_AUTOMOC ON)
  endif()

  foreach(_t CommQT commqt cserialport_commqt)
    if(TARGET ${_t})
      set_property(TARGET ${_t} PROPERTY AUTOMOC OFF)
    endif()
  endforeach()

  if(TARGET MotionGloveUI)
    target_include_directories(MotionGloveUI PRIVATE
      "${CMAKE_BINARY_DIR}/_deps/cserialport-src"
      "${CMAKE_BINARY_DIR}/_deps/cserialport-src/include"
      "${CMAKE_BINARY_DIR}/_deps/cserialport-src/src"
      "${CMAKE_BINARY_DIR}/_deps/cserialport-src/CSerialPort"
      "${CMAKE_BINARY_DIR}/_deps/cserialport-src/src/CSerialPort"
      "${CMAKE_BINARY_DIR}/_deps/cserialport-src/include/CSerialPort"
    )
  endif()

  # eigen demos
  file(GLOB_RECURSE _eigen3_demo_headers
    "${CMAKE_BINARY_DIR}/_deps/eigen3-src/demos/*.h"
    "${CMAKE_BINARY_DIR}/_deps/eigen3-src/demos/*.hpp"
    "${CMAKE_BINARY_DIR}/_deps/eigen3-src/demos/**/*.h"
    "${CMAKE_BINARY_DIR}/_deps/eigen3-src/demos/**/*.hpp"
  )
  if(_eigen3_demo_headers)
    set_source_files_properties(${_eigen3_demo_headers} PROPERTIES SKIP_AUTOMOC ON)
  endif()
endfunction()
