SetupDll_Zip

Create windows ce cab files which install a complete directory structure onto a device. I created 
this small tool, as I needed an easy to use tool to create a pseudo installation cab.

The DLL will unzip a archiv file with all dirs onto a device. To build a new cab use the directory
structure you will find below cabdir.

First cleanup all files below cabdir\source. Then place all files into this directory as they should be 
unpacked onto the device. For example for an application called barcodegame, I created the following 
structure below source:

<pre>source
    +---Program Files
    ¦   +---barcodegame
    ¦       +---de
    ¦       +---en
    +---Temp
    +---windows
        +---Start Menu
            +---Programs
</pre>
Then let cabbuild.bat create a new archive with all files and then a cab with this packed file. For example
change to the cabdir (cd cabdir) and then launch 

<pre>Buildcab.bat "BarcodeGame3"</pre>

The batch file will patch an inf file needed for cabwiz and zip all files below source. The batch will also
patch the file SetupDll_zip2.ini.tmpl which is used to specify the install location and the zip filename:

<pre>
;SetupDll_zip2.ini
;directory values have to end with a \

[unzip]
basedir=\
reboot=0
zipfile=!APPNAME!.zip
</pre>

The cab builder template files are hardcoded to 
a) unpack the zip from cab file to \Temp on the device. 
b) the unzip function uses the hardcoded directory \Temp\ to look for the zip file

The unzip directory could also be controlled by the registry but the ini file is looked up first.

If you like to control the unzip by the registry, you have to change the template file and use these
settings:

<pre>
"HKLM\Software\SetupDll_ZIP2"
    registry entries (all REG_SZ):
    unzipbasedir: "\"
    zipfilename: "install.zip"
    doreboot:"0"|"1"
</pre>

Currently the batch and everything is adjusted to work with the ini file.

Dependencies

The ini and inf template files are patched using the GNU tool sed.
The zip file is created with 7za
The cab is created with the MS SDK tool cabwiz.

The setupdll is written with the help of iniutil.cpp/h and unzip.cpp/h
