@echo off

SET LIBS=user32.lib d3d9.lib d3dx9.lib dinput8.lib dsound.lib strmiids.lib Winmm.lib
SET DIRECTX_LIB_PATH="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64"
SET DIRECTX_INCLUDE_PATH="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"

IF NOT EXIST build mkdir build

pushd build
cl /Zi ..\code\*.cpp /ogame %LIBS% ^
/I%DIRECTX_INCLUDE_PATH% ^
/link /LIBPATH:%DIRECTX_LIB_PATH%
popd
