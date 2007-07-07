#ifndef __WX_APP_H__
#define __WX_APP_H__

class WXDLLEXPORT wxApp: public wxAppBase
{
public:
    wxApp();
    ~wxApp();

    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();
    
    virtual void Exit();
    virtual void WakeUpIdle();
    virtual bool Yield(bool onlyIfNeeded = false);

private:
    friend class wxGUIEventLoop;
    wxString m_sessionId;
    wxString m_remoteIp;
    wxString m_requestFifoPath;
    wxString m_responseFifoPath;

private:
    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

#endif // __WX_APP_H__
