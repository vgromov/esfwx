# static framework components linkage pseudo-project
cmake_minimum_required(VERSION 2.8)
project(static-linkage)

# reset certain cache values
# remember BUILD_SHARED_LIBS setting to restore it later
set(oldBUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# wxWidgets dependencies, force wxWidgets search re-cache, in static mode this time
set(wxWidgets_DEFINITIONS "" CACHE INTERNAL "" FORCE)
set(wxWidgets_FOUND 0 CACHE INTERNAL "" FORCE)
set(wxWidgets_LIB_DIR "NOTFOUND" CACHE PATH "" FORCE)
set(wxWidgets_USE_LIBS base core net adv)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})
# force freechart re-search
set(freechart_FOUND 0 CACHE INTERNAL "" FORCE)
USE_FREECHART()

add_subdirectory(../escore ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEscore)
add_subdirectory(../esmath ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEsmath)
add_subdirectory(../escomm ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEscomm)
add_subdirectory(../esshell-base ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEsshell-base)
add_subdirectory(../eschart ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEschart)
add_subdirectory(../esmcuprog ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEsmcuprog)

# export common static-linkage targets for aux projects
export(TARGETS ${escoreLib_s} ${escommLib_s} ${eschartLib_s} ${esshell_baseLib_s}						
       FILE ../esshell-framework-static.cmake
	   )
	   
# purge wxWidgets cache again
set(wxWidgets_DEFINITIONS "" CACHE INTERNAL "" FORCE)
set(wxWidgets_FOUND 0 CACHE INTERNAL "" FORCE)
set(wxWidgets_LIB_DIR "NOTFOUND" CACHE PATH "" FORCE)
# purge freechart search result again
set(freechart_FOUND 0 CACHE INTERNAL "" FORCE)

# restore shared libs option					
set(BUILD_SHARED_LIBS ${oldBUILD_SHARED_LIBS} CACHE BOOL "" FORCE)	   