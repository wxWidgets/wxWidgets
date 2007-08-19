#ifndef __WX_APP_H__
#define __WX_APP_H__

class WXDLLEXPORT wxApp: public wxAppBase
{
public:
    wxApp();
    virtual ~wxApp();

    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();
    
    virtual void Exit();
    virtual void WakeUpIdle();
    virtual bool Yield(bool onlyIfNeeded = false);

    const wxString& GetResourcePath() const;
    const wxString& GetResourceUrl() const;

private:
    bool WriteTemplate();
    wxString GetTemplate();

private:
    friend class wxGUIEventLoop;
    friend class wxWindowDC;
    wxString m_sessionId;
    wxString m_remoteIp;
    wxString m_requestFifoPath;
    wxString m_responseFifoPath;
    wxString m_resourcePath;
    wxString m_resourceUrl;

protected:
    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

#endif // __WX_APP_H__
