###########################################################################
## $Id$
## Author: juergen.mellinger@uni-tuebingen.de
## Description: Build information for PythonSource

# Set the executable name
SET( EXECUTABLE_NAME PythonSource )

# Add the eponymous main cpp file (if you omit this, the default main file will be used:
# that will compile OK, but will then cause intermittent problems with some versions of
# Python due to interrupted os.popen(some_system_command).read() calls:  a known issue
# with Python not playing nice with QT.  This main file ensures a non-QT executable.
SET( SRC_PROJECT
  ${SRC_PROJECT}
  PythonSource.cpp
)

# Create the signal processing module
BCI2000_ADD_SIGNAL_SOURCE_MODULE( 
  "${EXECUTABLE_NAME}" 
  "${SRC_PROJECT}" "${HDR_PROJECT}" 
)
