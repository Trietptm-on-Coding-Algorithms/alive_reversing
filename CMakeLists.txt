cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(ar VERSION 0.1 LANGUAGES C CXX)

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(SET_TARGET_PRECOMPILED_HEADER Target PrecompiledHeader PrecompiledSource)
  if(MSVC)
     SET_TARGET_PROPERTIES(${Target} PROPERTIES COMPILE_FLAGS "/Yu${PrecompiledHeader}")
     set_source_files_properties(${PrecompiledSource} PROPERTIES COMPILE_FLAGS "/Yc${PrecompiledHeader}")
  endif(MSVC)
endfunction(SET_TARGET_PRECOMPILED_HEADER)

enable_testing()
add_subdirectory(3rdParty)
add_subdirectory(Source/AliveLib)
add_subdirectory(Source/AliveExe)
add_subdirectory(Source/AliveDll)

if(WIN32)
    # On Windows generate ZIP packages
    SET(CPACK_GENERATOR "ZIP")
endif()
INCLUDE(CPack)
