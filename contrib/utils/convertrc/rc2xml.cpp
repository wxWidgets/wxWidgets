// rc2xml.cpp: implementation of the wxRC2XML class.
//
//////////////////////////////////////////////////////////////////////
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "rc2xml.h"
#include "wx/image.h"
#include "wx/resource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxRC2XML::wxRC2XML()
{
m_done=FALSE;
m_bitmaplist=new wxList(wxKEY_STRING);
m_stringtable=new wxList(wxKEY_STRING);
m_iconlist = new wxList(wxKEY_STRING);
}

wxRC2XML::~wxRC2XML()
{
delete m_bitmaplist;
delete m_stringtable;
delete m_iconlist;
}

bool wxRC2XML::Convert(wxString rcfile, wxString xmlfile)
{
m_rc.Open(rcfile.c_str());
m_filesize=m_rc.Length();

bool result;
result=m_xmlfile.Open(xmlfile.c_str(),"w+t");
wxASSERT_MSG(result,"Couldn't create XML file");	
if (!result)
	return FALSE;

	
/* Write Basic header for XML file */
m_xmlfile.Write("<?xml version=\"1.0\" ?>\n");
m_xmlfile.Write("<resource>\n");

//Gather all the resource we need for toolbars,menus, and etc
FirstPass();
m_done=FALSE;
m_rc.Seek(0);
//Read in dialogs, toolbars,menus
SecondPass();

m_xmlfile.Write("</resource>\n");
m_xmlfile.Close();
m_rc.Close();

return TRUE;
}


void wxRC2XML::ParseDialog(wxString dlgname)
{
	wxString token;
	static int dlgid=999;
	dlgid++;
	/* Make sure that this really is a dialog 
	microsoft reuses the keyword DIALOG for other things
	*/
	token=PeekToken();
	//Microsoft notation?
	if (token=="DISCARDABLE")
	{
		token=GetToken();
		token=PeekToken();
	}
	//Error isn't a Dialog resource eject eject
	if (!token.IsNumber())
		return;

//Record x,y,width,height
int x,y,width,height;
ReadRect(x,y,width,height);
//Get Title
token=GetToken();
wxString title;

while ((token!="BEGIN")&(token!="{"))
{
if (token=="CAPTION")
	{
	title=GetQuoteField();
	}
token=GetToken();
}
m_xmlfile.Write("	<dialog");
//Avoid duplicate names this way
dlgname.Replace("IDD_","DLG_");

WriteBasicInfo(x,y,width,height,dlgname);
WriteTitle(title);
m_xmlfile.Write("	<children>\n");
ParseControls();
m_xmlfile.Write("	</children>\n");
m_xmlfile.Write("	</dialog>\n");
}

/*
BEGIN
    EDITTEXT        IDC_BANDS,36,83,22,14,ES_AUTOHSCROLL | ES_NUMBER | NOT 
                    WS_TABSTOP
    LTEXT           "Bands",IDC_STATIC,11,86,21,8
    EDITTEXT        IDC_NAME,10,3,75,14,ES_AUTOHSCROLL
END
*/
void wxRC2XML::ParseControls()
{
wxString token;

token=GetToken();
while ((token!="END")&(token!="}"))
	{
	if (token=="LTEXT")
	    ParseStaticText();
	else if (token=="EDITTEXT")
	    ParseTextCtrl();
	else if (token=="PUSHBUTTON")
	    ParsePushButton();
	else if (token=="DEFPUSHBUTTON")
	    ParsePushButton();
	else if (token=="GROUPBOX")
	    ParseGroupBox();
	else if (token=="COMBOBOX")
	    ParseComboBox();
	else if (token=="CONTROL")
	    ParseControlMS();
	else if (token=="LISTBOX")
	    ParseListBox();
	else if (token=="ICON")
		ParseIconStatic();
	else if (token=="SCROLLBAR")
		ParseScrollBar();
	token=GetToken();
	}

}
//LTEXT           "Radius",IDC_STATIC,9,67,23,8
void wxRC2XML::ParseStaticText()
{
wxString token;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();
int x,y,width,height;
ReadRect(x,y,width,height);

m_xmlfile.Write("		<statictext");
WriteBasicInfo(x,y,width,height,varname);WriteLabel(phrase);
m_xmlfile.Write("		</statictext>\n");

}
//EDITTEXT        IDC_RADIUS,36,65,40,14,ES_AUTOHSCROLL
void wxRC2XML::ParseTextCtrl()
{
wxString token;
wxString varname,style;
varname=GetToken();
int x,y,width,height;
ReadRect(x,y,width,height);
//TODO
//style=GetToken();

m_xmlfile.Write("		<textctrl");
WriteBasicInfo(x,y,width,height,varname);
m_xmlfile.Write("\n		</textctrl>\n");

}
//PUSHBUTTON      "Create/Update",IDC_CREATE,15,25,53,13,NOT WS_TABSTOP
void wxRC2XML::ParsePushButton()
{
wxString token;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();


int x,y,width,height;
ReadRect(x,y,width,height);

m_xmlfile.Write("		<button");
WriteBasicInfo(x,y,width,height,varname);
WriteLabel(phrase);
m_xmlfile.Write("		</button>\n");

}


