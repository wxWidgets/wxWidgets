// wxr2xml.cpp: implementation of the wxr2xml class.
// 8/30/00  Brian Gavin
// only tested on wxMSW so far
//License: wxWindows Liscense
// ////////////////////////////////////////////////////////////////////

/*
How to use class:
#include "wxr2xml.h"
...
wxr2xml trans;
trans->Convert("Myfile.wxr","Myfile.xml");
*/
#ifdef __GNUG__
#pragma implementation "wxr2xml.h"
#endif

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
    result = m_xmlfile.Open(xmlfile.c_str(), "w+t");
    wxASSERT_MSG(result, "Couldn't create XML file");
    if (!result)
        return FALSE;

    result = m_table.ParseResourceFile(wxrfile);
    wxASSERT_MSG(result, "Couldn't Load WXR file");
    if (!result)
        return FALSE;
    // Write basic xml header
    m_xmlfile.Write("<?xml version=\"1.0\" ?>\n");
    m_xmlfile.Write("<resource>\n");
    result = ParseResources();
    m_xmlfile.Write("</resource>\n");

    m_xmlfile.Close();

    return result;
}

bool wxr2xml::ParseResources()
{
    m_table.BeginFind();
    wxNode *node;

    while ((node = m_table.Next())) 
        {
        wxItemResource *res = (wxItemResource *) node->Data();
        wxString resType(res->GetType());
        if (resType == "wxDialog")
            ParseDialog(res);
        else if (resType == "wxPanel")
            ParsePanel(res);
        else if (resType == "wxPanel")
            ParsePanel(res);
        else if (resType == "wxMenu")
            ParseMenuBar(res);
        else if (resType == "wxBitmap")
            ParseBitmap(res);
        else
            wxLogError("Found unsupported resource " + resType);
    }
    return TRUE;
}

void wxr2xml::ParsePanel(wxItemResource * res)
{
    m_xmlfile.Write("\t<object class=\"wxPanel\"");
    PanelStuff(res);
    WriteControlInfo(res);
    m_xmlfile.Write("\n");
    ParseControls(res);
    m_xmlfile.Write("\t</object>\n\n");
}

void wxr2xml::ParseDialog(wxItemResource * res)
{
    PanelStuff(res);
    m_xmlfile.Write("\t<object class=\"wxDialog\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetTitle(res));

    m_xmlfile.Write("\n");
    ParseControls(res);
    m_xmlfile.Write("\t</object>\n\n");
}

void wxr2xml::ParseControls(wxItemResource * res)
{
    wxNode *node = res->GetChildren().First();
    while (node) 
        {
        wxItemResource *res = (wxItemResource *) node->Data();
        wxString resType(res->GetType());
        if (resType == "wxButton")
            ParseButton(res);
        else if ((resType == "wxTextCtrl") | (resType == "wxText")
         | (resType == "wxMultiText"))
            ParseTextCtrl(res);
        else if (resType == "wxCheckBox")
            ParseCheckBox(res);
        else if (resType == "wxRadioBox")
            ParseRadioBox(res);
        else if (resType == "wxListBox")
            ParseListBox(res);
        else if ((resType == "wxStaticText") | (resType == "wxMessage"))
            ParseStaticText(res);
        else if (resType == "wxChoice")
            ParseChoice(res);
        else if (resType == "wxGauge")
           ParseGauge(res);
        else if (resType == "wxSlider")
            ParseSlider(res);
        else if (resType == "wxComboBox")
            ParseComboBox(res);
        else if (resType == "wxRadioButton")
            ParseRadioButton(res);
        else if (resType == "wxStaticBitmap")
            ParseStaticBitmap(res);
        else if (resType == "wxScrollBar")
            ParseScrollBar(res);
        else if ((resType == "wxStaticBox") | (resType == "wxGroupBox"))
            ParseStaticBox(res);
        else if (resType == "wxBitmapButton")
            ParseBitmapButton(res);
        else
            wxLogError("Found unsupported resource " + resType);
        node = node->Next();
        }
}

// Write out basic stuff every control has
// name,position,size,bg,fg
void wxr2xml::WriteControlInfo(wxItemResource * res)
{
    m_xmlfile.Write(GenerateName(res));
    m_xmlfile.Write(">\n");
    m_xmlfile.Write(GetPosition(res));
    m_xmlfile.Write(GetSize(res));
    m_xmlfile.Write(GetStyles(res));
    WriteFontInfo(res);
}

wxString wxr2xml::GetSize(wxItemResource * res)
{
    wxString msg;
    if (m_dlgunits)
        msg << "\t\t\t\t<size>" << res->GetWidth() << "," << res->GetHeight() << "d</size>\n";
    else
        msg << "\t\t\t\t<size>" << res->GetWidth() << "," << res->GetHeight() << "</size>\n";
    return msg;
}

wxString wxr2xml::GetPosition(wxItemResource * res)
{
    wxString msg;
    if (m_dlgunits)
        msg << "\t\t\t\t<pos>" << res->GetX() << "," << res->GetY() << "d</pos>\n";
    else
        msg << "\t\t\t\t<pos>" << res->GetX() << "," << res->GetY() << "</pos>\n";
    return msg;
}

void wxr2xml::ParseButton(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxButton\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write("\t\t\t</object>\n");
}

void wxr2xml::ParseTextCtrl(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxTextCtrl\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue4(res));
    m_xmlfile.Write("\t\t\t</object>\n");

}

