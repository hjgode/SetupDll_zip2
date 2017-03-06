@echo off
if ""%1""=="""" goto usage

rem not needed any more as we will patch another file
REM if not exist %1.inf goto infmiss

rem goto exit

echo.
echo.
echo *************************************************************************
echo *                      Building %1.cab                             *
echo *               Pocket PC and Windows Mobile Builds                     *
echo *************************************************************************

rem echo ...copying actual DEBUG runtime
rem *************************************************************************
rem if EXIST ..\ARMV4IDbg\SetupDll_zip2.dll copy ..\ARMV4IDbg\SetupDll_zip2.dll .\runtime


rem *************************************************************************
echo ...removing directory tempinstall
rem *************************************************************************
rmdir /q /s tempinstall


rem *************************************************************************
echo ...create directory structure for zipping
rem *************************************************************************
md tempinstall

rem *************************************************************************
echo ...copy files into directory structure for zipping
rem *************************************************************************
xcopy /R /E source\*.*            tempinstall\*.*
 
Rem *************************************************************************
echo ...remove the attributes from the files 
Rem *************************************************************************

attrib -r /s tempinstall\*.*

Rem *************************************************************************
echo ...zip the files
Rem *************************************************************************
cd tempinstall
..\7za a -r -tzip %1.zip *.*
cd ..

if errorlevel 1 goto Failed1
if not errorlevel 1 goto Continue1

:Failed1
echo.
echo.
echo.
echo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo! Zip app failed to build the zip file %1.zip
echo! exiting build.......                                                  !
echo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo.
echo.
echo.
goto exit

:continue1
Rem *************************************************************************
echo ...patch the inf file
Rem *************************************************************************
sed "s/!APPNAME!/%1/ig" cabtemplate.inf >%1.inf

Rem *************************************************************************
echo ...patch the .ini file
Rem *************************************************************************
sed "s/!APPNAME!/%1/ig" SetupDll_zip2.ini.tmpl >tempinstall\SetupDll_zip2.ini

Rem *************************************************************************
echo ...build the cab file
Rem *************************************************************************

if exist %1.cab del %1.cab
if exist %1.dat del %1.dat

rem "C:\Programme\Microsoft eMbedded Tools\EVB\cabwiz.exe" IBrowse.inf /err error.out
cabwiz.exe %1.inf /err error.out
if exist error.out type error.out
if not exist error.out echo Build Complete - No errors or warnings in %1.inf
echo Press any key to copy the file to device and launch the installer
pause
echo ...copying %1.cab to device
echo on
pput %1.cab %1.cab
echo off
echo ...launching install of %1.cab on device
echo on
prun \Windows\wceload.exe \%1.cab
echo off

goto exit
:usage
echo *************************************************************************
echo            build a zip and package all into one cab file
echo start with the application name as argument
echo for example: 'buildcab Installer'
echo.
echo need all source files in source\....
echo needs the inf and ini template file in current dir
echo *************************************************************************
goto exit

:infmiss
echo *************************************************************************
echo  missing the inf file for %1
echo *************************************************************************
goto usage

:exit
