// hello.h : Declares the class interfaces for the application.
//         Hello is a simple program which consists of a main window
//         and an "About" dialog which can be invoked by a menu choice.
//         It is intended to serve as a starting-point for new
//         applications.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __MFCTEST_H__
#define __MFCTEST_H__

/////////////////////////////////////////////////////////////////////////////

// CMainWindow:
// See hello.cpp for the code to the member functions and the message map.
//
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
};

// A dummy CWnd pointing to a wxWindow's HWND
class CDummyWindow: public CWnd
{
  public:
    CDummyWindow(HWND hWnd);
    ~CDummyWindow(void);
};

/////////////////////////////////////////////////////////////////////////////

// CTheApp:
// See hello.cpp for the code to the InitInstance member function.
//
class CTheApp : public CWinApp
{
public:
	BOOL InitInstance();
    int ExitInstance();

    // Override this to provide wxWindows message loop
    // compatibility
    BOOL PreTranslateMessage(MSG *msg);
    BOOL OnIdle(LONG lCount);
};

/////////////////////////////////////////////////////////////////////////////

#endif // __MFCTEST_H__
