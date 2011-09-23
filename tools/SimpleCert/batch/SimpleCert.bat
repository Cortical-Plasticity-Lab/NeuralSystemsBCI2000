:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: $Id$
:: Description: Run BCI2000, then perform a simple certification procedure
::    Command Line: SimpleCert [Executable] [Amp Prm Fragment]
::    Paths are relative to bci2000/prog
::
:: $BEGIN_BCI2000_LICENSE$
:: 
:: This file is part of BCI2000, a platform for real-time bio-signal research.
:: [ Copyright (C) 2000-2011: BCI2000 team and many external contributors ]
:: 
:: BCI2000 is free software: you can redistribute it and/or modify it under the
:: terms of the GNU General Public License as published by the Free Software
:: Foundation, either version 3 of the License, or (at your option) any later
:: version.
:: 
:: BCI2000 is distributed in the hope that it will be useful, but
::                         WITHOUT ANY WARRANTY
:: - without even the implied warranty of MERCHANTABILITY or FITNESS FOR
:: A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
:: 
:: You should have received a copy of the GNU General Public License along with
:: this program.  If not, see <http://www.gnu.org/licenses/>.
:: 
:: $END_BCI2000_LICENSE$
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
@echo off
set SIMPLECERT=%CD%
call batch/DetectWinVer.bat
call batch/Setup.bat
set I=0
set DATADIR=%SIMPLECERT%\data\%~n1\%OSNAME%\
cls
echo Certifying %1 on %OSNAME% with %ITERATIONS% iteration(s).
echo Saving data to %DATADIR%
cd ..\..\prog

:start
set /a TESTNUM=%I%+1
echo Starting Iteration %TESTNUM%/%ITERATIONS%
start %1 127.0.0.1
start P3SignalProcessing.exe 127.0.0.1
start StimulusPresentation.exe 127.0.0.1
start /wait operator.exe ^
   --Title %~n0 ^
   --OnConnect "-LOAD PARAMETERFILE %2; LOAD PARAMETERFILE ..\tools\SimpleCert\parms\SimpleCert.prm; SET PARAMETER DataDirectory %DATADIR%; SET PARAMETER WindowWidth %WINWIDTH%; SET PARAMETER WindowHeight %WINHEIGHT%; SET PARAMETER WindowLeft %WINX%; SET PARAMETER WindowTop %WINY%; SETCONFIG" ^
   --OnSetConfig "-START" ^
   --OnSuspend "-QUIT"
echo Waiting 5 Seconds...
ping 2.2.2.2 -n 1 -w 5000 >NUL
set /a I=%I%+1
if %I% LSS %ITERATIONS% goto start
echo Test Ended.  

cd %SIMPLECERT%
echo Running Analysis...
python analysis.py %DATADIR%
echo Saved timing figures in %DATADIR%
pause


