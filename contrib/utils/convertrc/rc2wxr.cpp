// rc2wxr.cpp: implementation of the rc2wxr class.
//
//////////////////////////////////////////////////////////////////////
//Author:  Brian Gavin 9/24/00
//License: wxWindows License
/*
WARNING- I know this code has some bugs to work out but
I don't plan to fix them since I feel that wxr files will
not be used much longer.
This code was used as a starting point for my rc2xml converter
*/
#ifdef __GNUG__
#pragma implementation "rc2wxr.cpp"
#pragma interface "rc2wxr.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include "rc2wxr.h"
#include "wx/image.h"
#include "wx/resource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

rc2wxr::rc2wxr()
{
m_done=FALSE;
m_controlid=6000;
}

rc2wxr::~rc2wxr()
{

}

void rc2wxr::Convert(wxString wxrfile, wxString rcfile)
{
m_rc.Open(rcfile);
m_filesize=m_rc.Length();
if( (m_wxr  = fopen( wxrfile, "wt" )) == NULL )
{
  return;
}


wxString tok,prevtok;


while (!m_done)
{

tok=GetToken();

if (tok=="DIALOG")
{
ParseDialog(prevtok);
}
	

if (tok=="MENU")
{
ParseMenu(prevtok);
}	

prevtok=tok;
}

fclose(m_wxr);
//fclose(m_rc);  
m_rc.Close();

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
void rc2wxr::ParseDialog(wxString dlgname)
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
fprintf(m_wxr,"static char *dialog%i = \"dialog(name = '%s',\\\n",dlgid,dlgname.mb_str());
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
fprintf(m_wxr,"title = '%s',\\\n",title.mb_str());
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
void rc2wxr::ParseControls()
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
void rc2wxr::ParseStaticText()
{
wxString tok;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"  control = [%i,wxStaticText,'%s','0','%s',",m_controlid,phrase.mb_str(),varname.mb_str());
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}
//EDITTEXT        IDC_RADIUS,36,65,40,14,ES_AUTOHSCROLL
void rc2wxr::ParseTextCtrl()
{
wxString tok;
wxString varname;
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"  control = [%i,wxTextCtrl,'','0','%s',",m_controlid,varname.mb_str());
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");

}
//PUSHBUTTON      "Create/Update",IDC_CREATE,15,25,53,13,NOT WS_TABSTOP
void rc2wxr::ParsePushButton()
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
fprintf(m_wxr,"  control = [%i,wxButton,'%s','0','%s',",c,phrase.mb_str(),varname.mb_str());
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");

}


bool rc2wxr::Seperator(int ch)
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

void rc2wxr::ParseGroupBox()
{
//    GROUPBOX        "Rotate",IDC_STATIC,1,1,71,79
wxString tok;
wxString phrase,varname;
phrase=GetQuoteField();
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"  control = [%i,wxStaticBox,'%s','0','%s',",m_controlid,phrase.mb_str(),varname.mb_str());
fprintf(m_wxr,"%i,%i,%i,%i,'',\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");


}

void rc2wxr::ReadRect(int & x, int & y, int & width, int & height)
{
x=atoi(GetToken());
y=atoi(GetToken());
width=atoi(GetToken());
height=atoi(GetToken());

}

wxString rc2wxr::GetToken()
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

wxString rc2wxr::GetQuoteField()
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

void rc2wxr::ReadChar(int &ch)
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

void rc2wxr::ParseComboBox()
{
/* COMBOBOX        IDC_SCALECOMBO,10,110,48,52,CBS_DROPDOWNLIST | CBS_SORT | 
                    WS_VSCROLL | WS_TABSTOP */
wxString tok;
wxString varname;
varname=GetToken();
m_controlid++;
int x,y,width,height;
ReadRect(x,y,width,height);

fprintf(m_wxr,"  control = [%i,wxChoice,'','0','%s',",m_controlid,varname.mb_str());
fprintf(m_wxr,"%i,%i,%i,%i,[],\\\n",x,y,width,height);
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");


}

void rc2wxr::ParseMenu(wxString name)
{
wxString tok="";
static int menuid=0;
menuid++;
fprintf(m_wxr,"static char *MenuBar%i = \"menu(name = '%s',\\\n",menuid,name.mb_str());  
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

void rc2wxr::ParsePopupMenu()
{
static int menuitem=99;
menuitem++;

wxString tok;
tok=GetQuoteField();
int spot;
//Remove /t because it causes problems
spot=tok.First("\\t");
tok=tok.Left(spot);
fprintf(m_wxr,"  ['%s',%i,'',\\\n",tok.mb_str(),menuitem);
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
fprintf(m_wxr,"      ['%s',%i,''],\\\n",tok.mb_str(),menuitem);
}
}

}

    
}

wxString rc2wxr::PeekToken()
{
wxString tok;
int p;
p=m_rc.Tell();
tok=GetToken();

m_rc.Seek(p);
return tok;
}
//Windows pain in the butt CONTROL
void rc2wxr::ParseControlMS()
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

void rc2wxr::ParseSlider(wxString label, wxString varname)
{
wxString tok;
while (ReadOrs(tok));
fprintf(m_wxr,"  control = [%i,wxSlider,'','wxSL_HORIZONTAL','%s',",m_controlid,varname.mb_str());  
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
void rc2wxr::ParseProgressBar(wxString label, wxString varname)
{
wxString tok;
while (ReadOrs(tok));
fprintf(m_wxr,"  control = [%i,wxGauge,'','wxGA_HORIZONTAL','%s',",m_controlid,varname.mb_str());  
int x,y,width,height;
ReadRect(x,y,width,height);
fprintf(m_wxr,"%i,%i,%i,%i,",x,y,width,height);
fprintf(m_wxr," 0, 10,\\\n");
fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}

bool rc2wxr::ReadOrs(wxString & w)
{
wxString tok;
tok=PeekToken();
if (tok.IsNumber())
   return false;
w=GetToken();
return TRUE;
}

//Is it a check button or a radio button
void rc2wxr::ParseCtrlButton(wxString label, wxString varname)
{
wxString tok;
tok=GetToken();

m_controlid++;
int x,y,width,height;

if (tok=="BS_AUTOCHECKBOX")
{
    fprintf(m_wxr,"  control = [%i,wxCheckBox,'%s','0','%s',",m_controlid,label.mb_str(),varname.mb_str());    
    while (ReadOrs(tok));	
    ReadRect(x,y,width,height);
    fprintf(m_wxr,"%i,%i,%i,%i,0,\\\n",x,y,width,height);
    fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}

if (tok=="BS_AUTORADIOBUTTON")
{
    fprintf(m_wxr,"  control = [%i,wxRadioButton,'%s','0','%s',",m_controlid,label.mb_str(),varname.mb_str());    
    while(ReadOrs(tok));	
    ReadRect(x,y,width,height);
    fprintf(m_wxr,"%i,%i,%i,%i,0,\\\n",x,y,width,height);
    fprintf(m_wxr,"[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n");
}



}

