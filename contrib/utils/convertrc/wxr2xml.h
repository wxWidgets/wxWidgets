// wxr2xml.h: 
// 8/30/00 Brian Gavin
//////////////////////////////////////////////////////////////////////

#if !defined(WXR2XML_H)
#define WXR2XML_H

#include <wx/ffile.h>
#include "wx/resource.h"


class wxr2xml : public wxObject  
{
public:
    bool Convert(wxString wxrfile,wxString xmlfile);
    wxr2xml();
    virtual ~wxr2xml();

protected:
    void GetFontWeight(wxFont font);
    void GetFontStyle(wxFont font);
    void ParseBitmapButton(wxItemResource *res);
    void GetFontFace(wxFont font);
    void WriteFontInfo(wxItemResource *res);
    wxString GetValue2(wxItemResource *res);
    wxString GetValue3(wxItemResource *res);
    wxString GetValue5(wxItemResource *res);
    void PanelStuff(wxItemResource *res);
    bool m_systemdefaults;
    bool m_dlgunits;
    void ParseBitmap(wxItemResource *res);
    void ParseStaticBitmap(wxItemResource *res);
    wxString FixMenuString(wxString phrase);
    void ParseMenuItem(wxItemResource *res);
    void ParseMenu(wxItemResource *res);
    void ParseMenuBar(wxItemResource *res);
    wxString GenerateName(wxItemResource *res);
    wxString GetStyles(wxItemResource *res);
    wxString GetDimension(wxItemResource *res);
    void ParsePanel(wxItemResource *res);
    void ParseRadioButton(wxItemResource *res);
    wxString GetMin(wxItemResource *res);
    wxString GetCheckStatus(wxItemResource *res);
    void ParseScrollBar(wxItemResource *res);
    void ParseComboBox(wxItemResource * res);
    wxString GetMax(wxItemResource *res);
    void ParseSlider(wxItemResource *res);
    wxString GetValue1(wxItemResource *res);
    wxString GetRange(wxItemResource *res);
    void ParseGauge(wxItemResource *res);
    void ParseChoice(wxItemResource *res);
    void WriteStringList(wxItemResource *res);
    void ParseStaticBox(wxItemResource *res);
    void ParseRadioBox(wxItemResource *res);
    wxString GetLabel(wxItemResource *res);
    void ParseCheckBox(wxItemResource *res);
    wxString GetValue4(wxItemResource *res);
    wxString GetTitle(wxItemResource *res);
    void ParseTextCtrl(wxItemResource *res);
    void ParseButton(wxItemResource *res);
    wxString GetPosition(wxItemResource *res);
    void WriteControlInfo(wxItemResource *res);
    void ParseStaticText(wxItemResource *res);
    void ParseListBox(wxItemResource *res);
    wxString GetSize(wxItemResource *res);
    void ParseControls(wxItemResource *res);
    void ParseDialog(wxItemResource *res);
    bool ParseResources();
    //Variables
    wxResourceTable m_table;
    wxFFile m_xmlfile;
};

#endif 
