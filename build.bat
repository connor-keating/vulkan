@echo off

set TurnOffWarnings= -wd4189 -wd4201
set CommonCompilerFlags=-Zi -nologo -W4 %TurnOffWarnings% 
@REM Add build macros
set CommonCompilerFlags= -D DEBUG_BUILD=1 %CommonCompilerFlags%
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib opengl32.lib


IF NOT EXIST .build mkdir .build
pushd .build

del *.pdb > NUL 2> NUL

set files=..\src\main.c
set includes=-I^
..\include

set year=%date:~-4,4%
set month=%date:~-10,2%
set day=%date:~-7,2%
set hour=%time:~0,2%
set minute=%time:~3,2%
set second=%time:~6,2%
:: Remove leading space in hour
if "%hour:~0,1%"==" " set hour=0%hour:~1,1%
set timestamp=%year%%month%%day%_%hour%%minute%%second%

set namePDB=application_%timestamp%.pdb

@REM 64-bit build command
call cl %CommonCompilerFlags% ..\src\application.c -Fmhandmade.map -Foapplication.obj -Fd%namePDB% -LD -link -incremental:no -opt:ref -PDB:%namePDB% opengl32.lib 
@REM call cl /c /Zi /nologo src\application.c /Foapplication.obj 
@REM call link opengl32.lib /nologo /DLL /DEBUG /OUT:application.dll application.obj
call cl %CommonCompilerFlags% %includes% %files% -link %CommonLinkerFlags% -OUT:dungeon.exe

@REM Example external hot reload.
@REM cl %CommonCompilerFlags% ..\src\external.cpp /LD /link -PDB:external.pdb /EXPORT:MyExport
@REM cl %CommonCompilerFlags% /I ..\src ..\src\win32_main.cpp /link %CommonLinkerFlags%


popd
