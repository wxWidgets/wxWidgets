// wxHandleWXR.h: interface for the wxHandleWXR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(RC2WXR_H)
#define RC2WXR_H

#include "wx/file.h"
#include "stdio.h"

class wxRC2WXR : public wxObject  
{
public:
wxRC2WXR();
~wxRC2WXR();
void Open(wxString wxrfile, wxString rcfile);

private:
wxFile m_rc;
FILE *m_wxr;
int m_filesize;
bool m_done;
int m_controlid;
void ParseDialog(wxString dlgname);
void ParseControls();
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
void ParseMenu(wxString name);
void ParsePopupMenu();
wxString PeekToken();
void ParseControlMS();
void ParseSlider(wxString label, wxString varname);
void ParseProgressBar(wxString label, wxString varname);
bool ReadOrs(wxString & w);
void ParseCtrlButton(wxString label, wxString varname);

};

class wxFileProgressDlg : public wxDialog
{
public:
	void UpdateProgress(wxFile *f);

wxFileProgressDlg();
virtual ~wxFileProgressDlg();

protected:
wxGauge *m_pProgress;
wxStaticText *m_pCompleteLabel;

  DECLARE_EVENT_TABLE()

};
class GenerateBitmapSrc : public wxObject  
{
public:
	bool Create(wxString imfile, wxString srcfile,wxString varname);
	GenerateBitmapSrc();
	virtual ~GenerateBitmapSrc();

};

#endif