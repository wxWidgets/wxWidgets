/*
                wxActiveX Library Licence, Version 3
                ====================================

  Copyright (C) 2003 Lindsay Mathieson [, ...]

  Everyone is permitted to copy and distribute verbatim copies
  of this licence document, but changing it is not allowed.

                       wxActiveX LIBRARY LICENCE
     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
  
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public Licence as published by
  the Free Software Foundation; either version 2 of the Licence, or (at
  your option) any later version.
  
  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
  General Public Licence for more details.

  You should have received a copy of the GNU Library General Public Licence
  along with this software, usually in a file named COPYING.LIB.  If not,
  write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA 02111-1307 USA.

  EXCEPTION NOTICE

  1. As a special exception, the copyright holders of this library give
  permission for additional uses of the text contained in this release of
  the library as licenced under the wxActiveX Library Licence, applying
  either version 3 of the Licence, or (at your option) any later version of
  the Licence as published by the copyright holders of version 3 of the
  Licence document.

  2. The exception is that you may use, copy, link, modify and distribute
  under the user's own terms, binary object code versions of works based
  on the Library.

  3. If you copy code from files distributed under the terms of the GNU
  General Public Licence or the GNU Library General Public Licence into a
  copy of this library, as this licence permits, the exception does not
  apply to the code that you add in this way.  To avoid misleading anyone as
  to the status of such modified files, you must delete this exception
  notice from such code and/or adjust the licensing conditions notice
  accordingly.

  4. If you write modifications of your own for this library, it is your
  choice whether to permit this exception to apply to your modifications. 
  If you do not wish that, you must delete the exception notice from such
  code and/or adjust the licensing conditions notice accordingly.
*/

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#if defined(__WXGTK__) || defined(__WXMOTIF__)
        #include "wx/wx.h"
#endif
#include "wx/wxprec.h"
#include "wx/filedlg.h"
#include "wx/textdlg.h"
#include "wxActiveXFrame.h"
#include <sstream>
using namespace std;
#include "wx/splitter.h"
#include "wx/textctrl.h"
#include "wx/clipbrd.h"
#include <wx/msgdlg.h>

enum
{
    // menu items
    MENU_GETTYPEINFO = 700,
	MENU_INVOKEMETHOD,
	MENU_TEST
};

BEGIN_EVENT_TABLE(wxActiveXFrame, wxFrame)
    EVT_MENU(MENU_GETTYPEINFO, wxActiveXFrame::OnGetTypeInfo)
	EVT_MENU(MENU_INVOKEMETHOD, wxActiveXFrame::OnInvokeMethod)
	EVT_MENU(MENU_TEST, wxActiveXFrame::OnTest)
END_EVENT_TABLE()


