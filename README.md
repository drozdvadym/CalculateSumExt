This extension for windows shell, that provides accumulating information about files and saving it to log file

Tested under Windows 7 (x64/x86) and Windows 8.1 x64

INSTALL:

1) Run cmd.exe from administrator
2) Due to your platform, change current directory to build directory (if you have x64 go to CalculateSumExt\build\x64VSv120\Release)
3) run command regsvr32 CalculateSumExt.dll

USING:

click  right mouse button on file (or file groups) and click menu item "Calculate Information"
after some time information will be saved into curent directory (directory of selected files) to file  "file_inf.log" in alphabetical order

UNINSTALL:

1) Run cmd.exe from administrator
2) Due to your platform, change current directory to build directory (if you have x64 go to CalculateSumExt\build\x64VSv120\Release)
3) run command regsvr32 /u CalculateSumExt.dll

EXECUTABLE:

There are test utility (testSample.exe) that can Calculate information about all files in directory that passed to it as argument. and save this information to file "file_inf.log" 

ALSO:

You can build src (there is VS2013 solution), modify them and using it as you want and whenever you want. 

DEPENDENCIES:

This software depend on boost (boost filesystem), OpenSSL (calcMD5 function) and WIndows API.

Software has some C++11 tricks


|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|---	|---	|---	|---	|---	|---	|---	|---	|---	|---	|---	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|   	|
