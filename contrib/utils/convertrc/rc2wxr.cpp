// RC2WXR.cpp: implementation of the wxRC2WXR class.
//
//////////////////////////////////////////////////////////////////////
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "rc2wxr.h"
#include "wx/image.h"
#include "wx/resource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxRC2WXR::wxRC2WXR()
{
m_done=FALSE;
m_controlid=6000;
}

wxRC2WXR::~wxRC2WXR()
{

}

void wxRC2WXR::Open(wxString wxrfile, wxString rcfile)
{
    wxFileProgressDlg fileprog;


	m_rc.Open(rcfile);
	m_filesize=m_rc.Length();
if( (m_wxr  = fopen( wxrfile, "wt" )) == NULL )
{
	  return;
}

fileprog.Show(TRUE);

wxString tok,prevtok;


while (!m_done)
{

tok=GetToken();

if (tok=="DIALOG")
{
	ParseDialog(prevtok);
	fileprog.UpdateProgress(&m_rc);
}
	

if (tok=="MENU")
{
	ParseMenu(prevtok);
	fileprog.UpdateProgress(&m_rc);
}	

prevtok=tok;
}
fileprog.UpdateProgress(&m_rc);
fclose(m_wxr);
//fclose(m_rc);  
m_rc.Close();

fileprog.Show(FALSE);
}


/*
Example .rc 
Microsoft style as of v5.0
IDD_ABOUTBOX DIALOG DISCARDABLE  0, 0, 217, 55
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "About Funimator"
FONT 8, "MS Sans Serif"

  Borland 4.5 style rc
IDD_DIBATTR DIALOG 7, 16, 172, 119
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "DIB Attributes"
FONT 8, "MS Sans Serif"
{
 DEFPUSHBUTTON "Ok", IDOK, 114, 8, 50, 14
 PUSHBUTTON "Cancel", IDCANCEL, 114, 28, 50, 14



*/
void wxRC2WXR::ParseDialog(wxString dlgname)
{
	wxString tok;
	static int dlgid=999;
	dlgid++;
	/* Make sure that this really is a dialog 
	microsoft reuses the keyword DIALOG for other things
	*/
	tok=PeekToken();
	//Microsoft notation?
	if (tok=="DISCARDABLE")
	{
		tok=GetToken();
		tok=PeekToken();
	}
	//This isn't a Dialog resource eject eject
	if (!tok.IsNumber())
		return;
//Generate Dialog text
fprintf(m_wxr,"static char *dialog%i = \"dialog(name = '%s',\\\n",dlgid,dlgname);
//be lazy about style for now. add it later
fprintf(m_wxr,"style = 'wxRAISED_BORDER | wxCAPTION | wxTHICK_FRAME | wxSYSTEM_MENU',\\\n");

fprintf(m_wxr,"id = %i,\\\n",dlgid);

//Record x,y,width,height
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"x = %i, y = %i, width = %i, height = %i,\\\n",x,y,width,height);


//CAPTION "About Funimator"
//Get Title
tok=GetToken();
wxString title;

while ((tok!="BEGIN")&(tok!="{"))
{
if (tok=="CAPTION")
	{
	title=GetQuoteField();
	fprintf(m_wxr,"title = '%s',\\\n",title);
	}
tok=GetToken();
}
fprintf(m_wxr,"use_dialog_units = 1,\\\n");
fprintf(m_wxr,"use_system_defaults = 0,\\\n");

fprintf(m_wxr,"font = [8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif'],\\\n");
ParseControls();
fprintf(m_wxr,").\";\n\n");
}

/*
BEGIN



    EDITTEXT        IDC_BANDS,36,83,22,14,ES_AUTOHSCROLL | ES_NUMBER | NOT 
                    WS_TABSTOP
    LTEXT           "Bands",IDC_STATIC,11,86,21,8
    EDITTEXT        IDC_NAME,10,3,75,14,ES_AUTOHSCROLL
END
*/
void wxRC2WXR::ParseControls()
{
wxString tok;

tok=GetToken();
while ((tok!="END")&(tok!="}"))
{
if (tok=="LTEXT")
    ParseStaticText();
if (tok=="EDITTEXT")
    ParseTextCtrl();
if (tok=="PUSHBUTTON")
    ParsePushButton();
if (tok=="DEFPUSHBUTTON")
    ParsePushButton();
if (tok=="GROUPBOX")
    ParseGroupBox();
if (tok=="COMBOBOX")
    ParseComboBox();
if (tok=="CONTROL")
    ParseControlMS();

tok=GetToken();
}

}
//LTEXT           "Radius",IDC_STATIC,9,67,23,8
void wxRC2WXR::ParseStaticText()
{
wxString tok;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"  control = [%i,wxStaticText,'%s','0','%s',",m_controlid,phrase,varname);
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}
//EDITTEXT        IDC_RADIUS,36,65,40,14,ES_AUTOHSCROLL
void wxRC2WXR::ParseTextCtrl()
{
wxString tok;
wxString varname;
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"  control = [%i,wxTextCtrl,'','0','%s',",m_controlid,varname);
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");

}
//PUSHBUTTON      "Create/Update",IDC_CREATE,15,25,53,13,NOT WS_TABSTOP
void wxRC2WXR::ParsePushButton()
{
wxString tok;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();
int c;
m_controlid++;
c=m_controlid;
if (varname=="IDOK")
c=wxID_OK;

if (varname=="IDCANCEL")
c=wxID_CANCEL;

if (varname=="IDAPPLY")
c=wxID_APPLY;

int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"  control = [%i,wxButton,'%s','0','%s',",c,phrase,varname);
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");

}


