/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/app.h
// Purpose:     wxApp class for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_APP_H_
#define _WX_RETRO68_APP_H_

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxApp : public wxAppBase
{
public:
    wxApp();
    virtual ~wxApp();

    // override base class (pure) virtuals
    virtual bool Initialize(int& argc, wxChar **argv) override;
    virtual void CleanUp() override;

    virtual void WakeUpIdle() override;

    // Mac-specific functions, called when the application receives the
    // corresponding Apple events.
    virtual void MacOpenFiles(const wxArrayString& fileNames);
    virtual void MacOpenFile(const wxString& fileName);
    virtual void MacPrintFile(const wxString& fileName);
    virtual void MacNewFile();
    virtual void MacReopenApp();

    // Return true if the application is in foreground (i.e. not suspended).
    bool IsInForeground() const { return m_isInForeground; }

    // implementation only
    void MacSetInForeground(bool inForeground) { m_isInForeground = inForeground; }

private:
    bool m_isInForeground;

    wxDECLARE_DYNAMIC_CLASS(wxApp);
};

#endif // _WX_RETRO68_APP_H_
