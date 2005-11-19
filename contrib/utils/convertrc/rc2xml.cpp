// rc2xml.cpp: implementation of the rc2xml class.
//Author:  Brian Gavin 9/24/00
//License: wxWindows License
/*
How to use:
#include "rc2xml.h"
...
rc2xml trans;
trans->Convert("Myfile.rc","Myfile.xml");
*/
/* TODO
1.  Figure how to fix memory leaks in all wxLists in this class
2.  Find a way to rename MS Windows fonts so that they work
cross platform (wxGTK,etc)
3.  Be able to abort incorrectly formatted files without crashing
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


#include "rc2xml.h"
#include "wx/image.h"
#include "wx/deprecated/setup.h"
#include "wx/deprecated/resource.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

rc2xml::rc2xml()
{
    m_done=false;
    m_bitmaplist=new wxList(wxKEY_STRING);
    m_stringtable=new wxList(wxKEY_STRING);
    m_iconlist = new wxList(wxKEY_STRING);
    m_resourcelist =new wxList(wxKEY_INTEGER);
}

rc2xml::~rc2xml()
{
    delete m_bitmaplist;
    delete m_stringtable;
    delete m_iconlist;
    delete m_resourcelist;
}

bool rc2xml::Convert(wxString rcfile, wxString xmlfile)
{
    m_rc.Open(rcfile.c_str());
    m_filesize=m_rc.Length();


    m_workingpath=wxPathOnly(rcfile);

    m_targetpath=wxPathOnly(xmlfile) + _T("\\");



    wxSetWorkingDirectory(m_workingpath);


    bool result;
    result=m_xmlfile.Open(xmlfile.c_str(),_T("w+t"));
    wxASSERT_MSG(result,_T("Couldn't create XML file"));
    if (!result)
        return false;


/* Write Basic header for XML file */
    m_xmlfile.Write(_T("<?xml version=\"1.0\" ?>\n"));
    m_xmlfile.Write(_T("<resource>\n"));

//Read resource.h
    ParseResourceHeader();
//Gather all the resource we need for toolbars,menus, and etc
    FirstPass();
    m_done=false;
    m_rc.Seek(0);
//Read in dialogs, toolbars,menus
    SecondPass();

    m_xmlfile.Write(_T("</resource>\n"));
    m_xmlfile.Close();
    m_rc.Close();
    wxMessageBox(_("Conversion complete."), _("Done"),
                            wxOK | wxICON_INFORMATION);

return true;
}


void rc2xml::ParseDialog(wxString dlgname)
{
    wxString token;
    static int dlgid=999;
    dlgid++;
/* Make sure that this really is a dialog
microsoft reuses the keyword DIALOG for other things
*/
    token=PeekToken();
//Microsoft notation?
    while ((token==_T("DISCARDABLE"))
            ||(token==_T("LOADONCALL"))||(token==_T("MOVEABLE")))
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
    wxString ptsize,face;

    m_xmlfile.Write(_T("\t<object class=\"wxDialog\""));
    //Avoid duplicate names this way
    dlgname.Replace(_T("IDD_"),_T("DLG_"));
    WriteBasicInfo(x,y,width,height,dlgname);
    WriteTitle(title);


    while ((token!=_T("BEGIN"))&(token!=_T("{")))
    {
        if (token==_T("CAPTION"))
        {
            title=GetQuoteField();
        }

//TODO fix face name so that it is cross platform name
//  FONT 8, "MS Sans Serif"
    if (token==_T("FONT"))
    {
        ptsize=GetToken();
        face=GetQuoteField();
        m_xmlfile.Write(_T("\t\t<font>\n"));
        m_xmlfile.Write(_T("\t\t\t<size>")+ptsize+_T("</size>\n"));
        m_xmlfile.Write(_T("\t\t\t<face>")+face+_T("</face>\n"));
        m_xmlfile.Write(_T("\t\t</font>\n"));
    }

    token=GetToken();
    }

    ParseControls();
    m_xmlfile.Write(_T("\t</object>\n"));
}

