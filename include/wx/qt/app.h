/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_APP_H_
#define _WX_QT_APP_H_

#include <wx/scopedarray.h>

class QApplication;
class WXDLLIMPEXP_CORE wxApp : public wxAppBase
{
public:
    wxApp();
    ~wxApp();

    virtual bool Initialize(int& argc, wxChar **argv) wxOVERRIDE;

private:
    wxScopedPtr<QApplication> m_qtApplication;
    int m_qtArgc;
    wxScopedArray<char*> m_qtArgv;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxApp );
};

#endif // _WX_QT_APP_H_
