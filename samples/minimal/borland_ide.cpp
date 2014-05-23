/*************************************************
// Name:        borland_ide.cpp
// Author:      Chris Elliott
// Modified by:
// Created:     26/Apr/07
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
//
// Purpose:     Minimal wxWidgets start source file for
// Borland's Turbo explorer or Developer Studio 2006.
// Doubleclick / open borland.bdsproj. The current version is
// to be used with a dynamic build of wxWidgets-made by running
// make -f Makefile.bcc -DBUILD=debug -DSHARED=1
// in wxWidgets\build\msw. You also need the wxWidgets\lib\bcc_dll
// directory in your PATH.

// To use this to debug other samples, copy the borland_ide.cpp
// and borland.bdsproj files, then replace all occurences of
// "minimal" with the name of the new project files
**************************************************/

#define WinMain WinMain

//wx libs (you may not need all of these
#pragma link  "wxbase31ud.lib"
#pragma link  "wxmsw31ud_adv.lib"
#pragma link  "wxmsw31ud_core.lib"
#pragma link  "wxmsw31ud_html.lib"
#pragma link  "wxmsw31ud_media.lib"
#pragma link  "wxmsw31ud_xrc.lib"
#pragma link  "wxmsw31ud_aui.lib"
#pragma link  "wxmsw31ud_stc.lib"
#pragma link  "wxmsw31ud_richtext.lib"

//graphics libs
#pragma link  "wxpngd.lib"
#pragma link  "wxregexud.lib"
#pragma link  "wxtiffd.lib"
#pragma link  "wxzlibd.lib"
//code aware lib
#pragma link  "wxscintillad.lib"

