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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you

// need because it includes almost all "standard" wxWidgets headers

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/deprecated/setup.h"
#include "wx/deprecated/resource.h"

#include "rc2wxr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

rc2wxr::rc2wxr()
{
    m_done=false;
    m_controlid=6000;
}

rc2wxr::~rc2wxr()
{
}

void rc2wxr::Convert(wxString wxrfile, wxString rcfile)
{
    m_rc.Open(rcfile);
    m_filesize=m_rc.Length();
    if( (m_wxr  = wxFopen( wxrfile, _T("wt") )) == NULL )
    {
        return;
    }

    wxString tok,prevtok;

    while (!m_done)
    {
        tok=GetToken();

        if (tok==_T("DIALOG"))
        {
            ParseDialog(prevtok);
        }

        if (tok==_T("MENU"))
        {
            ParseMenu(prevtok);
        }

        prevtok=tok;
    }

    fclose(m_wxr);

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

if (tok==_T("DISCARDABLE"))

{

tok=GetToken();

tok=PeekToken();

}

//This isn't a Dialog resource eject eject

if (!tok.IsNumber())

    return;

//Generate Dialog text

wxFprintf(m_wxr,_T("static char *dialog%i = \"dialog(name = '%s',\\\n"),dlgid,dlgname.c_str());

//be lazy about style for now. add it later

wxFprintf(m_wxr,_T("style = 'wxRAISED_BORDER | wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU',\\\n"));
wxFprintf(m_wxr,_T("id = %i,\\\n"),dlgid);

//Record x,y,width,height

int x,y,width,height;

ReadRect(x,y,width,height);

wxFprintf(m_wxr,_T("x = %i, y = %i, width = %i, height = %i,\\\n"),x,y,width,height);


//CAPTION "About Funimator"

//Get Title

tok=GetToken();

wxString title;



while ((tok!=_T("BEGIN"))&(tok!=_T("{")))

{

if (tok==_T("CAPTION"))

{

title=GetQuoteField();

wxFprintf(m_wxr,_T("title = '%s',\\\n"),title.c_str());

}

tok=GetToken();

}

wxFprintf(m_wxr,_T("use_dialog_units = 1,\\\n"));

wxFprintf(m_wxr,_T("use_system_defaults = 0,\\\n"));



wxFprintf(m_wxr,_T("font = [8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif'],\\\n"));

ParseControls();

wxFprintf(m_wxr,_T(").\";\n\n"));

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

while ((tok!=_T("END"))&(tok!=_T("}")))

{

if (tok==_T("LTEXT"))

    ParseStaticText();

if (tok==_T("EDITTEXT"))

    ParseTextCtrl();

if (tok==_T("PUSHBUTTON"))

    ParsePushButton();

if (tok==_T("DEFPUSHBUTTON"))

    ParsePushButton();

if (tok==_T("GROUPBOX"))

    ParseGroupBox();

if (tok==_T("COMBOBOX"))

    ParseComboBox();

if (tok==_T("CONTROL"))

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

wxFprintf(m_wxr,_T("  control = [%i,wxStaticText,'%s','0','%s',"),m_controlid,phrase.c_str(),varname.c_str());

wxFprintf(m_wxr,_T("%i,%i,%i,%i,'',\\\n"),x,y,width,height);

wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));

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

wxFprintf(m_wxr,_T("  control = [%i,wxTextCtrl,'','0','%s',"),m_controlid,varname.c_str());

wxFprintf(m_wxr,_T("%i,%i,%i,%i,'',\\\n"),x,y,width,height);

wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));



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

if (varname==_T("IDOK"))

c=wxID_OK;



if (varname==_T("IDCANCEL"))

c=wxID_CANCEL;



if (varname==_T("IDAPPLY"))

c=wxID_APPLY;



int x,y,width,height;

ReadRect(x,y,width,height);

wxFprintf(m_wxr,_T("  control = [%i,wxButton,'%s','0','%s',"),c,phrase.c_str(),varname.c_str());

wxFprintf(m_wxr,_T("%i,%i,%i,%i,'',\\\n"),x,y,width,height);

wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));



}





bool rc2wxr::Separator(int ch)

{

if ((ch==' ')|(ch==',')|(ch==13)|(ch==10)|(ch=='|'))

   return true;



if (ch==EOF)

{

m_done=true;

return true;

}

return false;

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

wxFprintf(m_wxr,_T("  control = [%i,wxStaticBox,'%s','0','%s',"),m_controlid,phrase.c_str(),varname.c_str());

wxFprintf(m_wxr,_T("%i,%i,%i,%i,'',\\\n"),x,y,width,height);

wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));





}



void rc2wxr::ReadRect(int & x, int & y, int & width, int & height)

{

x=wxAtoi(GetToken());

y=wxAtoi(GetToken());

width=wxAtoi(GetToken());

height=wxAtoi(GetToken());



}



wxString rc2wxr::GetToken()

{

wxString tok=wxEmptyString;



if (m_rc.Eof())

{

m_done=true;

return tok;

}



int ch=0;

ReadChar(ch);

if (ch==EOF)

{

m_done=true;

return tok;

}



while (Separator(ch))

{

  ReadChar(ch);

  if (m_done)

  return tok;

}



if (ch==EOF)

{

m_done=true;



}





while (!Separator(ch))

{

tok+=(char)ch;

ReadChar(ch);



}



if (ch==EOF)

   m_done=true;





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
    wxFileOffset result = m_rc.Tell();

    if ( result >= m_filesize )
        m_done=true;

    result = m_rc.Read(&ch,1);

    if ( result==wxInvalidOffset )
        m_done=true;

    if(ch==EOF)
        m_done=true;
}


/* COMBOBOX        IDC_SCALECOMBO,10,110,48,52,CBS_DROPDOWNLIST | CBS_SORT |
                    WS_VSCROLL | WS_TABSTOP */
void rc2wxr::ParseComboBox()
{
    int x,y,width,height;
    wxString tok;
    wxString varname = GetToken();

    m_controlid++;

    ReadRect(x,y,width,height);
    wxFprintf(m_wxr,_T("  control = [%i,wxChoice,'','0','%s',"),m_controlid,varname.c_str());
    wxFprintf(m_wxr,_T("%i,%i,%i,%i,[],\\\n"),x,y,width,height);
    wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));
}


void rc2wxr::ParseMenu(wxString name)
{
    wxString tok;
    static int menuid=0;
    menuid++;
    wxFprintf(m_wxr,_T("static char *MenuBar%i = \"menu(name = '%s',\\\n"),menuid,name.c_str());
    wxFprintf(m_wxr,_T("menu = \\\n"));
    wxFprintf(m_wxr,_T("[\\\n"));

    while ((tok!=_T("BEGIN"))&(tok!=_T("{")))
        tok=GetToken();

    while ((tok!=_T("END"))&(tok!=_T("}")))
    {
        tok=GetToken();

        if (tok==_T("POPUP"))
        {
            ParsePopupMenu();
            wxFprintf(m_wxr,_T("  ],\\\n"));
        }
    }

    wxFprintf(m_wxr,_T("]).\";\n\n"));
}


void rc2wxr::ParsePopupMenu()
{
    static int menuitem=99;

    menuitem++;

    wxString tok = GetQuoteField();
    int spot;

    //Remove /t because it causes problems
    spot=tok.First(_T("\\t"));
    tok=tok.Left(spot);

    wxFprintf(m_wxr,_T("  ['%s',%i,'',\\\n"),tok.c_str(),menuitem);

    while ((tok!=_T("BEGIN"))&(tok!=_T("{")))
        tok=GetToken();

    while ((tok!=_T("END"))&(tok!=_T("}")))
    {
        tok=GetToken();

        if (tok==_T("MENUITEM"))
        {
            if (PeekToken()==_T("SEPARATOR"))
            {
                wxFprintf(m_wxr,_T("      [],\\\n"));
            }
            else
            {
                tok=GetQuoteField();
                //Remove /t because it causes problems
                spot=tok.First(_T("\\t"));
                tok=tok.Left(spot);
                menuitem++;
                wxFprintf(m_wxr,_T("      ['%s',%i,''],\\\n"),tok.c_str(),menuitem);
            }
        }
    }
}