bool wxRC2XML::Seperator(int ch)
{
//if ((ch==' ')|(ch==',')|(ch==13)|(ch==10)|(ch=='|')|(ch=='\t'))
if ((ch==' ')|(ch==',')|(ch==13)|(ch==10)|(ch=='\t'))
   return TRUE;

if (ch==EOF)
{
	m_done=TRUE;
	return TRUE;
}
return FALSE;
}

void wxRC2XML::ParseGroupBox()
{
//    GROUPBOX        "Rotate",IDC_STATIC,1,1,71,79
wxString token;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();
int x,y,width,height;
ReadRect(x,y,width,height);

m_xmlfile.Write("		<staticbox");
WriteBasicInfo(x,y,width,height,varname);
WriteLabel(phrase);
m_xmlfile.Write("		</staticbox>\n");


}

void wxRC2XML::ReadRect(int & x, int & y, int & width, int & height)
{
x=atoi(GetToken());
y=atoi(GetToken());
width=atoi(GetToken());
height=atoi(GetToken());

}

wxString wxRC2XML::GetToken()
{
wxString token="";

if (m_rc.Eof())
{
m_done=TRUE;
return token;
}

int ch=0;
ReadChar(ch);
if (ch==EOF)
{
m_done=TRUE;
return token;
}

while (Seperator(ch))
{
  ReadChar(ch);
  if (m_done)
	  return token;
}

if (ch==EOF)
{
	m_done=TRUE;

}
  

while (!Seperator(ch))
{
	token+=(char)ch;
	ReadChar(ch);

}

if (ch==EOF)
   m_done=TRUE;


return token;
}

wxString wxRC2XML::GetQuoteField()
{
wxString phrase;
//ASCII code 34 "
int ch=0;
ReadChar(ch);

while (ch!=34)
  ReadChar(ch);
  
  ReadChar(ch);

while (ch!=34)
{
  phrase+=(char)ch;
  ReadChar(ch);
}
return phrase;
}

void wxRC2XML::ReadChar(int &ch)
{
	int result;
result=m_rc.Tell();

if((result>=m_filesize))
    m_done=TRUE;

result=m_rc.Read(&ch,1);

if((result==-1))
   m_done=TRUE;

if(ch==EOF)
   m_done=TRUE;
}

void wxRC2XML::ParseComboBox()
{
/* COMBOBOX        IDC_SCALECOMBO,10,110,48,52,CBS_DROPDOWNLIST | CBS_SORT | 
                    WS_VSCROLL | WS_TABSTOP */
wxString token;
wxString varname;
varname=GetToken();
int x,y,width,height;
ReadRect(x,y,width,height);

m_xmlfile.Write("		<combobox");
WriteBasicInfo(x,y,width,height,varname);
m_xmlfile.Write("\n		</combobox>\n");



}

void wxRC2XML::ParseMenu(wxString varname)
{
wxString token="";

//Write menubar to xml file
m_xmlfile.Write("	<menubar");
//Avoid duplicate names this way
varname.Replace("IDR_","MB_");
WriteName(varname);
m_xmlfile.Write(">\n");
m_xmlfile.Write("		<children>\n");

while ((token!="BEGIN")&(token!="{"))
   token=GetToken();

while ((token!="END")&(token!="}"))
{
   token=GetToken();
if (token=="POPUP")
	{
    ParsePopupMenu();
	}
}
m_xmlfile.Write("		</children>\n");
m_xmlfile.Write("	</menubar>\n");
}

