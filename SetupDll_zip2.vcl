<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: SetupDll_zip2 - Win32 (WCE ARMV4I) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "G:\Temp\RSP231.tmp" with contents
[
/nologo /W3 /D _WIN32_WCE=500 /D "ARM" /D "_ARM_" /D "WCE_PLATFORM_CK60Prem" /D "ARMV4I" /D UNDER_CE=500 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_USRDLL" /D "SetupDll_zip2_EXPORTS" /FR"ARMV4IRel/" /Fp"ARMV4IRel/SetupDll_zip2.pch" /YX /Fo"ARMV4IRel/" /QRarch4T /QRinterwork-return /O2 /MC /c 
"D:\C-Source\Active\SetupDll_zip2\iniutil.cpp"
"D:\C-Source\Active\SetupDll_zip2\nclog.cpp"
"D:\C-Source\Active\SetupDll_zip2\SetupDll_zip2.cpp"
"D:\C-Source\Active\SetupDll_zip2\unzip.cpp"
]
Creating command line "clarm.exe @G:\Temp\RSP231.tmp" 
Creating temporary file "G:\Temp\RSP232.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:no /pdb:"ARMV4IRel/SetupDll_zip2.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /def:".\SetupDll_zip2.def" /out:"ARMV4IRel/SetupDll_zip2.dll" /implib:"ARMV4IRel/SetupDll_zip2.lib" /subsystem:windowsce,5.00 /MACHINE:THUMB 
".\ARMV4IRel\iniutil.obj"
".\ARMV4IRel\nclog.obj"
".\ARMV4IRel\SetupDll_zip2.obj"
".\ARMV4IRel\unzip.obj"
]
Creating command line "link.exe @G:\Temp\RSP232.tmp"
<h3>Output Window</h3>
Compiling...
iniutil.cpp
c:\programme\windows ce tools\wce500\windows mobile 5.0 pocket pc sdk\include\armv4i\windbase.h(662) : warning C4068: unknown pragma
nclog.cpp
c:\programme\windows ce tools\wce500\windows mobile 5.0 pocket pc sdk\include\armv4i\windbase.h(662) : warning C4068: unknown pragma
SetupDll_zip2.cpp
unzip.cpp
c:\programme\windows ce tools\wce500\windows mobile 5.0 pocket pc sdk\include\armv4i\windbase.h(662) : warning C4068: unknown pragma
Linking...
   Creating library ARMV4IRel/SetupDll_zip2.lib and object ARMV4IRel/SetupDll_zip2.exp
Creating command line "bscmake.exe /nologo /o"ARMV4IRel/SetupDll_zip2.bsc"  ".\ARMV4IRel\iniutil.sbr" ".\ARMV4IRel\nclog.sbr" ".\ARMV4IRel\SetupDll_zip2.sbr" ".\ARMV4IRel\unzip.sbr""
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
SetupDll_zip2.dll - 0 error(s), 3 warning(s)
</pre>
</body>
</html>
