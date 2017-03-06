Readme for SetupDll_zip2

What it does

You should be able to build a cab file with only one zip file in it

The dll will take the zip file and extract all files to the device

The default basedir for the unzip is the root dir of the device

All included paths and files will be extracted with there paths

Sample ini file:

%<---------------------------------------
;SetupDll_zip2.ini
;directory values have to end with a \

[unzip]
basedir=\
reboot=0
zipfile=install.zip
%<---------------------------------------

The basedir can also been set and read from registry:

Software\\SetupDll_ZIP2:unzipbasedir(REG_SZ)

If no ini is found, the registry is tried, if both fail the root is used