###########################################################################
## $Id$
## Authors: griffin.milsap@gmail.com, juergen.mellinger@uni-tuebingen.de
## Description: Sets up include directories and dependencies for 
##   Application Modules using the AppModule library

UTILS_INCLUDE( frameworks/Core )

INCLUDE_DIRECTORIES(
  ${PROJECT_SRC_DIR}/shared
  ${PROJECT_SRC_DIR}/shared/accessors
  ${PROJECT_SRC_DIR}/shared/bcistream
  ${PROJECT_SRC_DIR}/shared/config
  ${PROJECT_SRC_DIR}/shared/modules
  ${PROJECT_SRC_DIR}/shared/types
  ${PROJECT_SRC_DIR}/shared/utils
  ${PROJECT_SRC_DIR}/shared/utils/Expression
  ${PROJECT_SRC_DIR}/shared/fileio
  ${PROJECT_SRC_DIR}/shared/gui
  ${PROJECT_SRC_DIR}/shared/modules/application
  ${PROJECT_SRC_DIR}/shared/modules/application/utils
  ${PROJECT_SRC_DIR}/shared/modules/application/audio
  ${PROJECT_SRC_DIR}/shared/modules/application/gui
  ${PROJECT_SRC_DIR}/shared/modules/application/human_interface_devices
  ${PROJECT_SRC_DIR}/shared/modules/application/speller
  ${PROJECT_SRC_DIR}/shared/modules/application/stimuli
  ${BCI2000_APPINCLUDEDIRS_EXT} 
)

IF( WIN32 )
  BCI2000_USE( "SAPI" )
  BCI2000_USE( "DSOUND" )
ENDIF( WIN32 )
BCI2000_USE( "3DAPI" )
BCI2000_USE( "Qt" )

SET( REGISTRY_NAME AppRegistry )
FORCE_INCLUDE_OBJECT( ${REGISTRY_NAME} )
SET( LIBS ${LIBS} BCI2000FrameworkAppModule )
ADD_DEFINITIONS( -DIS_LAST_MODULE ) 
