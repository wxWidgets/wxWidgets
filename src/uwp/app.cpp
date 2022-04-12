/////////////////////////////////////////////////////////////////////////////
// Name:        src/uwp/app.cpp
// Purpose:     wxApp
// Author:      Yann Clotioloman Yéo
// Modified by:
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Web.Syndication.h>

bool wxApp::Initialize(int& argc_, wxChar** argv_)
{
    winrt::init_apartment();
    return wxAppBase::Initialize(argc_, argv_))
}

void wxApp::CleanUp()
{
    wxAppBase::CleanUp();
}

void wxApp::Exit()
{

}

// ----------------------------------------------------------------------------
// wxApp ctor/dtor
// ----------------------------------------------------------------------------
wxApp::wxApp()
{
}

wxApp::~wxApp()
{
}

// ----------------------------------------------------------------------------
// wxApp idle handling
// ----------------------------------------------------------------------------

void wxApp::WakeUpIdle()
{
}
