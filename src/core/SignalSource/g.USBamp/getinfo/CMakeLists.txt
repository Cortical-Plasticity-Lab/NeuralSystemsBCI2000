###########################################################################
## $Id$
## Author: juergen.mellinger@uni-tuebingen.de
## Description: Build information for USBampgetinfo

SET( NAME gUSBampgetinfo )

UTILS_INCLUDE( frameworks/Core )
# Tell the macro to link against the gUSBamp API found in EXTLIB
BCI2000_INCLUDE( "GUSBAMP" )
BCI2000_ADD_TARGET( INFO Tool EXECUTABLE ${NAME} gUSBampgetinfo.cpp )
IF( NOT FAILED )
  BCI2000_ADD_TO_INVENTORY( Tool ${NAME} )
ENDIF( NOT FAILED )
