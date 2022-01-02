/////////////////////////////////////////////////////////////////////////////
// Name:        wx/uwp/app.h
// Purpose:     wxApp class
// Author:      Cloyz2021
// Modified by:
// Created:     02/01/2020
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

// ----------------------------------------------------------------------------
// the wxApp class for UWP - see wxAppBase for more details
// ----------------------------------------------------------------------------
class WXDLLIMPEXP_CORE wxApp : public wxAppBase
{
public:
    wxApp();
    virtual ~wxApp();

    // override base class (pure) virtuals
    // -----------------------------------

    virtual void Exit();

    virtual void CleanUp() wxOVERRIDE;

    virtual void WakeUpIdle();

public:
    // Implementation
    virtual bool Initialize(int& argc, wxChar** argv);

protected:
    wxDECLARE_DYNAMIC_CLASS(wxApp);
};
#endif // _WX_APP_H_