void wxRC2XML::ParsePopupMenu()
{
static menucount=0;
menucount++;
wxString token,name,msg,longhelp,tip;
token=GetQuoteField();
int spot;
//Remove /t because it causes problems

spot=token.First("\\t");
token=token.Left(spot);

//Write Menu item
//Generate a fake name since RC menus don't have one
name<<"Menu_"<<menucount;
m_xmlfile.Write("		<menu");
WriteName(name);
m_xmlfile.Write(">\n");
WriteLabel(token);
m_xmlfile.Write("			<children>\n");

while ((token!="BEGIN")&(token!="{"))
   token=GetToken();

while ((token!="END")&(token!="}"))
	{
	token=GetToken();
	
	if (token=="POPUP")
	    ParsePopupMenu();
	
	if (token=="MENUITEM")
		ParseMenuItem();
	
	}
m_xmlfile.Write("			</children>\n");
m_xmlfile.Write("		</menu>\n");
}

wxString wxRC2XML::PeekToken()
{
wxString token;
int p;
p=m_rc.Tell();
token=GetToken();

m_rc.Seek(p);
return token;
}
//MS Windows pain in the butt CONTROL
void wxRC2XML::ParseControlMS()
{
wxString label,varname,kindctrl,token;
token=PeekToken();

if (token.Contains("\""))
	ParseNormalMSControl();
else
	ParseWeirdMSControl();

}

/*    CONTROL         "Slider1",IDC_SLIDER1,"msctls_trackbar32",TBS_BOTH | 
                    TBS_NOTICKS | WS_TABSTOP,52,73,100,15
*/

void wxRC2XML::ParseSlider(wxString label, wxString varname)
{
wxString token;
while (ReadOrs(token));
//TODO Handle styles
//fprintf(m_wxrfile,"  control = [%i,wxSlider,'','wxSL_HORIZONTAL','%s',",m_controlid,varname);  
int x,y,width,height;
ReadRect(x,y,width,height);
m_xmlfile.Write("		<slider");
WriteBasicInfo(x,y,width,height,varname);
m_xmlfile.Write("\n		</slider>\n");

}
/*    
CONTROL         "Progress1",CG_IDC_PROGDLG_PROGRESS,"msctls_progress32",
                    WS_BORDER,15,52,154,13
*/
void wxRC2XML::ParseProgressBar(wxString label, wxString varname)
{
wxString token;
while (ReadOrs(token));
int x,y,width,height;
ReadRect(x,y,width,height);
wxString style;
//Always horizontal in MFC
style+="wxGA_HORIZONTAL";
m_xmlfile.Write("		<gauge");
WriteBasicInfo(x,y,width,height,varname);
WriteStyle(style);
m_xmlfile.Write("		</gauge>\n");
}

/* FIXME: this function needs to be rewritten */
bool wxRC2XML::ReadOrs(wxString & w)
{
wxString token;
token=PeekToken();

if (token.IsNumber())
   return FALSE;

w=GetToken();
return TRUE;
}

//Is it a check button or a radio button
void wxRC2XML::ParseCtrlButton(wxString label, wxString varname)
{
wxString token;
	token=GetToken();
	int x,y,width,height;

if (token=="BS_AUTOCHECKBOX")
	{
    while (ReadOrs(token));	
	ReadRect(x,y,width,height);
	m_xmlfile.Write("		<checkbox");
	WriteBasicInfo(x,y,width,height,varname);
	WriteLabel(label);
	m_xmlfile.Write("		</checkbox>\n");
	}

if (token=="BS_AUTORADIOBUTTON")
	{
    while(ReadOrs(token));	
	ReadRect(x,y,width,height);
	m_xmlfile.Write("		<radiobutton");
	WriteBasicInfo(x,y,width,height,varname);
	WriteLabel(label);
	m_xmlfile.Write("		</radiobutton>\n");
    }



}


void wxRC2XML::WriteSize(int width, int height)
{
wxString msg;
msg<<" <size>"<<width<<","<<height<<"d</size>";
m_xmlfile.Write(msg);
}

