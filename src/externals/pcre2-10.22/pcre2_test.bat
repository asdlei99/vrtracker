@REM This is a generated file.
@echo off
setlocal
SET srcdir="C:\projects\vrtracker\src\externals\pcre2-10.22"
SET pcre2test="C:\projects\vrtracker\src\externals\pcre2-10.22\DEBUG\pcre2test.exe"
if not [%CMAKE_CONFIG_TYPE%]==[] SET pcre2test="C:\projects\vrtracker\src\externals\pcre2-10.22\%CMAKE_CONFIG_TYPE%\pcre2test.exe"
call %srcdir%\RunTest.Bat
if errorlevel 1 exit /b 1
echo RunTest.bat tests successfully completed
