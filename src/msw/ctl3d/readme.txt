
CTL3D
-----

CTL3D gives 3D controls to 16-bit Windows 3.1 applications.
Its use in wxWindows is controlled by the CTL3D symbol
in include/base/wx_setup.h.

If using a 16-bit compiler, copy ctl3dv2.lib to your compiler
library directory, and remember to distribute ctl3dv2.dll
with your applications. The DLL should be copied to
windows/system and DELETED from the application installation
directory.

If using Watcom C++ in 386 mode, things are slightly more complex: you need
to link with Patrick Halke's ctl3d32.obj which provides an interface
from 32-bits to the 16-bit CTL3DV2 library. Link your application
with ctl3d32.obj file instead of ctl3dv2.lib, distributing
ctl3dv2.dll as above.

ctl3d.dll           ; Version 1 of the CTL3D library DLL: obsolete
ctl3dv2.dll         ; Version 2 of the CTL3D library DLL
readme.txt          ; This file

msvc/ctl3d.h        ; Header file for either version of CTL3D
msvc/ctl3d.lib      ; Import library for 16-bit compilers
watcom/import32.zip ; Import libraries for Watcom WIN32 compilation
wat386/             ; Source & objects for Watcom 386 object file to
                    ; interface 16<->32 bit modes
borland/            ; Makefiles for making import libraries for Borland

