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
3.  Be able to abort incorrectly formated files without crashing
*/

#ifdef __GNUG__
#pragma implementation "rc2xml.cpp"
#pragma interface "rc2xml.cpp"
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


#include "rc2xml.h"
#include "wx/image.h"
#include "wx/resource.h"
#include <wx/textfile.h>
#include <wx/tokenzr.h>



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

rc2xml::rc2xml()
{
    m_done=FALSE;
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
    m_targetpath=wxPathOnly(xmlfile)+"\\";

    wxSetWorkingDirectory(m_workingpath);

    bool result;
    result=m_xmlfile.Open(xmlfile.c_str(),"w+t");
    wxASSERT_MSG(result,"Couldn't create XML file");	
    if (!result)
        return FALSE;

	
/* Write Basic header for XML file */
    m_xmlfile.Write("<?xml version=\"1.0\" ?>\n");
    m_xmlfile.Write("<resource>\n");
    
//Read resource.h
    ParseResourceHeader();
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
    wxString ptsize,face;
    
    m_xmlfile.Write("\t<object class=\"wxDialog\"");
    //Avoid duplicate names this way
    dlgname.Replace("IDD_","DLG_");
    WriteBasicInfo(x,y,width,height,dlgname);
    WriteTitle(title);
    

    while ((token!="BEGIN")&(token!="{"))
    {
    if (token=="CAPTION")
        {
        title=GetQuoteField();
        }

//TODO fix face name so that it is cross platform name
//  FONT 8, "MS Sans Serif"
    if (token=="FONT")
        {
        ptsize=GetToken();
        face=GetQuoteField();
        m_xmlfile.Write("\t\t<font>\n");
        m_xmlfile.Write("\t\t\t<size>"+ptsize+"</size>\n");
        m_xmlfile.Write("\t\t\t<face>"+face+"</face>\n");
        m_xmlfile.Write("\t\t</font>\n");
        }

    token=GetToken();
    }

    ParseControls();
    m_xmlfile.Write("\t</object>\n");
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
void rc2xml::ParseStaticText()
{
    wxString token;
    wxString phrase,varname;
    phrase=GetQuoteField();
    varname=GetToken();
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write("\t\t<object class=\"wxStaticText\"");
    WriteBasicInfo(x,y,width,height,varname);WriteLabel(phrase);
    m_xmlfile.Write("\t\t</object>\n");

}
//EDITTEXT        IDC_RADIUS,36,65,40,14,ES_AUTOHSCROLL
void rc2xml::ParseTextCtrl()
{
    wxString token;
    wxString varname,style;
    varname=GetToken();
    int x,y,width,height;
    ReadRect(x,y,width,height);
//TODO
//style=GetToken();
    m_xmlfile.Write("\t\t<object class\"wxTextCtrl\"");
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write("\t\t</object>\n");

}
//PUSHBUTTON      "Create/Update",IDC_CREATE,15,25,53,13,NOT WS_TABSTOP
void rc2xml::ParsePushButton()
{
    wxString token;
    wxString phrase,varname;
    phrase=GetQuoteField();
    varname=GetToken();

    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write("\t\t<object class=\"wxButton\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteLabel(phrase);
    m_xmlfile.Write("\t\t</object>\n");

}


bool rc2xml::Seperator(int ch)
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

void rc2xml::ParseGroupBox()
{
//    GROUPBOX        "Rotate",IDC_STATIC,1,1,71,79
    wxString token;
    wxString phrase,varname;
    phrase=GetQuoteField();
    varname=GetToken();
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write("\t\t<object class=\"wxStaticBox\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteLabel(phrase);
    m_xmlfile.Write("\t\t</object>\n");
}

void rc2xml::ReadRect(int & x, int & y, int & width, int & height)
{
    x=atoi(GetToken());
    y=atoi(GetToken());
    width=atoi(GetToken());
    height=atoi(GetToken());
}

wxString rc2xml::GetToken()
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

wxString rc2xml::GetQuoteField()
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

void rc2xml::ReadChar(int &ch)
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

void rc2xml::ParseComboBox()
{
/* COMBOBOX        IDC_SCALECOMBO,10,110,48,52,CBS_DROPDOWNLIST | CBS_SORT | 
                    WS_VSCROLL | WS_TABSTOP */
    wxString token;
    wxString varname;
    varname=GetToken();
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write("\t\t<object class=\"wxComboBox\"");
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write("\n\t\t</object>\n");

}

void rc2xml::ParseMenu(wxString varname)
{
    wxString token="";

    //Write menubar to xml file
    m_xmlfile.Write("\t<object class=\"wxMenuBar\"");
    //Avoid duplicate names this way
    varname.Replace("IDR_","MB_");
    WriteName(varname);
    m_xmlfile.Write(">\n");

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
    m_xmlfile.Write("\t</object>\n");
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
    name<<"Menu_"<<menucount;
    m_xmlfile.Write("\t\t<object class=\"wxMenu\"");
    WriteName(name);
    m_xmlfile.Write(">\n");
    WriteLabel(token);

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
    m_xmlfile.Write("\t\t\t</object>\n");
}

wxString rc2xml::PeekToken()
{
    wxString token;
    int p;
    p=m_rc.Tell();
    token=GetToken();

    m_rc.Seek(p);
    return token;
}
//MS Windows pain in the butt CONTROL
void rc2xml::ParseControlMS()
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

void rc2xml::ParseSlider(wxString label, wxString varname)
{
    wxString token,style;
    ReadOrs(token);
    if (token.Find("TBS_VERT")!=-1)
        style+="wxSL_VERTICAL";
    //MFC RC Default is horizontal
    else
        style+="wxSL_HORIZONTAL";

    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write("\t\t<object class=\"wxSlider\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write("\n\t\t</object>\n");

}
/*    
CONTROL         "Progress1",CG_IDC_PROGDLG_PROGRESS,"msctls_progress32",
                    WS_BORDER,15,52,154,13
*/
void rc2xml::ParseProgressBar(wxString label, wxString varname)
{
    wxString token,style;
    ReadOrs(token);
   
    int x,y,width,height;
    ReadRect(x,y,width,height);

//Always horizontal in MFC
    m_xmlfile.Write("\t\t<object class=\"wxGauge\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write("\t\t</object>\n");
}

bool rc2xml::ReadOrs(wxString & orstring)
{
    wxString token;

    token=PeekToken();
    if (token.IsNumber())
        return FALSE;
    orstring=GetToken();

    while(PeekToken()==_T("|"))
    {
    //Grab | 
    orstring+=GetToken();
    //Grab next token
    orstring+=GetToken();
    }
    return TRUE;
}

//Is it a check button or a radio button
void rc2xml::ParseCtrlButton(wxString label, wxString varname)
{
    wxString token;
    ReadOrs(token);
    int x,y,width,height;

    if (token.Find("BS_AUTOCHECKBOX")!=-1)
        {
        ReadRect(x,y,width,height);
        m_xmlfile.Write("\t\t<object class=\"wxCheckBox\"");
        WriteBasicInfo(x,y,width,height,varname);
        WriteLabel(label);
        m_xmlfile.Write("\t\t</object>\n");
        }

    if (token.Find("BS_AUTORADIOBUTTON")!=-1)
        {
        ReadRect(x,y,width,height);
        m_xmlfile.Write("\t\t<object class=\"wxRadioButton\"");
        WriteBasicInfo(x,y,width,height,varname);
        WriteLabel(label);
        m_xmlfile.Write("\t\t</object>\n");
        }

}


void rc2xml::WriteSize(int width, int height)
{
    wxString msg;
    msg<<" <size>"<<width<<","<<height<<"d</size>";
    m_xmlfile.Write(msg);
}

void rc2xml::WritePosition(int x, int y)
{
    wxString msg;
    msg<<" <pos>"<<x<<","<<y<<"d</pos>";
    m_xmlfile.Write(msg);
}

void rc2xml::WriteTitle(wxString title)
{
    wxString msg;
    msg=_T("\t\t<title>"+title+"</title>\n");
    m_xmlfile.Write(msg);
}

void rc2xml::WriteName(wxString name)
{
	
//Try to convert any number ids into names
name=LookUpId(name);
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

void rc2xml::WriteLabel(wxString label)
{
    label.Replace("&","$");
    m_xmlfile.Write("\t\t\t<label>"+label+"</label>\n");
}

void rc2xml::WriteBasicInfo(int x, int y, int width, int height, wxString name)
{
    WriteName(name);
    m_xmlfile.Write(">\n");
    m_xmlfile.Write("\t\t\t");
    WritePosition(x,y);
    WriteSize(width,height);
    m_xmlfile.Write("\n");
}

void rc2xml::WriteStyle(wxString style)
{
    if (style.Length()==0)
        return;
    m_xmlfile.Write("\n\t\t<style>"+style+"</style>\n");
}
/*
    LISTBOX         IDC_LIST1,16,89,48,40,LBS_SORT | LBS_MULTIPLESEL | 
                    LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP
*/
void rc2xml::ParseListBox()
{
    wxString token;
    wxString varname;
    varname=GetToken();
    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write("\t\t<object class=\"wxListBox\"");
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write("\n\t\t</object>\n");

}
/*
    CONTROL         "",IDC_RICHEDIT1,"RICHEDIT",ES_AUTOHSCROLL | WS_BORDER | 
                    WS_TABSTOP,103,110,40,14
*/
void rc2xml::ParseRichEdit(wxString label, wxString varname)
{
    wxString token;
    //while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    wxString style;
//Make it a rich text control
    style+="wxTE_MULTILINE ";
    m_xmlfile.Write("\t\t<object class=\"wxTextCtrl\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write("\t\t</object>\n");

}
/*
CONTROL         "Spin1",IDC_SPIN1,"msctls_updown32",UDS_ARROWKEYS,209,72,
                 19,26
*/
void rc2xml::ParseSpinCtrl(wxString label, wxString varname)
{
    wxString token,style;
    
    ReadOrs(token);
    if (token.Find("UDS_HORZ")!=-1)
        style="wxSP_HORIZONTAL";
    //MFC default
    else
        style="wxSP_VERTICAL";

    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write("\t\t<object class=\"wxSpinButton\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write("\n\t\t</object>\n");

}

void rc2xml::FirstPass()
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

void rc2xml::ParseBitmap(wxString varname)
{
    wxString token,*bitmapfile;
    
    token=PeekToken();
    //Microsoft notation?
    if (token=="DISCARDABLE")
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
        if (token=="DIALOG")
            ParseDialog(prevtok);
        else if (token=="MENU")
            ParseMenu(prevtok);
        else if (token=="TOOLBAR")
            ParseToolBar(prevtok);
        
        prevtok=token;
        }

}

void rc2xml::ParseToolBar(wxString varname)
{
    wxString token;	
    token=GetToken();
    wxASSERT_MSG(token=="DISCARDABLE","Error in toolbar parsing");
//Look up bitmap for toolbar and load
    wxNode *node=m_bitmaplist->Find(LookUpId(varname));
    wxString *bitmappath;
    bitmappath=(wxString *)node->Data();
    wxBitmap bitmap;
    if (!bitmap.LoadFile(*bitmappath,wxBITMAP_TYPE_BMP ))
        wxLogError("Unable to load bitmap:"+*bitmappath);

//Write toolbar to xml file
    m_xmlfile.Write("	<object class=\"wxToolBar\"");
//Avoid duplicate names this way
    varname.Replace("IDR_","TB_");
    WriteName(varname);
    m_xmlfile.Write(">\n");
    wxString style;
    style+="wxTB_FLAT";
    WriteStyle(style);


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
            m_xmlfile.Write("\t\t\t<object class=\"tool\"");
            WriteName(buttonname);
            m_xmlfile.Write(">\n");
    //Write tool tip if any
            if (LookUpString(buttonname,msg))
                {
                SplitHelp(msg,tip,longhelp);
                m_xmlfile.Write("\t\t\t\t<tooltip>"+tip+"</tooltip>\n");
                m_xmlfile.Write("		<longhelp>"+longhelp+"</longhelp>\n");
                }
            //Make a bitmap file name
            buttonname=CleanName(buttonname);
            buttonname+=".bmp";
            m_xmlfile.Write("\t\t\t\t<bitmap>"+buttonname+"</bitmap>\n");
        WriteToolButton(buttonname,c,width,height,bitmap);
        m_xmlfile.Write("\t\t\t</object>\n");
        c++;
        }
    else if (token=="SEPARATOR")
    {
    m_xmlfile.Write("\t\t\t<object class=\"separator\"/>\n");
    }
    token=GetToken();
    }
    m_xmlfile.Write("\t</object>\n");
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

void rc2xml::ParseStringTable(wxString varname)
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

bool rc2xml::LookUpString(wxString strid,wxString & st)
{
    wxNode *node=m_stringtable->Find(strid);
    wxString *s;
    if (node==NULL)
        return FALSE;

    s=(wxString *)node->Data();
    st=*s;

    return TRUE;
}

bool rc2xml::SplitHelp(wxString msg, wxString &shorthelp, wxString &longhelp)
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

void rc2xml::ParseMenuItem()
{
    wxString token,name,msg,tip,longhelp;
//int spot;
    if (PeekToken()=="SEPARATOR")
        {
        m_xmlfile.Write("\t\t\t<object class=\"separator\"/>\n");
        return;
        }

    token=GetQuoteField();
    name=GetToken();
//Remove \t because it causes problems
//spot=token.First("\\t");
//token=token.Left(spot);
    m_xmlfile.Write("\t\t\t<object class=\"wxMenuItem\"");
    WriteName(name);
    m_xmlfile.Write(">\n");
    WriteLabel(token);
//Look up help if any listed in stringtable
    if (LookUpString(name,msg))
        {
        SplitHelp(msg,tip,longhelp);
        m_xmlfile.Write("\t\t\t<help>"
            +longhelp+"</help>\n");
        }
//look for extra attributes like checked and break
    wxString ptoken;
    ptoken=PeekToken();
    while ((ptoken!="MENUITEM")&(ptoken!="POPUP")&(ptoken!="END"))
        {
        token=GetToken();
        if (token=="CHECKED")
            m_xmlfile.Write("\t\t\t<checkable>1</checkable>\n");
        else if (token=="MENUBREAK");
//m_xmlfile.Write("\t\t\t</break>\n");
        else if (token=="GRAYED");
        else
            wxLogError("Unknown Menu Item token:"+token);
        
        ptoken=PeekToken();
        }
    m_xmlfile.Write("\t\t\t</object>\n"); 

}

//ICON            IDR_MAINFRAME,IDC_STATIC,11,17,20,20
void rc2xml::ParseIconStatic()
{
    wxString token;
    wxString varname,iconname;
    iconname=GetToken();
//Look up icon
    varname=GetToken();

    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write("\t\t<object class=\"wxStaticBitmap\"");
    WriteBasicInfo(x,y,width,height,varname);
//Save icon as a bitmap
    WriteIcon(iconname);
    m_xmlfile.Write("\t\t</object>\n");

}
//IDR_MAINFRAME           ICON    DISCARDABLE     "res\\mfcexample.ico"
void rc2xml::ParseIcon(wxString varname)
{
    wxString token,*iconfile;
    iconfile=new wxString;
    token=PeekToken();

    *iconfile=GetQuoteField();
    m_iconlist->Append(varname,iconfile);
    

}

wxString rc2xml::CleanName(wxString name)
{
    name.MakeLower();
    name.Replace("id_","");
    name.Replace("idr_","");
    name.Replace("idb_","");
    name.Replace("idc_","");
    name.Replace(".ico","");
    name.Replace(".bmp","");
    return name;
}
// And the award for most messed up control goes to...
//    CONTROL         IDB_FACE,IDC_STATIC,"Static",SS_BITMAP,26,62,32,30
void rc2xml::ParseStaticBitmap(wxString bitmapname, wxString varname)
{
    wxString token;
    //Grab SS_BITMAP
    token=GetToken();



    int x,y,width,height;
    ReadRect(x,y,width,height);

    m_xmlfile.Write("\t\t<object class=\"wxStaticBitmap\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteBitmap(bitmapname);
    m_xmlfile.Write("\t\t</object>\n");

}

void rc2xml::ParseNormalMSControl()
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

void rc2xml::ParseWeirdMSControl()
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

void rc2xml::ParseScrollBar()
{
    wxString token;
    wxString varname;

    varname=GetToken();
    int x,y,width,height;
    ReadRect(x,y,width,height);
    wxString style;

    ReadOrs(token);
    
if (token.Find("SBS_VERT")!=-1)
    style=_T("wxSB_VERTICAL");
//Default MFC style is horizontal
    else
        style=_T("wxSB_HORIZONTAL");

    m_xmlfile.Write("\t\t<object class=\"wxScrollBar\"");
    WriteBasicInfo(x,y,width,height,varname);
    WriteStyle(style);
    m_xmlfile.Write("\n\t\t</object>\n");

}
//    CONTROL         "Tree1",IDC_TREE1,"SysTreeView32",WS_BORDER | WS_TABSTOP,
//                      7,7,66,61

void rc2xml::ParseTreeCtrl(wxString label, wxString varname)
{
    wxString token;
//while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write("\t\t<object class=\"wxTreeCtrl\"");
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write("\t\t</object>\n");

}
//    CONTROL         "MonthCalendar1",IDC_MONTHCALENDAR1,"SysMonthCal32",
                    //MCS_NOTODAY | WS_TABSTOP,105,71,129,89

void rc2xml::ParseCalendar(wxString label, wxString varname)
{
    wxString token;
//while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write("\t\t<object class=\"wxCalendarCtrl\"");
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write("\t\t</object>\n");
}
//    CONTROL         "List1",IDC_LIST1,"SysListView32",WS_BORDER | WS_TABSTOP,
  //                  7,89,68,71

void rc2xml::ParseListCtrl(wxString label, wxString varname)
{
    wxString token;
    //while (ReadOrs(token));
    ReadOrs(token);
    int x,y,width,height;
    ReadRect(x,y,width,height);
    m_xmlfile.Write("\t\t<object class=\"wxListCtrl\"");
    WriteBasicInfo(x,y,width,height,varname);
    m_xmlfile.Write("\t\t</object>\n");

}

void rc2xml::WriteBitmap(wxString bitmapname)
{
//Look up bitmap
    wxNode *node=m_bitmaplist->Find(LookUpId(bitmapname));
    if (node==NULL)
        {
        m_xmlfile.Write("\t\t\t<bitmap>missingfile</bitmap>\n");
        wxLogError("Unable to find bitmap:"+bitmapname);
        return;
        }
    
    wxString *bitmappath;
    bitmappath=(wxString *)node->Data();
    bitmapname=wxFileNameFromPath(*bitmappath);
    wxBitmap bitmap;
    if (!bitmap.LoadFile(*bitmappath,wxBITMAP_TYPE_BMP ))
        wxLogError("Unable to load bitmap:"+*bitmappath);

    //Make a bitmap file name
    bitmapname=CleanName(bitmapname);
    bitmapname+=".bmp";
    m_xmlfile.Write("\t\t\t<bitmap>"+bitmapname+"</bitmap>\n");
    bitmap.SaveFile(m_targetpath+bitmapname,wxBITMAP_TYPE_BMP);
}

void rc2xml::WriteIcon(wxString iconname)
{
wxNode *node=m_iconlist->Find(iconname);
    if (node==NULL)
        {
        m_xmlfile.Write("\t\t\t<bitmap>missing_file</bitmap>\n");
        wxLogError("Unable to find icon:"+iconname);
        }
    wxString *iconpath;
    iconpath=(wxString *)node->Data();
    wxIcon icon;
    wxBitmap bitmap;
    if (!icon.LoadFile(*iconpath,wxBITMAP_TYPE_ICO ))
        wxLogError("Unable to load icon:"+*iconpath);
#ifdef __WXMSW__
    bitmap.CopyFromIcon(icon);
#else
    bitmap = icon;
#endif
    iconname=wxFileNameFromPath(*iconpath);
    //Make a bitmap file name
    iconname=CleanName(iconname);
    iconname+=".bmp";
    m_xmlfile.Write("\t\t\t<bitmap>"+iconname+"</bitmap>\n");
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
    if (!r.Open("resource.h"))
        {
        wxLogError("Warining Unable to load resource.h file");
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
    if (str.Find("#define")!=-1)
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

    s=(wxString *)node->Data();
    st=*s;
return st;
}
