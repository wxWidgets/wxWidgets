/////////////////////////////////////////////////////////////////////////////
// Name:        dllwidget.cpp
// Purpose:     Dynamically loadable C++ widget for wxPython
// Author:      Vaclav Slavik
// Created:     2001/12/03
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dllwidget.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"
#include "wx/dynlib.h"
#include "wx/sizer.h"

#include "dllwidget.h"


IMPLEMENT_ABSTRACT_CLASS(wxDllWidget, wxPanel)

wxDllWidget::wxDllWidget(wxWindow *parent,
                         wxWindowID id,
                         const wxString& dllName, const wxString& className,
                         const wxPoint& pos, const wxSize& size,
                         long style)
    : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER,
              className + wxT("_container")),
    m_widget(NULL), m_lib(NULL), m_controlAdded(false)
{
    SetBackgroundColour(wxColour(255, 0, 255));
    if ( !!className )
        LoadWidget(dllName, className, style);
}

wxDllWidget::~wxDllWidget()
{
    UnloadWidget();
}

void wxDllWidget::AddChild(wxWindowBase *child)
{
    wxASSERT_MSG( !m_controlAdded, wxT("Couldn't load two widgets into one container!") );

    wxPanel::AddChild(child);

    m_controlAdded = true;
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add((wxWindow*)child, 1, wxEXPAND);
    SetSizer(sizer);
    Layout();
}


wxString wxDllWidget::GetDllExt()
{
    return wxDllLoader::GetDllExt();
}


typedef WXDLLEXPORT bool (*DLL_WidgetFactory_t)(const wxString& className,
                                                wxWindow *parent,
                                                long style,
                                                wxWindow **classInst,
                                                wxSendCommandFunc *cmdFunc);

bool wxDllWidget::LoadWidget(const wxString& dll, const wxString& className,
                             long style)
{
    UnloadWidget();

    // Load the dynamic library
    m_lib = new wxDynamicLibrary(dll);
    if ( !m_lib->IsLoaded() )
    {
        delete m_lib;
        m_lib = NULL;
        return false;
    }

    DLL_WidgetFactory_t factory;
    factory = (DLL_WidgetFactory_t) m_lib->GetSymbol(wxT("DLL_WidgetFactory"));
    if ( factory == NULL)
    {
        delete m_lib;
        m_lib = NULL;
        return false;
    }

    if ( !factory(className, this, style, &m_widget, &m_cmdFunc) )
    {
        delete m_widget;
        delete m_lib;
        m_lib = NULL;
        m_widget = NULL;
        return false;
    }

    return true;
}

void wxDllWidget::UnloadWidget()
{
    if ( m_widget )
    {
        DestroyChildren();
        m_widget = NULL;
        delete m_lib;
        m_lib = NULL;
    }
}

int wxDllWidget::SendCommand(int cmd, const wxString& param)
{
    wxASSERT_MSG( m_widget && m_cmdFunc, wxT("Sending command to not loaded widget!"));

    return m_cmdFunc(m_widget, cmd, param);
}
