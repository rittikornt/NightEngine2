cmake_minimum_required(VERSION 3.0)
project(NightEngine2)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#****************************************************************

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(NightEngine2/thirdparty/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(NightEngine2/thirdparty/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(NightEngine2/thirdparty/bullet)

add_subdirectory(NightEngine2/)
#****************************************************************

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(NightEngine2/src/
                    ${CMAKE_CURRENT_BINARY_DIR}/NightEngine2/thirdparty/assimp/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/NightEngine2/thirdparty/assimp/include/
                    NightEngine2/thirdparty/bullet/src/
                    NightEngine2/thirdparty/glad/include/
                    NightEngine2/thirdparty/glfw/include/
                    NightEngine2/thirdparty/glm/
                    NightEngine2/thirdparty/stb/
                    NightEngine2/thirdparty/taocpp_json
                    NightEngine2/thirdparty/imgui)

#****************************************************************

file(GLOB THIRDPARTY_SOURCES NightEngine2/thirdparty/glad/src/glad.c)

file(GLOB PROJECT_SOURCES_IMGUI NightEngine2/thirdparty/imgui/imgui/*.cpp
                                NightEngine2/thirdparty/imgui/imgui/*.h)

#file(GLOB_RECURSE PROJECT_SOURCES NightEngine2/Sources/*.cpp)
file(GLOB PROJECT_SOURCES NightEngine2/src/*.cpp
                        NightEngine2/src/*.hpp
                        NightEngine2/src/*.h)

file(GLOB PROJECT_SOURCES_INPUT NightEngine2/src/Input/*.cpp
                                NightEngine2/src/Input/*.hpp
                                NightEngine2/src/Input/*.inl)

file(GLOB PROJECT_SOURCES_PHYSIC NightEngine2/src/Physics/*.cpp
                               NightEngine2/src/Physics/*.hpp)

file(GLOB PROJECT_SHADERS Assets/Shaders/*.comp
                            Assets/Shaders/*.frag
                            Assets/Shaders/*.geom
                            Assets/Shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

#****************************************************************

#https://stackoverflow.com/questions/31422680/how-to-set-visual-studio-filters-for-nested-sub-directory-using-cmake
source_group("src" FILES ${PROJECT_SOURCES})

source_group("shaders" FILES ${PROJECT_SHADERS})

source_group("src\\Physics" FILES ${PROJECT_SOURCES_PHYSIC})

source_group("src\\Editor" FILES ${PROJECT_SOURCES_EDITOR})
source_group("src\\Input" FILES ${PROJECT_SOURCES_INPUT})

source_group("thirdparty" FILES ${THIRDPARTY_SOURCES})
source_group("thirdparty\\imgui" FILES ${PROJECT_SOURCES_IMGUI})

#****************************************************************

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               $<TARGET_OBJECTS:NightEngine2_Core>
                               ${PROJECT_SOURCES_IMGUI}
                               ${THIRDPARTY_SOURCES} 
                               $<TARGET_OBJECTS:NightEngine2_Graphics> 
                               $<TARGET_OBJECTS:NightEngine2_Editor> 
                               ${PROJECT_SOURCES_INPUT} 
                               $<TARGET_OBJECTS:NightEngine2_UnitTest> 
                               ${PROJECT_SOURCES_PHYSIC})

target_link_libraries(${PROJECT_NAME} assimp glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
                      
# Properties->General->Output Directory
# "Build/NightEngine2/{Debug/Release}"
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})