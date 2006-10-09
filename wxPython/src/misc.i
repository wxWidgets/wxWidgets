/////////////////////////////////////////////////////////////////////////////
// Name:        misc.i
// Purpose:     An extension module for miscelaneious stuff
//
// Author:      Robin Dunn
//
// Created:     3-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module(package="wx") _misc

//---------------------------------------------------------------------------

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/pyistream.h"    
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = _core }


MAKE_CONST_WXSTRING_NOSWIG(EmptyString);


// Include all the files that make up this module
%include _settings.i
%include _functions.i
%include _misc.i
%include _tipdlg.i
%include _timer.i
%include _log.i
%include _process.i
%include _joystick.i
%include _sound.i
%include _mimetype.i
%include _artprov.i
%include _config.i
%include _datetime.i
%include _dataobj.i
%include _dnd.i
%include _clipbrd.i
%include _display.i
%include _stdpaths.i
%include _power.i
%include _about.i

//---------------------------------------------------------------------------
