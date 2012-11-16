cmake_minimum_required(VERSION 2.8)

project(VesselRegistration)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(VesselRegistration vesselRegistration.cxx)

if( "${ITK_VERSION_MAJOR}" LESS 4)
	target_link_libraries(VesselRegistration ${ITK_LIBRARIES})
else( "${ITK_VERSION_MAJOR}" LESS 4)
	target_link_libraries(VesselRegistration ${ITK_LIBRARIES})
endif( "${ITK_VERSION_MAJOR}" LESS 4)
