###########################################################################
## $Id$
## Authors: griffin.milsap@gmail.com
## Description: Contains a macro for creating an application module

MACRO( BCI2000_ADD_APPLICATION_MODULE NAME SOURCES HEADERS APPSOURCES APPHEADERS INCLUDES )

# DEBUG
MESSAGE( "-- Adding Application Project: " ${NAME} )

# Generate the required framework
INCLUDE( ${BCI2000_CMAKE_DIR}/frameworks/DefaultAppFramework.cmake )

# Setup Global Application Modules
INCLUDE( ${BCI2000_CMAKE_DIR}/ApplicationModules.cmake )

# Add the global and specific application files into the framework for this project
SET( SRC_BCI2000_FRAMEWORK "${SRC_BCI2000_FRAMEWORK}" "${APPSOURCES}" "${BCI2000_APPSOURCES}" )
SET( HDR_BCI2000_FRAMEWORK "${HDR_BCI2000_FRAMEWORK}" "${APPHEADERS}" "${BCI2000_APPHEADERS}" )

# Set the Project Source Groups
SOURCE_GROUP( Source\\Project FILES ${SOURCES} )
SOURCE_GROUP( Headers\\Project FILES ${HEADERS} )

# Add in external required libraries
BCI2000_SETUP_EXTLIB_DEPENDENCIES( SRC_BCI2000_FRAMEWORK HDR_BCI2000_FRAMEWORK LIBS )

# If we're building a Qt project, we need to automoc the sources, generating new files
IF( NOT BORLAND )
QT4_AUTOMOC( ${SOURCES} )

# Moc Framework Sources differently, as Automoc doesn't like relative paths.
QT_WRAP_CPP( ${NAME} GENERATED ${HDR_BCI2000_MOC} )
SOURCE_GROUP( Generated FILES ${GENERATED} )
SET( SRC_BCI2000_FRAMEWORK
  ${SRC_BCI2000_FRAMEWORK}
  ${GENERATED}
)

# Include Qt Modules specified elsewhere
INCLUDE ( ${QT_USE_FILE} )
ENDIF( NOT BORLAND )

# Set Application Include Directories
INCLUDE_DIRECTORIES( ${INCLUDES} )

# Add Pre-processor defines
ADD_DEFINITIONS( 
  -DMODTYPE=3
  -DNO_PCHINCLUDES
)
IF( WIN32 )
ADD_DEFINITIONS(
  -D_WINDOWS
)
ENDIF( WIN32 )

# Add the executable to the project
ADD_EXECUTABLE( ${NAME} WIN32 ${SRC_BCI2000_FRAMEWORK} ${HDR_BCI2000_FRAMEWORK} ${SOURCES} ${HEADERS} )

# Set the output directories
SET_TARGET_PROPERTIES( ${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BCI2000_ROOT_DIR}/prog )
SET_TARGET_PROPERTIES( ${NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${BCI2000_ROOT_DIR}/prog )
SET_TARGET_PROPERTIES( ${NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BCI2000_ROOT_DIR}/prog )
IF( MSVC OR XCODE )
  SET_TARGET_PROPERTIES( ${NAME} PROPERTIES 
    PREFIX "../"
    IMPORT_PREFIX "../" 
  )
ENDIF( MSVC OR XCODE )

# Link against the Qt/VCL Libraries
IF( BORLAND )
TARGET_LINK_LIBRARIES( ${NAME} vcl rtl ${VXL_VGUI_LIBRARIES} ${LIBS} cp32mt.lib )
ELSE( BORLAND )
TARGET_LINK_LIBRARIES( ${NAME} ${QT_LIBRARIES} ${LIBS} )
ENDIF( BORLAND )

ENDMACRO( BCI2000_ADD_APPLICATION_MODULE NAME SOURCES HEADERS APPSOURCES APPHEADERS INCLUDES )