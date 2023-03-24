echo off
set GRAPH=
TRP %1
goto findtpe
:tpepgmerr
TRP %1DE
goto findtpe
:startover
TRP %1D
:findtpe
if errorlevel 10 goto calculate
if errorlevel 1 goto err
if errorlevel 0 goto quit
echo Invalid error code.
pause
goto startover
:calculate
set GRAPH=T
if errorlevel 13 goto tpe1egt
if errorlevel 12 goto tpe356
if errorlevel 11 goto tpe8
if errorlevel 10 goto tpe10
:tpe10
rem     TPE331-10/11 program call
echo TPE331-10/11
tpepgm
if errorlevel 1 goto tpepgmerr
goto startover
:tpe8
rem     TPE331-8  program call
echo TPE331-8
tped8
if errorlevel 1 goto tpepgmerr
goto startover
:tpe356
rem     TPE331-3/-5/-6   program call
echo TPE331-3/-5/-6
tpe840
if errorlevel 1 goto tpepgmerr
goto startover
:tpe1egt
rem     TPE331-1EGT   program call
echo TPE331-1EGT
tpepgm1
if errorlevel 1 goto tpepgmerr
goto startover
:err
echo Program terminated abnormally
:quit
set GRAPH=
