cmake_minimum_required(VERSION 3.4)
project(3DOpenGL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

set(SOURCE_FILES src/main.cpp src/Utility/float2.h src/Utility/float3.h src/Objects/Mesh.h src/Objects/Mesh.cpp src/Utility/stb_image.c src/Objects/ObjectCollection.cpp src/Objects/ObjectCollection.h src/Objects/Object.cpp src/Objects/Object.h src/Materials/Material.cpp src/Materials/Material.h src/Materials/TexturedMaterial.cpp src/Materials/TexturedMaterial.h src/Camera.cpp src/Camera.h src/Lighting/LightSource.cpp src/Lighting/LightSource.h src/Lighting/DirectionalLight.cpp src/Lighting/DirectionalLight.h src/Lighting/PointLight.cpp src/Lighting/PointLight.h src/Objects/Billboard.cpp src/Objects/Billboard.h src/Objects/Literals/Teapot.cpp src/Objects/Literals/Teapot.h src/Objects/Literals/Ground.cpp src/Objects/Literals/Ground.h src/Objects/MeshInstance.cpp src/Objects/MeshInstance.h src/Objects/Literals/Bullet.cpp src/Objects/Literals/Bullet.h src/Objects/Properties/Mover.cpp src/Objects/Properties/Mover.h src/Objects/Properties/LifeSpan.cpp src/Objects/Properties/LifeSpan.h)
add_executable(3DOpenGL ${SOURCE_FILES})

target_link_libraries(3DOpenGL ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} winmm.lib)