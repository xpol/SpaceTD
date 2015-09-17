cmake_minimum_required (VERSION 2.8)

project(SpaceTD)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
	include
	${SDL2_INCLUDE_DIRS}
	${SDL2_GFX_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${SDL2_MIXER_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
)


set(PROJECT_VERSION "1.0.0")

file(GLOB SRC_FILES "src/*.c" "src/*.cc" "src/*.cpp")
file(GLOB_RECURSE INCS_H "include/*.h" "include/*.hpp")

add_executable(${PROJECT_NAME} ${SRC_FILES} ${INCS_H})
set_target_properties(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED TRUE
)

set(LIBS
	${SDL2_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	${SDL2_GFX_LIBRARIES}
	${SDL2_MIXER_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME} ${LIBS})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

	include (CMakeParseArguments)

#
#
# add_copy_target_for_ide_debugging(TARGET name FILES file1 [file2...] [FLATTEN])
function(add_copy_target_for_ide_debugging)

	set(options FLATTEN)
	set(oneValueArgs TARGET)
	set(multiValueArgs FILES)
	cmake_parse_arguments(ACTFID_ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if(CMAKE_GENERATOR STREQUAL Xcode)
		# for Xcode the debug working directory is the target directory
		cmake_policy(SET CMP0026 OLD) # allow get LOCATION property
		get_property(TARGET_LOCATION TARGET ${PROJECT_NAME} PROPERTY LOCATION)
		get_filename_component(DEBUG_WORKING_DIR ${TARGET_LOCATION} DIRECTORY)
	elseif(MSVC)
		# for MSVC the debug working directory is the project directory that is CMAKE_CURRENT_BINARY_DIR
		set(DEBUG_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR})
	endif()


	# list of files for which we add a copy rule
	set(OUTPUTS)
	foreach(ITEM IN LISTS ACTFID_ARG_FILES)
		if(ACTFID_ARG_FLATTEN)
			get_filename_component(BASE "${ITEM}" NAME)
			set(ITEM_OUTPUT_PATH "${DEBUG_WORKING_DIR}/${BASE}")
		else()
			set(ITEM_OUTPUT_PATH "${DEBUG_WORKING_DIR}/${ITEM}")
		endif()
		message(STATUS ${ITEM_OUTPUT_PATH})
		add_custom_command(
			OUTPUT "${ITEM_OUTPUT_PATH}"
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}" "${ITEM_OUTPUT_PATH}"
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}"
		)
		list(APPEND OUTPUTS "${ITEM_OUTPUT_PATH}")
	endforeach()

	message(STATUS ${ACTFID_ARG_TARGET})
	add_custom_target(${ACTFID_ARG_TARGET} ALL DEPENDS ${OUTPUTS})
endfunction(add_copy_target_for_ide_debugging)

# search_runtime_dlls(OUT )
macro(directory Var List)
	foreach(ITEM IN LISTS ${List})
		get_filename_component(DIR ${ITEM} DIRECTORY)
		list(APPEND ${Var} "${DIR}")
		#message(STATUS "${DIR}")
	endforeach()
	list(REMOVE_DUPLICATES ${Var})
endmacro(directory)

macro(append Var List Suffix)
	foreach(Item IN LISTS ${List})
		list(APPEND ${Var} "${Item}${Suffix}")
		#message(STATUS "${Item}${Suffix}")
	endforeach()
endmacro(append)


file(GLOB_RECURSE RESOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "resource/*.*")
add_copy_target_for_ide_debugging(TARGET CopyResources FILES ${RESOURCES})
add_dependencies(${PROJECT_NAME} CopyResources)


message(STATUS "dlls ${RUNTIME_DLLS}")

if(MSVC)
	directory(LIBDIRS LIBS)
	append(PATTERNS LIBDIRS "/*.dll")
	file(GLOB RUNTIME_DLLS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" ${PATTERNS})
	message(STATUS "${RUNTIME_DLLS}")
	add_copy_target_for_ide_debugging(TARGET CopyDlls FILES ${RUNTIME_DLLS} FLATTEN)
	add_dependencies(${PROJECT_NAME} CopyDlls)
endif()
