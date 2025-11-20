cmake_minimum_required(VERSION 3.26)
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")


project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

set(SFML_WARNINGS_AS_ERRORS OFF)

# NOTE: Also update SFML_VERSION env var in .github/workflows/cmake.yml:33
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        # renovate: datasource=github-tags depName=SFML/SFML versioning=loose
        GIT_TAG        3.0.1
        # renovate: depName=SFML/SFML
        # GIT_TAG        016bea9491ccafc3529019fe1d403885a8b3a6ae # 3.0.1
        GIT_SHALLOW    1  # works only with branches or tags, not with arbitrary commit hashes
)

FetchContent_MakeAvailable(SFML)

find_package(Threads REQUIRED)
if(APPLE)
elseif(UNIX)
    find_package(X11)
endif()

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing name here
add_executable(${MAIN_EXECUTABLE_NAME}
        main.cpp
        src/MenuResolution.cpp
        src/MenuResolution.h
        src/GameManager.cpp
        src/GameManager.h
        src/Block.cpp
        src/Block.h
        src/PicrossHints.cpp
        src/PicrossHints.h
        src/Grid.cpp
        src/Grid.h
        src/GridRenderer.cpp
        src/GridRenderer.h
        src/GameMenu.cpp
        src/GameMenu.h
        src/MenuPanorama.cpp
        src/MenuPanorama.h
        src/SplashText.cpp
        src/MenuPanorama.cpp
        src/SplashText.cpp
        src/MenuButton.cpp
        src/MenuButton.h
        src/ShadowedText.cpp
        src/ShadowedText.h
        src/GameOverScreen.cpp
        src/ScoreMode.cpp
        src/MistakesMode.cpp
        src/GameMode.cpp
        src/GameOverScreen.h
)

target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ${SFML_SOURCE_DIR}/include)
target_link_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${SFML_BINARY_DIR}/lib)
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE SFML::Graphics SFML::Window SFML::Audio SFML::System Threads::Threads)
if(APPLE)
elseif(UNIX)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE X11)
endif()
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()
copy_files(FILES tastatura.txt TARGET_NAME ${MAIN_EXECUTABLE_NAME})
copy_files(DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
copy_files(DIRECTORY nivele COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})