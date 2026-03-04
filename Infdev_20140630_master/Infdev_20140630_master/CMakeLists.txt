include("CMakeFunctions.txt")

cmake_minimum_required(VERSION 3.8)

define_property(TARGET PROPERTY FOLDER INHERITED)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add source to this project's executable.
try_create_symlink(SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/src/" "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(INC_DIR "${SRC_DIR}")
set(EXT_DIR "vendor")

file(GLOB_RECURSE ALL_FILES
	"${INC_DIR}/*.h"
	"${SRC_DIR}/*.cpp"
)

# EXTERNAL LIBRARIES
set(INCLUDE_DIRS
	${INC_DIR}
	${EXT_DIR}
	${EXT_DIR}/bullet3/src
	${EXT_DIR}/assimp/include
	${EXT_DIR}/SFML/include
	${EXT_DIR}/RmlUi/Include
	${EXT_DIR}/bullet3/src
	${EXT_DIR}/assimp/include
	${EXT_DIR}/SFML/include
	${EXT_DIR}/RmlUi/Include
	${EXT_DIR}/other/include
	${EXT_DIR}/rapidjson/include
	${EXT_DIR}/cppu/include
	${EXT_DIR}/glm
#	${EXT_DIR}/glew/include
	G:/Projects/C++/extlibs/GLEW/include
	${EXT_DIR}/asio/asio/include
)

# PREPROCESSOR DEFINITIONS
set(COMPILE_DEFINITIONS
	ESTEEM_EXPORT_STATIC
	
	CPPU_USE_NAMESPACE
	
	GLEW_STATIC
	SFML_STATIC
	
	GLM_FORCE_INTRINSICS
	GLM_ENABLE_EXPERIMENTAL
	GLM_FORCE_RADIANS
	GLM_FORCE_LEFT_HANDED
	GLM_FORCE_DEPTH_ZERO_TO_ONE
	
	RMLUI_STATIC_LIB
	RMLUI_VERSION="4.0-dev"
	STB_TRUETYPE_IMPLEMENTATION
)

if(MSVC)
	list(APPEND COMPILE_DEFINITIONS
		WIN32
		WIN32_LEAN_AND_MEAN
		_WIN32_WINNT=0x0601 # Windows 7
		
		_CRT_SECURE_NO_DEPRECATE
		_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
	)
endif()

# PROGRAM
project(Esteem)

# PROJECT SETUP
add_library(Esteem STATIC ${ALL_FILES})
target_include_directories(Esteem PUBLIC ${INCLUDE_DIRS})
target_compile_definitions(Esteem PUBLIC ${COMPILE_DEFINITIONS})

# PROJECT OUTPUT
set_target_properties(Esteem PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
	
	INSTALL_RPATH "@loader_path/../Frameworks"
	
	DEBUG_POSTFIX -d_${CMAKE_SYSTEM_PROCESSOR}
	RELEASE_POSTFIX _${CMAKE_SYSTEM_PROCESSOR}
	MINSIZEREL_POSTFIX _${CMAKE_SYSTEM_PROCESSOR}
)

# DEPENDENCIES
add_subproject(Esteem PATH "vendor/bullet3" INTERFACE BulletSoftBody BulletDynamics BulletCollision LinearMath)
add_subproject(Esteem PATH "vendor/SFML" INTERFACE sfml-main sfml-system sfml-audio sfml-graphics sfml-window sfml-network)
add_subproject(Esteem PATH "vendor/assimp" INTERFACE assimp)
add_subproject(Esteem PATH "vendor/RmlUi" INTERFACE RmlCore)
add_subproject(Esteem PATH "vendor/minizip" INTERFACE minizip)
add_subproject(Esteem PATH "vendor/zlib" INTERFACE zlib)

foreach(library BulletSoftBody BulletDynamics BulletCollision)
	target_compile_definitions(${library} PUBLIC "BULLET_VIRTUAL_SWEEP_TEST=virtual")
endforeach()

if(MSVC)
	target_link_directories(Esteem INTERFACE
#		"${EXT_DIR}/glew/lib/Release/$(Platform)/"
		G:/Projects/C++/extlibs/GLEW/lib
	)
	
	target_link_libraries(Esteem INTERFACE
		glu32
		glew32s
	)
endif()

# REORGANIZE FOLDERS
get_property_folder_base(BASE_FOLDER)
set_target_properties(
	sfml-main
	sfml-system
	sfml-audio
	sfml-graphics
	sfml-window
	sfml-network
	PROPERTIES FOLDER "${BASE_FOLDER}vendor/SFML"
)

# ALTER DEPENDENCIES
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/contrib/unzip/unzip.c" TARGET_DIRECTORY assimp PROPERTIES HEADER_FILE_ONLY ON)