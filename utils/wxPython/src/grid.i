/////////////////////////////////////////////////////////////////////////////
// Name:        newgrid.i
// Purpose:     SWIG definitions for the new wxGrid and related classes
//
// Author:      Robin Dunn
//
// Created:     6/2/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module newgrid

%{
#include "helpers.h"
#include <wx/grid.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import controls.i
%import events.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
