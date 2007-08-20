#ifndef __WX_APP_H__
#define __WX_APP_H__

class wxWindowWeb;

WX_DECLARE_OBJARRAY(wxString, wxResourceFileArray);
WX_DECLARE_OBJARRAY(wxWindowWeb*, wxWindowArray);


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

    // Flush all client-side evaluation buffers
    virtual void FlushBuffers();
    virtual void RequestFlush(wxWindowWeb* win);

    //Override this to set the browser window title while the application is
    //  loading
    virtual wxString GetTitle() const { return "Loading..."; }

    //Override this to set the HTML-formatted warning that the user will see if
    //  Javascript is unavailable
    virtual wxString GetNoScript() const;

    virtual wxString GetResourceFile();
    virtual const wxString& GetResourcePath() const;
    virtual const wxString& GetResourceUrl() const;

private:
    bool WriteTemplate();
    wxString GetTemplate() const;

private:
    friend class wxGUIEventLoop;
    friend class wxWindowDC;
    wxString m_sessionId;
    wxString m_remoteIp;
    wxString m_requestFifoPath;
    wxString m_responseFifoPath;
    wxString m_resourcePath;
    wxString m_resourceUrl;
    wxResourceFileArray m_resourceFiles;
    wxWindowArray m_flushTargets;

    static const char* DEFAULT_CANVAS_ID;

protected:
    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

#endif // __WX_APP_H__
