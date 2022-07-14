#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2021-2022 Bytedance Inc.
#
# https://axis-project.github.io
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.10)

set(APP_NAME Tanks4)

project(${APP_NAME})


if(NOT DEFINED BUILD_ENGINE_DONE) # to test Tanks4 into root project
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    set(AXIS_ROOT "$ENV{AXIS_ROOT}")

    if(NOT (AXIS_ROOT STREQUAL ""))
        set(AXIS_ROOT_PATH "${AXIS_ROOT}")
        file(TO_CMAKE_PATH ${AXIS_ROOT_PATH} AXIS_ROOT_PATH) # string(REPLACE "\\" "/" AXIS_ROOT_PATH ${AXIS_ROOT_PATH})
        message(STATUS "Using system env var AXIS_ROOT=${AXIS_ROOT}")
    else()
        message(FATAL_ERROR "Please run setup.py add system env var 'AXIS_ROOT' to specific the engine root")
    endif()

    set(CMAKE_MODULE_PATH ${AXIS_ROOT_PATH}/cmake/Modules/)

    include(AxisBuildSet)

    # axis cmake flags override for Tanks4
    set(AX_ENABLE_EXT_PHYSICS_NODE OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_ASSETMANAGER OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_COCOSTUDIO OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_DRAGONBONES OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_FAIRYGUI OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_PARTICLE3D OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_IMGUI OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_GUI OFF CACHE BOOL "")
    set(AX_ENABLE_EXT_SPINE OFF CACHE BOOL "")
    set(AX_WITH_BULLET OFF CACHE BOOL "")
    set(AX_WITH_BOX2D OFF CACHE BOOL "")
    set(AX_WITH_CHIPMUNK OFF CACHE BOOL "")
    set(AX_WITH_WEBP OFF CACHE BOOL "")
    set(AX_WITH_JPEG OFF CACHE BOOL "")
    set(AX_WITH_RECAST OFF CACHE BOOL "")
    set(AX_WITH_ZLIB OFF CACHE BOOL "")

    set(_AX_USE_PREBUILT FALSE)
    if (WIN32 AND DEFINED AX_PREBUILT_DIR AND IS_DIRECTORY ${AXIS_ROOT_PATH}/${AX_PREBUILT_DIR})
        set(_AX_USE_PREBUILT TRUE)
    endif()

    if (NOT _AX_USE_PREBUILT)
        add_subdirectory(${AXIS_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/axis/core)
    endif()
endif()

# compile flags for Tank4
# todo: add "CC_ENABLE_SCRIPT_BINDING=0", now it breaks compilation for android
target_compile_definitions(${AXIS_CORE_LIB} PUBLIC
        CC_USE_PHYSICS=0
        CC_USE_3D_PHYSICS=0
        CC_USE_NAVMESH=0
        CC_ENABLE_CHIPMUNK_INTEGRATION=0
        CC_USE_JPEG=0
        CC_USE_TIFF=0
        CC_USE_WEBP=0)

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/Engine.cpp
     Classes/Scenes/AboutScene.cpp
     Classes/Objects/TiledSprite.cpp
     Classes/Objects/Projectile.cpp
     Classes/Objects/GameObject.cpp
     Classes/Objects/Explosion.cpp
     Classes/Objects/Bonus.cpp
     Classes/GUI/SimpleMenuItem.cpp
     Classes/GUI/SimpleMenu.cpp
     Classes/GUI/ModalDialog.cpp
     Classes/Components/Pathfinder.cpp
     Classes/Components/AI.cpp
     Classes/Objects/LevelGrid.cpp
     Classes/Objects/Tank.cpp
     Classes/Scenes/GameplayScene.cpp
     Classes/Scenes/GameSettingsScene.cpp
     Classes/Scenes/HighScoresScene.cpp
     Classes/Scenes/MainMenuScene.cpp
     Classes/Scenes/SettingsScene.cpp
     Classes/SneakyInput/SneakyButtonSkinnedBase.cpp
     Classes/SneakyInput/SneakyJoystickSkinnedBase.cpp
     Classes/SneakyInput/SneakyJoystick.cpp
     Classes/SneakyInput/SneakyButton.cpp
     Classes/Scenes/BaseScene.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/Engine.h
     Classes/Objects/LevelGrid.h
     Classes/SneakyInput/SneakyButtonSkinnedBase.h
     Classes/SneakyInput/SneakyJoystickSkinnedBase.h
     Classes/SneakyInput/SneakyJoystick.h
     Classes/SneakyInput/SneakyButton.h
     Classes/Scenes/SettingsScene.h
     Classes/Scenes/MainMenuScene.h
     Classes/Scenes/HighScoresScene.h
     Classes/Scenes/GameplayScene.h
     Classes/Scenes/GameSettingsScene.h
     Classes/Scenes/BaseScene.h
     Classes/Scenes/AboutScene.h
     Classes/Objects/TiledSprite.h
     Classes/Objects/Tank.h
     Classes/Objects/Projectile.h
     Classes/Objects/GameObject.h
     Classes/Objects/Bonus.h
     Classes/Objects/Explosion.h
     Classes/Math/Pos2.h
     Classes/GUI/SimpleMenuItem.h
     Classes/GUI/SimpleMenu.h
     Classes/GUI/ModalDialog.h
     Classes/Components/Pathfinder.h
     Classes/Components/AI.h
     )

if(ANDROID)
    # the APP_NAME should match on AndroidManifest.xml
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app compile info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    # whole archive for jni
    add_subdirectory(${AXIS_ROOT_PATH}/core/platform/android ${ENGINE_BINARY_PATH}/core/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
    config_android_shared_libs("org.cocos2dx.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

if (NOT _AX_USE_PREBUILT)
    target_link_libraries(${APP_NAME} ${AXIS_CORE_LIB})
endif()

target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${AXIS_ROOT_PATH}/core/audio
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.axis.hellocpp")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    if(NOT _AX_USE_PREBUILT)
        cocos_copy_target_dll(${APP_NAME})
    endif()
endif()

if((WINDOWS AND (CMAKE_GENERATOR STREQUAL "Ninja")) OR LINUX)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
elseif(WINDOWS)
    set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${GAME_RES_FOLDER}")
    if(NOT DEFINED BUILD_ENGINE_DONE)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    endif()
endif()

# The optional thirdparties(not dependent by engine)
if (AX_WITH_YAML_CPP)
    target_include_directories(${APP_NAME} PRIVATE ${AXIS_ROOT_PATH}/thirdparty/yaml-cpp/include)
    target_link_libraries(${APP_NAME} yaml-cpp)
endif()

if (_AX_USE_PREBUILT) # support windows only
    include(${AXIS_ROOT_PATH}/cmake/Modules/AxisConfigDefine.cmake)
    use_axis_compile_define(${APP_NAME})

    include(${AXIS_ROOT_PATH}/cmake/Modules/AxisLinkHelpers.cmake)
    axis_link_cxx_prebuilt(${APP_NAME} ${AXIS_ROOT_PATH} ${AX_PREBUILT_DIR})
endif()