
This Windows-specific sample demonstrates how to use wxWidgets-based UI from
within a foreign host application that may be written in any toolkit
(including wxWidgets).

For this to work, you have to overcome two obstacles:


(1) wx's event loop in the DLL must not conflict with the host app's loop
(2) if the host app is written in wx, its copy of wx must not conflict
    with the DLL's one


Number (1) is dealt with by running DLL's event loop in a thread of its own.
DLL's wx library will consider this thread to be the "main thread".

The simplest way to solve number (2) is to share the wxWidgets library between
the DLL and the host, in the form of wxWidgets DLLs build. But this requires
both the host and the DLL to be compiled against exactly same wx version,
which is often impractical.

So we do something else here: the DLL is compiled against *static* build of
wx. This way none of its symbols or variables will leak into the host app.
Win32 runtime conflicts are eliminated by using DLL's HINSTANCE instead of
host app's one and by using unique window class names (automatically done
since wx-2.9).