wxString rc2wxr::PeekToken()
{
    wxFileOffset p = m_rc.Tell();
    wxString tok = GetToken();
    m_rc.Seek(p);
    return tok;
}

//Windows pain in the butt CONTROL
void rc2wxr::ParseControlMS()
{
    wxString tok;
    wxString label=GetQuoteField();
    wxString varname=GetToken();
    wxString kindctrl=GetQuoteField();

    kindctrl.MakeUpper();
    if (kindctrl==_T("MSCTLS_TRACKBAR32"))
        ParseSlider(label,varname);
    if (kindctrl==_T("MSCTLS_PROGRESS32"))
        ParseProgressBar(label,varname);
    if (kindctrl==_T("BUTTON"))
        ParseCtrlButton(label,varname);
}

/*    CONTROL         "Slider1",IDC_SLIDER1,"msctls_trackbar32",TBS_BOTH |
                    TBS_NOTICKS | WS_TABSTOP,52,73,100,15
*/
void rc2wxr::ParseSlider(wxString WXUNUSED(label), wxString varname)
{
    int x,y,width,height;
    wxString tok;

    while (ReadOrs(tok))
        ;
    wxFprintf(m_wxr,_T("  control = [%i,wxSlider,'','wxSL_HORIZONTAL','%s',"),m_controlid,varname.c_str());
    ReadRect(x,y,width,height);
    wxFprintf(m_wxr,_T("%i,%i,%i,%i,"),x,y,width,height);
    wxFprintf(m_wxr,_T(" 1, 1, 10,\\\n"));
    wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));
}

/*
CONTROL         "Progress1",CG_IDC_PROGDLG_PROGRESS,"msctls_progress32",
                    WS_BORDER,15,52,154,13
*/
void rc2wxr::ParseProgressBar(wxString WXUNUSED(label), wxString varname)
{
    int x,y,width,height;
    wxString tok;

    while (ReadOrs(tok))
        ;

    wxFprintf(m_wxr,_T("  control = [%i,wxGauge,'','wxGA_HORIZONTAL','%s',"),m_controlid,varname.c_str());
    ReadRect(x,y,width,height);
    wxFprintf(m_wxr,_T("%i,%i,%i,%i,"),x,y,width,height);
    wxFprintf(m_wxr,_T(" 0, 10,\\\n"));
    wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));
}


bool rc2wxr::ReadOrs(wxString & w)
{
    wxString tok = PeekToken();
    if (tok.IsNumber())
        return false;
    w=GetToken();
    return true;
}


//Is it a check button or a radio button
void rc2wxr::ParseCtrlButton(wxString label, wxString varname)
{
    int x,y,width,height;
    wxString tok = GetToken();

    m_controlid++;

    if (tok==_T("BS_AUTOCHECKBOX"))
    {
        wxFprintf(m_wxr,_T("  control = [%i,wxCheckBox,'%s','0','%s',"),m_controlid,label.c_str(),varname.c_str());
        while (ReadOrs(tok))
            ;

        ReadRect(x,y,width,height);
        wxFprintf(m_wxr,_T("%i,%i,%i,%i,0,\\\n"),x,y,width,height);
        wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));
    }

    if (tok==_T("BS_AUTORADIOBUTTON"))
    {
        wxFprintf(m_wxr,_T("  control = [%i,wxRadioButton,'%s','0','%s',"),m_controlid,label.c_str(),varname.c_str());
        while(ReadOrs(tok))
            ;

        ReadRect(x,y,width,height);
        wxFprintf(m_wxr,_T("%i,%i,%i,%i,0,\\\n"),x,y,width,height);
        wxFprintf(m_wxr,_T("[8, 'wxSWISS', 'wxNORMAL', 'wxNORMAL', 0, 'MS Sans Serif']],\\\n"));
    }
}
