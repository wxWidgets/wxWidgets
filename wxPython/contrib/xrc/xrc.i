/////////////////////////////////////////////////////////////////////////////
// Name:        xrc.i
// Purpose:     Wrappers for the XML based Resource system
//
// Author:      Robin Dunn
//
// Created:     4-June-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module xrc


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/pyistream.h"

#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = core }


MAKE_CONST_WXSTRING_NOSWIG(EmptyString);
MAKE_CONST_WXSTRING2(UTF8String, "UTF-8");
MAKE_CONST_WXSTRING2(StyleString, "style");
MAKE_CONST_WXSTRING2(SizeString, "size");
MAKE_CONST_WXSTRING2(PosString, "pos");
MAKE_CONST_WXSTRING2(BitmapString, "bitmap");
MAKE_CONST_WXSTRING2(IconString, "icon");
MAKE_CONST_WXSTRING2(FontString, "font");


%include _xrc_rename.i


// Include all the files that make up this module
%include _xmlres.i
%include _xmlsub.i
%include _xml.i
%include _xmlhandler.i



//---------------------------------------------------------------------------

%init %{

    wxXmlInitResourceModule();
    wxXmlResource::Get()->InitAllHandlers();

%}


%pythoncode "_xrc_ex.py";


//---------------------------------------------------------------------------
