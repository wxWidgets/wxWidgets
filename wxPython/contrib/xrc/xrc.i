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

    static const wxString wxPyEmptyString(wxT(""));
    static const wxString wxPyUTF8String(wxT("UTF-8"));
    static const wxString wxPyStyleString(wxT("style"));
    static const wxString wxPySizeString(wxT("size"));
    static const wxString wxPyPosString(wxT("pos"));
    static const wxString wxPyBitmapString(wxT("bitmap"));
    static const wxString wxPyIconString(wxT("icon"));
    static const wxString wxPyFontString(wxT("font"));
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = core }

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
