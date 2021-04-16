cmake_minimum_required (VERSION 3.0)
set (CMAKE_CXX_STANDARD 14)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

set(Qt5_DIR /opt/Qt/5.12.5/gcc_64/lib/cmake/Qt5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules")

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
      Core
      Widgets
)

set(tpl_engine_SRC
      Template.cpp
      Template.h
      TemplateMap.cpp
      TemplateMap.h
      TemplateObject.cpp
      TemplateObject.h
      TemplateParserException.cpp
      TemplateParserException.h
      TemplateList.cpp
      TemplateList.h
      TemplateExprs.cpp
      TemplateExprs.h
   )

include_directories(${Boost_INCLUDE_DIRS})

string(REPLACE "-fno-exceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_library(TplEngine STATIC ${tpl_engine_SRC})
target_link_libraries (TplEngine 
                       Qt5::Widgets
                       ${Boost_LIBRARIES})
