mkdir %APPVEYOR_BUILD_FOLDER%-build
cd %APPVEYOR_BUILD_FOLDER%-build
qmake -o %APPVEYOR_BUILD_FOLDER%-build -r -Wall -spec win32-msvc CONFIG+=%CONFIGURATION% "VERSION_MAJOR=%VERSION_MAJOR%" "VERSION_MINOR=%VERSION_MINOR%" "VERSION_BUILD=%VERSION_BUILD%" %APPVEYOR_BUILD_FOLDER%/Gui && jom qmake_all
jom
jom install