void wxRC2XML::WritePosition(int x, int y)
{
wxString msg;
msg<<" <pos>"<<x<<","<<y<<"d</pos>";
m_xmlfile.Write(msg);
}

void wxRC2XML::WriteTitle(wxString title)
{
wxString msg;
msg=_T("		<title>"+title+"</title>\n");
m_xmlfile.Write(msg);
}

void wxRC2XML::WriteName(wxString name)
{
	
//Replace common MS ids with wxWindows ids
//I didn't do everyone of them
if (name=="IDOK")
	name="wxID_OK";
else if (name=="IDCANCEL")
	name="wxID_CANCEL";
else if (name=="IDAPPLY")
	name="wxID_APPLY";
else if (name=="ID_FILE_OPEN")
	name="wxID_OPEN";
else if (name=="ID_FILE_CLOSE")
	name="wxID_CLOSE";
else if (name=="ID_FILE_SAVE")
	name="wxID_SAVE";
else if (name=="ID_FILE_SAVE_AS")
	name="wxID_SAVEAS";
else if (name=="ID_APP_EXIT")
	name="wxID_EXIT";
else if (name=="ID_FILE_PRINT")
	name="wxID_PRINT";
else if (name=="ID_FILE_PRINT_PREVIEW")
	name="wxID_PREVIEW";
else if (name=="ID_FILE_PRINT_SETUP")
	name="wxID_PRINT_SETUP";
else if (name=="ID_APP_ABOUT")
	name="wxID_ABOUT";
else if (name=="ID_EDIT_UNDO")
	name="wxID_UNDO";
else if (name=="ID_EDIT_CUT")
	name="wxID_CUT";
else if (name=="ID_EDIT_COPY")
	name="wxID_COPY";
else if (name=="ID_EDIT_PASTE")
	name="wxID_PASTE";

m_xmlfile.Write(" name= \""+name+"\"");
}

void wxRC2XML::WriteLabel(wxString label)
{
label.Replace("&","$");
m_xmlfile.Write("			<label>"+label+"</label>\n");
}

void wxRC2XML::WriteBasicInfo(int x, int y, int width, int height, wxString name)
{
WriteName(name);
m_xmlfile.Write(">\n");
m_xmlfile.Write("		");
WritePosition(x,y);
WriteSize(width,height);
m_xmlfile.Write("\n");

}

void wxRC2XML::WriteStyle(wxString style)
{
if (style.Length()==0)
	return;
m_xmlfile.Write("\n		<style>"+style+"</style>\n");
}
/*
    LISTBOX         IDC_LIST1,16,89,48,40,LBS_SORT | LBS_MULTIPLESEL | 
                    LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP
*/
void wxRC2XML::ParseListBox()
{
wxString token;
wxString varname;

varname=GetToken();
int x,y,width,height;
ReadRect(x,y,width,height);

m_xmlfile.Write("		<listbox");
WriteBasicInfo(x,y,width,height,varname);
m_xmlfile.Write("\n		</listbox>\n");

}
/*
    CONTROL         "",IDC_RICHEDIT1,"RICHEDIT",ES_AUTOHSCROLL | WS_BORDER | 
                    WS_TABSTOP,103,110,40,14
*/
void wxRC2XML::ParseRichEdit(wxString label, wxString varname)
{
wxString token;
while (ReadOrs(token));
int x,y,width,height;
ReadRect(x,y,width,height);
wxString style;
//Make it a rich text control
style+="wxTE_MULTILINE ";
m_xmlfile.Write("		<textctrl");
WriteBasicInfo(x,y,width,height,varname);
WriteStyle(style);
m_xmlfile.Write("		</textctrl>\n");

}
/*
CONTROL         "Spin1",IDC_SPIN1,"msctls_updown32",UDS_ARROWKEYS,209,72,
                 19,26
*/
void wxRC2XML::ParseSpinCtrl(wxString label, wxString varname)
{
wxString token;
while (ReadOrs(token));
int x,y,width,height;
ReadRect(x,y,width,height);
wxString style;
m_xmlfile.Write("		<spinbutton");
WriteBasicInfo(x,y,width,height,varname);
WriteStyle(style);
m_xmlfile.Write("\n		</spinbutton>\n");


}

