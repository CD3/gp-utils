cmake_minimum_required( VERSION 3.9 )

project( gp-utils-applications VERSION 1.0.0.0 )

# We can't build applications unless the boost program_options library is available.
find_package( Boost ${${PROJECT_NAME}_Boost_LIBRARY_VERSION} REQUIRED COMPONENTS program_options )
add_subdirectory( .. gputils)

# The code below will detect all .cpp files in the `applications` directory
# and compile them into executables.

file( GLOB_RECURSE application_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	"*.cpp" 
	)
		
message(STATUS "Detected Application Sources:")
foreach(appSrc ${application_SOURCES})
	message(STATUS "  ${appSrc}" )
endforeach()
	
foreach(appSrc ${application_SOURCES})
	get_filename_component(appName ${appSrc} NAME_WE)
	add_executable( ${appName} ${appSrc} )
  install( TARGETS ${appName} DESTINATION bin )

	target_link_libraries( ${appName} 
	PUBLIC
		gputils::gputils
		Boost::program_options
		)

	set_target_properties( ${appName} 
	PROPERTIES 
		CXX_STANDARD 11 
		)

endforeach()