wxString wxr2xml::GetTitle(wxItemResource * res)
{
    wxString msg;
    msg = _T("\t\t\t\t<title>" + res->GetTitle() + "</title>");
    return msg;
}

wxString wxr2xml::GetValue4(wxItemResource * res)
{
    wxString msg;
    msg = _T("\t\t\t\t<value>" + res->GetValue4() + "</value>\n");
    return msg;
}

void wxr2xml::ParseCheckBox(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxCheckBox\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write(GetCheckStatus(res));
    m_xmlfile.Write("\t\t\t</object>\n");
}

wxString wxr2xml::GetLabel(wxItemResource * res)
{
    return _T("\t\t\t\t<label>" + res->GetTitle() + "</label>\n");
}

void wxr2xml::ParseRadioBox(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxRadioBox\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    // Add radio box items
    WriteStringList(res);
    // Value1
    m_xmlfile.Write(GetDimension(res));
    m_xmlfile.Write("\t\t\t</object>\n");
}

void wxr2xml::ParseListBox(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxListBox\"");
    WriteControlInfo(res);
    WriteStringList(res);
    m_xmlfile.Write("\t\t\t</object>\n");
}

void wxr2xml::ParseStaticText(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxStaticText\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write("\t\t\t</object>\n");
}

void wxr2xml::ParseStaticBox(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxStaticBox\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));
    m_xmlfile.Write("\t\t\t</object>\n");
}

void wxr2xml::WriteStringList(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t\t<content>\n");
    for (wxStringListNode * node = res->GetStringValues().GetFirst();
        node;node = node->GetNext()) {
        const wxString s1 = node->GetData();
        m_xmlfile.Write("\t\t\t\t\t<item>" + s1 + "</item>\n");
    }
    m_xmlfile.Write("\t\t\t\t</content>\n");
}

void wxr2xml::ParseChoice(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxChoice\"");
    WriteControlInfo(res);
    // Add choice items
    WriteStringList(res);
    m_xmlfile.Write("\t\t\t</object>\n");
}

void wxr2xml::ParseGauge(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxGauge\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write(GetRange(res));
    m_xmlfile.Write("\n\t\t\t</object>\n");
}

wxString wxr2xml::GetValue1(wxItemResource * res)
{
    wxString msg;
    msg << "\t\t\t\t<value>" << res->GetValue1() << "</value>\n";
    return msg;
}

wxString wxr2xml::GetRange(wxItemResource * res)
{
    wxString msg;
    msg << "\t\t\t\t<range>" << res->GetValue2() << "</range>";
    return msg;
}

void wxr2xml::ParseSlider(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxSlider\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write(GetMax(res));
    m_xmlfile.Write(GetMin(res));
    m_xmlfile.Write("\n\t\t\t</object>\n");
}

wxString wxr2xml::GetMax(wxItemResource * res)
{
    wxString msg;
    msg << "\t\t\t\t<max>" << res->GetValue3() << "</max>\n";
    return msg;
}

wxString wxr2xml::GetMin(wxItemResource * res)
{
    wxString msg;
    msg << "\t\t\t\t<min>" << res->GetValue2() << "</min>";
    return msg;
}

void wxr2xml::ParseComboBox(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxComboBox\"");
    WriteControlInfo(res);
    // Add combo items
    WriteStringList(res);
    m_xmlfile.Write("\n\t\t\t</object>\n");
}

void wxr2xml::ParseRadioButton(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxRadioButton\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetLabel(res));

    wxString msg;
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write(GetCheckStatus(res));
    m_xmlfile.Write("\t\t\t</object>\n");
}

