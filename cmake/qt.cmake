if(QT_VERSION VERSION_LESS 6.3)
    macro(qt_standard_project_setup)
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTOUIC ON)
        set(CMAKE_AUTORCC ON)
    endmacro()
endif()

if(QT_VERSION VERSION_LESS 6.0)
    macro(qt_add_executable name)
         if(ANDROID)
            add_library(name SHARED ${ARGN})
        else()
            add_executable(${ARGV})
        endif()
    endmacro()

    # Qt5 doesn't provide qt_add_resources(); enable AUTORCC instead.
    macro(qt_add_resources out_var)
        # No explicit resource target needed in Qt5 when CMAKE_AUTORCC is ON and .qrc is part of sources.
        set(${out_var} ${ARGN})
    endmacro()
endif()

# Function to locate windeployqt and add a post-build step to run it for a target.
function(run_windeployqt target)
  if(NOT WIN32)
    return()
  endif()

  # Allow caller to override WINDEPLOYQT_EXECUTABLE before calling
  if(NOT DEFINED WINDEPLOYQT_EXECUTABLE)
    # Try the Qt bin directory if user supplied QT5_DIR or QT_DIR
    if(DEFINED QT5_BIN_DIR)
      find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt.exe windeployqt HINTS "${QT5_BIN_DIR}" PATH NO_DEFAULT_PATH)
    endif()
    if(NOT WINDEPLOYQT_EXECUTABLE)
      find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt.exe windeployqt PATH $ENV{QTDIR} $ENV{QT_DIR})
    endif()
  endif()

  if(WINDEPLOYQT_EXECUTABLE)
    message(STATUS "windeployqt found: ${WINDEPLOYQT_EXECUTABLE}")
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E echo "Running windeployqt to deploy Qt runtime DLLs for ${target}"
      COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${target}>"
      COMMENT "Auto-deploying Qt DLLs with windeployqt"
    )
  else()
    message(WARNING "windeployqt not found. Qt runtime DLLs will not be auto-copied. Set QTDIR/Qt5_DIR or add windeployqt to PATH.")
  endif()
endfunction()