/*
BEGIN
    EDITTEXT        IDC_BANDS,36,83,22,14,ES_AUTOHSCROLL | ES_NUMBER | NOT
                    WS_TABSTOP
    LTEXT           "Bands",IDC_STATIC,11,86,21,8
    EDITTEXT        IDC_NAME,10,3,75,14,ES_AUTOHSCROLL
END
*/
void rc2xml::ParseControls()
{
    wxString token;
    wxString label,varname;

    token=GetToken();
    while ((token!=_T("END"))&(token!=_T("}")))
        {
        if (token==_T("AUTOCHECKBOX"))
           {
            label=GetQuoteField();
            varname=GetToken();
            ParseCheckBox(label,varname);
            }
        else
        if (token==_T("AUTORADIOBUTTON"))
           {
            label=GetQuoteField();
            varname=GetToken();
            ParseRadioButton(label,varname);
           }
        else
        if (token==_T("LTEXT"))
           {
            label=GetQuoteField();
            varname=GetToken();
            ParseStaticText(label,varname);
            }
        else if (token==_T("EDITTEXT"))
           {
            varname=GetToken();
            ParseTextCtrl(varname);
            }
        else if ((token==_T("PUSHBUTTON"))||(token==_T("DEFPUSHBUTTON")))
           {
            label=GetQuoteField();
            varname=GetToken();
            ParsePushButton(label,varname);
            }
        else if (token==_T("GROUPBOX"))
           {
            label=GetQuoteField();
            varname=GetToken();
            ParseGroupBox(label,varname);
            }
        else if (token==_T("COMBOBOX"))
            {
            varname=GetToken();
            ParseComboBox(varname);
             }
        else if (token==_T("CONTROL"))
            ParseControlMS();
        else if (token==_T("LISTBOX"))
           {
            varname=GetToken();
            ParseListBox(varname);
            }
        else if (token==_T("ICON"))
            ParseIconStatic();
        else if (token==_T("SCROLLBAR"))
            ParseScrollBar();
        token=GetToken();
        }

}
//LTEXT           "Radius",IDC_STATIC,9,67,23,8
void rc2xml::ParseStaticText(wxString phrase, wxString varname)
{
    wxString token;
    token=PeekToken();
    while (!token.IsNumber())
        {
        token=GetToken();
        token=PeekToken();
        }
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write(_T("\t\t<object class=\"wxStaticText\""));
    WriteBasicInfo(x,y,width,height,varname);WriteLabel(phrase);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}