void wxr2xml::ParseScrollBar(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxScrollBar\"");
    WriteControlInfo(res);
    m_xmlfile.Write(GetValue1(res));
    m_xmlfile.Write("\t\t\t\t<thumbsize>"+GetValue2(res)+"</thumbsize>\n");
    m_xmlfile.Write("\t\t\t\t<range>"+GetValue3(res)+"</range>\n");
    m_xmlfile.Write("\t\t\t\t<pagesize>"+GetValue5(res)+"</pagesize>\n");
    m_xmlfile.Write("\t\t\t</object>\n");
}

wxString wxr2xml::GetCheckStatus(wxItemResource * res)
{
    wxString msg;
    msg << "\t\t\t\t<checked>" << res->GetValue1() << "</checked>\n";
    return msg;
}

wxString wxr2xml::GetStyles(wxItemResource * res)
{
    // Very crude way to get styles
    long style;
    wxString s, restype;
    restype = res->GetType();
    style = res->GetStyle();

    s = "\t\t\t\t<style>";

    // Common styles for all controls
    if (style & wxSIMPLE_BORDER)
        s += "wxSIMPLE_BORDER|";
    if (style & wxSUNKEN_BORDER)
        s += "wxSUNKEN_BORDER|";
    if (style & wxSIMPLE_BORDER)
        s += "wxSIMPLE_BORDER|";
    if (style & wxDOUBLE_BORDER)
        s += "wxDOUBLE_BORDER|";
    if (style & wxRAISED_BORDER)
        s += "wxRAISED_BORDER|";
    if (style & wxTRANSPARENT_WINDOW)
        s += "wxTRANSPARENT_WINDOW|";
    if (style & wxWANTS_CHARS)
        s += "wxWANTS_CHARS|";
    if (style & wxNO_FULL_REPAINT_ON_RESIZE)
        s += "wxNO_FULL_REPAINT_ON_RESIZE|";

    if (restype == "wxDialog") 
        {
        if (style & wxDIALOG_MODAL)
            s += "wxDIALOG_MODAL|";
        if (style & wxDEFAULT_DIALOG_STYLE)
            s += "wxDEFAULT_DIALOG_STYLE|";
        if (style & wxDIALOG_MODELESS)
            s += "wxDIALOG_MODELESS|";
        if (style & wxNO_3D)
            s += "wxNO_3D|";
        if (style & wxTAB_TRAVERSAL)
            s += "wxTAB_TRAVERSAL|";
        if (style & wxWS_EX_VALIDATE_RECURSIVELY)
            s += "wxWS_EX_VALIDATE_RECURSIVELY|";
        if (style & wxSTAY_ON_TOP)
            s += "wxSTAY_ON_TOP|";
        if (style & wxCAPTION)
            s += "wxCAPTION|";
        if (style & wxTHICK_FRAME)
            s += "wxTHICK_FRAME|";
        if (style & wxRESIZE_BOX)
            s += "wxRESIZE_BOX|";
        if (style & wxRESIZE_BORDER)
            s += "wxRESIZE_BORDER|";
        if (style & wxSYSTEM_MENU)
            s += "wxSYSTEM_MENU|";
        if (style & wxCLIP_CHILDREN)
            s += "wxCLIP_CHILDREN|";
        }

    if (restype == "wxPanel") 
        {
        if (style & wxCLIP_CHILDREN)
            s += "wxCLIP_CHILDREN|";
        if (style & wxNO_3D)
            s += "wxNO_3D|";
        if (style & wxTAB_TRAVERSAL)
            s += "wxTAB_TRAVERSAL|";
        if (style & wxWS_EX_VALIDATE_RECURSIVELY)
            s += "wxWS_EX_VALIDATE_RECURSIVELY|";
        }

    if (restype == "wxComboBox") 
        {
        if (style & wxCB_SORT)
            s += "wxCB_SORT|";
        if (style & wxCB_SIMPLE)
            s += "wxCB_SIMPLE|";
        if (style & wxCB_READONLY)
            s += "wxCB_READONLY|";
        if (style & wxCB_DROPDOWN)
            s += "wxCB_DROPDOWN|";
        }

    if (restype == "wxGauge") 
        {
        if (style & wxGA_HORIZONTAL)
            s += "wxGA_HORIZONTAL|";
        if (style & wxGA_VERTICAL)
            s += "wxGA_VERTICAL|";
        if (style & wxGA_PROGRESSBAR)
            s += "wxGA_PROGRESSBAR|";
    // windows only
        if (style & wxGA_SMOOTH)
            s += "wxGA_SMOOTH|";
        }

    if (restype == "wxRadioButton") 
        {
        if (style & wxRB_GROUP)
        s += "wxRB_GROUP|";
        }

    if (restype == "wxStaticText") 
        {
        if (style & wxST_NO_AUTORESIZE)
            s += "wxST_NO_AUTORESIZEL|";
        }

    if (restype == "wxRadioBox") 
        {
        if (style & wxRA_HORIZONTAL)
            s += "wxRA_HORIZONTAL|";
        if (style & wxRA_SPECIFY_COLS)
            s += "wxRA_SPECIFY_COLS|";
        if (style & wxRA_SPECIFY_ROWS)
            s += "wxRA_SPECIFY_ROWS|";
        if (style & wxRA_VERTICAL)
            s += "wxRA_VERTICAL|";
        }

    if (restype == "wxListBox") 
        {
        if (style & wxLB_SINGLE)
            s += "wxLB_SINGLE|";
        if (style & wxLB_MULTIPLE)
            s += "wxLB_MULTIPLE|";
        if (style & wxLB_EXTENDED)
            s += "wxLB_EXTENDED|";
        if (style & wxLB_HSCROLL)
            s += "wxLB_HSCROLL|";
        if (style & wxLB_ALWAYS_SB)
            s += "wxLB_ALWAYS_SB|";
        if (style & wxLB_NEEDED_SB)
            s += "wxLB_NEEDED_SB|";
        if (style & wxLB_SORT)
        s += "wxLB_SORT|";
        }

    if (restype == "wxTextCtrl") 
        {
        if (style & wxTE_PROCESS_ENTER)
            s += "wxTE_PROCESS_ENTER|";
        if (style & wxTE_PROCESS_TAB)
            s += "wxTE_PROCESS_TAB|";
        if (style & wxTE_MULTILINE)
            s += "wxTE_MULTILINE|";
        if (style & wxTE_PASSWORD)
            s += "wxTE_PASSWORD|";
        if (style & wxTE_READONLY)
            s += "wxTE_READONLY|";
        if (style & wxHSCROLL)
            s += "wxHSCROLL|";
        }


    if (restype == "wxScrollBar")
        {
        if (style & wxSB_HORIZONTAL)
            s += "wxSB_HORIZONTAL|";
        if (style & wxSB_VERTICAL)  
            s += "wxSB_VERTICAL|";
        }

    int l;
    l = s.Length();
    // No styles defined
    if (l == 11)
        return _T("");
    // Trim off last |
    s = s.Truncate(l - 1);

    s += "</style>\n";
    return s;
}

