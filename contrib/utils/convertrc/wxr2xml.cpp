// wxr2xml.cpp: implementation of the wxWxr2Xml class.
//  8/30/00  Brian Gavin
// only tested on wxMSW so far
//////////////////////////////////////////////////////////////////////
/* TODO
  port to wxGTK should be very easy
  support fonts
  add unsupported controls when XML format adds them
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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxWxr2Xml::wxWxr2Xml()
{

}

wxWxr2Xml::~wxWxr2Xml()
{

}

bool wxWxr2Xml::Convert(wxString wxrfile, wxString xmlfile)
{
bool result;
result=m_xmlfile.Open(xmlfile.c_str(),"w+t");
wxASSERT_MSG(result,"Couldn't create XML file");	
if (!result)
	return FALSE;

result=m_table.ParseResourceFile(wxrfile);
wxASSERT_MSG(result,"Couldn't Load WXR file");
if (!result)
	return FALSE;
//Write basic xml header
m_xmlfile.Write("<?xml version=\"1.0\" ?>\n");
m_xmlfile.Write("<resource>\n");
result=ParseResources();
m_xmlfile.Write("</resource>\n");

m_xmlfile.Close();

return result;
}

bool wxWxr2Xml::ParseResources()
{
m_table.BeginFind();
wxNode *node;
	
while ((node = m_table.Next()))
    {
        wxItemResource *res = (wxItemResource *)node->Data();
        wxString resType(res->GetType());
		if (resType=="wxDialog")
			ParseDialog(res);
        else if (resType=="wxPanel")
			ParsePanel(res);
       else if (resType=="wxPanel")
			ParsePanel(res);
	   else if (resType=="wxMenu")
			ParseMenuBar(res);
	   else if (resType=="wxBitmap")
		   	ParseBitmap(res);
        else 
		   wxLogError("Found unsupported resource "+resType);
    }

return TRUE;
}

void wxWxr2Xml::ParsePanel(wxItemResource *res)
{

m_xmlfile.Write("	<panel");
PanelStuff(res);
WriteControlInfo(res);
m_xmlfile.Write("\n");	
m_xmlfile.Write("	<children>\n");
ParseControls(res);
m_xmlfile.Write("	</children>\n");

m_xmlfile.Write("	</panel>\n");
}


void wxWxr2Xml::ParseDialog(wxItemResource *res)
{
PanelStuff(res);
m_xmlfile.Write("	<dialog");
WriteControlInfo(res);
m_xmlfile.Write(GetTitle(res));

    
m_xmlfile.Write("\n");	
m_xmlfile.Write("	<children>\n");
ParseControls(res);
m_xmlfile.Write("	</children>\n");
m_xmlfile.Write("	</dialog>\n");
}

void wxWxr2Xml::ParseControls(wxItemResource *res)
{
wxNode *node = res->GetChildren().First();
while (node)
        {
	    wxItemResource *res = (wxItemResource *)node->Data();
	    wxString resType(res->GetType());
		if (resType=="wxButton")
			ParseButton(res);
		else if ((resType=="wxTextCtrl")|(resType=="wxText")
			|(resType=="wxMultiText"))
			ParseTextCtrl(res);
		else if (resType=="wxCheckBox")
			ParseCheckBox(res);
		else if (resType=="wxRadioBox")
			ParseRadioBox(res);
		else if (resType=="wxListBox")
			ParseListBox(res);
		else if ((resType=="wxStaticText")|(resType=="wxMessage"))
			ParseStaticText(res);
		else if (resType=="wxChoice")
			ParseChoice(res);
		else if (resType=="wxGauge")
			ParseGauge(res);
		else if (resType=="wxSlider")
			ParseSlider(res);
		else if (resType=="wxComboBox")
			ParseComboBox(res);
		else if (resType=="wxRadioButton")
			ParseRadioButton(res);
		else if (resType=="wxStaticBitmap")
			ParseStaticBitmap(res);
		else if (resType=="wxScrollBar")
			wxLogError("wxScrollBar unsupported");
		else if ((resType=="wxStaticBox")|(resType=="wxGroupBox"))
			wxLogError("wxStaticBox unsupported");
		else if (resType=="wxBitmapButton")
			wxLogError("wxBitmapButton unsupported");
		else
			wxLogError("Found unsupported resource "+resType);
            node = node->Next();
        }
}

//Write out basic stuff every control has
// name,position,size,bg,fg
void wxWxr2Xml::WriteControlInfo(wxItemResource *res)
{
m_xmlfile.Write(GenerateName(res));
m_xmlfile.Write(">\n");
m_xmlfile.Write(GetPosition(res));
m_xmlfile.Write(GetSize(res));
m_xmlfile.Write(GetStyles(res));
}

wxString wxWxr2Xml::GetSize(wxItemResource *res)
{
wxString msg;
if (m_dlgunits)
	msg<<" <size>"<<res->GetWidth()<<","<<res->GetHeight()<<"d</size>";
else
	msg<<" <size>"<<res->GetWidth()<<","<<res->GetHeight()<<"</size>";
return msg;
}

wxString wxWxr2Xml::GetPosition(wxItemResource *res)
{
wxString msg;
if (m_dlgunits)
	msg<<" <pos>"<<res->GetX()<<","<<res->GetY()<<"d</pos>";
else
	msg<<" <pos>"<<res->GetX()<<","<<res->GetY()<<"</pos>";
return msg;
}

void wxWxr2Xml::ParseButton(wxItemResource *res)
{
m_xmlfile.Write("		<button");
WriteControlInfo(res);
m_xmlfile.Write(GetLabel(res));
m_xmlfile.Write("</button>\n");
}

void wxWxr2Xml::ParseTextCtrl(wxItemResource *res)
{
m_xmlfile.Write("		<textctrl");
WriteControlInfo(res);
m_xmlfile.Write(GetValue4(res));
m_xmlfile.Write("</textctrl>\n");

}

wxString wxWxr2Xml::GetTitle(wxItemResource *res)
{
wxString msg;
msg=_T(" <title>"+res->GetTitle()+"</title>");
return msg;
}

wxString wxWxr2Xml::GetValue4(wxItemResource *res)
{
wxString msg;
msg=_T(" <value>"+res->GetValue4()+"</value>");
return msg;
}

void wxWxr2Xml::ParseCheckBox(wxItemResource *res)
{
m_xmlfile.Write("		<checkbox");
WriteControlInfo(res);
m_xmlfile.Write(GetLabel(res));
m_xmlfile.Write(GetCheckStatus(res));
m_xmlfile.Write("</checkbox>\n");
}

wxString wxWxr2Xml::GetLabel(wxItemResource *res)
{
return _T(" <label>"+res->GetTitle()+"</label>");
}

void wxWxr2Xml::ParseRadioBox(wxItemResource *res)
{
m_xmlfile.Write("		<radiobox");
WriteControlInfo(res);
m_xmlfile.Write(GetLabel(res));
//Add radio box items
WriteStringList(res);
//Value1
m_xmlfile.Write(GetDimension(res));
m_xmlfile.Write("\n			</radiobox>\n");
}

void wxWxr2Xml::ParseListBox(wxItemResource *res)
{
m_xmlfile.Write("		<listbox");
WriteControlInfo(res);
WriteStringList(res);
m_xmlfile.Write("</listbox>\n");
}

void wxWxr2Xml::ParseStaticText(wxItemResource *res)
{
m_xmlfile.Write("		<statictext");
WriteControlInfo(res);
m_xmlfile.Write(GetLabel(res));
m_xmlfile.Write("</statictext>\n");
}

void wxWxr2Xml::ParseStaticBox(wxItemResource *res)
{
m_xmlfile.Write("		<staticbox");
WriteControlInfo(res);
m_xmlfile.Write(GetLabel(res));
m_xmlfile.Write("</staticbox>\n");
}

void wxWxr2Xml::WriteStringList(wxItemResource *res)
{
m_xmlfile.Write("\n			<content>");
for ( wxStringListNode *node = res->GetStringValues().GetFirst(); node; 
	node = node->GetNext() )
    {
        const wxString s1 = node->GetData();
        m_xmlfile.Write("\n			<item>"+s1+"</item>");
    }
	m_xmlfile.Write("\n			</content>");
}

void wxWxr2Xml::ParseChoice(wxItemResource *res)
{
m_xmlfile.Write("		<choice");
WriteControlInfo(res);
//Add choice items
WriteStringList(res);
m_xmlfile.Write("\n		</choice>\n");
}

void wxWxr2Xml::ParseGauge(wxItemResource *res)
{
m_xmlfile.Write("		<gauge");
WriteControlInfo(res);
m_xmlfile.Write(GetValue1(res));
m_xmlfile.Write(GetRange(res));
m_xmlfile.Write("\n			</gauge>\n");
}


wxString wxWxr2Xml::GetValue1(wxItemResource *res)
{
wxString msg;
msg<<" <value>"<<res->GetValue1()<<"</value>";
return msg;
}

wxString wxWxr2Xml::GetRange(wxItemResource *res)
{
wxString msg;
msg<<" <range>"<<res->GetValue2()<<"</range>";
return msg;
}

void wxWxr2Xml::ParseSlider(wxItemResource *res)
{
m_xmlfile.Write("		<slider");
WriteControlInfo(res);
m_xmlfile.Write(GetValue1(res));
m_xmlfile.Write(GetMax(res));
m_xmlfile.Write(GetMin(res));
m_xmlfile.Write("\n			</slider>\n");
}

wxString wxWxr2Xml::GetMax(wxItemResource *res)
{
wxString msg;
msg<<" <max>"<<res->GetValue3()<<"</max>";
return msg;
}

wxString wxWxr2Xml::GetMin(wxItemResource *res)
{
wxString msg;
msg<<" <min>"<<res->GetValue2()<<"</min>";
return msg;
}

void wxWxr2Xml::ParseComboBox(wxItemResource *res)
{
m_xmlfile.Write("		<combobox");
WriteControlInfo(res);
//Add combo items
WriteStringList(res);
m_xmlfile.Write("\n		</combobox>\n");
}

void wxWxr2Xml::ParseRadioButton(wxItemResource *res)
{
m_xmlfile.Write("		<radiobutton");
WriteControlInfo(res);
m_xmlfile.Write(GetLabel(res));

wxString msg;
m_xmlfile.Write(GetValue1(res));
m_xmlfile.Write(GetCheckStatus(res));
m_xmlfile.Write("\n			</radiobutton>\n");
}

void wxWxr2Xml::ParseScrollBar(wxItemResource *res)
{
m_xmlfile.Write("		<scrollbar");
WriteControlInfo(res);
//TODO learn more about XML scrollbar format
m_xmlfile.Write("\n			</scrollbar>\n");
} 

wxString wxWxr2Xml::GetCheckStatus(wxItemResource *res)
{
wxString msg;
msg<<" <checked>"<<res->GetValue1()<<"</checked>";
return msg;
}

wxString wxWxr2Xml::GetStyles(wxItemResource *res)
{
//Very crude way to get styles
long style;
wxString s,restype;
restype=res->GetType();
style=res->GetStyle();

s="<style>";

//Common styles for all controls
if (style&wxSIMPLE_BORDER)
		s+="wxSIMPLE_BORDER|";
if (style&wxSUNKEN_BORDER)
		s+="wxSUNKEN_BORDER|";
if (style&wxSIMPLE_BORDER)
		s+="wxSIMPLE_BORDER|";
if (style&wxDOUBLE_BORDER)
		s+="wxDOUBLE_BORDER|";
if (style&wxRAISED_BORDER)
		s+="wxRAISED_BORDER|";
if (style&wxTRANSPARENT_WINDOW)
		s+="wxTRANSPARENT_WINDOW|";
if (style&wxWANTS_CHARS)
		s+="wxWANTS_CHARS|";
if (style&wxNO_FULL_REPAINT_ON_RESIZE)
		s+="wxNO_FULL_REPAINT_ON_RESIZE|";
	    
if (restype=="wxDialog")
{
	if (style&wxDIALOG_MODAL)
		s+="wxDIALOG_MODAL|";
	if (style&wxDEFAULT_DIALOG_STYLE)
		s+="wxDEFAULT_DIALOG_STYLE|";
	if (style&wxDIALOG_MODELESS)
		s+="wxDIALOG_MODELESS|";
	if (style&wxNO_3D)
		s+="wxNO_3D|";
	if (style&wxTAB_TRAVERSAL)
		s+="wxTAB_TRAVERSAL|";
	if (style&wxWS_EX_VALIDATE_RECURSIVELY)
		s+="wxWS_EX_VALIDATE_RECURSIVELY|";
	if (style&wxSTAY_ON_TOP)
		s+="wxSTAY_ON_TOP|";
	if (style&wxCAPTION)
		s+="wxCAPTION|";
	if (style&wxTHICK_FRAME)
		s+="wxTHICK_FRAME|";
	if (style&wxRESIZE_BOX)
		s+="wxRESIZE_BOX|";
	if (style&wxRESIZE_BORDER)
		s+="wxRESIZE_BORDER|";
	if (style&wxSYSTEM_MENU)
		s+="wxSYSTEM_MENU|";
	if (style&wxCLIP_CHILDREN)
		s+="wxCLIP_CHILDREN|";
	
	
}
    

    
if (restype=="wxPanel")
{
	if (style&wxCLIP_CHILDREN)
		s+="wxCLIP_CHILDREN|";
	if (style&wxNO_3D)
		s+="wxNO_3D|";
	if (style&wxTAB_TRAVERSAL)
		s+="wxTAB_TRAVERSAL|";
	if (style&wxWS_EX_VALIDATE_RECURSIVELY)
		s+="wxWS_EX_VALIDATE_RECURSIVELY|";
}

if (restype=="wxComboBox")
{
	if (style&wxCB_SORT)
		s+="wxCB_SORT|";
	if (style&wxCB_SIMPLE)
		s+="wxCB_SIMPLE|";
	if (style&wxCB_READONLY)
		s+="wxCB_READONLY|";
	if (style&wxCB_DROPDOWN)
		s+="wxCB_DROPDOWN|";
}

if (restype=="wxGauge")
{
	if (style&wxGA_HORIZONTAL)
		s+="wxGA_HORIZONTAL|";
	if (style&wxGA_VERTICAL)
		s+="wxGA_VERTICAL|";
	if (style&wxGA_PROGRESSBAR)
		s+="wxGA_PROGRESSBAR|";
	// windows only
	if (style&wxGA_SMOOTH)
		s+="wxGA_SMOOTH|";
}

if (restype=="wxRadioButton")
{
	if (style&wxRB_GROUP)
		s+="wxRB_GROUP|";
}

if (restype=="wxStaticText")
{
	if (style&wxST_NO_AUTORESIZE)
		s+="wxST_NO_AUTORESIZEL|";
}

if (restype=="wxRadioBox")
	{
	if (style&wxRA_HORIZONTAL)
		s+="wxRA_HORIZONTAL|";
	if (style&wxRA_SPECIFY_COLS)
		s+="wxRA_SPECIFY_COLS|";
	if (style&wxRA_SPECIFY_ROWS)
		s+="wxRA_SPECIFY_ROWS|";
	if (style&wxRA_VERTICAL)
		s+="wxRA_VERTICAL|";
	}

if (restype=="wxListBox")
{
	if (style&wxLB_SINGLE)
		s+="wxLB_SINGLE|";
	if (style&wxLB_MULTIPLE)
		s+="wxLB_MULTIPLE|";
	if (style&wxLB_EXTENDED)
		s+="wxLB_EXTENDED|";
	if (style&wxLB_HSCROLL)
		s+="wxLB_HSCROLL|";
	if (style&wxLB_ALWAYS_SB)
		s+="wxLB_ALWAYS_SB|";
	if (style&wxLB_NEEDED_SB)
		s+="wxLB_NEEDED_SB|";
	if (style&wxLB_SORT)
		s+="wxLB_SORT|";
}
	
if (restype=="wxTextCtrl")
{
	if (style&wxTE_PROCESS_ENTER)
		s+="wxTE_PROCESS_ENTER|";
	if (style&wxTE_PROCESS_TAB)
		s+="wxTE_PROCESS_TAB|";
	if (style&wxTE_MULTILINE)
		s+="wxTE_MULTILINE|";
	if (style&wxTE_PASSWORD)
		s+="wxTE_PASSWORD|";
	if (style&wxTE_READONLY)
		s+="wxTE_READONLY|";
	if (style&wxHSCROLL)
		s+="wxHSCROLL|";
}

int l;
l=s.Length();
//No styles defined
if (l==7)
	return _T("");
//Trim off last |
s=s.Truncate(l-1);

s+="</style>";
return s;	
}

wxString wxWxr2Xml::GetDimension(wxItemResource *res)
{
wxString msg;
msg<<" <dimension>"<<res->GetValue1()<<"</dimension>";
return msg;
}

wxString wxWxr2Xml::GenerateName(wxItemResource *res)
{
wxString name;
name=_T(" name=\"");
switch (res->GetId())
	{
	case wxID_OK:
		name+=_T("wxID_OK");
		break;
	case wxID_CANCEL:
		name+=_T("wxID_CANCEL");
		break;
	default:
		name+=res->GetName();
	}

name+="\"";	
return name;
}

void wxWxr2Xml::ParseMenuBar(wxItemResource *res)
{
	wxItemResource *child;
	wxNode *node = res->GetChildren().First();
	//Get Menu Bar Name
	m_xmlfile.Write("<menubar ");
	m_xmlfile.Write(GenerateName(res));
	m_xmlfile.Write(">\n");
	m_xmlfile.Write("	<children>\n");
    while (node)
    {
		child= (wxItemResource *)node->Data();
		ParseMenu(child);
        node = node->Next();
	}

	m_xmlfile.Write("	</children>\n");
	m_xmlfile.Write("</menubar> \n");
}


void wxWxr2Xml::ParseMenu(wxItemResource *res)
{
	wxItemResource *child;
	wxNode *node = res->GetChildren().First();
	//Get Menu 
	m_xmlfile.Write("	<menu ");
	wxString menuname;
	menuname<<"name = \"menu_"<<res->GetValue1()<<"\"";
	m_xmlfile.Write(menuname);
	m_xmlfile.Write(">\n");
	m_xmlfile.Write("		<label>"
		+FixMenuString(res->GetTitle())+"</label>\n");
	if (res->GetValue4()!="")
		m_xmlfile.Write("		<help>"+res->GetValue4()+"</help>\n");
	m_xmlfile.Write("		<children>\n");
	//Read in menu items and additional menus
	while (node)
    {
		child= (wxItemResource *)node->Data();
		if (!child->GetChildren().First())
			ParseMenuItem(child);
		else
			ParseMenu(child);

        node = node->Next();
	}
m_xmlfile.Write("		</children>\n");
m_xmlfile.Write("	</menu> \n");
}

void wxWxr2Xml::ParseMenuItem(wxItemResource *res)
{
	//Get Menu Item or Separator
if (res->GetTitle()=="")
	{
	m_xmlfile.Write("		<separator/>\n");
	}
else
	{
	m_xmlfile.Write("		<menuitem ");
	wxString menuname;
	menuname<<"name = \"menuitem_"<<res->GetValue1()<<"\"";
	m_xmlfile.Write(menuname);
	m_xmlfile.Write(">\n");
	m_xmlfile.Write("			<label>"
		+FixMenuString(res->GetTitle())+"</label>\n");
	if (res->GetValue4()!="")
		m_xmlfile.Write("			<help>"+res->GetValue4()+"</help>\n");
	if (res->GetValue2())
		m_xmlfile.Write("			<checkable>1</checkable>\n");
	m_xmlfile.Write("		</menuitem> \n");
	}
}

wxString wxWxr2Xml::FixMenuString(wxString phrase)
{
phrase.Replace("&","$");
return phrase;
}

void wxWxr2Xml::ParseStaticBitmap(wxItemResource *res)
{
m_xmlfile.Write("		<staticbitmap");
WriteControlInfo(res);
//value4 holds bitmap name
wxString bitmapname;
bitmapname=res->GetValue4();
wxBitmap bitmap;
bitmap= wxResourceCreateBitmap(bitmapname,&m_table);
bitmapname+=_T(".bmp");
bitmap.SaveFile(bitmapname,wxBITMAP_TYPE_BMP);
m_xmlfile.Write("\n		<bitmap>"+bitmapname+"</bitmap>");
m_xmlfile.Write("</staticbitmap>\n");
//bitmap5
}

void wxWxr2Xml::ParseBitmap(wxItemResource *res)
{

}

void wxWxr2Xml::PanelStuff(wxItemResource *res)
{
if ((res->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
	m_dlgunits=TRUE;
else 
	m_dlgunits=FALSE;

//If this is true ignore fonts, background color and use system
//defaults instead
if ((res->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) != 0)
	m_systemdefaults=TRUE;
else 
	m_systemdefaults=FALSE;

}