void wxRC2XML::FirstPass()
{
wxString token,prevtok;
while (!m_done)
	{
	token=GetToken();
	if (token=="BITMAP")
		ParseBitmap(prevtok);
	else if (token=="STRINGTABLE")
		ParseStringTable(prevtok);
	else if (token=="ICON")
		ParseIcon(prevtok);
	prevtok=token;
	}
}

void wxRC2XML::ParseBitmap(wxString varname)
{
wxString token,*bitmapfile;
bitmapfile=new wxString;
token=PeekToken();
//Microsoft notation?
if (token=="DISCARDABLE")
	{
	token=GetToken();
	token=PeekToken();
	}

*bitmapfile=GetQuoteField();
m_bitmaplist->Append(varname,bitmapfile);

}


void wxRC2XML::SecondPass()
{
wxString token,prevtok;
while (!m_done)
	{
	token=GetToken();
	if (token=="DIALOG")
		ParseDialog(prevtok);
	else if (token=="MENU")
		ParseMenu(prevtok);
	else if (token=="TOOLBAR")
		ParseToolBar(prevtok);
	prevtok=token;
	}

}

void wxRC2XML::ParseToolBar(wxString varname)
{
wxString token;	
token=GetToken();
wxASSERT_MSG(token=="DISCARDABLE","Error in toolbar parsing");
	
//Look up bitmap for toolbar and load
wxNode *node=m_bitmaplist->Find(varname);
wxString *bitmappath;
bitmappath=(wxString *)node->Data();
wxBitmap bitmap;
if (!bitmap.LoadFile(*bitmappath,wxBITMAP_TYPE_BMP ))
	wxLogError("Unable to load bitmap:"+*bitmappath);

//Write toolbar to xml file
m_xmlfile.Write("	<toolbar");
//Avoid duplicate names this way
varname.Replace("IDR_","TB_");
WriteName(varname);
m_xmlfile.Write(">\n");
wxString style;
style+="wxTB_FLAT";
WriteStyle(style);

m_xmlfile.Write("		<children>\n");

//Grab width and height
int width,height;
width=atoi(GetToken());
height=atoi(GetToken());

int c=0;	
wxString buttonname,msg,tip,longhelp;
token=GetToken();
while ((token!="BEGIN"))
	token=GetToken();

while ((token!="END")&(token!="}"))
	{
	if (token=="BUTTON")
		{
		buttonname=GetToken();
		m_xmlfile.Write("			<tool");
		WriteName(buttonname);
		m_xmlfile.Write(">\n");
		//Write tool tip if any
		if (LookUpString(buttonname,msg))
			{
			SplitHelp(msg,tip,longhelp);
			m_xmlfile.Write("		<tooltip>"+tip+"</tooltip>\n");
			m_xmlfile.Write("		<longhelp>"+longhelp+"</longhelp>\n");
			}
		//Make a bitmap file name
		buttonname=CleanName(buttonname);
		buttonname+=".bmp";
		m_xmlfile.Write("			<bitmap>"+buttonname+"</bitmap>\n");
		WriteToolButton(buttonname,c,width,height,bitmap);
		m_xmlfile.Write("			</tool>\n");
		c++;
		}
	  else if (token=="SEPARATOR")
		{
		m_xmlfile.Write("			<separator/>\n");
		}
	token=GetToken();
	}
m_xmlfile.Write("	</children>\n");
m_xmlfile.Write("	</toolbar>\n");
}

//Extract bitmaps from larger toolbar bitmap
void wxRC2XML::WriteToolButton(wxString name,int index, int width, int height, wxBitmap bitmap)
{
int x;
x=index*width;
wxRect r(x,0,width,height);
wxBitmap little;

little=bitmap.GetSubBitmap(r);
little.SaveFile(name,wxBITMAP_TYPE_BMP);
}

void wxRC2XML::ParseStringTable(wxString varname)
{
wxString token;
token=GetToken();
while ((token!="BEGIN"))
	token=GetToken();
token=GetToken();
wxString *msg;

while ((token!="END")&(token!="}"))
	{
	msg=new wxString;
	*msg=GetQuoteField();
	m_stringtable->Append(token,msg);
	token=GetToken();
	}

}

bool wxRC2XML::LookUpString(wxString strid,wxString & st)
{
wxNode *node=m_stringtable->Find(strid);
wxString *s;
if (node==NULL)
	return FALSE;

s=(wxString *)node->Data();
st=*s;

return TRUE;
}

