@echo off

set TurnOffWarnings= -wd4189 -wd4201
set CommonCompilerFlags=-std:c11 -Zi -nologo -W4 %TurnOffWarnings% 
@REM Add build macros
set CommonCompilerFlags= -D DEBUG_BUILD=1  -D VK_USE_PLATFORM_WIN32_KHR %CommonCompilerFlags%
set CommonLinkerFlags= -incremental:no -opt:ref -MACHINE:X64 user32.lib gdi32.lib winmm.lib 


IF NOT EXIST .build mkdir .build
pushd .build

del *.pdb > NUL 2> NUL

set files=..\src\main.c
:: Define included directories
set includes=-I"..\include" -I"C:\VulkanSDK\1.4.304.0\Include"
:: Define library paths
set libs=-LIBPATH:"C:\VulkanSDK\1.4.304.0\Lib"
set libs_platform=vulkan-1.lib

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
@REM call cl %CommonCompilerFlags% ..\src\application.c -Fmhandmade.map -Foapplication.obj -Fd%namePDB% -LD -link -incremental:no -opt:ref -PDB:%namePDB% opengl32.lib 
::call cl %CommonCompilerFlags% %includes% %libs% %files% -link %CommonLinkerFlags% -OUT:main.exe
call cl %CommonCompilerFlags% %includes% %files% -link %CommonLinkerFlags% %libs% %libs_platform% -PDB:%namePDB% -OUT:main.exe



popd