wxString wxr2xml::GetDimension(wxItemResource * res)
{
    wxString msg;
    msg << "\t\t\t\t<dimension>" << res->GetValue1() << "</dimension>\n";
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

    name += "\"";
    return name;
}

void wxr2xml::ParseMenuBar(wxItemResource * res)
{
    wxItemResource *child;
    wxNode *node = res->GetChildren().First();
    // Get Menu Bar Name
    m_xmlfile.Write("\t<object class=\"wxMenuBar\" ");
    m_xmlfile.Write(GenerateName(res));
    m_xmlfile.Write(">\n");
    while (node) {
        child = (wxItemResource *) node->Data();
        ParseMenu(child);
        node = node->Next();
    }

    m_xmlfile.Write("\t</object> \n\n");
}

void wxr2xml::ParseMenu(wxItemResource * res)
{
    wxItemResource *child;
    wxNode *node = res->GetChildren().First();
    // Get Menu 
    m_xmlfile.Write("\t\t\t<object class=\"wxMenu\" ");
    wxString menuname;
    menuname << "name = \"menu_" << res->GetValue1() << "\"";
    m_xmlfile.Write(menuname);
    m_xmlfile.Write(">\n");
    m_xmlfile.Write("\t\t\t\t<label>"
        + FixMenuString(res->GetTitle()) + "</label>\n");
    if (res->GetValue4() != "")
        m_xmlfile.Write("\t\t\t\t<help>" + res->GetValue4() +
        "</help>\n");
    // Read in menu items and additional menus
    while (node) {
        child = (wxItemResource *) node->Data();
        if (!child->GetChildren().First())
            ParseMenuItem(child);
        else
            ParseMenu(child);
            node = node->Next();
    }
    m_xmlfile.Write("\t\t\t</object> \n");
}

void wxr2xml::ParseMenuItem(wxItemResource * res)
{
    // Get Menu Item or Separator
    if (res->GetTitle() == "") {
        m_xmlfile.Write("\t\t\t<object class=\"separator\"/>\n");
    } else {
        m_xmlfile.Write("\t\t\t\t<object class=\"wxMenuItem\" ");
        wxString menuname;
        menuname << "name = \"menuitem_" << res->GetValue1() << "\"";
        m_xmlfile.Write(menuname);
        m_xmlfile.Write(">\n");
            m_xmlfile.Write("			<label>"
            + FixMenuString(res->GetTitle()) + "</label>\n");
        if (res->GetValue4() != "")
            m_xmlfile.Write("			<help>" +
        res->GetValue4() + "</help>\n");
        if (res->GetValue2())
            m_xmlfile.Write("\t\t\t\t<checkable>1</checkable>\n");
        m_xmlfile.Write("\t\t\t</object> \n");
    }
}

