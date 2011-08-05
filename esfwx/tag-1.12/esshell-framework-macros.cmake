# toolset suffixes
set(compilerVersion "")
set(binarySuffix "")
if( MSVC )
	set(binarySuffix vc)
  if(1200 EQUAL ${MSVC_VERSION})
		set(compilerVersion "6")
	elseif(1300 EQUAL ${MSVC_VERSION})
		set(compilerVersion "7")
  elseif(1310 EQUAL ${MSVC_VERSION})
		set(compilerVersion "7_1")
	elseif(1400 EQUAL ${MSVC_VERSION})
		set(compilerVersion "8")
	elseif(1500 EQUAL ${MSVC_VERSION})
		set(compilerVersion "9")
  elseif(1600 EQUAL ${MSVC_VERSION})
		set(compilerVersion "10")
	endif()	
elseif( BORLAND )
	set(binarySuffix bcc)
endif()

# freechart library stuff
# freechart usage macro
MACRO(USE_FREECHART)
	if( NOT freechart_FOUND )
		set(freechart_Root c:/freechart CACHE PATH "" FORCE)
		set(freechart_LibDir "" CACHE PATH "" FORCE)
		find_path(freechart_Srcs
						NAMES axis bars category gantt ohlc pie xy xyz
						PATHS ${freechart_Root} ENV PATH
						PATH_SUFFIXES src
						)
		if( freechart_Srcs )
			get_filename_component(freechart_Root ${freechart_Srcs}/../ REALPATH)
			set(freechart_Root ${freechart_Root} CACHE PATH "" FORCE)
		else()
			message(FATAL_ERROR "wxFreechart installation path could not be found")
		endif()
		find_path(freechart_Include wx
					PATHS ${freechart_Root}
					PATH_SUFFIXES include
					)		
		if( NOT freechart_Include )
			message(FATAL_ERROR "wxFreechart include path could not be found")
		endif()
		if( ${BUILD_SHARED_LIBS} )
			find_path(freechart_LibRoot ${binarySuffix}_dll
						PATHS ${freechart_Root}
						PATH_SUFFIXES lib
						)
			if( freechart_LibRoot )
				set(freechart_LibDir ${freechart_LibRoot}/${binarySuffix}_dll CACHE PATH "" FORCE)
			endif()	
		else()
			find_path(freechart_LibRoot ${binarySuffix}_lib
						PATHS ${freechart_Root}
						PATH_SUFFIXES lib
						)
			if( freechart_LibRoot )
				set(freechart_LibDir ${freechart_LibRoot}/${binarySuffix}_lib CACHE PATH "" FORCE)
			endif()
		endif()
		if( NOT freechart_LibDir )
			message(FATAL_ERROR "wxFreechart library path could not be found")
		endif()
		find_library(freechart_DebugLibs
							wxcode_msw29ud_freechart.lib
							PATHS ${freechart_LibDir}
							NO_DEFAULT_PATH
							)
		find_library(freechart_OptimizedLibs
							wxcode_msw29u_freechart.lib
							PATHS ${freechart_LibDir}
							NO_DEFAULT_PATH
							)
		if( NOT freechart_OptimizedLibs OR
				NOT freechart_DebugLibs )
			message(FATAL_ERROR "wxFreechart libraries could not be found")
		else()
			set(freechart_Libs debug ${freechart_DebugLibs} optimized ${freechart_OptimizedLibs} CACHE STRING "" FORCE)
		endif()
		set(freechart_FOUND TRUE CACHE BOOL "" FORCE)
		message(STATUS "USE_FREECHART freechart is found: ${freechart_FOUND}")
	endif()
ENDMACRO(USE_FREECHART)

# header extensions
set(headerExtensions .h .hpp .hxx .ipp .xpm .cxx .hmxz .pot .ru_RU.po .en_EN.po)

