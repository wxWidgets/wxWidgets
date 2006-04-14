// wxr2xml.cpp: implementation of the wxr2xml class.
// 8/30/00  Brian Gavin
// only tested on wxMSW so far
// License: wxWindows Liscense
// ////////////////////////////////////////////////////////////////////

/*
How to use class:
#include "wxr2xml.h"
...
wxr2xml trans;
trans->Convert("Myfile.wxr","Myfile.xml");
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxr2xml.h"

// ////////////////////////////////////////////////////////////////////
// Construction/Destruction
// ////////////////////////////////////////////////////////////////////

wxr2xml::wxr2xml()
{
}

wxr2xml::~wxr2xml()
{
}

bool wxr2xml::Convert(wxString wxrfile, wxString xmlfile)
{
    bool result;
    result = m_xmlfile.Open(xmlfile.c_str(), _T("w+t"));
    wxASSERT_MSG(result, _T("Couldn't create XML file"));
    if (!result)
        return false;

    result = m_table.ParseResourceFile(wxrfile);
    wxASSERT_MSG(result, _T("Couldn't Load WXR file"));
    if (!result)
        return false;
    // Write basic xml header
    m_xmlfile.Write(_T("<?xml version=\"1.0\" ?>\n"));
    m_xmlfile.Write(_T("<resource>\n"));
    result = ParseResources();
    m_xmlfile.Write(_T("</resource>\n"));

    m_xmlfile.Close();

    return result;
}

bool wxr2xml::ParseResources()
{
    m_table.BeginFind();
    wxHashTable::Node *node;

    node = m_table.Next();
    while (node)
        {
        wxItemResource *res = (wxItemResource *) node->GetData();
        wxString resType(res->GetType());
        if (resType == _T("wxDialog"))
            ParseDialog(res);
        else if (resType == _T("wxPanel"))
            ParsePanel(res);
        else if (resType == _T("wxPanel"))
            ParsePanel(res);
        else if (resType == _T("wxMenu"))
            ParseMenuBar(res);
        else if (resType == _T("wxBitmap"))
            ParseBitmap(res);
        else
            wxLogError(_T("Found unsupported resource ") + resType);
        node = m_table.Next();
    }
    return true;
}

void wxr2xml::ParsePanel(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t<object class=\"wxPanel\""));
    PanelStuff(res);
    WriteControlInfo(res);
    m_xmlfile.Write(_T("\n"));
    ParseControls(res);
    m_xmlfile.Write(_T("\t</object>\n\n"));
}

void wxr2xml::ParseDialog(wxItemResource * res)
{
    PanelStuff(res);
    m_xmlfile.Write(_T("\t<object class=\"wxDialog\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetTitle(res));

    m_xmlfile.Write(_T("\n"));
    ParseControls(res);
    m_xmlfile.Write(_T("\t</object>\n\n"));
}

void wxr2xml::ParseControls(wxItemResource * res)
{
    wxNode *node = res->GetChildren().GetFirst();
    while (node)
        {
        wxItemResource *res = (wxItemResource *) node->GetData();
        wxString resType(res->GetType());
        if (resType == _T("wxButton"))
            ParseButton(res);
        else if ((resType == _T("wxTextCtrl")) | (resType == _T("wxText"))
         | (resType == _T("wxMultiText")))
            ParseTextCtrl(res);
        else if (resType == _T("wxCheckBox"))
            ParseCheckBox(res);
        else if (resType == _T("wxRadioBox"))
            ParseRadioBox(res);
        else if (resType == _T("wxListBox"))
            ParseListBox(res);
        else if ((resType == _T("wxStaticText")) | (resType == _T("wxMessage")))
            ParseStaticText(res);
        else if (resType == _T("wxChoice"))
            ParseChoice(res);
        else if (resType == _T("wxGauge"))
           ParseGauge(res);
        else if (resType == _T("wxSlider"))
            ParseSlider(res);
        else if (resType == _T("wxComboBox"))
            ParseComboBox(res);
        else if (resType == _T("wxRadioButton"))
            ParseRadioButton(res);
        else if (resType == _T("wxStaticBitmap"))
            ParseStaticBitmap(res);
        else if (resType == _T("wxScrollBar"))
            ParseScrollBar(res);
        else if ((resType == _T("wxStaticBox")) | (resType == _T("wxGroupBox")))
            ParseStaticBox(res);
        else if (resType == _T("wxBitmapButton"))
            ParseBitmapButton(res);
        else
            wxLogError(_T("Found unsupported resource ") + resType);
        node = node->GetNext();
        }
}

// Write out basic stuff every control has
// name,position,size,bg,fg
void wxr2xml::WriteControlInfo(wxItemResource * res)
{
    m_xmlfile.Write(GenerateName(res));
    m_xmlfile.Write(_T(">\n"));
    m_xmlfile.Write(GetPosition(res));
    m_xmlfile.Write(GetSize(res));
    m_xmlfile.Write(GetStyles(res));
    WriteFontInfo(res);
}

wxString wxr2xml::GetSize(wxItemResource * res)
{
    wxString msg;
    if (m_dlgunits)
        msg << _T("\t\t\t\t<size>") << res->GetWidth() << _T(",") << res->GetHeight() << _T("d</size>\n");
    else
        msg << _T("\t\t\t\t<size>") << res->GetWidth() << _T(",") << res->GetHeight() << _T("</size>\n");
    return msg;
}

wxString wxr2xml::GetPosition(wxItemResource * res)
{
    wxString msg;
    if (m_dlgunits)
        msg << _T("\t\t\t\t<pos>") << res->GetX() << _T(",") << res->GetY() << _T("d</pos>\n");
    else
        msg << _T("\t\t\t\t<pos>") << res->GetX() << _T(",") << res->GetY() << _T("</pos>\n");
    return msg;
}

void wxr2xml::ParseButton(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxButton\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::ParseTextCtrl(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxTextCtrl\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue4(res));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));

}

wxString wxr2xml::GetTitle(wxItemResource * res)
{
    wxString msg;
    msg = _T("\t\t\t\t<title>") + res->GetTitle() + _T("</title>");
    return msg;
}

wxString wxr2xml::GetValue4(wxItemResource * res)
{
    wxString msg;
    msg = _T("\t\t\t\t<value>") + res->GetValue4() + _T("</value>\n");
    return msg;
}

void wxr2xml::ParseCheckBox(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxCheckBox\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write(GetCheckStatus(res));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

wxString wxr2xml::GetLabel(wxItemResource * res)
{
    return _T("\t\t\t\t<label>") + res->GetTitle() + _T("</label>\n");
}

void wxr2xml::ParseRadioBox(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxRadioBox\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    // Add radio box items
    WriteStringList(res);
    // Value1
    m_xmlfile.Write(GetDimension(res));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::ParseListBox(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxListBox\""));
    WriteControlInfo(res);
    WriteStringList(res);
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::ParseStaticText(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxStaticText\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::ParseStaticBox(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxStaticBox\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::WriteStringList(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t\t<content>\n"));
    for (wxStringListNode * node = res->GetStringValues().GetFirst();
        node;node = node->GetNext()) {
        const wxString s1 = node->GetData();
        m_xmlfile.Write(_T("\t\t\t\t\t<item>") + s1 + _T("</item>\n"));
    }
    m_xmlfile.Write(_T("\t\t\t\t</content>\n"));
}

void wxr2xml::ParseChoice(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxChoice\""));
    WriteControlInfo(res);
    // Add choice items
    WriteStringList(res);
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::ParseGauge(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxGauge\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write(GetRange(res));
    m_xmlfile.Write(_T("\n\t\t\t</object>\n"));
}

wxString wxr2xml::GetValue1(wxItemResource * res)
{
    wxString msg;
    msg << _T("\t\t\t\t<value>") << res->GetValue1() << _T("</value>\n");
    return msg;
}

wxString wxr2xml::GetRange(wxItemResource * res)
{
    wxString msg;
    msg << _T("\t\t\t\t<range>") << res->GetValue2() << _T("</range>");
    return msg;
}

void wxr2xml::ParseSlider(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxSlider\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write(GetMax(res));
    m_xmlfile.Write(GetMin(res));
    m_xmlfile.Write(_T("\n\t\t\t</object>\n"));
}

wxString wxr2xml::GetMax(wxItemResource * res)
{
    wxString msg;
    msg << _T("\t\t\t\t<max>") << res->GetValue3() << _T("</max>\n");
    return msg;
}

wxString wxr2xml::GetMin(wxItemResource * res)
{
    wxString msg;
    msg << _T("\t\t\t\t<min>") << res->GetValue2() << _T("</min>");
    return msg;
}

void wxr2xml::ParseComboBox(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxComboBox\""));
    WriteControlInfo(res);
    // Add combo items
    WriteStringList(res);
    m_xmlfile.Write(_T("\n\t\t\t</object>\n"));
}

void wxr2xml::ParseRadioButton(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxRadioButton\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));

    wxString msg;
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write(GetCheckStatus(res));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::ParseScrollBar(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxScrollBar\""));
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write(_T("\t\t\t\t<thumbsize>")+GetValue2(res)+_T("</thumbsize>\n"));
    m_xmlfile.Write(_T("\t\t\t\t<range>")+GetValue3(res)+_T("</range>\n"));
    m_xmlfile.Write(_T("\t\t\t\t<pagesize>")+GetValue5(res)+_T("</pagesize>\n"));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

wxString wxr2xml::GetCheckStatus(wxItemResource * res)
{
    wxString msg;
    msg << _T("\t\t\t\t<checked>") << res->GetValue1() << _T("</checked>\n");
    return msg;
}

wxString wxr2xml::GetStyles(wxItemResource * res)
{
    // Very crude way to get styles
    long style;
    wxString s, restype;
    restype = res->GetType();
    style = res->GetStyle();

    s = _T("\t\t\t\t<style>");

    // Common styles for all controls
    if (style & wxSIMPLE_BORDER)
        s += _T("wxSIMPLE_BORDER|");
    if (style & wxSUNKEN_BORDER)
        s += _T("wxSUNKEN_BORDER|");
    if (style & wxSIMPLE_BORDER)
        s += _T("wxSIMPLE_BORDER|");
    if (style & wxDOUBLE_BORDER)
        s += _T("wxDOUBLE_BORDER|");
    if (style & wxRAISED_BORDER)
        s += _T("wxRAISED_BORDER|");
    if (style & wxTRANSPARENT_WINDOW)
        s += _T("wxTRANSPARENT_WINDOW|");
    if (style & wxWANTS_CHARS)
        s += _T("wxWANTS_CHARS|");
    if (style & wxNO_FULL_REPAINT_ON_RESIZE)
        s += _T("wxNO_FULL_REPAINT_ON_RESIZE|");

    if (restype == _T("wxDialog"))
        {
        if (style & wxDEFAULT_DIALOG_STYLE)
            s += _T("wxDEFAULT_DIALOG_STYLE|");
#if WXWIN_COMPATIBILITY_2_6
        if (style & wxDIALOG_MODAL)
            s += _T("wxDIALOG_MODAL|");
        if (style & wxDIALOG_MODELESS)
            s += _T("wxDIALOG_MODELESS|");
        if (style & wxNO_3D)
            s += _T("wxNO_3D|");
#endif // WXWIN_COMPATIBILITY_2_6
        if (style & wxTAB_TRAVERSAL)
            s += _T("wxTAB_TRAVERSAL|");
        if (style & wxWS_EX_VALIDATE_RECURSIVELY)
            s += _T("wxWS_EX_VALIDATE_RECURSIVELY|");
        if (style & wxSTAY_ON_TOP)
            s += _T("wxSTAY_ON_TOP|");
        if (style & wxCAPTION)
            s += _T("wxCAPTION|");
        if (style & wxRESIZE_BORDER)
            s += _T("wxRESIZE_BORDER|");
#if WXWIN_COMPATIBILITY_2_6
        if (style & wxRESIZE_BOX)
            s += _T("wxRESIZE_BOX|");
#endif // WXWIN_COMPATIBILITY_2_6
        if (style & wxRESIZE_BORDER)
            s += _T("wxRESIZE_BORDER|");
        if (style & wxSYSTEM_MENU)
            s += _T("wxSYSTEM_MENU|");
        if (style & wxCLIP_CHILDREN)
            s += _T("wxCLIP_CHILDREN|");
        }

    if (restype == _T("wxPanel"))
        {
        if (style & wxCLIP_CHILDREN)
            s += _T("wxCLIP_CHILDREN|");
#if WXWIN_COMPATIBILITY_2_6
        if (style & wxNO_3D)
            s += _T("wxNO_3D|");
#endif // WXWIN_COMPATIBILITY_2_6
        if (style & wxTAB_TRAVERSAL)
            s += _T("wxTAB_TRAVERSAL|");
        if (style & wxWS_EX_VALIDATE_RECURSIVELY)
            s += _T("wxWS_EX_VALIDATE_RECURSIVELY|");
        }

    if (restype == _T("wxComboBox"))
        {
        if (style & wxCB_SORT)
            s += _T("wxCB_SORT|");
        if (style & wxCB_SIMPLE)
            s += _T("wxCB_SIMPLE|");
        if (style & wxCB_READONLY)
            s += _T("wxCB_READONLY|");
        if (style & wxCB_DROPDOWN)
            s += _T("wxCB_DROPDOWN|");
        }

    if (restype == _T("wxGauge"))
        {
        if (style & wxGA_HORIZONTAL)
            s += _T("wxGA_HORIZONTAL|");
        if (style & wxGA_VERTICAL)
            s += _T("wxGA_VERTICAL|");
    // windows only
        if (style & wxGA_SMOOTH)
            s += _T("wxGA_SMOOTH|");
        }

    if (restype == _T("wxRadioButton"))
        {
        if (style & wxRB_GROUP)
        s += _T("wxRB_GROUP|");
        }

    if (restype == _T("wxStaticText"))
        {
        if (style & wxST_NO_AUTORESIZE)
            s += _T("wxST_NO_AUTORESIZEL|");
        }

    if (restype == _T("wxRadioBox"))
        {
        if (style & wxRA_HORIZONTAL)
            s += _T("wxRA_HORIZONTAL|");
        if (style & wxRA_SPECIFY_COLS)
            s += _T("wxRA_SPECIFY_COLS|");
        if (style & wxRA_SPECIFY_ROWS)
            s += _T("wxRA_SPECIFY_ROWS|");
        if (style & wxRA_VERTICAL)
            s += _T("wxRA_VERTICAL|");
        }

    if (restype == _T("wxListBox"))
        {
        if (style & wxLB_SINGLE)
            s += _T("wxLB_SINGLE|");
        if (style & wxLB_MULTIPLE)
            s += _T("wxLB_MULTIPLE|");
        if (style & wxLB_EXTENDED)
            s += _T("wxLB_EXTENDED|");
        if (style & wxLB_HSCROLL)
            s += _T("wxLB_HSCROLL|");
        if (style & wxLB_ALWAYS_SB)
            s += _T("wxLB_ALWAYS_SB|");
        if (style & wxLB_NEEDED_SB)
            s += _T("wxLB_NEEDED_SB|");
        if (style & wxLB_SORT)
        s += _T("wxLB_SORT|");
        }

    if (restype == _T("wxTextCtrl"))
        {
        if (style & wxTE_PROCESS_ENTER)
            s += _T("wxTE_PROCESS_ENTER|");
        if (style & wxTE_PROCESS_TAB)
            s += _T("wxTE_PROCESS_TAB|");
        if (style & wxTE_MULTILINE)
            s += _T("wxTE_MULTILINE|");
        if (style & wxTE_PASSWORD)
            s += _T("wxTE_PASSWORD|");
        if (style & wxTE_READONLY)
            s += _T("wxTE_READONLY|");
        if (style & wxHSCROLL)
            s += _T("wxHSCROLL|");
        }


    if (restype == _T("wxScrollBar"))
        {
        if (style & wxSB_HORIZONTAL)
            s += _T("wxSB_HORIZONTAL|");
        if (style & wxSB_VERTICAL)
            s += _T("wxSB_VERTICAL|");
        }

    int l = s.length();
    // No styles defined
    if (l == 11)
        return wxEmptyString;
    // Trim off last |
    s = s.Truncate(l - 1);

    s += _T("</style>\n");
    return s;
}

wxString wxr2xml::GetDimension(wxItemResource * res)
{
    wxString msg;
    msg << _T("\t\t\t\t<dimension>") << res->GetValue1() << _T("</dimension>\n");
    return msg;
}

wxString wxr2xml::GenerateName(wxItemResource * res)
{
    wxString name;
    name = _T(" name=\"");
    switch (res->GetId()) {
    case wxID_OK:
        name += _T("wxID_OK");
    break;
    case wxID_CANCEL:
        name += _T("wxID_CANCEL");
    break;
    default:
        name += res->GetName();
    }

    name += _T("\"");
    return name;
}

void wxr2xml::ParseMenuBar(wxItemResource * res)
{
    wxItemResource *child;
    wxNode *node = res->GetChildren().GetFirst();
    // Get Menu Bar Name
    m_xmlfile.Write(_T("\t<object class=\"wxMenuBar\" "));
    m_xmlfile.Write(GenerateName(res));
    m_xmlfile.Write(_T(">\n"));
    while (node) {
        child = (wxItemResource *) node->GetData();
        ParseMenu(child);
        node = node->GetNext();
    }

    m_xmlfile.Write(_T("\t</object> \n\n"));
}

void wxr2xml::ParseMenu(wxItemResource * res)
{
    wxItemResource *child;
    wxNode *node = res->GetChildren().GetFirst();
    // Get Menu
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxMenu\" "));
    wxString menuname;
    menuname << _T("name = \"menu_") << res->GetValue1() << _T("\"");
    m_xmlfile.Write(menuname);
    m_xmlfile.Write(_T(">\n"));
    m_xmlfile.Write(_T("\t\t\t\t<label>")
        + FixMenuString(res->GetTitle()) + _T("</label>\n"));
    if (!res->GetValue4().empty())
        m_xmlfile.Write(_T("\t\t\t\t<help>") + res->GetValue4() +
        _T("</help>\n"));
    // Read in menu items and additional menus
    while (node) {
        child = (wxItemResource *) node->GetData();
        if (!child->GetChildren().GetFirst())
            ParseMenuItem(child);
        else
            ParseMenu(child);
            node = node->GetNext();
    }
    m_xmlfile.Write(_T("\t\t\t</object> \n"));
}

void wxr2xml::ParseMenuItem(wxItemResource * res)
{
    // Get Menu Item or Separator
    if (res->GetTitle().empty()) {
        m_xmlfile.Write(_T("\t\t\t<object class=\"separator\"/>\n"));
    } else {
        m_xmlfile.Write(_T("\t\t\t\t<object class=\"wxMenuItem\" "));
        wxString menuname;
        menuname << _T("name = \"menuitem_") << res->GetValue1() << _T("\"");
        m_xmlfile.Write(menuname);
        m_xmlfile.Write(_T(">\n"));
            m_xmlfile.Write(_T("\t\t\t<label>")
            + FixMenuString(res->GetTitle()) + _T("</label>\n"));
        if (!res->GetValue4().empty())
            m_xmlfile.Write(_T("\t\t\t<help>") +
        res->GetValue4() + _T("</help>\n"));
        if (res->GetValue2())
            m_xmlfile.Write(_T("\t\t\t\t<checkable>1</checkable>\n"));
        m_xmlfile.Write(_T("\t\t\t</object> \n"));
    }
}

wxString wxr2xml::FixMenuString(wxString phrase)
{
    phrase.Replace(_T("&"), _T("$"));
    return phrase;
}

void wxr2xml::ParseStaticBitmap(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxStaticBitmap\""));
    WriteControlInfo(res);
    // value4 holds bitmap name
    wxString bitmapname;
    bitmapname = res->GetValue4();
    wxBitmap bitmap;
    bitmap = wxResourceCreateBitmap(bitmapname, &m_table);
    bitmapname += _T(".bmp");
    bitmap.SaveFile(bitmapname, wxBITMAP_TYPE_BMP);
    m_xmlfile.Write(_T("\n\t\t\t\t<bitmap>") + bitmapname + _T("</bitmap>"));
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
    // bitmap5
}
//Parse a bitmap resource
void wxr2xml::ParseBitmap(wxItemResource * res)
{
    m_xmlfile.Write(_T("\t<object class=\"wxBitmap\" "));
    m_xmlfile.Write(GenerateName(res)+_T(">"));
    wxString bitmapname;
    bitmapname = res->GetName();
    wxBitmap bitmap;
    bitmap = wxResourceCreateBitmap(bitmapname, &m_table);
    bitmapname += _T(".bmp");
    bitmap.SaveFile(bitmapname, wxBITMAP_TYPE_BMP);
    m_xmlfile.Write(bitmapname);
    m_xmlfile.Write(_T("</object>\n\n"));
}

void wxr2xml::PanelStuff(wxItemResource * res)
{
    if ((res->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
        m_dlgunits = true;
    else
        m_dlgunits = false;

    // If this is true ignore fonts, background color and use system
    // defaults instead
    if ((res->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) != 0)
        m_systemdefaults = true;
    else
        m_systemdefaults = false;

}

wxString wxr2xml::GetValue2(wxItemResource *res)
{
    wxString msg;
    msg << res->GetValue2();
    return msg;
}

wxString wxr2xml::GetValue3(wxItemResource *res)
{
    wxString msg;
    msg << res->GetValue3();
    return msg;

}

wxString wxr2xml::GetValue5(wxItemResource *res)
{
    wxString msg;
    msg << res->GetValue5();
    return msg;

}

void wxr2xml::ParseBitmapButton(wxItemResource *res)
{

    m_xmlfile.Write(_T("\t\t\t<object class=\"wxBitmapButton\""));
    WriteControlInfo(res);
    // value4 holds bitmap name
    wxString bitmapname;
    bitmapname = res->GetValue4();
    wxBitmap bitmap;
    bitmap = wxResourceCreateBitmap(bitmapname, &m_table);
    bitmapname += _T(".bmp");
    bitmap.SaveFile(bitmapname, wxBITMAP_TYPE_BMP);
    m_xmlfile.Write(_T("\t\t\t\t<bitmap>") + bitmapname + _T("</bitmap>\n"));

    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

void wxr2xml::WriteFontInfo(wxItemResource *res)
{
//if systems_defaults true just ignore the fonts
    if (m_systemdefaults)
        return;
    wxFont font;
    font=res->GetFont();
    if (!font.GetRefData())
        return;

    m_xmlfile.Write(_T("\t\t\t<font>\n"));
    //Get font point size,font family,weight,font style,underline
    int pt;
    wxString msg;
    pt=font.GetPointSize();
    msg<<_T("\t\t\t\t<size>")<<pt<<_T("</size>\n");
    m_xmlfile.Write(msg);
    GetFontFace(font);
    GetFontStyle(font);
    GetFontWeight(font);

    if (font.GetUnderlined())
        m_xmlfile.Write(_T("\t\t\t\t<underlined>1</underlined>\n"));

    m_xmlfile.Write(_T("\t\t\t</font>\n"));
}

//WARNING possible make here
//I wasn't really sure the right way to do this.
void wxr2xml::GetFontFace(wxFont font)
{
    int family=font.GetFamily();

    switch (family)
        {
        case wxDEFAULT:
            break;
        case wxDECORATIVE:
            m_xmlfile.Write(_T("\t\t\t\t<face>decorative</face>\n"));
            break;
        case wxROMAN:
            m_xmlfile.Write(_T("\t\t\t\t<face>roman</face>\n"));
            break;
        case wxSCRIPT:
            m_xmlfile.Write(_T("\t\t\t\t<face>script</face>\n"));
            break;
        case wxSWISS:
            m_xmlfile.Write(_T("\t\t\t\t<face>swiss</face>\n"));
            break;
        case wxMODERN:
            m_xmlfile.Write(_T("\t\t\t\t<face>modern</face>\n"));
            break;
        default:
            wxLogError(_T("Unknown font face"));
        }
}

void wxr2xml::GetFontStyle(wxFont font)
{

    int style=font.GetStyle();

    switch (style)
        {
//since this is default no point in making file any larger
        case wxNORMAL:
            break;
        case wxITALIC:
            m_xmlfile.Write(_T("<style>italic</style>\n"));
            break;
        case wxSLANT:
            m_xmlfile.Write(_T("<style>slant</style>\n"));
            break;
        default:
            wxLogError(_T("Unknown font style"));
        }
}

void wxr2xml::GetFontWeight(wxFont font)
{
    int weight=font.GetWeight();

    switch (weight)
        {
//since this is default no point in making file any larger
        case wxNORMAL:
            break;
        case wxLIGHT:
            m_xmlfile.Write(_T("\t\t\t\t<weight>light</weight>\n"));
            break;
        case wxBOLD:
            m_xmlfile.Write(_T("\t\t\t\t<weight>bold</weight>\n"));
            break;
        default:
            wxLogError(_T("Unknown font weight"));
        }
}
