#///////////////////////////////////////////////////////////////////////////////
#// Project:     wxECharts
#// Home:        https://github.com/PBfordev/wxecharts
#// File Name:   CMakeLists.txt
#// Purpose:     To build wxECharts application
#// Author:      PB
#// Created:     2024-08-25
#// Copyright:   (c) 2024 PB
#// Licence:     wxWindows licence
#///////////////////////////////////////////////////////////////////////////////

cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(wxECharts)

if(NOT(WIN32 OR LINUX))
  message(FATAL_ERROR "${PROJECT_NAME} does not support this platform.")
endif()

find_package(wxWidgets 3.2 COMPONENTS webview core base REQUIRED)
if(wxWidgets_USE_FILE)
  include(${wxWidgets_USE_FILE})
endif()

if(WIN32)
  # Don't know how to tell in CMake if the target CPU is x64 or Arm64 (when crosscompiling)
  if(NOT("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64"))
    message(FATAL_ERROR "The only supported CPU architecture on Windows is x64.")
  endif()

  set(WEBVIEW2_FOLDER "" CACHE PATH "Path to webview2 package")
  if("${WEBVIEW2_FOLDER}" STREQUAL "")
    message(FATAL_ERROR "Variable \"WEBVIEW2_FOLDER\" must be set to the webview2 folder, e.g. \"c:\\libs\\wxwidgets\\3rdparty\\webiew2\".")
  endif()
  set(WEBVIEW2_LOADER_DLL "${WEBVIEW2_FOLDER}/build/native/x64/WebView2Loader.dll")
  if(NOT EXISTS ${WEBVIEW2_LOADER_DLL})
    message(FATAL_ERROR "Variable \"WEBVIEW2_FOLDER\" contains incorrect path (\"${WEBVIEW2_FOLDER}\").")
  endif()
elseif(LINUX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GLIB REQUIRED gio-2.0)
  pkg_check_modules(WEBKIT2 webkit2gtk-4.0)
  if(NOT WEBKIT2_FOUND)
     pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.1)
  endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

set(SOURCES
  chartdlgs.cpp
  chartdlgs.h
  charthelper.cpp
  charthelper.h
  mainframe.cpp
  mainframe.h
  wxecharts.cpp
  wxecharts.h
)
if(WIN32)
  list(APPEND SOURCES "wxecharts.rc")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
)

target_include_directories(${PROJECT_NAME} PRIVATE nlohmann)
target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES})

if(MINGW) # work around the breaking change in wxWidgets 3.3
  target_link_libraries(${PROJECT_NAME} PRIVATE gdiplus msimg32)
endif()

if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE wxUSE_RC_MANIFEST wxUSE_DPI_AWARE_MANIFEST=2)
  target_include_directories(${PROJECT_NAME} PRIVATE "${WEBVIEW2_FOLDER}/build/native/include")
  set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

  if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_DEPRECATE _CRT_NON_CONFORMING_SWPRINTFS _SCL_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /MP)
    target_link_options(${PROJECT_NAME} PRIVATE /MANIFEST:NO)
  else() # GCC or clang
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations)
  endif()
elseif(LINUX)
  target_include_directories(${PROJECT_NAME} PRIVATE ${GIO_INCLUDE_DIRS})
  target_link_directories(${PROJECT_NAME} PRIVATE ${GIO_LIBRARY_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${GIO_LIBRARIES})

  target_include_directories(${PROJECT_NAME} PRIVATE ${WEBKIT2_INCLUDE_DIRS})
  target_link_directories(${PROJECT_NAME} PRIVATE ${WEBKIT2_LIBRARY_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${WEBKIT2_LIBRARIES})
endif()

# copy WebView2Loader.dll to the folder with the application executable
if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${WEBVIEW2_LOADER_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

# copy the chart assets (HTML and JavaScript files)
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/chart-assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/chart-assets" "${CMAKE_BINARY_DIR}/chart-assets"
)