# precompiled header macro
MACRO(SPECIFY_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  if(MSVC)
    get_filename_component(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    set(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
    set(Sources ${${SourcesVar}})
    set_source_files_properties(${PrecompiledSource} PROPERTIES 
						COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
            OBJECT_OUTPUTS "${PrecompiledBinary}")
    set_source_files_properties(${Sources}
            PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
            OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    list(APPEND ${SourcesVar} ${PrecompiledSource})
  endif(MSVC)
ENDMACRO(SPECIFY_PRECOMPILED_HEADER)

# header specification macro
MACRO(SPECIFY_HEADER_FILES SourceFiles HeaderExtensions)
	foreach(f ${${SourceFiles}})
		get_filename_component(fExt ${f} EXT)
		list(FIND ${HeaderExtensions} "${fExt}" foundResult)
		if( ${foundResult} GREATER -1 )
			set(headersFound ${headersFound} ${f})
		endif()
	endforeach()
	set_source_files_properties(${headersFound} PROPERTIES
						HEADER_FILE_ONLY ON
						)
ENDMACRO(SPECIFY_HEADER_FILES)

# plugin targets finalization macro
MACRO(PLUGIN_TARGET_FINALIZE PluginTargetVar)
	# export preprocessor defines for dll
	if( NOT BUILD_SHARED_LIBS )
		message(FATAL_ERROR "${${PluginTargetVar}} must be build with BUILD_SHARED_LIBS flag set ON")
	endif()
	set_target_properties(${${PluginTargetVar}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						ENABLE_EXPORTS OFF
						FOLDER plugins
						)
	# tweak output directory for plugins
	# NB! MODULEs, which are plugins, are treated as libraries
	# apart from DLLs (SHARED)	which are treated as runtime
	foreach(t ${CMAKE_CONFIGURATION_TYPES})
		set(outputDir ${PROJECT_BINARY_DIR}/${t}/plugins)
		string(TOUPPER ${t} t)
		set_target_properties(${${PluginTargetVar}} PROPERTIES 
							LIBRARY_OUTPUT_DIRECTORY_${t} ${outputDir}
							)
	endforeach()
ENDMACRO(PLUGIN_TARGET_FINALIZE)

# localization integration macros
MACRO(I18N_ADD i18nComponent i18nFiles i18nBinaryRoot)
	# copy from cache
	set(tmpLanguages ${i18nLanguages})
	set(tmpComponents ${i18nComponents})
	# find if we already registered component 
	list(FIND tmpComponents "${i18nComponent}" tmp)
	if( -1 EQUAL ${tmp} )
		list(APPEND tmpComponents "${i18nComponent}")
		# create component variables for bin dir and language files
		get_filename_component(binRoot "${i18nBinaryRoot}" REALPATH)
		set("${i18nComponent}_BinRoot" "${binRoot}" CACHE INTERNAL "" FORCE)
	endif()
	# analyse incoming i18n files list
	foreach(f ${${i18nFiles}})
		set(lang "")
		if("${f}" MATCHES "${poPattern}")
			string(REGEX REPLACE 
							"${poPattern}" "\\1" 
							lang "${f}")
			if( lang )
				list(FIND tmpLanguages ${lang} tmp)
				if( -1 EQUAL tmp )
					list(APPEND tmpLanguages "${lang}")
				endif()
				list(FIND "${i18nComponent}${lang}" "${PROJECT_SOURCE_DIR}/${f}" tmp)
				if( -1 EQUAL tmp )
					set(tmp "${${i18nComponent}${lang}}" "${PROJECT_SOURCE_DIR}/${f}") 
					set("${i18nComponent}${lang}" ${tmp} CACHE INTERNAL "" FORCE)
				endif()
			endif()
		endif()
	endforeach()
	# update cache
	set(i18nLanguages ${tmpLanguages} CACHE INTERNAL "" FORCE)
	set(i18nComponents ${tmpComponents} CACHE INTERNAL "" FORCE)
ENDMACRO(I18N_ADD)

MACRO(I18N_TRACE)
	message("languages: ${i18nLanguages}")
	message("components: ${i18nComponents}")
	foreach(comp ${i18nComponents})
		message("${comp}_BinRoot: ${${comp}_BinRoot}")
		foreach(lang ${i18nLanguages})
			message("${comp}${lang} files: ${${comp}${lang}}")
		endforeach()
	endforeach()
ENDMACRO(I18N_TRACE)

MACRO(I18N_RESET)
	foreach(comp ${i18nComponents})
		unset("${comp}_BinRoot" CACHE)
		foreach(lang ${i18nLanguages})
			unset("${comp}${lang}" CACHE)
		endforeach()
	endforeach()
	set(i18nLanguages "" CACHE INTERNAL "" FORCE)
	set(i18nComponents "" CACHE INTERNAL "" FORCE)
ENDMACRO(I18N_RESET)

MACRO(LIST_TO_STRING outVar listVar)
	foreach(s ${${listVar}})
		set(${outVar} "${s}|${${outVar}}")
	endforeach()
ENDMACRO(LIST_TO_STRING)

MACRO(ADD_TO_SETUP_DEPENDENCIES targetName)
	set(setupDependencies ${setupDependencies} ${targetName} CACHE INTERNAL "" FORCE)
ENDMACRO(ADD_TO_SETUP_DEPENDENCIES)

MACRO(USE_HELP_AND_MANUAL)
	# help and manual compiler
	if( NOT helpAndManualCompiler )
		find_program(helpAndManualCompiler
							helpman.exe
							PATHS "c:/program files" "c:/program files (x86)" ENV PATH
							PATH_SUFFIXES "EC Software/HelpAndManual5"
			)
	endif()
	if( NOT helpAndManualCompiler )
		message(FATAL_ERROR "Help and manual compiler is not found")
	endif()
ENDMACRO(USE_HELP_AND_MANUAL)
