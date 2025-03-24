@echo off

if not exist .\build mkdir .\build

set TARGET=ant
set CFLAGS=/W2 /nologo /Od /Zi /std:c++14
set LIBS=User32.lib dxguid.lib D3D11.lib D3DCompiler.lib assimp-vc143-mt.lib
set SOURCES=.\src\win32_platform.cpp
set OUT_DIR=/Fo.\build\ /Fe.\build\%TARGET% /Fm.\build\
set INC_DIR=/Ilibs\glm /Ilibs\stb /Ilibs\assimp\include
set LNK_DIR=/LIBPATH:libs\assimp\lib\Release
set DEFINES=/DANT_DEBUG /DANT_CONSOLE

echo on

cl %CFLAGS% %DEFINES% %INC_DIR% %SOURCES% %OUT_DIR% /link %LNK_DIR% %LIBS% /SUBSYSTEM:CONSOLE

