# make doc_doxygen optional if someone does not have / like doxygen


# Create CMake build option for the target.
option(BUILD_DOC "Enable building of doxygen documentation" ON)

# Add a custom target for building the documentation.

FIND_PACKAGE(Doxygen)

IF (BUILD_DOC)

    IF (DOXYGEN_FOUND)
        MESSAGE(STATUS "Located ${DOXYGEN_EXECUTABLE} with version ${DOXYGEN_VERSION}")
        IF (DOXYGEN_VERSION VERSION_LESS 1.9.1)
            MESSAGE(WARNING "Only tested with doxygen version 1.9, your version(${DOXYGEN_VERSION}) is older and may not work. Consider updating.")
        ENDIF ()
        set(DOXYGEN_IN ${CMAKE_HOME_DIRECTORY}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        # Set parameters for Doxygen-file
        set(DOXYGEN_INPUT "../src \
                ../README.md")
        set(DOXYGEN_EXCLUDE "../src/outputWriter/vtk-unstructured.cpp \
                ../src/outputWriter/vtk-unstructured.h \
                ../src/outputWriter/vtk-unstructured.xsd")
        set(DOXYGEN_MAIN_PAGE ../README.md)

        set(DOXYGEN_IMAGES ../images/*.png)

        # Write the parameters to the doxygen-file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message(STATUS "Writing of doxygen-file done")

        # Add doc_doxygen as a target
        add_custom_target(doc_doxygen
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                VERBATIM)
    ELSE ()
        MESSAGE(WARNING "Doxygen wasn't found make sure to install doxygen, additionally you may need to install graphiz")
    ENDIF ()
ENDIF ()
