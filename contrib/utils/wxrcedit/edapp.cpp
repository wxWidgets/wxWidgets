/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation
    #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/xml/xml.h"
#include "wx/image.h"
#include "wx/wx.h"

#include "editor.h"
#include "preview.h"
#include "propframe.h"


// -- Application

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit()
{
    SetVendorName(wxT("wxWidgets"));
    SetAppName(wxT("wxrcedit"));
    wxString arg = (argc >= 1) ? argv[1] : _T("");
    wxInitAllImageHandlers();
    wxFrame *frame = new EditorFrame(NULL, arg);
    SetTopWindow(frame);
    frame->Show(true);
    PreviewFrame::Get()->Show(true);
    PropertiesFrame::Get()->Show(true);
    return true;
}

