#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16...3.21)
project(B4d)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(ROOT REQUIRED)
message(STATUS "Found Root ${ROOT_LIBRARY_DIR}")
set( ROOT_INC "${ROOT_LIBRARY_DIR}/../include")
message(STATUS "Found Root ${ROOT_INC}")
include_directories(${ROOT_INC})
link_directories(${ROOT_LIBRARY_DIR})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


# #####################
# #generate ROOT dictionary for detectors by CLING
# set (DICT myDet.cxx)
# set (CLING rootcint)
# set (LINKDEF LinkDef.h)
# set (HDRS_DET PrimaryGenerator.hh)

# #include for CLING
# get_directory_property(incdirs INCLUDE_DIRECTORIES)
# foreach(_inc ${incdirs})
#   set(dict_include ${dict_include} -I${_inc})
# endforeach()

# add_custom_command (
#   OUTPUT ${DICT}
#   COMMAND ${CLING}  
#   ARGS -f ${DICT} ${dict_include}
#   ${HDRS_DET} ${LINKDEF}
#   DEPENDS include/${LINKDEF}
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# )
# #####################


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(exampleB4d exampleB4d.cc ${sources} ${headers})
#########target_link_libraries(exampleB4d ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
target_link_libraries(exampleB4d ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${DICT})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS exampleB4d DESTINATION bin)
