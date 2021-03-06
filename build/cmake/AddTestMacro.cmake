###########################################################################
## $Id$
## Author: juergen.mellinger@uni-tuebingen.de
## Description: Macro to add tests

IF( NOT BUILD_TESTS )
  ADD_DEFINITIONS( "-DDISABLE_BCITEST=1" )
ENDIF()
SET( BCITESTS "" )

MACRO( BCI2000_ADD_LIBRARY_TEST libname_ )

  SET( name_ "bcitest_BCI2000Framework${libname_}" )
  UTILS_INCLUDE( frameworks/${libname_} )
  BCI2000_ADD_TEST_EXECUTABLE( ${name_} )

ENDMACRO()


MACRO( BCI2000_ADD_UNIT_TEST )

  GET_FILENAME_COMPONENT( name_ ${ARGV0} NAME_WE )
  SET( name_ "bcitest_${name_}" )
  UTILS_INCLUDE( frameworks/Core )
  BCI2000_ADD_TEST_EXECUTABLE( ${name_} ${ARGV} )

ENDMACRO()


MACRO( BCI2000_ADD_BCITEST name_ )

  IF( BUILD_TESTS )
    IF( CONFIG_VERBOSE )
      MESSAGE( STATUS "Adding test: " ${name_} )
    ENDIF()
    #SET_PROPERTY( TARGET ${name_} APPEND PROPERTY COMPILE_DEFINITIONS "main=main_bcitest_" )
    GET_PROPERTY( DIR TARGET ${name_} PROPERTY RUNTIME_OUTPUT_DIRECTORY )
    ADD_TEST( ${name_} "${DIR}/${name_}" --bcitest )
    ADD_TEST( "${name_} self" "${DIR}/${name_}" --bcitest=self )
    SET_PROPERTY( TEST "${name_} self" PROPERTY WILL_FAIL TRUE )
    SET( BCITESTS ${BCITESTS} ${name_} )
  ENDIF()

ENDMACRO()


MACRO( BCI2000_ADD_TEST_EXECUTABLE name_ )

  IF( BUILD_TESTS )
    SET( sources_
      ${ARGN}
      ${PROJECT_SRC_DIR}/shared/bcistream/BCITestMain.cpp
      ${PROJECT_SRC_DIR}/shared/bcistream/BCIStream_tool.cpp
    )
    SET_OUTPUT_DIRECTORY( "${BCI2000_ROOT_DIR}/build/buildutils/tests" )
    #BCI2000_ADD_REGISTRY( ${name_}_Registry sources_ DEPTARGETS )
    BCI2000_ADD_TARGET( EXECUTABLE ${name_} ${sources_} )

    IF( NOT FAILED )
      SET_PROPERTY( TARGET ${name_} PROPERTY FOLDER Tests/bcitest )
      SET_PROPERTY( TARGET ${name_} APPEND PROPERTY COMPILE_FLAGS "-DUSE_QT" )
      TARGET_LINK_LIBRARIES( ${name_} ${QT_LIBRARIES} )
      BCI2000_ADD_BCITEST( ${name_} )
    ENDIF( NOT FAILED )

  ENDIF( BUILD_TESTS )

ENDMACRO()