bool wxRC2XML::SplitHelp(wxString msg, wxString &shorthelp, wxString &longhelp)
{
int spot;
spot=msg.Find("\\n");
if (spot==-1)
	{
	shorthelp=msg;
	longhelp=msg;
	}

longhelp=msg.Left(spot);
spot=msg.Length()-spot-2;
shorthelp=msg.Right(spot);
return TRUE;
}

void wxRC2XML::ParseMenuItem()
{
wxString token,name,msg,tip,longhelp;
int spot;
if (PeekToken()=="SEPARATOR")
	{
	m_xmlfile.Write("			<separator/>\n");
	return;
	}

token=GetQuoteField();
name=GetToken();
//Remove /t because it causes problems
spot=token.First("\\t");
token=token.Left(spot);
m_xmlfile.Write("			<menuitem");
WriteName(name);
m_xmlfile.Write(">\n");
WriteLabel(token);
//Look up help if any listed in stringtable
if (LookUpString(name,msg))
	{
	SplitHelp(msg,tip,longhelp);
	m_xmlfile.Write("			<help>"
		+longhelp+"</help>\n");
	}
//look for extra attributes like checked and break
wxString ptoken;
ptoken=PeekToken();
while ((ptoken!="MENUITEM")&(ptoken!="POPUP")&(ptoken!="END"))
	{
	token=GetToken();
	if (token=="CHECKED")
		m_xmlfile.Write("			<checkable>1</checkable>\n");
	else if (token=="MENUBREAK");
		//m_xmlfile.Write("			</break>\n");
	else if (token=="GRAYED");
	else
		wxLogError("Unknown Menu Item token:"+token);
	ptoken=PeekToken();
	}
m_xmlfile.Write("			</menuitem>\n"); 

}

//ICON            IDR_MAINFRAME,IDC_STATIC,11,17,20,20
void wxRC2XML::ParseIconStatic()
{
wxString token;
wxString varname,iconname;
iconname=GetToken();
//Look up icon
wxNode *node=m_iconlist->Find(iconname);
wxString *iconpath;
iconpath=(wxString *)node->Data();
wxIcon icon;
wxBitmap bitmap;
if (!icon.LoadFile(*iconpath,wxBITMAP_TYPE_ICO ))
	wxLogError("Unable to load icon:"+*iconpath);
bitmap.CopyFromIcon(icon);
varname=GetToken();

int x,y,width,height;
ReadRect(x,y,width,height);

m_xmlfile.Write("		<staticbitmap");
WriteBasicInfo(x,y,width,height,varname);
//Save icon as a bitmap
//Make a bitmap file name
iconname=CleanName(iconname);
iconname+=".bmp";
m_xmlfile.Write("			<bitmap>"+iconname+"</bitmap>\n");
bitmap.SaveFile(iconname,wxBITMAP_TYPE_BMP);
m_xmlfile.Write("		</staticbitmap>\n");

}
//IDR_MAINFRAME           ICON    DISCARDABLE     "res\\mfcexample.ico"
void wxRC2XML::ParseIcon(wxString varname)
{
wxString token,*iconfile;
iconfile=new wxString;
token=PeekToken();

*iconfile=GetQuoteField();
m_iconlist->Append(varname,iconfile);

}

wxString wxRC2XML::CleanName(wxString name)
{
name.MakeLower();
name.Replace("id_","");
name.Replace("idr_","");
name.Replace("idb_","");
name.Replace("idc_","");
return name;
}
// And the award for most messed up control goes to...
//    CONTROL         IDB_FACE,IDC_STATIC,"Static",SS_BITMAP,26,62,32,30
void wxRC2XML::ParseStaticBitmap(wxString bitmapname, wxString varname)
{
wxString token;
//Grab SS_BITMAP
token=GetToken();

//Look up bitmap
wxNode *node=m_bitmaplist->Find(bitmapname);
wxString *bitmappath;
bitmappath=(wxString *)node->Data();
wxBitmap bitmap;
if (!bitmap.LoadFile(*bitmappath,wxBITMAP_TYPE_BMP ))
	wxLogError("Unable to load bitmap:"+*bitmappath);

int x,y,width,height;
ReadRect(x,y,width,height);

m_xmlfile.Write("		<staticbitmap");
WriteBasicInfo(x,y,width,height,varname);
//Make a bitmap file name
bitmapname=CleanName(bitmapname);
bitmapname+=".bmp";
m_xmlfile.Write("			<bitmap>"+bitmapname+"</bitmap>\n");
bitmap.SaveFile(bitmapname,wxBITMAP_TYPE_BMP);
m_xmlfile.Write("		</staticbitmap>\n");


}