wxActiveXFrame::wxActiveXFrame(wxWindow *parent, wxString title) :
    wxFrame(parent, -1, title) 
{
    // create a menu bar
    wxMenu *xMenu = new wxMenu("", wxMENU_TEAROFF);

	xMenu->Append(MENU_GETTYPEINFO, "Get Type Info", "");
	xMenu->Append(MENU_INVOKEMETHOD, "Invoke Method (no params)", "");
	xMenu->Append(MENU_TEST, "Test", "For debugging purposes");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(xMenu, "&ActiveX");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    wxSplitterWindow *sp = new wxSplitterWindow(this);
    X = new wxActiveX(sp, title, 101);

    textLog = new wxTextCtrl(sp, -1, "", wxPoint(0,0),  wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

    sp->SplitHorizontally(X, textLog, 0);

    // conenct all events
    for (int i = 0; i < X->GetEventCount(); i++)
    {
        const wxActiveX::FuncX& func = X->GetEventDesc(i);
        const wxEventType& ev = RegisterActiveXEvent((DISPID) func.memid);
        Connect(101, ev, (wxObjectEventFunction) OnActiveXEvent);
    };
}

wxString VarTypeAsString(VARTYPE vt)
{
#define VT(vtype, desc) case vtype : return desc

    if (vt & VT_BYREF)
        vt -= VT_BYREF;

    if (vt & VT_ARRAY)
        vt -= VT_ARRAY;

    switch (vt)
    {
    VT(VT_SAFEARRAY, "SafeArray");
    VT(VT_EMPTY, "empty");
    VT(VT_NULL, "null");
    VT(VT_UI1, "byte");
    VT(VT_I1, "char");
    VT(VT_I2, "short");
    VT(VT_I4, "long");
    VT(VT_UI2, "unsigned short");
    VT(VT_UI4, "unsigned long");
    VT(VT_INT, "int");
    VT(VT_UINT, "unsigned int");
    VT(VT_R4, "real(4)");
    VT(VT_R8, "real(8)");
    VT(VT_CY, "Currency");
    VT(VT_DATE, "wxDate");
    VT(VT_BSTR, "wxString");
    VT(VT_DISPATCH, "IDispatch");
    VT(VT_ERROR, "SCode Error");
    VT(VT_BOOL, "bool");
    VT(VT_VARIANT, "wxVariant");
    VT(VT_UNKNOWN, "IUknown");
    VT(VT_VOID, "void");
    VT(VT_PTR, "void *");
    VT(VT_USERDEFINED, "*user defined*");

    default:
        {
            wxString s;
            s << "Unknown(" << vt << ")";
            return s;
        };
    };

#undef VT
};

#define ENDL "\r\n"


void OutFunc(wxString& os, const wxActiveX::FuncX& func)
{
    os << VarTypeAsString(func.retType.vt) << " " << func.name << "(";
    for (unsigned int p = 0; p < func.params.size(); p++)
    {
        const wxActiveX::ParamX& param = func.params[p];
        if (param.IsIn() && param.IsOut())
            os << "[IN OUT] ";
        else if (param.IsIn())
            os << "[IN] ";
        else if (param.IsIn())
            os << "[OUT] ";
        os << VarTypeAsString(param.vt) << " " << (param.isPtr ? "*" : "") << param.name;
        if (p < func.params.size() - 1)
            os << ", ";
    };
    os << ")" << ENDL;
};

void wxActiveXFrame::OnGetTypeInfo(wxCommandEvent& event)
{
    wxString os;

	int i =0;
    os << 
        "Props" << ENDL <<
        "=====" << ENDL;
    for (i = 0; i < X->GetPropCount(); i++)
    {
        wxActiveX::PropX prop = X->GetPropDesc(i);
        os << VarTypeAsString(prop.type.vt) << " " << prop.name << "(";
        if (prop.CanSet())
        {
            os << VarTypeAsString(prop.arg.vt);
        };
        os << ")" << ENDL;

    };
	os << ENDL;

    os << 
        "Events" << ENDL <<
        "======" << ENDL;
    for (i = 0; i < X->GetEventCount(); i++)
		OutFunc(os, X->GetEventDesc(i));
	os << ENDL;

    os << 
        "Methods" << ENDL <<
        "=======" << ENDL;
    for (i = 0; i < X->GetMethodCount(); i++)
		OutFunc(os, X->GetMethodDesc(i));
	os << ENDL;


	if (wxTheClipboard->Open())
	{
		wxDataObjectSimple *wo = new wxTextDataObject(os);
		wxTheClipboard->SetData(wo);
        wxTheClipboard->Flush();
		wxTheClipboard->Close();
	};

    wxMessageBox(os, "Type Info", wxOK, this);
};

void wxActiveXFrame::OnInvokeMethod(wxCommandEvent& event)
{
	//wxTextEntryDialog dlg(this, "Method");
	//if (dlg.ShowModal() == wxID_OK)
	//	X->CallMethod(dlg.GetValue());
};

void wxActiveXFrame::OnTest(wxCommandEvent& event)
{
	// flash testing
    wxVariant args[] = {0L, "http://www.macromedia.com/support/flash/ts/documents/java_script_comm/flash_to_javascript.swf"};
    X->CallMethod("LoadMovie", args);   
	//X->Prop("Movie") = "http://www.macromedia.com/support/flash/ts/documents/java_script_comm/flash_to_javascript.swf";

	// mc cal testing
    //X->Prop("year") = 1964L;
    //X->Prop("Value") = wxDateTime::Now();

	// pdf testing
	//wxVariant file = "C:\\WINNT\\wx2\\docs\\pdf\\dialoged.pdf";
	//X->CallMethod("LoadFile", &file);
};

void wxActiveXFrame::OnActiveXEvent(wxActiveXEvent& event)
{
#ifdef UNICODE
    wostringstream os;
#else
    ostringstream os;
#endif

    os << (const wxChar *) event.EventName() << wxT("(");

    for (int p = 0; p < event.ParamCount(); p++)
    {
        os << 
            (const wxChar *) event.ParamType(p) << wxT(" ") << 
            (const wxChar *) event.ParamName(p) << wxT(" = ") <<
            (const wxChar *) (wxString) event[p];
        if (p < event.ParamCount() - 1)
            os << wxT(", ");
    };
    os << wxT(")") << endl;
    wxString data = os.str().c_str();
    textLog->AppendText(data);
};