wxString wxr2xml::FixMenuString(wxString phrase)
{
    phrase.Replace("&", "$");
    return phrase;
}

void wxr2xml::ParseStaticBitmap(wxItemResource * res)
{
    m_xmlfile.Write("\t\t\t<object class=\"wxStaticBitmap\"");
    WriteControlInfo(res);
    // value4 holds bitmap name
    wxString bitmapname;
    bitmapname = res->GetValue4();
    wxBitmap bitmap;
    bitmap = wxResourceCreateBitmap(bitmapname, &m_table);
    bitmapname += _T(".bmp");
    bitmap.SaveFile(bitmapname, wxBITMAP_TYPE_BMP);
    m_xmlfile.Write("\n\t\t\t\t<bitmap>" + bitmapname + "</bitmap>");
    m_xmlfile.Write("\t\t\t</object>\n");
    // bitmap5
}
//Parse a bitmap resource
void wxr2xml::ParseBitmap(wxItemResource * res)
{
    m_xmlfile.Write("\t<object class=\"wxBitmap\" ");
    m_xmlfile.Write(GenerateName(res)+">");
    wxString bitmapname;
    bitmapname = res->GetName();
    wxBitmap bitmap;
    bitmap = wxResourceCreateBitmap(bitmapname, &m_table);
    bitmapname += _T(".bmp");
    bitmap.SaveFile(bitmapname, wxBITMAP_TYPE_BMP);
    m_xmlfile.Write(bitmapname);
    m_xmlfile.Write("</object>\n\n");
}

void wxr2xml::PanelStuff(wxItemResource * res)
{
    if ((res->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
        m_dlgunits = TRUE;
    else
        m_dlgunits = FALSE;

    // If this is true ignore fonts, background color and use system
    // defaults instead
    if ((res->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) != 0)
        m_systemdefaults = TRUE;
    else
        m_systemdefaults = FALSE;

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
    
    m_xmlfile.Write("\t\t\t<object class=\"wxBitmapButton\"");
    WriteControlInfo(res);
    // value4 holds bitmap name
    wxString bitmapname;
    bitmapname = res->GetValue4();
    wxBitmap bitmap;
    bitmap = wxResourceCreateBitmap(bitmapname, &m_table);
    bitmapname += _T(".bmp");
    bitmap.SaveFile(bitmapname, wxBITMAP_TYPE_BMP);
    m_xmlfile.Write("\t\t\t\t<bitmap>" + bitmapname + "</bitmap>\n");
    
    m_xmlfile.Write("\t\t\t</object>\n");
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

    m_xmlfile.Write("\t\t\t<font>\n");
    //Get font point size,font family,weight,font style,underline
    int pt;
    wxString msg;
    pt=font.GetPointSize();
    msg<<"\t\t\t\t<size>"<<pt<<"</size>\n";
    m_xmlfile.Write(msg);
    GetFontFace(font);
    GetFontStyle(font);
    GetFontWeight(font);
    
    if (font.GetUnderlined())
        m_xmlfile.Write("\t\t\t\t<underlined>1</underlined>\n");
    
    m_xmlfile.Write("\t\t\t</font>\n");
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
            m_xmlfile.Write("\t\t\t\t<face>decorative</face>\n");
            break;
        case wxROMAN:
            m_xmlfile.Write("\t\t\t\t<face>roman</face>\n");
            break;
        case wxSCRIPT:
            m_xmlfile.Write("\t\t\t\t<face>script</face>\n");
            break;
        case wxSWISS:
            m_xmlfile.Write("\t\t\t\t<face>swiss</face>\n");
            break;
        case wxMODERN:
            m_xmlfile.Write("\t\t\t\t<face>modern</face>\n");
            break;
        default:
            wxLogError("Unknown font face");
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
            m_xmlfile.Write("<style>italic</style>\n");
            break;
        case wxSLANT:
            m_xmlfile.Write("<style>slant</style>\n");
            break;
        default:
            wxLogError("Unknown font style");
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
            m_xmlfile.Write("\t\t\t\t<weight>light</weight>\n");
            break;
        case wxBOLD:
            m_xmlfile.Write("\t\t\t\t<weight>bold</weight>\n");
            break;
        default:
            wxLogError("Unknown font weight");
        }
}
