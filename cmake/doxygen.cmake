#-------------------------------------------------------------------------------
#	DOXYGEN
#-------------------------------------------------------------------------------

find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
	message(WARNING "Doxygen not found, unable to generate documentation")
else ()
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	# Generate Doxygen
	configure_file(Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(${PROJECT_NAME}_documentation
	                  COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/Doxyfile"
	                  WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
	                  COMMENT "Generating documentation..."
	                  )
	# Generate a .pdf file from the latex
	find_program(PDF_PROGRAM pdflatex)
	if (PDF_PROGRAM)
		set(TEX_FILE ${CMAKE_BINARY_DIR}/doc/latex/refman.tex)
		add_custom_command(TARGET ${PROJECT_NAME}_documentation
		                   POST_BUILD
		                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc/latex/
		                   DEPENDS ${TEX_FILE}
		                   COMMAND ${PDF_PROGRAM} -quiet -job-name=${PROJECT_NAME}-${PROJECT_VERSION} ${TEX_FILE}
		                   COMMENT "Generating documentation PDF..."
		                   )
	else ()
		message(WARNING "pdfLatex not found, cannot generate PDF file.")
	endif (PDF_PROGRAM)
endif ()