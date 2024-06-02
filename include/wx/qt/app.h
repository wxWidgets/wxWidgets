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

#include <memory>

class QApplication;
class WXDLLIMPEXP_CORE wxApp : public wxAppBase
{
public:
    wxApp();
    ~wxApp();

    virtual bool Initialize(int& argc, wxChar **argv) override;

#ifdef _WIN32
    // returns 400, 470, 471 for comctl32.dll 4.00, 4.70, 4.71 or 0 if it
    // wasn't found at all
    static int GetComCtl32Version();
#endif // _WIN32

private:
    std::unique_ptr<QApplication> m_qtApplication;
    int m_qtArgc;
    wxScopedArray<char*> m_qtArgv;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxApp );
};

#endif // _WX_QT_APP_H_
