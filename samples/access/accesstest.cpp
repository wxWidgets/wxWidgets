/////////////////////////////////////////////////////////////////////////////
// Name:        accesstest.cpp
// Purpose:     wxWidgets accessibility sample
// Author:      Julian Smart
// Modified by:
// Created:     2002-02-12
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


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_ACCESSIBILITY
    #include "wx/access.h"
#endif // wxUSE_ACCESSIBILITY

#include "wx/splitter.h"
#include "wx/cshelp.h"

#ifdef __WXMSW__
    #include "windows.h"
    #include <ole2.h>
    #include <oleauto.h>

    #if wxUSE_ACCESSIBILITY
        #include <oleacc.h>
    #endif // wxUSE_ACCESSIBILITY

    #include "wx/msw/ole/oleutils.h"
    #include "wx/msw/winundef.h"

    #ifndef OBJID_CLIENT
        #define OBJID_CLIENT 0xFFFFFFFC
    #endif
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
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
    virtual bool OnInit() wxOVERRIDE;

};

#if wxUSE_ACCESSIBILITY

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

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
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
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(AccessTest_Quit,  MyFrame::OnQuit)
    EVT_MENU(AccessTest_Query,  MyFrame::OnQuery)
    EVT_MENU(AccessTest_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

#endif // wxUSE_ACCESSIBILITY

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_ACCESSIBILITY
    // Note: JAWS for Windows will only speak the context-sensitive
    // help if you use this help provider:
    // wxHelpProvider::Set(new wxHelpControllerHelpProvider(m_helpController)).
    // JAWS does not seem to be getting the help text from
    // the wxAccessible object.
    wxHelpProvider::Set(new wxSimpleHelpProvider());

    // create the main application window
    MyFrame *frame = new MyFrame("AccessTest wxWidgets App",
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
#else
    wxMessageBox( "This sample has to be compiled with wxUSE_ACCESSIBILITY", "Building error", wxOK);
    return false;
#endif // wxUSE_ACCESSIBILITY
}

#if wxUSE_ACCESSIBILITY

class FrameAccessible: public wxWindowAccessible
{
public:
    FrameAccessible(wxWindow* win): wxWindowAccessible(win) {}

            // Gets the name of the specified object.
    virtual wxAccStatus GetName(int childId, wxString* name) wxOVERRIDE
    {
        if (childId == wxACC_SELF)
        {
            * name = "Julian's Frame";
            return wxACC_OK;
        }
        else
            return wxACC_NOT_IMPLEMENTED;
    }
};

class ScrolledWindowAccessible: public wxWindowAccessible
{
public:
    ScrolledWindowAccessible(wxWindow* win): wxWindowAccessible(win) {}

            // Gets the name of the specified object.
    virtual wxAccStatus GetName(int childId, wxString* name) wxOVERRIDE
    {
        if (childId == wxACC_SELF)
        {
            * name = "My scrolled window";
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
    virtual wxAccStatus GetName(int childId, wxString* name) wxOVERRIDE;

        // Can return either a child object, or an integer
        // representing the child element, starting from 1.
    virtual wxAccStatus HitTest(const wxPoint& pt, int* childId, wxAccessible** childObject) wxOVERRIDE;

        // Returns the rectangle for this object (id = 0) or a child element (id > 0).
    virtual wxAccStatus GetLocation(wxRect& rect, int elementId) wxOVERRIDE;

        // Navigates from fromId to toId/toObject.
    virtual wxAccStatus Navigate(wxNavDir navDir, int fromId,
                int* toId, wxAccessible** toObject) wxOVERRIDE;

        // Gets the number of children.
    virtual wxAccStatus GetChildCount(int* childCount) wxOVERRIDE;

        // Gets the specified child (starting from 1).
        // If *child is NULL and return value is wxACC_OK,
        // this means that the child is a simple element and
        // not an accessible object.
    virtual wxAccStatus GetChild(int childId, wxAccessible** child) wxOVERRIDE;

        // Gets the parent, or NULL.
    virtual wxAccStatus GetParent(wxAccessible** parent) wxOVERRIDE;

        // Performs the default action. childId is 0 (the action for this object)
        // or > 0 (the action for a child).
        // Return wxACC_NOT_SUPPORTED if there is no default action for this
        // window (e.g. an edit control).
    virtual wxAccStatus DoDefaultAction(int childId) wxOVERRIDE;

        // Gets the default action for this object (0) or > 0 (the action for a child).
        // Return wxACC_OK even if there is no action. actionName is the action, or the empty
        // string if there is no action.
        // The retrieved string describes the action that is performed on an object,
        // not what the object does as a result. For example, a toolbar button that prints
        // a document has a default action of "Press" rather than "Prints the current document."
    virtual wxAccStatus GetDefaultAction(int childId, wxString* actionName) wxOVERRIDE;

        // Returns the description for this object or a child.
    virtual wxAccStatus GetDescription(int childId, wxString* description) wxOVERRIDE;

        // Returns help text for this object or a child, similar to tooltip text.
    virtual wxAccStatus GetHelpText(int childId, wxString* helpText) wxOVERRIDE;

        // Returns the keyboard shortcut for this object or child.
        // Return e.g. ALT+K
    virtual wxAccStatus GetKeyboardShortcut(int childId, wxString* shortcut) wxOVERRIDE;

        // Returns a role constant.
    virtual wxAccStatus GetRole(int childId, wxAccRole* role) wxOVERRIDE;

        // Returns a state constant.
    virtual wxAccStatus GetState(int childId, long* state) wxOVERRIDE;

        // Returns a localized string representing the value for the object
        // or child.
    virtual wxAccStatus GetValue(int childId, wxString* strValue) wxOVERRIDE;

        // Selects the object or child.
    virtual wxAccStatus Select(int childId, wxAccSelectionFlags selectFlags) wxOVERRIDE;

        // Gets the window with the keyboard focus.
        // If childId is 0 and child is NULL, no object in
        // this subhierarchy has the focus.
        // If this object has the focus, child should be 'this'.
    virtual wxAccStatus GetFocus(int* childId, wxAccessible** child) wxOVERRIDE;

        // Gets a variant representing the selected children
        // of this object.
        // Acceptable values:
        // - a null variant (IsNull() returns true)
        // - a list variant (GetType() == "list")
        // - an integer representing the selected child element,
        //   or 0 if this object is selected (GetType() == "long")
        // - a "void*" pointer to a wxAccessible child object
    virtual wxAccStatus GetSelections(wxVariant* selections) wxOVERRIDE;

};

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, wxID_ANY, title, pos, size, style)
{
    m_textCtrl = NULL;

    SetAccessible(new FrameAccessible(this));

    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(AccessTest_About, "&About", "Show about dialog");

    menuFile->Append(AccessTest_Query, "Query", "Query the window hierarchy");
    menuFile->AppendSeparator();
    menuFile->Append(AccessTest_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if 0 // wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR


    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);
    splitter->SetAccessible(new SplitterWindowAccessible(splitter));

    wxListBox* listBox = new wxListBox(splitter, wxID_ANY);
    listBox->Append("Cabbages");
    listBox->Append("Kings");
    listBox->Append("Sealing wax");
    listBox->Append("Strings");
    listBox->CreateAccessible();
    listBox->SetHelpText("This is a sample wxWidgets listbox, with a number of items in it.");

    m_textCtrl = new wxTextCtrl(splitter, wxID_ANY, "", wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    m_textCtrl->CreateAccessible();
    m_textCtrl->SetHelpText("This is a sample wxWidgets multiline text control.");

    splitter->SplitHorizontally(listBox, m_textCtrl, 150);

#if 0
    wxScrolledWindow* scrolledWindow = new wxScrolledWindow(this, wxID_ANY);
    scrolledWindow->SetAccessible(new ScrolledWindowAccessible(scrolledWindow));
#endif
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( "This is the About dialog of the AccessTest sample.\n"
                "Welcome to %s", wxVERSION_STRING);

    wxMessageBox(msg, "About AccessTest", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnQuery(wxCommandEvent& WXUNUSED(event))
{
    m_textCtrl->Clear();
    IAccessible* accessibleFrame = NULL;
    if (S_OK != AccessibleObjectFromWindow((HWND) GetHWND(), (DWORD)OBJID_CLIENT,
        IID_IAccessible, (void**) & accessibleFrame))
    {
        Log("Could not get object.");
        return;
    }
    if (accessibleFrame)
    {
        //Log("Got an IAccessible for the frame.");
        LogObject(0, accessibleFrame);
        Log("Checking children using AccessibleChildren()...");

        // Now check the AccessibleChildren function works OK
        long childCount = 0;
        if (S_OK != accessibleFrame->get_accChildCount(& childCount))
        {
            Log("Could not get number of children.");
            accessibleFrame->Release();
            return;
        }
        else if (childCount == 0)
        {
            Log("No children.");
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
                        str.Printf("Found child %s/%s", name, role);
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
            Log("AccessibleChildren failed.");
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
        text2.Replace("\n", " ");
        text2.Replace("\r", " ");
        m_textCtrl->SetInsertionPointEnd();
        m_textCtrl->WriteText(text2 + "\n");
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
        str.Printf("Name = %s; Role = %s", name, role);
        str.Pad(indent, ' ', false);
        Log(str);
    }

    long childCount = 0;
    if (S_OK == obj->get_accChildCount(& childCount))
    {
        wxString str;
        str.Printf("There are %d children.", (int) childCount);
        str.Pad(indent, ' ', false);
        Log(str);
        Log("");
    }

    int i;
    for (i = 1; i <= childCount; i++)
    {
        GetInfo(obj, i, name, role);

        wxString str;
        str.Printf("%d) Name = %s; Role = %s", i, name, role);
        str.Pad(indent, ' ', false);
        Log(str);

        VARIANT var;
        VariantInit(& var);
        var.vt = VT_I4;
        var.lVal = i;
        IDispatch* pDisp = NULL;
        IAccessible* childObject = NULL;

        if (S_OK == obj->get_accChild(var, & pDisp) && pDisp)
        {
            str.Printf("This is a real object.");
            str.Pad(indent+4, ' ', false);
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
            str.Printf("This is an element.");
            str.Pad(indent+4, ' ', false);
            Log(str);
        }
        // Log("");
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
        name = "NO NAME";
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
        role = "NO ROLE";
    }
}

/*
 * SplitterWindowAccessible implementation
 */

// Gets the name of the specified object.
wxAccStatus SplitterWindowAccessible::GetName(int childId, wxString* name)
{
    if (childId == wxACC_SELF)
    {
        * name = "Splitter window";
        return wxACC_OK;
    }
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter)
    {
        if (splitter->IsSplit())
        {
            // Two windows, and the sash.
            if (childId == 1 || childId == 3)
                return wxACC_NOT_IMPLEMENTED;
            else if (childId == 2)
            {
                *name = "Sash";
                return wxACC_OK;
            }
        }
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Can return either a child object, or an integer
// representing the child element, starting from 1.
wxAccStatus SplitterWindowAccessible::HitTest(const wxPoint& pt, int* childId, wxAccessible** WXUNUSED(childObject))
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter)
    {
        if (splitter->IsSplit())
        {
            wxPoint clientPt = splitter->ScreenToClient(pt);
            if (splitter->SashHitTest(clientPt.x, clientPt.y))
            {
                // We're over the sash
                *childId = 2;
                return wxACC_OK;
            }
        }
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Returns the rectangle for this object (id = 0) or a child element (id > 0).
wxAccStatus SplitterWindowAccessible::GetLocation(wxRect& rect, int elementId)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter && elementId == 2 && splitter->IsSplit())
    {
        wxSize clientSize = splitter->GetClientSize();
        if (splitter->GetSplitMode() == wxSPLIT_VERTICAL)
        {
            rect.x = splitter->GetSashPosition();
            rect.y = 0;
            rect.SetPosition(splitter->ClientToScreen(rect.GetPosition()));
            rect.width = splitter->GetSashSize();
            rect.height = clientSize.y;
        }
        else
        {
            rect.x = 0;
            rect.y = splitter->GetSashPosition();
            rect.SetPosition(splitter->ClientToScreen(rect.GetPosition()));
            rect.width = clientSize.x;
            rect.height = splitter->GetSashSize();
        }
        return wxACC_OK;
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Navigates from fromId to toId/toObject.
wxAccStatus SplitterWindowAccessible::Navigate(wxNavDir navDir, int fromId,
                int* toId, wxAccessible** toObject)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter && splitter->IsSplit())
    {
        switch (navDir)
        {
        case wxNAVDIR_DOWN:
            {
                if (splitter->GetSplitMode() != wxSPLIT_VERTICAL)
                {
                    if (fromId == 1)
                    {
                        *toId = 2;
                        *toObject = NULL;
                        return wxACC_OK;
                    }
                    else if (fromId == 2)
                    {
                        *toId = 3;
                        *toObject = splitter->GetWindow2()->GetAccessible();
                        return wxACC_OK;
                    }
                }
                return wxACC_FALSE;
                #if 0
                // below line is not executed due to earlier return
                break;
                #endif
            }
        case wxNAVDIR_FIRSTCHILD:
            {
                if (fromId == 2)
                    return wxACC_FALSE;
            }
            break;

        case wxNAVDIR_LASTCHILD:
            {
                if (fromId == 2)
                    return wxACC_FALSE;
            }
            break;

        case wxNAVDIR_LEFT:
            {
                if (splitter->GetSplitMode() != wxSPLIT_HORIZONTAL)
                {
                    if (fromId == 3)
                    {
                        *toId = 2;
                        *toObject = NULL;
                        return wxACC_OK;
                    }
                    else if (fromId == 2)
                    {
                        *toId = 1;
                        *toObject = splitter->GetWindow1()->GetAccessible();
                        return wxACC_OK;
                    }
                }
                return wxACC_FALSE;
            }
            #if 0
            // below line is not executed due to earlier return
            break;
            #endif

        case wxNAVDIR_NEXT:
            {
                if (fromId == 1)
                {
                    *toId = 2;
                    *toObject = NULL;
                    return wxACC_OK;
                }
                else if (fromId == 2)
                {
                    *toId = 3;
                    *toObject = splitter->GetWindow2()->GetAccessible();
                    return wxACC_OK;
                }
                    return wxACC_FALSE;
            }
            #if 0
            // below line is not executed due to earlier return
            break;
            #endif

        case wxNAVDIR_PREVIOUS:
            {
                if (fromId == 3)
                {
                    *toId = 2;
                    *toObject = NULL;
                    return wxACC_OK;
                }
                else if (fromId == 2)
                {
                    *toId = 1;
                    *toObject = splitter->GetWindow1()->GetAccessible();
                    return wxACC_OK;
                }
                    return wxACC_FALSE;
            }
            #if 0
            // below line is not executed due to earlier return
            break;
            #endif

        case wxNAVDIR_RIGHT:
            {
                if (splitter->GetSplitMode() != wxSPLIT_HORIZONTAL)
                {
                    if (fromId == 1)
                    {
                        *toId = 2;
                        *toObject = NULL;
                        return wxACC_OK;
                    }
                    else if (fromId == 2)
                    {
                        *toId = 3;
                        *toObject = splitter->GetWindow2()->GetAccessible();
                        return wxACC_OK;
                    }
                }
                // Can't go right spatially if split horizontally.
                return wxACC_FALSE;
            }
            #if 0
            // below line is not executed due to earlier return
            break;
            #endif

        case wxNAVDIR_UP:
            {
                if (splitter->GetSplitMode() != wxSPLIT_VERTICAL)
                {
                    if (fromId == 3)
                    {
                        *toId = 2;
                        return wxACC_OK;
                    }
                    else if (fromId == 2)
                    {
                        *toId = 1;
                        *toObject = splitter->GetWindow1()->GetAccessible();
                        return wxACC_OK;
                    }
                }

                // Can't go up spatially if split vertically.
                return wxACC_FALSE;
                #if 0
                // below line is not executed due to earlier return
                break;
                #endif
            }
        }

    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Gets the number of children.
wxAccStatus SplitterWindowAccessible::GetChildCount(int* childCount)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter)
    {
        if (splitter->IsSplit())
        {
            // Two windows, and the sash.
            *childCount = 3;
            return wxACC_OK;
        }
        else
        {
            // No sash -- 1 or 0 windows.
            if (splitter->GetWindow1() || splitter->GetWindow2())
            {
                *childCount = 1;
                return wxACC_OK;
            }
            else
            {
                *childCount = 0;
                return wxACC_OK;
            }
        }
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Gets the specified child (starting from 1).
// If *child is NULL and return value is wxACC_OK,
// this means that the child is a simple element and
// not an accessible object.
wxAccStatus SplitterWindowAccessible::GetChild(int childId, wxAccessible** child)
{
    if (childId == wxACC_SELF)
    {
        *child = this;
        return wxACC_OK;
    }

    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter)
    {
        if (splitter->IsSplit())
        {
            // Two windows, and the sash.
            if (childId == 1)
            {
                *child = splitter->GetWindow1()->GetAccessible();
            }
            else if (childId == 2)
            {
                *child = NULL; // Sash
            }
            else if (childId == 3)
            {
                *child = splitter->GetWindow2()->GetAccessible();
            }
            else
            {
                return wxACC_FAIL;
            }
            return wxACC_OK;
        }
        else
        {
            // No sash -- 1 or 0 windows.
            if (childId == 1)
            {
                if (splitter->GetWindow1())
                {
                    *child = splitter->GetWindow1()->GetAccessible();
                    return wxACC_OK;
                }
                else if (splitter->GetWindow2())
                {
                    *child = splitter->GetWindow2()->GetAccessible();
                    return wxACC_OK;
                }
                else
                {
                    return wxACC_FAIL;
                }
            }
            else
                return wxACC_FAIL;
        }
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Gets the parent, or NULL.
wxAccStatus SplitterWindowAccessible::GetParent(wxAccessible** WXUNUSED(parent))
{
    return wxACC_NOT_IMPLEMENTED;
}

// Performs the default action. childId is 0 (the action for this object)
// or > 0 (the action for a child).
// Return wxACC_NOT_SUPPORTED if there is no default action for this
// window (e.g. an edit control).
wxAccStatus SplitterWindowAccessible::DoDefaultAction(int WXUNUSED(childId))
{
    return wxACC_NOT_IMPLEMENTED;
}

// Gets the default action for this object (0) or > 0 (the action for a child).
// Return wxACC_OK even if there is no action. actionName is the action, or the empty
// string if there is no action.
// The retrieved string describes the action that is performed on an object,
// not what the object does as a result. For example, a toolbar button that prints
// a document has a default action of "Press" rather than "Prints the current document."
wxAccStatus SplitterWindowAccessible::GetDefaultAction(int childId, wxString* WXUNUSED(actionName))
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter && splitter->IsSplit() && childId == 2)
    {
        // No default action for the splitter.
        return wxACC_FALSE;
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Returns the description for this object or a child.
wxAccStatus SplitterWindowAccessible::GetDescription(int childId, wxString* description)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter)
    {
        if (splitter->IsSplit())
        {
            if (childId == 2)
            {
                * description = _("The splitter window sash.");
                return wxACC_OK;
            }
        }
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Returns help text for this object or a child, similar to tooltip text.
wxAccStatus SplitterWindowAccessible::GetHelpText(int childId, wxString* helpText)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter)
    {
        if (splitter->IsSplit())
        {
            if (childId == 2)
            {
                * helpText = _("The splitter window sash.");
                return wxACC_OK;
            }
        }
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Returns the keyboard shortcut for this object or child.
// Return e.g. ALT+K
wxAccStatus SplitterWindowAccessible::GetKeyboardShortcut(int childId, wxString* WXUNUSED(shortcut))
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter && splitter->IsSplit() && childId == 2)
    {
        // No keyboard shortcut for the splitter.
        return wxACC_FALSE;
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Returns a role constant.
wxAccStatus SplitterWindowAccessible::GetRole(int childId, wxAccRole* role)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter)
    {
        if (splitter->IsSplit())
        {
            if (childId == 2)
            {
                * role = wxROLE_SYSTEM_GRIP;
                return wxACC_OK;
            }
        }
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Returns a state constant.
wxAccStatus SplitterWindowAccessible::GetState(int childId, long* state)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter && splitter->IsSplit() && childId == 2)
    {
        // No particular state. Not sure what would be appropriate here.
        *state = wxACC_STATE_SYSTEM_UNAVAILABLE;
        return wxACC_OK;
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Returns a localized string representing the value for the object
// or child.
wxAccStatus SplitterWindowAccessible::GetValue(int childId, wxString* strValue)
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter && splitter->IsSplit() && childId == 2)
    {
        // The sash position is the value.
        wxString pos;
        pos << splitter->GetSashPosition();
        *strValue = pos;

        return wxACC_OK;
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Selects the object or child.
wxAccStatus SplitterWindowAccessible::Select(int childId, wxAccSelectionFlags WXUNUSED(selectFlags))
{
    wxSplitterWindow* splitter = wxDynamicCast(GetWindow(), wxSplitterWindow);
    if (splitter && splitter->IsSplit() && childId == 2)
    {
        // Can't select the sash.
        return wxACC_FALSE;
    }
    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Gets the window with the keyboard focus.
// If childId is 0 and child is NULL, no object in
// this subhierarchy has the focus.
// If this object has the focus, child should be 'this'.
wxAccStatus SplitterWindowAccessible::GetFocus(int* WXUNUSED(childId), wxAccessible** WXUNUSED(child))
{
    return wxACC_NOT_IMPLEMENTED;
}

// Gets a variant representing the selected children
// of this object.
// Acceptable values:
// - a null variant (IsNull() returns true)
// - a list variant (GetType() == "list")
// - an integer representing the selected child element,
//   or 0 if this object is selected (GetType() == "long")
// - a "void*" pointer to a wxAccessible child object
wxAccStatus SplitterWindowAccessible::GetSelections(wxVariant* WXUNUSED(selections))
{
    return wxACC_NOT_IMPLEMENTED;
}

#endif // wxUSE_ACCESSIBILITY
