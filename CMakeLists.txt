# CMakeList.txt : CMake project for OsgEditor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project ("OsgStretch")

# set(OSG_DIR "C:/Program Files/OpenSceneGraph")
# set(QTDIR "C:/Qt/5.11.1/msvc2017_64")
if(NOT OSG_DIR)
    set(OSG_DIR $ENV{OSG_DIR})
    string(REPLACE "\\" "/" OSG_DIR "${OSG_DIR}")
endif()

set(CMAKE_MODULE_PATH "${OSG_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
# find_package(Qt5 REQUIRED COMPONENTS Core Gui OpenGL)
find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

set(HeaderFiles
	IPlanarCurve.h
    PickStretchHandler.h
    Utility.h
    PointIntersector.h
    CurveIntersector.h
    OsgControlPoints.h
    Line.h
	)
set(SourceFiles
	IPlanarCurve.cpp
	main.cpp
    PickStretchHandler.cpp
    Utility.cpp
    PointIntersector.cpp
    CurveIntersector.cpp
    OsgControlPoints.cpp
    Line.cpp
	)
set(ResourceFiles
	)
if(WIN32)
list(APPEND HeaderFiles targetver.h stdafx.h)
list(APPEND SourceFiles dllmain.cpp)
endif()

if(MSVC)
    set_source_files_properties(stdafx.cpp
        PROPERTIES
        COMPILE_FLAGS "/Yc"
        )
    foreach( src_file ${SourceFiles} )
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yustdafx.h"
            )
    endforeach()
endif(MSVC)

# Add source to this project's executable.
list(APPEND SourceFiles stdafx.cpp)
# add_library (OsgEditor SHARED ${HeaderFiles} ${SourceFiles} ${ResourceFiles})
add_executable (OsgStretch ${HeaderFiles} ${SourceFiles} ${ResourceFiles})
target_link_libraries(OsgStretch ${OPENSCENEGRAPH_LIBRARIES})
if(WIN32)
# include(GenerateExportHeader)
# GENERATE_EXPORT_HEADER(OsgStretch
#              EXPORT_MACRO_NAME OsgEditor_API
# )
endif()

source_group("Header Files" FILES ${HeaderFiles})
source_group("Source Files" FILES ${SourceFiles})
source_group("Resource Files" FILES ${ResourceFiles})

# TODO: Add tests and install targets if needed.
