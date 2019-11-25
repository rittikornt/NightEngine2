#****************************************************************
# UnitTest
#****************************************************************
set(proj_name "NightEngine2_UnitTest")
file(GLOB PROJECT_SOURCES_TEST src/UnitTest/*.cpp
                               src/UnitTest/*.hpp)
source_group("src" FILES ${PROJECT_SOURCES_TEST})
include_directories(src/
                    thirdparty/glad/include/
                    thirdparty/glfw/include/
                    thirdparty/glm/
                    thirdparty/taocpp_json
                    thirdparty/imgui)

add_nightengine2_target(${proj_name} OBJECT ${PROJECT_SOURCES_TEST})

# Properties->General->Output Directory
set_target_properties(${proj_name} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

# Adds logic to INSTALL.vcproj to copy math.dll to the destination directory
#install (TARGETS ${proj_name}
#           RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
#           LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

#****************************************************************
# Graphics
#****************************************************************
set(proj_name "NightEngine2_Graphics")
file(GLOB PROJECT_SOURCES_GRAPHIC src/Graphic/*.cpp
                              src/Graphic/*.hpp)
source_group("src" FILES ${PROJECT_SOURCES_GRAPHIC})

file(GLOB PROJECT_SOURCES_GRAPHIC_OPENGL src/Graphic/Opengl/*.cpp
                              src/Graphic/Opengl/*.hpp)
source_group("src\\Opengl" FILES ${PROJECT_SOURCES_GRAPHIC_OPENGL})

file(GLOB PROJECT_SOURCES_POSTPROCESS_OPENGL src/Graphic/Opengl/Postprocess/*.cpp
                                      src/Graphic/Opengl/Postprocess/*.hpp)
source_group("src\\Opengl\\PostProcess" FILES ${PROJECT_SOURCES_POSTPROCESS_OPENGL})

include_directories(src/
                    ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/assimp/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/include/
                    thirdparty/bullet/src/
                    thirdparty/glad/include/
                    thirdparty/glfw/include/
                    thirdparty/glm/
                    thirdparty/stb/
                    thirdparty/taocpp_json
                    thirdparty/imgui)

                    add_nightengine2_target(${proj_name} OBJECT ${PROJECT_SOURCES_GRAPHIC} 
                                                                ${PROJECT_SOURCES_GRAPHIC_OPENGL} 
                                                                ${PROJECT_SOURCES_POSTPROCESS_OPENGL})

set_target_properties(${proj_name} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

#****************************************************************
# Editor
#****************************************************************
set(proj_name "NightEngine2_Editor")
file(GLOB PROJECT_SOURCES_EDITOR src/Editor/*.cpp
                                  src/Editor/*.hpp)
source_group("src" FILES ${PROJECT_SOURCES_EDITOR})

include_directories(src/
                    ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/assimp/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/include/
                    thirdparty/bullet/src/
                    thirdparty/glad/include/
                    thirdparty/glfw/include/
                    thirdparty/glm/
                    thirdparty/stb/
                    thirdparty/taocpp_json
                    thirdparty/imgui)

                    add_nightengine2_target(${proj_name} OBJECT ${PROJECT_SOURCES_EDITOR})

set_target_properties(${proj_name} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

#****************************************************************
# Core
#****************************************************************
set(proj_name "NightEngine2_Core")
file(GLOB PROJECT_SOURCES_CORE src/Core/Utility/*.cpp
                                src/Core/Utility/*.hpp
                                src/Core/*.cpp
                                src/Core/*.hpp)
source_group("src\\" FILES ${PROJECT_SOURCES_CORE})

file(GLOB PROJECT_SOURCES_REFLECTION src/Core/Reflection/*.cpp
                               src/Core/Reflection/*.hpp)
source_group("src\\Reflection" FILES ${PROJECT_SOURCES_REFLECTION})

file(GLOB PROJECT_SOURCES_CONTAINER src/Core/Container/*.cpp
                                src/Core/Container/*.hpp)
source_group("src\\Container" FILES ${PROJECT_SOURCES_CONTAINER})

file(GLOB PROJECT_SOURCES_EC            src/Core/EC/*.cpp
                                        src/Core/EC/*.hpp
                                        src/Core/EC/Components/*.cpp
                                        src/Core/EC/Components/*.hpp)
source_group("src\\EC" FILES ${PROJECT_SOURCES_EC})

file(GLOB PROJECT_SOURCES_SERIALIZATION src/Core/Serialization/*.cpp
                                        src/Core/Serialization/*.hpp)
source_group("src\\Serialization" FILES ${PROJECT_SOURCES_SERIALIZATION})

file(GLOB PROJECT_SOURCES_MESSAGE src/Core/Message/*.cpp
                               src/Core/Message/*.hpp)
source_group("src\\Message" FILES ${PROJECT_SOURCES_MESSAGE})

include_directories(src/
                    ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/assimp/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/include/
                    thirdparty/bullet/src/
                    thirdparty/glad/include/
                    thirdparty/glfw/include/
                    thirdparty/glm/
                    thirdparty/stb/
                    thirdparty/taocpp_json
                    thirdparty/imgui)
                    
                    add_nightengine2_target(${proj_name} OBJECT ${PROJECT_SOURCES_REFLECTION}
                                    ${PROJECT_SOURCES_CORE}
                                    ${PROJECT_SOURCES_CONTAINER}
                                    ${PROJECT_SOURCES_MESSAGE}
                                    ${PROJECT_SOURCES_EC}
                                    ${PROJECT_SOURCES_SERIALIZATION})

# Properties->General->Output Directory
set_target_properties(${proj_name} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

#****************************************************************