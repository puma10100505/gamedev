CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(GameDev)

# SET(ProjectName SimpleGame)
# SET(ProjectName GamePhysx)
SET(ProjectName PlayGround)

SET(BinName ${ProjectName})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")

SET(SolutionRoot ${CMAKE_CURRENT_SOURCE_DIR}/)
SET(ProjectRoot ${CMAKE_CURRENT_SOURCE_DIR}/src/${ProjectName})

FILE(GLOB SourceFiles 
	${ProjectRoot}/*.cpp
	${SolutionRoot}/src/glad.c
	${SolutionRoot}/src/imgui/*.cpp
)

INCLUDE_DIRECTORIES(
	${SolutionRoot}/include
	${ProjectRoot}
	/usr/local/include
	/usr/include
)

LINK_DIRECTORIES(
	/usr/local/lib
	/usr/lib64
	${SolutionRoot}/lib
)

MESSAGE(${BinName})
MESSAGE(${SourceFiles})
MESSAGE(${SolutionRoot})
MESSAGE(${ProjectRoot})

FIND_LIBRARY(lib_ODE ode /usr/local/lib)
FIND_LIBRARY(lib_Drawstuff drawstuff /usr/local/lib)
FIND_LIBRARY(lib_GLFW glfw3 /usr/local/lib)

ADD_EXECUTABLE(
	${BinName}
	${SourceFiles}
)

TARGET_LINK_LIBRARIES(
	${BinName}
	${lib_ODE}
	${lib_GLFW}
	${lib_Drawstuff}
	-ldl
	-lstdc++
	-lpthread
	-lrt
	-lGL
	-lX11
	-lGLU
	-lGLEW
	-lSDL2
	)

MESSAGE(${CMAKE_BINARY_DIR})