bool wxRC2WXR::Seperator(int ch)
{
if ((ch==' ')|(ch==',')|(ch==13)|(ch==10)|(ch=='|'))
   return TRUE;

if (ch==EOF)
{
	m_done=TRUE;
	return TRUE;
}
return FALSE;
}

void wxRC2WXR::ParseGroupBox()
{
//    GROUPBOX        "Rotate",IDC_STATIC,1,1,71,79
wxString tok;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"  control = [%i,wxStaticBox,'%s','0','%s',",m_controlid,phrase,varname);
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");


}

void wxRC2WXR::ReadRect(int & x, int & y, int & width, int & height)
{
x=atoi(GetToken());
y=atoi(GetToken());
width=atoi(GetToken());
height=atoi(GetToken());

}

wxString wxRC2WXR::GetToken()
{
wxString tok="";

if (m_rc.Eof())
{
m_done=TRUE;
return tok;
}

int ch=0;
ReadChar(ch);
if (ch==EOF)
{
m_done=TRUE;
return tok;
}

while (Seperator(ch))
{
  ReadChar(ch);
  if (m_done)
	  return tok;
}

if (ch==EOF)
{
	m_done=TRUE;

}
  

while (!Seperator(ch))
{
	tok+=(char)ch;
	ReadChar(ch);

}

if (ch==EOF)
   m_done=TRUE;


return tok;
}

wxString wxRC2WXR::GetQuoteField()
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

void wxRC2WXR::ReadChar(int &ch)
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

void wxRC2WXR::ParseComboBox()
{
/* COMBOBOX        IDC_SCALECOMBO,10,110,48,52,CBS_DROPDOWNLIST | CBS_SORT | 
                    WS_VSCROLL | WS_TABSTOP */
wxString tok;
wxString varname;
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);

fprintf(m_wxr,"  control = [%i,wxChoice,'','0','%s',",m_controlid,varname);
fprintf(m_wxr,"%i,%i,%i,%i,[],\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");


}

void wxRC2WXR::ParseMenu(wxString name)
{
wxString tok="";
static int menuid=0;
menuid++;
fprintf(m_wxr,"static char *MenuBar%i = \"menu(name = '%s',\\\n",menuid,name);  
fprintf(m_wxr,"menu = \\\n");
fprintf(m_wxr,"[\\\n");

while ((tok!="BEGIN")&(tok!="{"))
   tok=GetToken();

while ((tok!="END")&(tok!="}"))
{
   tok=GetToken();
if (tok=="POPUP")
	{
    ParsePopupMenu();
	fprintf(m_wxr,"  ],\\\n");
	}
}

fprintf(m_wxr,"]).\";\n\n");
}

void wxRC2WXR::ParsePopupMenu()
{
static int menuitem=99;
menuitem++;

wxString tok;
tok=GetQuoteField();
int spot;
//Remove /t because it causes problems
spot=tok.First("\\t");
tok=tok.Left(spot);
fprintf(m_wxr,"  ['%s',%i,'',\\\n",tok,menuitem);
while ((tok!="BEGIN")&(tok!="{"))
   tok=GetToken();

while ((tok!="END")&(tok!="}"))
{
   tok=GetToken();
if (tok=="MENUITEM")
	{
	if (PeekToken()=="SEPARATOR")
		fprintf(m_wxr,"      [],\\\n");
	else
	{
	tok=GetQuoteField();
	//Remove /t because it causes problems
	spot=tok.First("\\t");
	tok=tok.Left(spot);
	menuitem++;
    fprintf(m_wxr,"      ['%s',%i,''],\\\n",tok,menuitem);
	}
	}

}

    
}

wxString wxRC2WXR::PeekToken()
{
wxString tok;
int p;
p=m_rc.Tell();
tok=GetToken();

m_rc.Seek(p);
return tok;
}
//Windows pain in the butt CONTROL
void wxRC2WXR::ParseControlMS()
{
wxString label,varname,kindctrl,tok;
label=GetQuoteField();
varname=GetToken();
kindctrl=GetQuoteField();
kindctrl.MakeUpper();


if (kindctrl=="MSCTLS_TRACKBAR32")
   ParseSlider(label,varname);
if (kindctrl=="MSCTLS_PROGRESS32")
   ParseProgressBar(label,varname);
if (kindctrl=="BUTTON")
   ParseCtrlButton(label,varname);
}
/*    CONTROL         "Slider1",IDC_SLIDER1,"msctls_trackbar32",TBS_BOTH | 
                    TBS_NOTICKS | WS_TABSTOP,52,73,100,15
*/

