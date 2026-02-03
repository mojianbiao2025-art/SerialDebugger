cmake_minimum_required(VERSION 3.16)

project(SerialDebugger VERSION 1.0.0 LANGUAGES CXX)

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

# Find Qt6
if(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly with Qt6")
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
else()
    message(STATUS "Building for native platform with Qt6")
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialPort)
endif()

message(STATUS "Using Qt ${Qt6_VERSION}")

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
if(ANDROID)
    qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
elseif(WIN32)
    qt_add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})
else()
    qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

# Link Qt libraries
if(EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
    )
    # Qt 6 WebAssembly settings
    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_WASM_PTHREAD_POOL_SIZE 0
        QT_WASM_INITIAL_MEMORY 128MB
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::SerialPort
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
if(ANDROID)
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
