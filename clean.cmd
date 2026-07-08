@echo off

setlocal
call :setESC

CALL :printHeader, "Removing folders"
call :removeFolder, "external"
call :removeFolder, "ColosseumLib\deps"
call :removeFolder, "ColosseumLib\temp"
call :removeFolder, "ColosseumLib\lib"
ECHO(   

CALL :printHeader, "Cleaning visual studio solutions: Configuration = Debug"
msbuild /p:Platform=x64 /p:Configuration=Debug Colosseum.sln /t:Clean
if ERRORLEVEL 1 goto :buildfailed
ECHO(   

CALL :printHeader, "Cleaning visual studio solutions: Configuration = Release"
msbuild /p:Platform=x64 /p:Configuration=Release Colosseum.sln /t:Clean
if ERRORLEVEL 1 goto :buildfailed
ECHO(   

CALL :printHeader, "Cleaning visual studio solutions: Configuration = RelWithDebInfo"
msbuild /p:Platform=x64 /p:Configuration=RelWithDebInfo Colosseum.sln /t:Clean
if ERRORLEVEL 1 goto :buildfailed
ECHO( 

CALL :printHeader, "Cleaning git"
git clean -ffdx
ECHO(

CALL :printHeader, "Pulling latest in git"
git pull
ECHO( 

CALL :printHeader, "Clean completed successfully"
goto :eof

:buildfailed
CALL :printHeader, "Clean failed"
goto :eof

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)
exit /B 0

:removeFolder
    IF EXIST %~1 (
        ECHO Removing folder: %~1
        rd /s/q %~1
    ) ELSE (
        ECHO folder "%~1" does not exist!
    )
exit /b 0

:printHeader
REM //---------- PrintHeader function ----------
if not "%~1"=="" (
    ECHO %ESC%[33m*****************************************************************************************%ESC%[0m
    ECHO %ESC%[33m%~1%ESC%%[0
    ECHO %ESC%[33m*****************************************************************************************%ESC%%[0m
)
exit /b 0