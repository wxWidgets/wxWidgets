// rc2xml.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(RC2XML_H)
#define RC2XML_H

#include "wx/file.h"
#include <wx/ffile.h>
#include <wx/list.h>


class rc2xml : public wxObject  
{
public:
    void WriteIcon(wxString iconname);
    void ParseNormalMSControl();
    bool Convert(wxString rcfile, wxString xmlfile);
    rc2xml();
    ~rc2xml();

protected:
	wxString LookUpId(wxString id);
	
    void ParseResourceHeader();
    void WriteBitmap(wxString bitmapname);
    void ParseListCtrl(wxString label,wxString varname);
    void ParseCalendar(wxString label,wxString varname);
    void ParseTreeCtrl(wxString label,wxString varname);
    void ParseScrollBar();
    void ParseWeirdMSControl();
    void ParseStaticBitmap(wxString label,wxString varname);
    wxString CleanName(wxString name);
    void ParseIcon(wxString varname);
    wxList * m_iconlist;
    void ParseIconStatic();
    void ParseMenuItem();
	
//Functions
    bool SplitHelp(wxString msg, wxString &shorthelp, wxString &longhelp);
    bool LookUpString(wxString strid,wxString & st);
    void ParseStringTable(wxString varname);
    void WriteToolButton(wxString name,int index,int width,int height,wxBitmap bitmap);
    wxString LookupString(wxString varname,wxStringList id,wxStringList msg);
    void ParseToolBar(wxString varname);
    void SecondPass();
    void FirstPass();
    void ParseBitmap(wxString varname);
    void ParseSpinCtrl(wxString label,wxString varname);
    void ParseRichEdit(wxString label, wxString varname);
    void ParseDialog(wxString dlgname);
    void ParseControls();
    void ParseListBox();
    void ParseStaticText();
    void ParseTextCtrl();
    void ParsePushButton();
    bool Seperator(int ch);
    void ParseGroupBox();
    void ReadRect(int & x, int & y, int & width, int & height);
    wxString GetToken();
    wxString GetQuoteField();
    void ReadChar(int &ch);
    void ParseComboBox();
    void ParseMenu(wxString varname);
    void ParsePopupMenu();
    wxString PeekToken();
    void ParseControlMS();
    void ParseSlider(wxString label, wxString varname);
    void ParseProgressBar(wxString label, wxString varname);
    bool ReadOrs(wxString & w);
    void ParseCtrlButton(wxString label, wxString varname);
    void WriteStyle(wxString style);
    void WriteBasicInfo(int x,int y,int width,int height,wxString name);
    void WriteName(wxString name);
    void WriteTitle(wxString title);
    void WritePosition(int x,int y);
    void WriteSize(int width,int height);
    void WriteLabel(wxString label);
//variables
    wxList * m_stringtable;
    wxList *m_bitmaplist;
    wxList * m_resourcelist;
    wxFile m_rc;
    wxFFile m_xmlfile;
    int m_filesize;
    bool m_done;

};


#endif