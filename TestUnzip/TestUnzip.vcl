<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: TestUnzip - Win32 (WCE ARMV4I) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "G:\Temp\RSP150.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=500 /D _WIN32_WCE=500 /D "WCE_PLATFORM_CK60Prem" /D "UNICODE" /D "_UNICODE" /FR"ARMV4IDbg/" /Fo"ARMV4IDbg/" /Fd"ARMV4IDbg/" /QRarch4T /QRinterwork-return /MC /c 
"D:\C-Source\Active\SetupDll_zip2\TestUnzip\TestUnzip.cpp"
]
Creating command line "clarm.exe @G:\Temp\RSP150.tmp" 
Creating temporary file "G:\Temp\RSP151.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"ARMV4IDbg/TestUnzip.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4IDbg/TestUnzip.exe" /subsystem:windowsce,5.00 /MACHINE:THUMB 
".\ARMV4IDbg\iniutil.obj"
".\ARMV4IDbg\nclog.obj"
".\ARMV4IDbg\StdAfx.obj"
".\ARMV4IDbg\TestUnzip.obj"
".\ARMV4IDbg\unzip.obj"
".\ARMV4IDbg\TestUnzip.res"
]
Creating command line "link.exe @G:\Temp\RSP151.tmp"
<h3>Output Window</h3>
Compiling...
TestUnzip.cpp
c:\programme\windows ce tools\wce500\windows mobile 5.0 pocket pc sdk\include\armv4i\windbase.h(662) : warning C4068: unknown pragma
Linking...
unzip.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x0000560C
corelibc.lib(pegwmain.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
corelibc.lib(onexit.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
corelibc.lib(crt0dat.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
corelibc.lib(crt0init.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
Creating command line "bscmake.exe /nologo /o"ARMV4IDbg/TestUnzip.bsc"  ".\ARMV4IDbg\StdAfx.sbr" ".\ARMV4IDbg\iniutil.sbr" ".\ARMV4IDbg\nclog.sbr" ".\ARMV4IDbg\TestUnzip.sbr" ".\ARMV4IDbg\unzip.sbr""
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
TestUnzip.exe - 0 error(s), 6 warning(s)
</pre>
</body>
</html>
