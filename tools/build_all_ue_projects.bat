@echo off
REM //---------- set up variable ----------
setlocal
set ROOT_DIR=%~dp0

REM should rebuild?
set RebuildColosseum=%1
REM root folder containing all Unreal projects you want to build
set UnrealProjsPath=%2
REM Output folder where the builds would be stored
set OutputPath=%3
REM folder containing Colosseum repo
set ColosseumPath=%4
REM path for UE toolset
set ToolPath=%5

REM set defaults if ars are not supplied
if "%UnrealProjsPath%"=="" set "UnrealProjsPath=D:\vso\msresearch\Theseus"
if "%ColosseumPath%"=="" set "ColosseumPath=C:\GitHubSrc\Colosseum"
if "%OutputPath%"=="" set "OutputPath=%ROOT_DIR%build"
if "%RebuildColosseum%"=="" set "RebuildColosseum=true"

REM re-build colosseum
if "%RebuildColosseum%"=="true" (
    cd /D "%ColosseumPath%"
    CALL clean
    CALL build
    if ERRORLEVEL 1 goto :failed
    cd /D "%ROOT_DIR%"
)

IF NOT EXIST "%OutputPath%" mkdir "%OutputPath%"

call:doOneProject "TalkingHeads"
call:doOneProject "ZhangJiaJie"
call:doOneProject "ColosseumEnvNH"
call:doOneProject "SimpleMaze"
call:doOneProject "LandscapeMountains"
call:doOneProject "Africa_001" "Africa"
call:doOneProject "Forest"
call:doOneProject "Coastline"
call:doOneProject "TrapCamera"
call:doOneProject "CityEnviron"
call:doOneProject "Warehouse"
call:doOneProject "Plains" "" 4.19

goto :done

:doOneProject
REM args: OutputPath ToolPath UEVer
if "%~2"=="" (
 cd /D "%UnrealProjsPath%\%~1"
) else (
 cd /D "%UnrealProjsPath%\%~2"
)
if ERRORLEVEL 1 goto :failed

robocopy "%ColosseumPath%\Unreal\Environments\BlocksV2" . *.bat  /njh /njs /ndl /np

CALL update_from_git.bat "%ColosseumPath%"
if ERRORLEVEL 1 goto :failed

CALL package.bat "%OutputPath%" "%ToolPath%" %~3
if ERRORLEVEL 1 goto :failed

goto :done

:failed
echo "Error occured while building all UE projects"
exit /b 1

:done
cd "%ROOT_DIR%"
if "%1"=="" pause