void wxRC2XML::ParseNormalMSControl()
{
wxString label,varname,kindctrl;

label=GetQuoteField();
varname=GetToken();
kindctrl=GetQuoteField();
kindctrl.MakeUpper();

if (kindctrl=="MSCTLS_UPDOWN32")
   ParseSpinCtrl(label,varname);
if (kindctrl=="MSCTLS_TRACKBAR32")
   ParseSlider(label,varname);
if (kindctrl=="MSCTLS_PROGRESS32")
   ParseProgressBar(label,varname);
if (kindctrl=="SYSTREEVIEW32")
   ParseTreeCtrl(label,varname);
if (kindctrl=="SYSMONTHCAL32")
   ParseCalendar(label,varname);
if (kindctrl=="SYSLISTVIEW32")
   ParseListCtrl(label,varname);
if (kindctrl=="BUTTON")
   ParseCtrlButton(label,varname);
if (kindctrl=="RICHEDIT")
	ParseRichEdit(label,varname);

}

void wxRC2XML::ParseWeirdMSControl()
{
wxString kindctrl;
wxString varname;
wxString id;
id=GetToken();
varname=GetToken();
kindctrl=GetQuoteField();
kindctrl.MakeUpper();
//    CONTROL         IDB_FACE,IDC_STATIC,"Static",SS_BITMAP,26,62,32,30
if (kindctrl=="STATIC")
	{
	if (PeekToken()=="SS_BITMAP")
		ParseStaticBitmap(id,varname);
	else
		wxLogError("Unknown MS Control Static token");
	}

}
//SCROLLBAR       IDC_SCROLLBAR1,219,56,10,40,SBS_VERT

void wxRC2XML::ParseScrollBar()
{
wxString token;
wxString varname;

varname=GetToken();
int x,y,width,height;
ReadRect(x,y,width,height);
wxString style;
//Default MFC style is horizontal
style=_T("wxSB_HORIZONTAL");
/*
while (ReadOrs(token))
{
if (token=="SBS_VERT")
	style=_T("wxSB_VERTICAL");
}
*/
m_xmlfile.Write("		<scrollbar");
WriteBasicInfo(x,y,width,height,varname);
WriteStyle(style);
m_xmlfile.Write("\n		</scrollbar>\n");


}
//    CONTROL         "Tree1",IDC_TREE1,"SysTreeView32",WS_BORDER | WS_TABSTOP,
//                      7,7,66,61

void wxRC2XML::ParseTreeCtrl(wxString label, wxString varname)
{
wxString token;
while (ReadOrs(token));
int x,y,width,height;
ReadRect(x,y,width,height);
m_xmlfile.Write("		<treectrl");
WriteBasicInfo(x,y,width,height,varname);
m_xmlfile.Write("		</treectrl>\n");

}
//    CONTROL         "MonthCalendar1",IDC_MONTHCALENDAR1,"SysMonthCal32",
                    //MCS_NOTODAY | WS_TABSTOP,105,71,129,89

void wxRC2XML::ParseCalendar(wxString label, wxString varname)
{
wxString token;
while (ReadOrs(token));
int x,y,width,height;
ReadRect(x,y,width,height);
m_xmlfile.Write("		<calendarctrl");
WriteBasicInfo(x,y,width,height,varname);
m_xmlfile.Write("		</calendarctrl>\n");
}
//    CONTROL         "List1",IDC_LIST1,"SysListView32",WS_BORDER | WS_TABSTOP,
  //                  7,89,68,71

void wxRC2XML::ParseListCtrl(wxString label, wxString varname)
{
wxString token;
while (ReadOrs(token));
int x,y,width,height;
ReadRect(x,y,width,height);
m_xmlfile.Write("		<listctrl");
WriteBasicInfo(x,y,width,height,varname);
m_xmlfile.Write("		</listctrl>\n");

}
