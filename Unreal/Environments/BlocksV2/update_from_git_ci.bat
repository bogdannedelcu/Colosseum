@echo off
REM //---------- set up variable ----------
setlocal
set ROOT_DIR=%~dp0

set ColosseumPath=%1

REM default path works for Blocks environment
if "%ColosseumPath%"=="" set "ColosseumPath=..\..\.."

IF NOT EXIST "%ColosseumPath%" (
	echo "ColosseumPath %ColosseumPath% was not found"
	goto :failed
)

echo Using ColosseumPath = %ColosseumPath%

robocopy /MIR "%ColosseumPath%\Unreal\Plugins\Colosseum" Plugins\Colosseum /XD temp *. /njh /njs /ndl /np
robocopy /MIR "%ColosseumPath%\ColosseumLib" Plugins\Colosseum\Source\ColosseumLib /XD temp *. /njh /njs /ndl /np
robocopy  /njh /njs /ndl /np "%ColosseumPath%\Unreal\Environments\BlocksV2" "." *.bat 
robocopy  /njh /njs /ndl /np "%ColosseumPath%\Unreal\Environments\BlocksV2" "." *.sh  
rem robocopy /njh /njs /ndl /np "%ColosseumPath%" "." *.gitignore

REM cmd /c clean.bat
REM cmd /c GenerateProjectFiles.bat

goto :done

:failed
echo Error occured while updating.
exit /b 1

:done
REM if "%1"=="" pause