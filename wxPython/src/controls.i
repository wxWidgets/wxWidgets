/////////////////////////////////////////////////////////////////////////////
// Name:        controls.i
// Purpose:     Control (widget) classes for wxPython
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module controls

//---------------------------------------------------------------------------

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"

    DECLARE_DEF_STRING(PanelNameStr);
    DECLARE_DEF_STRING(EmptyString);
    DECLARE_DEF_STRING(ControlNameStr);

%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = core }


// Include all the files that make up this module

%include _controls_rename.i

//%include _control.i  <-- It's in core so other modules don't have to %import controls.i
%include _button.i
%include _checkbox.i
%include _choice.i
%include _combobox.i
%include _gauge.i
%include _statctrls.i
%include _listbox.i
%include _textctrl.i
%include _scrolbar.i
%include _spin.i
%include _radio.i
%include _slider.i
%include _tglbtn.i
%include _notebook.i
%include _toolbar.i
%include _listctrl.i
%include _treectrl.i
%include _dirctrl.i
%include _pycontrol.i
%include _cshelp.i


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