//EDITTEXT        IDC_RADIUS,36,65,40,14,ES_AUTOHSCROLL
void rc2xml::ParseTextCtrl(wxString varname)
{
    wxString token;
    wxString style;
    token=PeekToken();
    while (!token.IsNumber())
        {
        token=GetToken();
        token=PeekToken();
        }
    int x,y,width,height;
    ReadRect(x,y,width,height);
//TODO
//style=GetToken();
    m_xmlfile.Write(_T("\t\t<object class=\"wxTextCtrl\""));
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}
//AUTOCHECKBOX "&log.", ID_XLOG, 25, 24, 21, 12
void rc2xml::ParseCheckBox(wxString phrase, wxString varname)
{
    wxString token;
    token=PeekToken();
    while (!token.IsNumber())
        {
        token=GetToken();
        token=PeekToken();
        }
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write(_T("\t\t<object class=\"wxCheckBox\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteLabel(phrase);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}
//AUTORADIOBUTTON "&text", ID_SW10, 13, 12, 68, 10, BS_AUTORADIOBUTTON | WS_GROUP
void rc2xml::ParseRadioButton(wxString phrase, wxString varname)
{
    wxString token,style;
    int x,y,width,height;
    bool GotOrs;
    GotOrs = ReadOrs(token);
    if (ReadRect(x,y,width,height))
        if (GotOrs==false)
      ReadOrs(token);
    if (token.Find(_T("WS_GROUP")) != wxNOT_FOUND)
        style += _T("wxRB_GROUP");

    m_xmlfile.Write(_T("\t\t<object class=\"wxRadioButton\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteLabel(phrase);
    WriteStyle(style);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}

//PUSHBUTTON      "Create/Update",IDC_CREATE,15,25,53,13,NOT WS_TABSTOP
void rc2xml::ParsePushButton(wxString phrase, wxString varname)
{
    wxString token;

    token=PeekToken();
    while (!token.IsNumber())
        {
        token=GetToken();
        token=PeekToken();
        }
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write(_T("\t\t<object class=\"wxButton\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteLabel(phrase);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}


bool rc2xml::Separator(int ch)
{
//if ((ch==' ')|(ch==',')|(ch==13)|(ch==10)|(ch=='|')|(ch=='\t'))
    if ((ch==' ')|(ch==',')|(ch==13)|(ch==10)|(ch=='\t'))
        return true;

    if (ch==EOF)
    {
        m_done=true;
        return true;
    }

    return false;
}

void rc2xml::ParseGroupBox(wxString phrase, wxString varname)
{
//    GROUPBOX        "Rotate",IDC_STATIC,1,1,71,79
    wxString token;
    token=PeekToken();
    while (!token.IsNumber())
        {
        token=GetToken();
        token=PeekToken();
        }
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write(_T("\t\t<object class=\"wxStaticBox\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteLabel(phrase);
    m_xmlfile.Write(_T("\t\t</object>\n"));
}

bool rc2xml::ReadRect(int & x, int & y, int & width, int & height)
{
    x=wxAtoi(GetToken());
    y=wxAtoi(GetToken());
    width=wxAtoi(GetToken());
    bool ret;
    wxString tmp = GetToken(&ret);
    height=wxAtoi(tmp);
    return ret; // check for more parameters
}

wxString rc2xml::GetToken(bool *listseparator)
{
    wxString token=wxEmptyString;

    if (m_rc.Eof())
    {
    m_done=true;
    return token;
}

    int ch=0;
    ReadChar(ch);
    if (ch==EOF)
    {
    m_done=true;
    return token;
    }

    while (Separator(ch))
    {
    ReadChar(ch);
    if (m_done)
        return token;
    }

    if (ch==EOF)
    {
    m_done=true;
    }


    while (!Separator(ch))
    {
    token += (char)ch;
    ReadChar(ch);
    }

    if (ch == EOF)
        m_done = true;

    if (listseparator)
      *listseparator = (ch == ',');
    return token;
}

wxString rc2xml::GetQuoteField()
{
    wxString phrase;
    //ASCII code 34 "
    int ch=0;
    int ch1=0;

    ReadChar(ch);

    // !! Changed by MS, 15th/11/04. Can now read strings such as
    // """Catapult"" - blah blah", ...

    while (ch!=34)
        ReadChar(ch);

    // found first '"'
    while (true)
    {
        ReadChar(ch);
        if (ch == 34)
        {
            // another quote?
            ReadChar(ch1);
            if (ch1 != 34)
            {
                // real end of string..
                break;
            }

            // add a single quote - fall through
        }
        phrase+=(char)ch;
    }

    return phrase;
}

// string in stringtable may contain embedded quotes
// escape characters retained to allow strings to be rewritten
wxString rc2xml::GetStringQuote()
{
    wxString phrase;
    //ASCII code 34 "
    bool done=false;
    int ch=0,lastch=0;
    ReadChar(ch);

    while (ch!=34)
        ReadChar(ch);

    ReadChar(ch);
    while (done==false)
    {
        if ((ch==34)&&(lastch!='\\'))
        {
            wxFileOffset p = m_rc.Tell();
            ReadChar(ch);
            // RC supports "", for embedded quote, as well as  \"
            if (ch==34)
                phrase+='\\';
            else
            {
                m_rc.Seek(p);
                done = true;
                }
            }
         if (done==true)
             break;
         if (ch=='\r')
             ReadChar(ch);                    // skip
         if ((ch=='\n')&&(lastch=='\\'))      // lastch <should> be this
             phrase+='n';                     // escape
         else
             phrase+=(char)ch;

         lastch=ch;
         ReadChar(ch);
    }

    return phrase;
}

void rc2xml::ReadChar(int &ch)
{
    wxFileOffset result = m_rc.Tell();

    if((result>=m_filesize))
        m_done=true;

    result = m_rc.Read(&ch,1);

    if( result == wxInvalidOffset )
        m_done=true;

    if(ch==EOF)
        m_done=true;
}

void rc2xml::ParseComboBox(wxString varname)
{
/* COMBOBOX        IDC_SCALECOMBO,10,110,48,52,CBS_DROPDOWNLIST | CBS_SORT |
                    WS_VSCROLL | WS_TABSTOP */
    wxString token,style;
    int x,y,width,height;
    bool GotOrs;
    GotOrs = ReadOrs(token);
    if (ReadRect(x,y,width,height))
        if (GotOrs==false)
      ReadOrs(token);

    m_xmlfile.Write(_T("\t\t<object class=\"wxComboBox\""));
    WriteBasicInfo(x,y,width,height,varname);
    if (token.Find(_T("CBS_SIMPLE")) != wxNOT_FOUND)
        WriteStyle(_T("wxCB_SIMPLE"));
    if (token.Find(_T("CBS_SORT")) != wxNOT_FOUND)
        WriteStyle(_T("wxCB_SORT"));
    if (token.Find(_T("CBS_DISABLENOSCROLL")) != wxNOT_FOUND)
        WriteStyle(_T("wxLB_ALWAYS_SB"));
    m_xmlfile.Write(_T("\n\t\t</object>\n"));

}

void rc2xml::ParseMenu(wxString varname)
{
    wxString token=wxEmptyString;

    //Write menubar to xml file
    m_xmlfile.Write(_T("\t<object class=\"wxMenuBar\""));
    //Avoid duplicate names this way
    varname.Replace(_T("IDR_"),_T("MB_"));
    WriteName(varname);
    m_xmlfile.Write(_T(">\n"));

    while ((token!=_T("BEGIN"))&(token!=_T("{")))
        token=GetToken();

    while ((token!=_T("END"))&(token!=_T("}")))
    {
    token=GetToken();
    token.MakeUpper();

    if (token==_T("POPUP"))
        {
        ParsePopupMenu();
        }
    }
    m_xmlfile.Write(_T("\t</object>\n"));
}

void rc2xml::ParsePopupMenu()
{
    static int menucount=0;
    menucount++;
    wxString token,name,msg,longhelp,tip;
    token=GetQuoteField();

//Remove \t because it causes problems

//spot=token.First("\\t");
//token=token.Left(spot);

//Write Menu item
//Generate a fake name since RC menus don't have one
    name << _T("Menu_") << menucount;
    m_xmlfile.Write(_T("\t\t<object class=\"wxMenu\""));
    WriteName(name);
    m_xmlfile.Write(_T(">\n"));
    WriteLabel(token);

    while ((token!=_T("BEGIN"))&(token!=_T("{")))
        token=GetToken();

    while ((token!=_T("END"))&(token!=_T("}")))
    {
    token=GetToken();
    token.MakeUpper();

    if (token==_T("POPUP"))
        ParsePopupMenu();

    if (token==_T("MENUITEM"))
        ParseMenuItem();
    }
    m_xmlfile.Write(_T("\t\t\t</object>\n"));
}

wxString rc2xml::PeekToken()
{
    wxFileOffset p = m_rc.Tell();
    wxString token=GetToken();

    m_rc.Seek(p);
    return token;
}

//MS Windows pain in the butt CONTROL
void rc2xml::ParseControlMS()
{
    wxString token = PeekToken();

    if (token.Contains(_T("\"")))
        ParseNormalMSControl();
    else
        ParseWeirdMSControl();
}

/*    CONTROL         "Slider1",IDC_SLIDER1,"msctls_trackbar32",TBS_BOTH |
                    TBS_NOTICKS | WS_TABSTOP,52,73,100,15
*/

void rc2xml::ParseSlider(wxString WXUNUSED(label), wxString varname)
{
    wxString token,style;
    ReadOrs(token);
    if (token.Find(_T("TBS_VERT"))!=wxNOT_FOUND)
        style+=_T("wxSL_VERTICAL");
    //MFC RC Default is horizontal
    else
        style+=_T("wxSL_HORIZONTAL");

    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write(_T("\t\t<object class=\"wxSlider\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write(_T("\n\t\t</object>\n"));

}
/*
CONTROL         "Progress1",CG_IDC_PROGDLG_PROGRESS,"msctls_progress32",
                    WS_BORDER,15,52,154,13
*/
void rc2xml::ParseProgressBar(wxString WXUNUSED(label), wxString varname)
{
    wxString token,style;
    ReadOrs(token);

    int x,y,width,height;
    ReadRect(x,y,width,height);

//Always horizontal in MFC
    m_xmlfile.Write(_T("\t\t<object class=\"wxGauge\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write(_T("\t\t</object>\n"));
}

bool rc2xml::ReadOrs(wxString & orstring)
{
    wxString token;

    token=PeekToken();
    if (token.IsNumber())
        return false;
    orstring=GetToken();

    while(PeekToken()==_T("|"))
    {
    //Grab |
    orstring+=GetToken();
    //Grab next token
    orstring+=GetToken();
    }
    return true;
}

//Is it a checkbutton or a radiobutton or a pushbutton or a groupbox
void rc2xml::ParseCtrlButton(wxString label, wxString varname)
{
    wxString token;
    wxFileOffset p = m_rc.Tell();
    ReadOrs(token);
    m_rc.Seek(p);

    if (token.Find(_T("BS_AUTOCHECKBOX"))!=wxNOT_FOUND)
        ParseCheckBox(label, varname);
    else if ((token.Find(_T("BS_AUTORADIOBUTTON"))!=wxNOT_FOUND)||
             (token.Find(_T("BS_RADIOBUTTON"))!=wxNOT_FOUND))
        ParseRadioButton(label, varname);
    else if (token.Find(_T("BS_GROUPBOX"))!=wxNOT_FOUND)
        ParseGroupBox(label, varname);
    else  // if ((token.Find("BS_PUSHBUTTON")!=wxNOT_FOUND)||
//                (token.Find("BS_DEFPUSHBUTTON")!=wxNOT_FOUND))
        ParsePushButton(label, varname);           // make default case
}

void rc2xml::WriteSize(int width, int height)
{
    wxString msg;
    msg << _T(" <size>") << width << _T(",") << height << _T("d</size>");
    m_xmlfile.Write(msg);
}

void rc2xml::WritePosition(int x, int y)
{
    wxString msg;
    msg << _T(" <pos>") << x << _T(",") << y << _T("d</pos>");
    m_xmlfile.Write(msg);
}

void rc2xml::WriteTitle(wxString title)
{
    wxString msg;
    msg=_T("\t\t<title>")+title+_T("</title>\n");
    m_xmlfile.Write(msg);
}

void rc2xml::WriteName(wxString name)
{

//Try to convert any number ids into names
name=LookUpId(name);
//Replace common MS ids with wxWidgets ids
//I didn't do everyone of them
    if (name==_T("IDOK"))
        name=_T("wxID_OK");
    else if (name==_T("IDCANCEL"))
        name=_T("wxID_CANCEL");
    else if (name==_T("IDAPPLY"))
        name=_T("wxID_APPLY");
    else if (name==_T("ID_FILE_OPEN"))
        name=_T("wxID_OPEN");
    else if (name==_T("ID_FILE_CLOSE"))
        name=_T("wxID_CLOSE");
    else if (name==_T("ID_FILE_SAVE"))
        name=_T("wxID_SAVE");
    else if (name==_T("ID_FILE_SAVE_AS"))
        name=_T("wxID_SAVEAS");
    else if (name==_T("ID_APP_EXIT"))
        name=_T("wxID_EXIT");
    else if (name==_T("ID_FILE_PRINT"))
        name=_T("wxID_PRINT");
    else if (name==_T("ID_FILE_PRINT_PREVIEW"))
        name=_T("wxID_PREVIEW");
    else if (name==_T("ID_FILE_PRINT_SETUP"))
        name=_T("wxID_PRINT_SETUP");
    else if (name==_T("ID_APP_ABOUT"))
        name=_T("wxID_ABOUT");
    else if (name==_T("ID_EDIT_UNDO"))
        name=_T("wxID_UNDO");
    else if (name==_T("ID_EDIT_CUT"))
        name=_T("wxID_CUT");
    else if (name==_T("ID_EDIT_COPY"))
        name=_T("wxID_COPY");
    else if (name==_T("ID_EDIT_PASTE"))
        name=_T("wxID_PASTE");
    else if (name==_T("IDYES"))
        name=_T("wxID_YES");
    else if (name==_T("IDNO"))
        name=_T("wxID_NO");
    else if (name==_T("IDHELP"))
        name=_T("wxID_HELP");

    m_xmlfile.Write(_T(" name= \"")+name+_T("\""));
}

void rc2xml::WriteLabel(wxString label)
{
    label.Replace(_T("&"),_T("$"));
    // changes by MS, handle '<' '>' characters within a label.
    label.Replace(_T("<"),_T("&lt;"));
    label.Replace(_T(">"),_T("&gt;"));
    m_xmlfile.Write(_T("\t\t\t<label>")+label+_T("</label>\n"));
}

void rc2xml::WriteBasicInfo(int x, int y, int width, int height, wxString name)
{
    WriteName(name);
    m_xmlfile.Write(_T(">\n"));
    m_xmlfile.Write(_T("\t\t\t"));
    WritePosition(x,y);
    WriteSize(width,height);
    m_xmlfile.Write(_T("\n"));
}

void rc2xml::WriteStyle(wxString style)
{
    if (style.Length()==0)
        return;
    m_xmlfile.Write(_T("\t\t\t<style>")+style+_T("</style>\n"));
}
/*
    LISTBOX         IDC_LIST1,16,89,48,40,LBS_SORT | LBS_MULTIPLESEL |
                    LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP
*/
void rc2xml::ParseListBox(wxString varname)
{
    wxString token;
    token=PeekToken();
    while (!token.IsNumber())
        {
        token=GetToken();
        token=PeekToken();
        }
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write(_T("\t\t<object class=\"wxListBox\""));
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write(_T("\n\t\t</object>\n"));

}
/*
    CONTROL         "",IDC_RICHEDIT1,"RICHEDIT",ES_AUTOHSCROLL | WS_BORDER |
                    WS_TABSTOP,103,110,40,14
*/
void rc2xml::ParseRichEdit(wxString WXUNUSED(label), wxString varname)
{
    wxString token;
    //while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    wxString style;
//Make it a rich text control
    style+=_T("wxTE_MULTILINE ");
    m_xmlfile.Write(_T("\t\t<object class=\"wxTextCtrl\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}
/*
CONTROL         "Spin1",IDC_SPIN1,"msctls_updown32",UDS_ARROWKEYS,209,72,
                 19,26
*/
void rc2xml::ParseSpinCtrl(wxString WXUNUSED(label), wxString varname)
{
    wxString token,style;

    ReadOrs(token);
    if (token.Find(_T("UDS_HORZ"))!=wxNOT_FOUND)
        style=_T("wxSP_HORIZONTAL");
    //MFC default
    else
        style=_T("wxSP_VERTICAL");

    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write(_T("\t\t<object class=\"wxSpinButton\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write(_T("\n\t\t</object>\n"));

}

void rc2xml::FirstPass()
{
    wxString token,prevtok;
    while (!m_done)
        {
        token=GetToken();
        if (token==_T("BITMAP"))
            ParseBitmap(prevtok);
        else if (token==_T("STRINGTABLE"))
            ParseStringTable(prevtok);
        else if (token==_T("ICON"))
            ParseIcon(prevtok);

        prevtok=token;
        }
}

void rc2xml::ParseBitmap(wxString varname)
{
    wxString token;
    wxString *bitmapfile;

    token=PeekToken();
    //Microsoft notation?
    if (token==_T("DISCARDABLE"))
        {
        token=GetToken();
        token=PeekToken();
        }
    bitmapfile=new wxString;
    *bitmapfile=GetQuoteField();
    m_bitmaplist->Append(varname,bitmapfile);

}


void rc2xml::SecondPass()
{
    wxString token,prevtok;
    while (!m_done)
        {
        token=GetToken();
        if ((token==_T("DIALOG"))||(token==_T("DIALOGEX")))
            ParseDialog(prevtok);
        else if (token==_T("MENU"))
            ParseMenu(prevtok);
        else if (token==_T("TOOLBAR"))
            ParseToolBar(prevtok);

        prevtok=token;
        }

}

void rc2xml::ParseToolBar(wxString varname)
{
    wxString token;
    token=GetToken();
    wxASSERT_MSG(token==_T("DISCARDABLE"),_T("Error in toolbar parsing"));
//Look up bitmap for toolbar and load
    wxNode *node=m_bitmaplist->Find(LookUpId(varname));
    wxString *bitmappath;
    bitmappath=(wxString *)node->GetData();
    wxBitmap bitmap;
    if (!bitmap.LoadFile(*bitmappath,wxBITMAP_TYPE_BMP ))
        wxLogError(_T("Unable to load bitmap:")+*bitmappath);

//Write toolbar to xml file
    m_xmlfile.Write(_T("\t<object class=\"wxToolBar\""));
//Avoid duplicate names this way
    varname.Replace(_T("IDR_"),_T("TB_"));
    WriteName(varname);
    m_xmlfile.Write(_T(">\n"));
    wxString style;
    style+=_T("wxTB_FLAT");
    WriteStyle(style);


//Grab width and height
    int width,height;
    width=wxAtoi(GetToken());
    height=wxAtoi(GetToken());

    int c=0;
    wxString buttonname,msg,tip,longhelp;
    token=GetToken();
    while ((token!=_T("BEGIN"))&(token!=_T("{")))
        token=GetToken();

    while ((token!=_T("END"))&(token!=_T("}")))
        {
        if (token==_T("BUTTON"))
            {
            buttonname=GetToken();
            m_xmlfile.Write(_T("\t\t\t<object class=\"tool\""));
            WriteName(buttonname);
            m_xmlfile.Write(_T(">\n"));
    //Write tool tip if any
            if (LookUpString(buttonname,msg))
                {
                SplitHelp(msg,tip,longhelp);
                m_xmlfile.Write(_T("\t\t\t\t<tooltip>")+tip+_T("</tooltip>\n"));
                m_xmlfile.Write(_T("\t\t<longhelp>")+longhelp+_T("</longhelp>\n"));
                }
            //Make a bitmap file name
            buttonname=CleanName(buttonname);
            buttonname+=_T(".bmp");
            m_xmlfile.Write(_T("\t\t\t\t<bitmap>")+buttonname+_T("</bitmap>\n"));
        WriteToolButton(buttonname,c,width,height,bitmap);
        m_xmlfile.Write(_T("\t\t\t</object>\n"));
        c++;
        }
    else if (token==_T("SEPARATOR"))
    {
    m_xmlfile.Write(_T("\t\t\t<object class=\"separator\"/>\n"));
    }
    token=GetToken();
    }
    m_xmlfile.Write(_T("\t</object>\n"));
}

//Extract bitmaps from larger toolbar bitmap
void rc2xml::WriteToolButton(wxString name,int index, int width, int height, wxBitmap bitmap)
{
    int x;
    x=index*width;
    wxRect r(x,0,width,height);
    wxBitmap little;
    little=bitmap.GetSubBitmap(r);
    little.SaveFile(m_targetpath+name,wxBITMAP_TYPE_BMP);
}

void rc2xml::ParseStringTable(wxString WXUNUSED(varname))
{
    wxString token;
    token=GetToken();
    while ((token!=_T("BEGIN"))&(token!=_T("{")))
        token=GetToken();
    token=GetToken();
    wxString *msg;

    while ((token!=_T("END"))&(token!=_T("}")))
    {
        msg=new wxString;
        *msg=GetStringQuote();
        m_stringtable->Append(token,msg);
        token=GetToken();
    }
}

bool rc2xml::LookUpString(wxString strid,wxString & st)
{
    wxNode *node=m_stringtable->Find(strid);
    wxString *s;
    if (node==NULL)
        return false;

    s=(wxString *)node->GetData();
    st=*s;

    return true;
}

bool rc2xml::SplitHelp(wxString msg, wxString &shorthelp, wxString &longhelp)
{
    int spot;
    spot=msg.Find(_T("\\n"));
    if (spot==wxNOT_FOUND)
        {
        shorthelp=msg;
        longhelp=msg;
        }

    longhelp=msg.Left(spot);
    spot=msg.Length()-spot-2;
    shorthelp=msg.Right(spot);
    return true;
}

void rc2xml::ParseMenuItem()
{
    wxString token,name,msg,tip,longhelp;
//int spot;
    if (PeekToken()==_T("SEPARATOR"))
        {
        m_xmlfile.Write(_T("\t\t\t<object class=\"separator\"/>\n"));
        return;
        }

    token=GetQuoteField();
    name=GetToken();
//Remove \t because it causes problems
//spot=token.First("\\t");
//token=token.Left(spot);
    m_xmlfile.Write(_T("\t\t\t<object class=\"wxMenuItem\""));
    WriteName(name);
    m_xmlfile.Write(_T(">\n"));
    WriteLabel(token);
//Look up help if any listed in stringtable
//can't assume numbers correlate, restrict to string identifiers
    if ((!name.IsNumber())&&(LookUpString(name,msg)))
        {
        SplitHelp(msg,tip,longhelp);
        m_xmlfile.Write(_T("\t\t\t<help>")
            +longhelp+_T("</help>\n"));
        }
//look for extra attributes like checked and break
    wxString ptoken;
    ptoken=PeekToken();
    ptoken.MakeUpper();
    while ((ptoken!=_T("MENUITEM"))&(ptoken!=_T("POPUP"))&(ptoken!=_T("END")))
        {
        token=GetToken();
        ptoken.MakeUpper();
        if (token==_T("CHECKED"))
            m_xmlfile.Write(_T("\t\t\t<checkable>1</checkable>\n"));
        else if (token==_T("MENUBREAK"))
            ;
//m_xmlfile.Write("\t\t\t</break>\n");
        else if (token==_T("GRAYED"))
            ;
        else
            wxLogError(_T("Unknown Menu Item token:")+token);

        ptoken=PeekToken();
        ptoken.MakeUpper();
        }
    m_xmlfile.Write(_T("\t\t\t</object>\n"));

}

//ICON            IDR_MAINFRAME,IDC_STATIC,11,17,20,20
void rc2xml::ParseIconStatic()
{
    wxString token;
    wxString varname,iconname;
    token = PeekToken();
    if (token.Contains(_T("\"")))
        iconname = GetQuoteField();
    else
    iconname=GetToken();
//Look up icon
    varname=GetToken();

    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write(_T("\t\t<object class=\"wxStaticBitmap\""));
    WriteBasicInfo(x,y,width,height,varname);
//Save icon as a bitmap
    WriteIcon(iconname);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}
//IDR_MAINFRAME           ICON    DISCARDABLE     "res\\mfcexample.ico"
void rc2xml::ParseIcon(wxString varname)
{
    wxString token;
    wxString *iconfile;
    iconfile=new wxString;
    token=PeekToken();

    *iconfile=GetQuoteField();
    m_iconlist->Append(varname,iconfile);


}

wxString rc2xml::CleanName(wxString name)
{
    name.MakeLower();
    name.Replace(_T("id_"),wxEmptyString);
    name.Replace(_T("idr_"),wxEmptyString);
    name.Replace(_T("idb_"),wxEmptyString);
    name.Replace(_T("idc_"),wxEmptyString);

    name.Replace(_T(".ico"),wxEmptyString);

    name.Replace(_T(".bmp"),wxEmptyString);
    return name;
}
// And the award for most messed up control goes to...
//    CONTROL         IDB_FACE,IDC_STATIC,"Static",SS_BITMAP,26,62,32,30
void rc2xml::ParseStaticBitmap(wxString bitmapname, wxString varname)
{
    wxString token;
    //Grab SS_BITMAP
    ReadOrs(token);


    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write(_T("\t\t<object class=\"wxStaticBitmap\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteBitmap(bitmapname);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}

void rc2xml::ParseNormalMSControl()
{
    wxString label=GetQuoteField();
    wxString varname=GetToken();
    wxString kindctrl=GetQuoteField();
    kindctrl.MakeUpper();

    if (kindctrl==_T("MSCTLS_UPDOWN32"))
        ParseSpinCtrl(label,varname);
    else if (kindctrl==_T("MSCTLS_TRACKBAR32"))
        ParseSlider(label,varname);
    else if (kindctrl==_T("MSCTLS_PROGRESS32"))
        ParseProgressBar(label,varname);
    else if (kindctrl==_T("SYSTREEVIEW32"))
        ParseTreeCtrl(label,varname);
    else if (kindctrl==_T("SYSMONTHCAL32"))
        ParseCalendar(label,varname);
    else if (kindctrl==_T("SYSLISTVIEW32"))
        ParseListCtrl(label,varname);
    else if (kindctrl==_T("BUTTON"))
        ParseCtrlButton(label,varname);
    else if (kindctrl==_T("RICHEDIT"))
        ParseRichEdit(label,varname);
    else if (kindctrl==_T("STATIC"))
    {
        wxString token;
        wxFileOffset p = m_rc.Tell();
        ReadOrs(token);
        m_rc.Seek(p);
        if (token.Find(_T("SS_BITMAP"))!=wxNOT_FOUND)
            ParseStaticBitmap(label,varname);
        else
            ParseStaticText(label,varname);
    }
    else if (kindctrl==_T("EDIT"))
        ParseTextCtrl(varname);
    else if (kindctrl==_T("LISTBOX"))
        ParseListBox(varname);
    else if (kindctrl==_T("COMBOBOX"))
        ParseComboBox(varname);
}

void rc2xml::ParseWeirdMSControl()
{
    wxString id = GetToken();
    wxString varname = GetToken();
    wxString kindctrl = GetQuoteField();
    kindctrl.MakeUpper();
//    CONTROL         IDB_FACE,IDC_STATIC,"Static",SS_BITMAP,26,62,32,30
    if (kindctrl==_T("STATIC"))
    {
        if (PeekToken()==_T("SS_BITMAP"))
            ParseStaticBitmap(id,varname);
        else
            wxLogError(_T("Unknown MS Control Static token"));
    }
}

//SCROLLBAR       IDC_SCROLLBAR1,219,56,10,40,SBS_VERT
void rc2xml::ParseScrollBar()
{
    wxString token;
    wxString varname;

    varname=GetToken();
    int x,y,width,height;
    ReadRect(x,y,width,height);
    wxString style;

    ReadOrs(token);

if (token.Find(_T("SBS_VERT"))!=wxNOT_FOUND)
    style=_T("wxSB_VERTICAL");
//Default MFC style is horizontal
    else
        style=_T("wxSB_HORIZONTAL");

    m_xmlfile.Write(_T("\t\t<object class=\"wxScrollBar\""));
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write(_T("\n\t\t</object>\n"));

}
//    CONTROL         "Tree1",IDC_TREE1,"SysTreeView32",WS_BORDER | WS_TABSTOP,
//                      7,7,66,61

void rc2xml::ParseTreeCtrl(wxString WXUNUSED(label), wxString varname)
{
    wxString token;
//while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write(_T("\t\t<object class=\"wxTreeCtrl\""));
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}
//    CONTROL         "MonthCalendar1",IDC_MONTHCALENDAR1,"SysMonthCal32",
                    //MCS_NOTODAY | WS_TABSTOP,105,71,129,89

void rc2xml::ParseCalendar(wxString WXUNUSED(label), wxString varname)
{
    wxString token;
//while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write(_T("\t\t<object class=\"wxCalendarCtrl\""));
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write(_T("\t\t</object>\n"));
}
//    CONTROL         "List1",IDC_LIST1,"SysListView32",WS_BORDER | WS_TABSTOP,
  //                  7,89,68,71

void rc2xml::ParseListCtrl(wxString WXUNUSED(label), wxString varname)
{
    wxString token;
    //while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write(_T("\t\t<object class=\"wxListCtrl\""));
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write(_T("\t\t</object>\n"));

}

void rc2xml::WriteBitmap(wxString bitmapname)
{
//Look up bitmap
    wxNode *node=m_bitmaplist->Find(LookUpId(bitmapname));
    if (node==NULL)
        {
        m_xmlfile.Write(_T("\t\t\t<bitmap>missingfile</bitmap>\n"));
        wxLogError(_T("Unable to find bitmap:")+bitmapname);
        return;
        }

    wxString *bitmappath;
    bitmappath=(wxString *)node->GetData();

    bitmapname=wxFileNameFromPath(*bitmappath);
    wxBitmap bitmap;
    if (!bitmap.LoadFile(*bitmappath,wxBITMAP_TYPE_BMP ))
        wxLogError(_T("Unable to load bitmap:")+*bitmappath);

    //Make a bitmap file name
    bitmapname=CleanName(bitmapname);
    bitmapname+=_T(".bmp");
    m_xmlfile.Write(_T("\t\t\t<bitmap>")+bitmapname+_T("</bitmap>\n"));
    bitmap.SaveFile(m_targetpath+bitmapname,wxBITMAP_TYPE_BMP);
}

void rc2xml::WriteIcon(wxString iconname)
{
wxNode *node=m_iconlist->Find(iconname);
    if (node==NULL)
        {
        m_xmlfile.Write(_T("\t\t\t<bitmap>missing_file</bitmap>\n"));
        wxLogError(_T("Unable to find icon:")+iconname);
        }
    wxString *iconpath;
    iconpath=(wxString *)node->GetData();
    wxIcon icon;
    wxBitmap bitmap;
    if (!icon.LoadFile(*iconpath,wxBITMAP_TYPE_ICO ))
        wxLogError(_T("Unable to load icon:")+*iconpath);
#ifdef __WXMSW__
    bitmap.CopyFromIcon(icon);
#else
    bitmap = icon;
#endif
    iconname=wxFileNameFromPath(*iconpath);
    //Make a bitmap file name
    iconname=CleanName(iconname);
    iconname+=_T(".bmp");
    m_xmlfile.Write(_T("\t\t\t<bitmap>")+iconname+_T("</bitmap>\n"));
    bitmap.SaveFile(m_targetpath+iconname,wxBITMAP_TYPE_BMP);


}
/*Unfortunately sometimes the great MSVC Resource editor decides
to use numbers instead of the word id.  I have no idea why they
do this, but that is the way it is.
*/
/* this is a quick and dirty way to parse the resource.h file
it will not recognize #ifdef so it can be easily fooled
*/
void rc2xml::ParseResourceHeader()
{
wxTextFile r;
//Attempt to load resource.h in current path
    if (!r.Open(_T("resource.h")))
        {
        wxLogError(_T("Warining Unable to load resource.h file"));
        return;
        }

    wxString str;
    wxString id,v;
    wxStringTokenizer tok;
    wxString *varname;


    long n;

//Read through entire file
    for ( str = r.GetFirstLine(); !r.Eof(); str = r.GetNextLine() )
    {
    if (str.Find(_T("#define"))!=wxNOT_FOUND)
        {
        tok.SetString(str);
        //Just ignore #define token
        tok.GetNextToken();
        v=tok.GetNextToken();
        id=tok.GetNextToken();
        if (id.IsNumber())
            {
            varname=new wxString;
            id.ToLong(&n);
            *varname=v;
            m_resourcelist->Append(n,varname);
            }
        }
    }



}


wxString rc2xml::LookUpId(wxString id)
{
wxString st;

if (!id.IsNumber())
    return id;
long n;
id.ToLong(&n);
wxNode *node=m_resourcelist->Find(n);
    wxString *s;
    if (node==NULL)
        return id;

    s=(wxString *)node->GetData();
    st=*s;
return st;
}
