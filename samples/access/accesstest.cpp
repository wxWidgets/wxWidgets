/////////////////////////////////////////////////////////////////////////////
// Name:        accesstest.cpp
// Purpose:     wxWindows accessibility sample
// Author:      Julian Smart
// Modified by:
// Created:     2002-02-12
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/access.h"
#include "wx/splitter.h"

#ifdef __WXMSW__
#include "windows.h"
#include <ole2.h>
#include <oleauto.h>
#include <oleacc.h>
#include "wx/msw/ole/oleutils.h"
#include "wx/msw/winundef.h"

#ifndef OBJID_CLIENT
#define OBJID_CLIENT 0xFFFFFFFC
#endif

#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();

};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnQuery(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    // Log messages to the text control
    void Log(const wxString& text);

    // Recursively give information about an object
    void LogObject(int indent, IAccessible* obj);

    // Get info for a child (id > 0) or object (id == 0)
    void GetInfo(IAccessible* accessible, int id, wxString& name, wxString& role);
private:
    wxTextCtrl* m_textCtrl;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    AccessTest_Quit = 1,

    // query the hierarchy
    AccessTest_Query,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    AccessTest_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(AccessTest_Quit,  MyFrame::OnQuit)
    EVT_MENU(AccessTest_Query,  MyFrame::OnQuery)
    EVT_MENU(AccessTest_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("AccessTest wxWindows App"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

class FrameAccessible: public wxWindowAccessible
{
public:
    FrameAccessible(wxWindow* win): wxWindowAccessible(win) {}

            // Gets the name of the specified object.
    virtual wxAccStatus GetName(int childId, wxString* name)
    {
#if 1
        if (childId == wxACC_SELF)
        {
            * name = wxT("Julian's Frame");
            return wxACC_OK;
        }
        else
#endif
            return wxACC_NOT_IMPLEMENTED;
    }
};

class ScrolledWindowAccessible: public wxWindowAccessible
{
public:
    ScrolledWindowAccessible(wxWindow* win): wxWindowAccessible(win) {}

            // Gets the name of the specified object.
    virtual wxAccStatus GetName(int childId, wxString* name)
    {
        if (childId == wxACC_SELF)
        {
            * name = wxT("My scrolled window");
            return wxACC_OK;
        }
        else
            return wxACC_NOT_IMPLEMENTED;
    }
};

class SplitterWindowAccessible: public wxWindowAccessible
{
public:
    SplitterWindowAccessible(wxWindow* win): wxWindowAccessible(win) {}

            // Gets the name of the specified object.
    virtual wxAccStatus GetName(int childId, wxString* name)
    {
        if (childId == wxACC_SELF)
        {
            * name = wxT("Splitter window");
            return wxACC_OK;
        }
        else
            return wxACC_NOT_IMPLEMENTED;
    }

        // Can return either a child object, or an integer
        // representing the child element, starting from 1.
    virtual wxAccStatus HitTest(const wxPoint& pt, int* childId, wxAccessible** childObject)
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns the rectangle for this object (id = 0) or a child element (id > 0).
    virtual wxAccStatus GetLocation(wxRect& WXUNUSED(rect), int WXUNUSED(elementId))
         { return wxACC_NOT_IMPLEMENTED; }

        // Navigates from fromId to toId/toObject.
    virtual wxAccStatus Navigate(wxNavDir WXUNUSED(navDir), int WXUNUSED(fromId),
                int* WXUNUSED(toId), wxAccessible** WXUNUSED(toObject))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the number of children.
    virtual wxAccStatus GetChildCount(int* WXUNUSED(childId))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the specified child (starting from 1).
        // If *child is NULL and return value is wxACC_OK,
        // this means that the child is a simple element and
        // not an accessible object.
    virtual wxAccStatus GetChild(int WXUNUSED(childId), wxAccessible** WXUNUSED(child))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the parent, or NULL.
    virtual wxAccStatus GetParent(wxAccessible** WXUNUSED(parent))
         { return wxACC_NOT_IMPLEMENTED; }

        // Performs the default action. childId is 0 (the action for this object)
        // or > 0 (the action for a child).
        // Return wxACC_NOT_SUPPORTED if there is no default action for this
        // window (e.g. an edit control).
    virtual wxAccStatus DoDefaultAction(int WXUNUSED(childId))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the default action for this object (0) or > 0 (the action for a child).
        // Return wxACC_OK even if there is no action. actionName is the action, or the empty
        // string if there is no action.
        // The retrieved string describes the action that is performed on an object,
        // not what the object does as a result. For example, a toolbar button that prints
        // a document has a default action of "Press" rather than "Prints the current document."
    virtual wxAccStatus GetDefaultAction(int WXUNUSED(childId), wxString* WXUNUSED(actionName))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns the description for this object or a child.
    virtual wxAccStatus GetDescription(int WXUNUSED(childId), wxString* WXUNUSED(description))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns help text for this object or a child, similar to tooltip text.
    virtual wxAccStatus GetHelpText(int WXUNUSED(childId), wxString* WXUNUSED(helpText))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns the keyboard shortcut for this object or child.
        // Return e.g. ALT+K
    virtual wxAccStatus GetKeyboardShortcut(int WXUNUSED(childId), wxString* WXUNUSED(shortcut))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns a role constant.
    virtual wxAccStatus GetRole(int WXUNUSED(childId), wxAccRole* WXUNUSED(role))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns a state constant.
    virtual wxAccStatus GetState(int WXUNUSED(childId), long* WXUNUSED(state))
         { return wxACC_NOT_IMPLEMENTED; }

        // Returns a localized string representing the value for the object
        // or child.
    virtual wxAccStatus GetValue(int WXUNUSED(childId), wxString* WXUNUSED(strValue))
         { return wxACC_NOT_IMPLEMENTED; }

        // Selects the object or child.
    virtual wxAccStatus Select(int WXUNUSED(childId), wxAccSelectionFlags WXUNUSED(selectFlags))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets the window with the keyboard focus.
        // If childId is 0 and child is NULL, no object in
        // this subhierarchy has the focus.
        // If this object has the focus, child should be 'this'.
    virtual wxAccStatus GetFocus(int* WXUNUSED(childId), wxAccessible** WXUNUSED(child))
         { return wxACC_NOT_IMPLEMENTED; }

        // Gets a variant representing the selected children
        // of this object.
        // Acceptable values:
        // - a null variant (IsNull() returns TRUE)
        // - a list variant (GetType() == wxT("list"))
        // - an integer representing the selected child element,
        //   or 0 if this object is selected (GetType() == wxT("long"))
        // - a "void*" pointer to a wxAccessible child object
    virtual wxAccStatus GetSelections(wxVariant* WXUNUSED(selections))
         { return wxACC_NOT_IMPLEMENTED; }

};

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
    m_textCtrl = NULL;

//    SetAccessible(new FrameAccessible(this));

    // set the frame icon
    SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(AccessTest_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(AccessTest_Query, _T("Query"), _T("Query the window hierarchy"));
    menuFile->AppendSeparator();
    menuFile->Append(AccessTest_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if 0 // wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR

#if 1
    wxSplitterWindow* splitter = new wxSplitterWindow(this, -1);
    splitter->CreateAccessible();

    wxListBox* listBox = new wxListBox(splitter, -1);
    listBox->CreateAccessible();

    m_textCtrl = new wxTextCtrl(splitter, -1, wxT(""), wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    m_textCtrl->CreateAccessible();

    splitter->SplitHorizontally(listBox, m_textCtrl, 150);
#endif

#if 0
#if 1
    wxListBox* listBox = new wxListBox(this, -1);
    //listBox->SetAccessible(new wxAccessible(listBox));
#else
    wxScrolledWindow* scrolledWindow = new wxScrolledWindow(this, -1);
    scrolledWindow->SetAccessible(new ScrolledWindowAccessible(scrolledWindow));
#endif
#endif
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the AccessTest sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About AccessTest"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnQuery(wxCommandEvent& WXUNUSED(event))
{
    m_textCtrl->Clear();
    IAccessible* accessibleFrame = NULL;
    if (S_OK != AccessibleObjectFromWindow((HWND) GetHWND(), OBJID_CLIENT,
        IID_IAccessible, (void**) & accessibleFrame))
    {
        Log(wxT("Could not get object."));
        return;
    }
    if (accessibleFrame)
    {
        //Log(wxT("Got an IAccessible for the frame."));
        LogObject(0, accessibleFrame);
        Log(wxT("Checking children using AccessibleChildren()..."));

        // Now check the AccessibleChildren function works OK
        long childCount = 0;
        if (S_OK != accessibleFrame->get_accChildCount(& childCount))
        {
            Log(wxT("Could not get number of children."));
            accessibleFrame->Release();
            return;
        }
        else if (childCount == 0)
        {
            Log(wxT("No children."));
            accessibleFrame->Release();
            return;
        }
        

        long obtained = 0;
        VARIANT *var = new VARIANT[childCount];
        int i;
        for (i = 0; i < childCount; i++)
        {
            VariantInit(& (var[i]));
            var[i].vt = VT_DISPATCH;
        }
        
        if (S_OK == AccessibleChildren(accessibleFrame, 0, childCount, var, &obtained))
        {
            for (i = 0; i < childCount; i++)
            {
                IAccessible* childAccessible = NULL;
                if (var[i].pdispVal)
                {
                    if (var[i].pdispVal->QueryInterface(IID_IAccessible, (LPVOID*) & childAccessible) == S_OK)
                    {
                        var[i].pdispVal->Release();
                        
                        wxString name, role;
                        GetInfo(childAccessible, 0, name, role);
                        wxString str;
                        str.Printf(wxT("Found child %s/%s"), name.c_str(), role.c_str());
                        Log(str);
                        childAccessible->Release();
                    }
                    else
                    {
                        var[i].pdispVal->Release();
                    }
                }
            }
        }
        else
        {
            Log(wxT("AccessibleChildren failed."));
        }
        delete[] var;


        accessibleFrame->Release();
    }
}

// Log messages to the text control
void MyFrame::Log(const wxString& text)
{
    if (m_textCtrl)
    {
        wxString text2(text);
        text2.Replace(wxT("\n"), wxT(" "));
        text2.Replace(wxT("\r"), wxT(" "));
        m_textCtrl->SetInsertionPointEnd();
        m_textCtrl->WriteText(text2 + wxT("\n"));
    }
}

// Recursively give information about an object
void MyFrame::LogObject(int indent, IAccessible* obj)
{
    wxString name, role;
    if (indent == 0)
    {
        GetInfo(obj, 0, name, role);

        wxString str;
        str.Printf(wxT("Name = %s; Role = %s"), name.c_str(), role.c_str());
        str.Pad(indent, wxT(' '), FALSE);
        Log(str);
    }

    long childCount = 0;
    if (S_OK == obj->get_accChildCount(& childCount))
    {
        wxString str;
        str.Printf(wxT("There are %d children."), (int) childCount);
        str.Pad(indent, wxT(' '), FALSE);
        Log(str);
        Log(wxT(""));
    }

    int i;
    for (i = 1; i <= childCount; i++)
    {
        GetInfo(obj, i, name, role);

        wxString str;
        str.Printf(wxT("%d) Name = %s; Role = %s"), i, name.c_str(), role.c_str());
        str.Pad(indent, wxT(' '), FALSE);
        Log(str);

        VARIANT var;
        VariantInit(& var);
        var.vt = VT_I4;
        var.lVal = i;
        IDispatch* pDisp = NULL;
        IAccessible* childObject = NULL;

        if (S_OK == obj->get_accChild(var, & pDisp) && pDisp)
        {
            wxString str;
            str.Printf(wxT("This is a real object."));
            str.Pad(indent+4, wxT(' '), FALSE);
            Log(str);

            if (pDisp->QueryInterface(IID_IAccessible, (LPVOID*) & childObject) == S_OK)
            {
                LogObject(indent + 4, childObject);
                childObject->Release();
            }
            pDisp->Release();
        }
        else
        {
            wxString str;
            str.Printf(wxT("This is an element."));
            str.Pad(indent+4, wxT(' '), FALSE);
            Log(str);
        }
        // Log(wxT(""));
    }

}

// Get info for a child (id > 0) or object (id == 0)
void MyFrame::GetInfo(IAccessible* accessible, int id, wxString& name, wxString& role)
{
    VARIANT var;
    VariantInit(& var);
    var.vt = VT_I4;
    var.lVal = id;
    
    BSTR bStrName = 0;
    HRESULT hResult = accessible->get_accName(var, & bStrName);
    
    if (hResult == S_OK)
    {
        name = wxConvertStringFromOle(bStrName);
        SysFreeString(bStrName);
    }
    else
    {
        name = wxT("NO NAME");
    }
    
    VARIANT varRole;
    VariantInit(& varRole);
    
    hResult = accessible->get_accRole(var, & varRole);
    
    if (hResult == S_OK && varRole.vt == VT_I4)
    {
        wxChar buf[256];
        GetRoleText(varRole.lVal, buf, 256);
        
        role = buf;
    }
    else
    {
        role = wxT("NO ROLE");
    }    
}
