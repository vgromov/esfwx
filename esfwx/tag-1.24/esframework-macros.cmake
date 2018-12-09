
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
  else()
    # Just append source to SourcesVar, if do not know how to precompile
    list(APPEND ${SourcesVar} ${PrecompiledSource})
  endif()
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
	if( NOT ES_BUILD_SHARED_LIBS )
		message(FATAL_ERROR
      "${${PluginTargetVar}} must be build with ES_BUILD_SHARED_LIBS flag set ON"
    )
	endif()
	set_target_properties(${${PluginTargetVar}} PROPERTIES
						COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}"
						ENABLE_EXPORTS OFF
						FOLDER plugins
						)
	# tweak output directory for plugins
	# NB! MODULEs, which are plugins, are treated as libraries
	# apart from DLLs (SHARED)	which are treated as runtime
	foreach(t ${CMAKE_CONFIGURATION_TYPES})
		set(outputDir ${binaryRoot}/${t}/plugins)
		string(TOUPPER ${t} t)
		set_target_properties(${${PluginTargetVar}} PROPERTIES
							LIBRARY_OUTPUT_DIRECTORY_${t} ${outputDir}
							)
	endforeach()
ENDMACRO(PLUGIN_TARGET_FINALIZE)

# localization integration macros
MACRO(I18N_ADD i18nComponent i18nFiles i18nBinaryRoot)
	# copy from cache
	set(tmpLanguages ${ESI18N_LANGUAGES})
	set(tmpComponents ${ESI18N_DOMAINS})
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
		if("${f}" MATCHES "${ESI18N_PO_PATTERN}")
			string(REGEX REPLACE
							"${ESI18N_PO_PATTERN}" "\\1"
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
	set(ESI18N_LANGUAGES ${tmpLanguages} CACHE STRING "" FORCE)
	set(ESI18N_DOMAINS ${tmpComponents} CACHE STRING "" FORCE)
ENDMACRO(I18N_ADD)

MACRO(I18N_TRACE)
	message(
    STATUS
    "I18N languages: ${ESI18N_LANGUAGES}"
  )
	message(
    STATUS
    "I18N components: ${ESI18N_DOMAINS}"
  )
	foreach(comp ${ESI18N_DOMAINS})
		message(
      STATUS 
      "I18N ${comp}_BinRoot: ${${comp}_BinRoot}"
    )
		foreach(lang ${ESI18N_LANGUAGES})
			message(
        STATUS
        "I18N ${comp}${lang} files: ${${comp}${lang}}"
      )
		endforeach()
	endforeach()
ENDMACRO(I18N_TRACE)

MACRO(I18N_RESET)
	foreach(comp ${ESI18N_DOMAINS})
		unset("${comp}_BinRoot" CACHE)
		foreach(lang ${ESI18N_LANGUAGES})
			unset("${comp}${lang}" CACHE)
		endforeach()
	endforeach()
	set(ESI18N_LANGUAGES "" CACHE INTERNAL "" FORCE)
	set(ESI18N_DOMAINS "" CACHE INTERNAL "" FORCE)
ENDMACRO(I18N_RESET)

MACRO(LIST_TO_STRING outVar listVar)
	foreach(s ${${listVar}})
		set(${outVar} "${s}|${${outVar}}")
	endforeach()
ENDMACRO(LIST_TO_STRING)

MACRO(ADD_TO_SETUP_DEPENDENCIES targetName)
	set(setupDependencies ${setupDependencies} ${targetName} CACHE STRING "" FORCE)
ENDMACRO(ADD_TO_SETUP_DEPENDENCIES)

# help and manual compiler
MACRO(USE_HELP_AND_MANUAL)
	if( NOT helpAndManualCompiler )
		find_program(helpAndManualCompiler
							helpman.exe
							PATHS "c:/program files" "c:/program files (x86)" ENV PATH
							PATH_SUFFIXES "EC Software/HelpAndManual5" "EC Software/HelpAndManual6"
			)
	endif()
	if( NOT helpAndManualCompiler )
		message(FATAL_ERROR
      "Help and manual compiler is not found"
    )
	endif()
ENDMACRO(USE_HELP_AND_MANUAL)

#plugin help target macro
MACRO(PLUGIN_HELP_TARGET_ADD helpFileSrc pluginModuleName)
get_filename_component(helpFileSrc ${helpFileSrc} REALPATH)
get_filename_component(helpFileBaseName ${helpFileSrc} NAME_WE)
set(helpFileOut help/${helpFileBaseName}.chm)
add_custom_command(OUTPUT ${helpFileOut}
						COMMAND ${helpAndManualCompiler} "${helpFileSrc}" "/CHM"
					)
add_custom_target(${pluginModuleName}_help ALL
						DEPENDS ${helpFileOut}
					)
set_target_properties(${pluginModuleName}_help PROPERTIES
					FOLDER plugins
					)
ENDMACRO(PLUGIN_HELP_TARGET_ADD)

# scriptlet encryption target macro
MACRO(SCRIPTLET_ENCRYPTION_TARGET_ADD pluginModuleName pluginNamesAndKeys)
foreach(pk ${pluginNamesAndKeys})
	string(REGEX MATCH "^[^\\|]+" pluginName ${pk})
	string(REGEX MATCH "[^\\|]+$" encryptionKey ${pk})
	# message("pluginName=${pluginName} encryptionKey=${encryptionKey}")
	set(scriptletSrcBaseName ${pluginModuleName}.plug.${pluginName})
	set(scriptletSrc essl/${scriptletSrcBaseName}.essl)
	get_filename_component(scriptletSrc ${scriptletSrc} REALPATH)
	set(scriptletOut ${binaryRoot}/plugins/${scriptletSrcBaseName}.essle)
	add_custom_command(OUTPUT ${scriptletOut}
						COMMAND ${fcodecExe}
							ARGS /t /k "${encryptionKey}" /i "${scriptletSrc}" /o "${scriptletOut}"
					)
	add_custom_target(${scriptletSrcBaseName}_essle ALL
						DEPENDS ${scriptletOut}
					)
	set_target_properties(${scriptletSrcBaseName}_essle PROPERTIES
					FOLDER plugins
					)
endforeach()
ENDMACRO(SCRIPTLET_ENCRYPTION_TARGET_ADD)