void wxRC2WXR::ParseSlider(wxString label, wxString varname)
{
	wxString tok;
	while (ReadOrs(tok));
fprintf(m_wxr,"  control = [%i,wxSlider,'','wxSL_HORIZONTAL','%s',",m_controlid,varname);  
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"%i,%i,%i,%i,",x,y,width,height);
fprintf(m_wxr," 1, 1, 10,\\\n");
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}
/*    
CONTROL         "Progress1",CG_IDC_PROGDLG_PROGRESS,"msctls_progress32",
                    WS_BORDER,15,52,154,13
*/
void wxRC2WXR::ParseProgressBar(wxString label, wxString varname)
{
wxString tok;
while (ReadOrs(tok));
fprintf(m_wxr,"  control = [%i,wxGauge,'','wxGA_HORIZONTAL','%s',",m_controlid,varname);  
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"%i,%i,%i,%i,",x,y,width,height);
fprintf(m_wxr," 0, 10,\\\n");
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}

bool wxRC2WXR::ReadOrs(wxString & w)
{
wxString tok;
tok=PeekToken();
if (tok.IsNumber())
   return false;
w=GetToken();
return TRUE;
}

//Is it a check button or a radio button
void wxRC2WXR::ParseCtrlButton(wxString label, wxString varname)
{
wxString tok;
	tok=GetToken();

m_controlid++;
	int x,y,width,height;

if (tok=="BS_AUTOCHECKBOX")
{
    fprintf(m_wxr,"  control = [%i,wxCheckBox,'%s','0','%s',",m_controlid,label,varname);    
    while (ReadOrs(tok));	
	ReadRect(x,y,width,height);
    fprintf(m_wxr,"%i,%i,%i,%i,0,\\\n",x,y,width,height);
    fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}

if (tok=="BS_AUTORADIOBUTTON")
{
    fprintf(m_wxr,"  control = [%i,wxRadioButton,'%s','0','%s',",m_controlid,label,varname);    
    while(ReadOrs(tok));	
	ReadRect(x,y,width,height);
    fprintf(m_wxr,"%i,%i,%i,%i,0,\\\n",x,y,width,height);
    fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}



}

BEGIN_EVENT_TABLE(wxFileProgressDlg,wxDialog)
END_EVENT_TABLE()

wxFileProgressDlg::wxFileProgressDlg()
{
wxPoint pos;
wxSize size;
pos = ConvertDialogToPixels(wxPoint(10,10));
size = ConvertDialogToPixels(wxSize(170,31));
Create(GetParent(),100,"Parsing RC File",pos,size,603985920);
SetClientSize(size);
Move(pos);
//wxGauge Control
pos = ConvertDialogToPixels(wxPoint(16,16));
size = ConvertDialogToPixels(wxSize(136,6));
m_pProgress = new wxGauge(this,101,100,pos,size);
//wxStaticText Control
pos = ConvertDialogToPixels(wxPoint(72,4));
size = ConvertDialogToPixels(wxSize(18,6));
m_pCompleteLabel= new wxStaticText(this,102,"0",pos,size,0);
}
wxFileProgressDlg::~wxFileProgressDlg()
{

}

void wxFileProgressDlg::UpdateProgress(wxFile * f)
{
int p;
p=(int)((float)f->Tell()/(float)f->Length()*100.0);
m_pProgress->SetValue(p);
wxString t;
t.sprintf("%i%%",p);
m_pCompleteLabel->SetLabel(t);
Refresh();
}



//////////////////////////////////////////////////////////////////////
// GenerateBitmapSrc Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GenerateBitmapSrc::GenerateBitmapSrc()
{

}

GenerateBitmapSrc::~GenerateBitmapSrc()
{

}

bool GenerateBitmapSrc::Create(wxString imfile, wxString srcfile,wxString varname)
{

wxImage img;
FILE *src;

int h,w;

img.LoadFile(imfile,wxBITMAP_TYPE_ANY);
h=img.GetHeight();
w=img.GetWidth();

if( (src  = fopen( srcfile, "at" )) == NULL )
      return FALSE;
fprintf(src,"#if !defined(IMG_%s)\n",varname);
fprintf(src,"#define IMG_%s\n",varname);

fprintf(src,"//Data from bitmap file %s \n",imfile);
fprintf(src,"//Image Height=%i,Width=%i RGB format\n",h,w);
fprintf(src,"static unsigned char %s[][3]={\n",varname);


for (int y=0;y<h;y++)
{
	for (int x=0;x<w;x++)
	{
	//fprintf(src,"{%i,%i,%i},",img.GetRed(x,y),img.GetGreen(x,y),img.GetBlue(x,y));

	}
fprintf(src,"\n");
}	

fprintf(src,"};\n\n");

fprintf(src,"wxBitmap Load%s()\n{\n",varname);
fprintf(src,"wxImage myimg(%i,%i);\n",w,h);
int size=w*h*3;
fprintf(src,"memcpy(myimg.GetData(),&%s[0][0],%i);\n",varname,size);
fprintf(src,"return myimg.ConvertToBitmap();\n");
fprintf(src,"}\n");
fprintf(src,"#endif\n");
fclose(src);

return TRUE;
}

