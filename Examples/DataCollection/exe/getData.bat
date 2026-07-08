ECHO OFF
SET NUM=1000
SET COLOSSEUMPATH=E:\ColosseumBinaries
SET DATAPATH=D:\Data\Colosseum
SET EXE=DataCollectorSGM.exe

mkdir %DATAPATH%

START /I %COLOSSEUMPATH%\Africa\Africa_001.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\AA
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM Africa_001.exe

START /I %COLOSSEUMPATH%\ColosseumNH\ColosseumNH.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\NH
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM ColosseumNH.exe

START /I %COLOSSEUMPATH%\CityEnviron\CityEnviron.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\CE
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM CityEnviron.exe

START /I %COLOSSEUMPATH%\Coastline\Coastline.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\CL
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM Coastline.exe

START /I %COLOSSEUMPATH%\Forest\Forest.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\FT
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM Forest.exe

START /I %COLOSSEUMPATH%\LandscapeMountains\LandscapeMountains.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\LM
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM LandscapeMountains.exe

START /I %COLOSSEUMPATH%\SimpleMaze\Car_Maze.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\SM
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM Car_Maze.exe

START /I %COLOSSEUMPATH%\Warehouse\Factory.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\WH
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM Factory.exe

START /I %COLOSSEUMPATH%\ZhangJiaJie\ZhangJiaJie.exe -windowed
TIMEOUT 30 & REM Waits 30 seconds
START /B /wait %EXE% %NUM% %DATAPATH%\ZJ
TIMEOUT 10 & REM Waits 10 seconds
TASKKILL /F /IM ZhangJiaJie.exe