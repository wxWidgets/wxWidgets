/////////////////////////////////////////////////////////////////////////////
// Name:        mfctest.h
// Purpose:     Sample to demonstrate mixing MFC and wxWidgets code
// Author:      Julian Smart
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __MFCTEST_H__
#define __MFCTEST_H__

// CMainWindow: just a normal MFC window class.
class CMainWindow : public CFrameWnd
{
public:
    CMainWindow();

    //{{AFX_MSG( CMainWindow )
    afx_msg void OnPaint();
    afx_msg void OnAbout();
    afx_msg void OnTest();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

private:
    class wxNativeContainerWindow* m_containerWX;
};

#if START_WITH_MFC_WINDOW

// There is no need to define an application class if the default behaviour of
// using the wxWindow created in wxApp::OnInit() as main window is acceptable,
// but if we want to create the initial window in MFC, we need this class in
// order to override InitMainWnd() in it.
class SampleMFCWinApp : public wxMFCWinApp
{
protected:
    BOOL InitMainWnd() wxOVERRIDE
    {
        // Demonstrate creation of an initial MFC main window.
        m_pMainWnd = new CMainWindow();
        m_pMainWnd->ShowWindow( m_nCmdShow );
        m_pMainWnd->UpdateWindow();

        return TRUE;
    }
};

#else // !START_WITH_MFC_WINDOW

typedef wxMFCWinApp SampleMFCWinApp;

#endif // START_WITH_MFC_WINDOW/!START_WITH_MFC_WINDOW

#endif // __MFCTEST_H__
