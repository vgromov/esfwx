# static framework components linkage pseudo-project
cmake_minimum_required(VERSION 3.11)
project(esfwx-static-linkage)

# reset certain cache values
# remember ES_BUILD_SHARED_LIBS setting to restore it later
set(oldBUILD_SHARED_LIBS ${ES_BUILD_SHARED_LIBS})
set(ES_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(../escore ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEscore)
add_subdirectory(../esmath ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEsmath)
add_subdirectory(../escomm ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEscomm)
add_subdirectory(../esscript ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEsscript)

# export common static-linkage targets for aux projects
export(TARGETS 
	${escoreLib_s} 
	${escommLib_s}	
  ${esmathLib_s} 
	${esscriptLib_s} 
  FILE ${PROJECT_BINARY_DIR}/esframework-static.cmake
	)
	   
# restore shared libs option					
set(ES_BUILD_SHARED_LIBS ${oldBUILD_SHARED_LIBS} CACHE BOOL "" FORCE)	   
