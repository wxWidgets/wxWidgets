/////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/app.h
// Purpose:     wxApp class
// Author:      Andrei Matuk
// RCS-ID:      $Id$
// Copyright:   (c) Andrei Matuk
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_SYMBIAN_H_
#define _WX_APP_SYMBIAN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "app.h"
#endif

#include "wx/event.h"
#if wxUSE_GUI
#include "wx/icon.h"
#endif
#include "eikapp.h"
#include "eikmenup.h"
#include "aknviewappui.h"
#include "akndoc.h"
#include "aknview.h"

class WXDLLIMPEXP_CORE wxFrame;
class WXDLLIMPEXP_CORE wxWindow;
class WXDLLIMPEXP_CORE wxApp;
class WXDLLIMPEXP_CORE wxKeyEvent;
class WXDLLIMPEXP_BASE wxLog;

void wxBlockAsyncStart();
void wxBlockAsyncStop();
bool wxIsAsyncBlocked();

// ----------------------------------------------------------------------------
// Symbian blank view - this is just a default blank background as you
// have to have one. Normally a user window would be shown on top
// ----------------------------------------------------------------------------
#if wxUSE_GUI
class wxSymbianBlankView : public CCoeControl
{
public:
    void ConstructL(const TRect& aRect)
    {
        CreateWindowL();
        SetRect(aRect);
        ActivateL();
    }

    ~wxSymbianBlankView() {};

private:

    void Draw(const TRect& aRect) const
    {
        CWindowGc& gc = SystemGc();

//TODO: Temp draw code just to see it worked
        gc.SetPenStyle( CGraphicsContext::ENullPen );
        gc.SetBrushColor( KRgbRed );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( aRect );
    }
};

// ----------------------------------------------------------------------------
// Symbian UI class - the OS requires a document object like this to
// manage menu selections, button presses etc
// ----------------------------------------------------------------------------

class wxSymbianUI : public CAknViewAppUi
{
public:
    void ConstructL()
    {
        BaseConstructL();

        m_blankview = new (ELeave) wxSymbianBlankView;
        m_blankview->SetMopParent(this);
        m_blankview->ConstructL(ClientRect());
        AddToStackL(m_blankview);
    }

    ~wxSymbianUI();

private:
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
        TVwsViewId activeViewId;
        GetActiveViewId(activeViewId);

        CAknView *activeView = View(activeViewId.iViewUid);
        if (activeView)
            activeView->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

    void DynInitMenuBarL(TInt aResourceId, CEikMenuBar* aMenuBar)
    {
        // Called just before a menu is popped up, eg to turn check marks on
        TVwsViewId activeViewId;
        GetActiveViewId(activeViewId);

        CAknView *activeView = View(activeViewId.iViewUid);
        if (activeView)
            activeView->DynInitMenuBarL(aResourceId, aMenuBar);
    }

    void HandleCommandL(TInt aCommand);

    TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
        // Called when a key is pressed
        return EKeyWasNotConsumed;
    };

private:
    wxSymbianBlankView* m_blankview;
};

// ----------------------------------------------------------------------------
// Symbian document class - the OS requires a document object like this to
// manage non-GUI things.
// ----------------------------------------------------------------------------

class wxSymbianDocument : public CAknDocument
{
public:
    static wxSymbianDocument* NewL(CEikApplication& aApp)
    {
        wxSymbianDocument* self = new (ELeave) wxSymbianDocument( aApp );
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop();

        return self;
    }

    virtual ~wxSymbianDocument() {}

private:
    wxSymbianDocument(CEikApplication& aApp) : CAknDocument(aApp) {}
    void ConstructL() {}
    wxSymbianUI* CreateAppUiL()
    {
        return new wxSymbianUI;
    };
};
#endif
// ----------------------------------------------------------------------------
// wxApp
// ----------------------------------------------------------------------------
// Represents the application. Derive OnInit and declare
// a new App object to start application

class WXDLLEXPORT wxApp : public wxAppBase, public CEikApplication
{
    DECLARE_DYNAMIC_CLASS(wxApp)

public:
    wxApp();
    virtual ~wxApp();

    // override base class (pure) virtuals
    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();
    virtual void ExitMainLoop();

    virtual bool Yield(bool onlyIfNeeded = false);
    virtual void WakeUpIdle();

    virtual void SetPrintMode(int mode) { m_printMode = mode; }
    virtual int GetPrintMode() const { return m_printMode; }
    virtual int FilterEvent(wxEvent& event);

    // implementation only
    void OnIdle(wxIdleEvent& event);
    void OnEndSession(wxCloseEvent& event);
    void OnQueryEndSession(wxCloseEvent& event);

#if wxUSE_EXCEPTIONS
    virtual bool OnExceptionInMainLoop();
#endif // wxUSE_EXCEPTIONS

    TInt DoSymbianIdle();

protected:
    int    m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT

public:
    // Implementation
    static bool RegisterWindowClasses();
    static bool UnregisterWindowClasses();

#if wxUSE_RICHEDIT
    // initialize the richedit DLL of (at least) given version, return true if
    // ok (Win95 has version 1, Win98/NT4 has 1 and 2, W2K has 3)
    static bool InitRichEdit(int version = 2);
#endif // wxUSE_RICHEDIT

    // returns 400, 470, 471 for comctl32.dll 4.00, 4.70, 4.71 or 0 if it
    // wasn't found at all
    static int GetComCtl32Version();

    // the SW_XXX value to be used for the frames opened by the application
    // (currently seems unused which is a bug -- TODO)
    static int m_nCmdShow;

private:
    // Functions required by Symbian app class

    // You can implement the UUID with a
    // DEFINE_SYMBIAN_APP_UUID(0xeeca1093)
    TUid AppDllUid() const;

    CApaDocument* wxApp::CreateDocumentL()
    {
        return wxSymbianDocument::NewL( *this );
    }

protected:
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxApp)

    bool   m_needAppInit;       // Can't create app forms etc before the Symbian framework is started. The app is initialised in the first Idle message
    char **m_symbianargv;
    CIdle *m_idle;
};

static TInt OnSymbianIdle(TAny *ptr)
{
    ((wxApp *)ptr)->DoSymbianIdle();
    return true;
}


// ----------------------------------------------------------------------------
// Symbian specific IMPLEMENT_WXWIN_MAIN definition
// ----------------------------------------------------------------------------

// There is no main() in Symbian as such, apps come from a class factory where you
// derive from CEikApplication then implement the NewApplication.
// The implementation for NewApplication is provided in the IMPLEMENT_APP macros.

#if defined(__SERIES60_1X__) || defined(__SERIES60_2X__)
    #define IMPLEMENT_WXWIN_MAIN                                            \
                                                                            \
        GLDEF_C TInt E32Dll( TDllReason )                                    \
        {                                                                    \
            return KErrNone;                                                \
        }
#else
    #include <eikstart.h>
    #define IMPLEMENT_WXWIN_MAIN                                            \
                                                                            \
        GLDEF_C TInt E32Main()                                                \
        {                                                                    \
            return EikStart::RunApplication( NewApplication );                \
        }
#endif

#endif // _WX_APP_H_

