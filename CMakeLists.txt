cmake_minimum_required(VERSION 3.16)

project(SerialDebugger VERSION 1.0.0 LANGUAGES CXX)

# WebAssembly specific configuration - MUST be before find_package
if(EMSCRIPTEN)
    # Set CMAKE_PREFIX_PATH to help find Qt modules
    if(DEFINED ENV{QT_ROOT})
        list(APPEND CMAKE_PREFIX_PATH 
            "$ENV{QT_ROOT}"
            "$ENV{QT_ROOT}/lib/cmake"
            "$ENV{QT_ROOT}/lib/cmake/Qt5"
            "$ENV{QT_ROOT}/lib/cmake/Qt5Core"
            "$ENV{QT_ROOT}/lib/cmake/Qt5Gui"
            "$ENV{QT_ROOT}/lib/cmake/Qt5Widgets"
            "$ENV{QT_ROOT}/lib/cmake/Qt6"
            "$ENV{QT_ROOT}/lib/cmake/Qt6Core"
            "$ENV{QT_ROOT}/lib/cmake/Qt6Gui"
            "$ENV{QT_ROOT}/lib/cmake/Qt6Widgets"
        )
        message(STATUS "Using Qt from: $ENV{QT_ROOT}")
        message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Disable runtime checks to avoid linking errors
if(MSVC)
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTCc" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTCs" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTCu" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    # Add UTF-8 support for MSVC
    add_compile_options(/utf-8)
endif()

# Try to find Qt6 first, fall back to Qt5
if(EMSCRIPTEN)
    message(STATUS "Searching for Qt6 (EMSCRIPTEN mode)...")
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
    find_package(Qt6 QUIET COMPONENTS Core Gui Widgets)
    if(NOT Qt6_FOUND)
        message(STATUS "Qt6 not found, trying Qt5...")
        find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
        set(QT_VERSION_MAJOR 5)
    else()
        message(STATUS "Qt6 found!")
        set(QT_VERSION_MAJOR 6)
    endif()
else()
    message(STATUS "Searching for Qt6 (non-EMSCRIPTEN mode)...")
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
    message(STATUS "QT_HOST_PATH: $ENV{QT_HOST_PATH}")
    find_package(Qt6 QUIET COMPONENTS Core Gui Widgets SerialPort)
    if(NOT Qt6_FOUND)
        message(STATUS "Qt6 not found, trying Qt5...")
        find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets SerialPort)
        set(QT_VERSION_MAJOR 5)
    else()
        message(STATUS "Qt6 found!")
        set(QT_VERSION_MAJOR 6)
    endif()
endif()

message(STATUS "Using Qt${QT_VERSION_MAJOR}")

# Source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    translations.h
    translations.cpp
    plotwidget.h
    plotwidget.cpp
    resources.qrc
)

# Add WebAssembly specific sources
if(EMSCRIPTEN)
    list(APPEND PROJECT_SOURCES
        webserialport.cpp
        webserialport.h
    )
endif()

# Add Windows resource file for icon
if(WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} app_icon.rc)
endif()

# Create executable
if(ANDROID AND QT_VERSION_MAJOR EQUAL 6)
    # Qt6 Android requires qt_add_executable
    qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
elseif(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

# Link Qt libraries
if(EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
    )
    # WebAssembly specific settings
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] --bind"
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::SerialPort
    )
    
    # Android specific: link zlib
    if(ANDROID)
        target_link_libraries(${PROJECT_NAME} PRIVATE z)
    endif()
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Windows specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Android specific configuration for Qt6
if(ANDROID AND QT_VERSION_MAJOR EQUAL 6)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/android
    )
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Copy language files to build directory
file(GLOB LANG_FILES "${CMAKE_SOURCE_DIR}/lang_*.ini")
foreach(LANG_FILE ${LANG_FILES})
    get_filename_component(LANG_FILENAME ${LANG_FILE} NAME)
    configure_file(${LANG_FILE} ${CMAKE_BINARY_DIR}/bin/${LANG_FILENAME} COPYONLY)
endforeach()

# Copy styles.qss to build directory if it exists
if(EXISTS "${CMAKE_SOURCE_DIR}/styles.qss")
    configure_file("${CMAKE_SOURCE_DIR}/styles.qss" "${CMAKE_BINARY_DIR}/bin/styles.qss" COPYONLY)
    install(FILES "${CMAKE_SOURCE_DIR}/styles.qss" DESTINATION bin)
endif()

# Install language files
if(LANG_FILES)
    install(FILES ${LANG_FILES} DESTINATION bin)
endif()
