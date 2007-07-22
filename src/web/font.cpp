/////////////////////////////////////////////////////////////////////////////
// Name:        src/web/font.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: font.cpp 45188 2007-03-31 15:10:20Z VZ $
// Copyright:   (c) 1998 Robert Roebling, Julian Smart, and John Wilmes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/cmndata.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/fontutil.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the default size (in points) for the fonts
static const int wxDEFAULT_FONT_SIZE = 12;

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

class wxFontRefData : public wxObjectRefData {
public:
    wxFontRefData(int size = -1,
                  int family = wxFONTFAMILY_DEFAULT,
                  int style = wxFONTSTYLE_NORMAL,
                  int weight = wxFONTWEIGHT_NORMAL,
                  bool underlined = false,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    wxFontRefData(const wxString& fontname);
    wxFontRefData(const wxFontRefData& data);
    virtual ~wxFontRefData();

    void SetPointSize(int pointSize);
    void SetFamily(int family);
    void SetStyle(int style);
    void SetWeight(int weight);
    void SetUnderlined(bool underlined);
    bool SetFaceName(const wxString& facename);
    void SetEncoding(wxFontEncoding encoding);

    // and this one also modifies all the other font data fields
    void SetNativeFontInfo(const wxNativeFontInfo& info);

protected:
    // common part of all ctors
    void Init(int pointSize,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    // set all fields from (already initialized and valid) m_nativeFontInfo
    void InitFromNative();

private:
    int             m_pointSize;
    int             m_family,
                    m_style,
                    m_weight;
    bool            m_underlined;
    wxString        m_faceName;
    wxFontEncoding  m_encoding;

    wxNativeFontInfo m_nativeFontInfo;

    friend class wxFont;
};

#define M_FONTDATA ((wxFontRefData*)m_refData)

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         int family,
                         int style,
                         int weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_family = family;

    m_faceName = faceName;

    // we accept both wxDEFAULT and wxNORMAL here - should we?
    m_style = style == wxDEFAULT ? wxFONTSTYLE_NORMAL : style;
    m_weight = weight == wxDEFAULT ? wxFONTWEIGHT_NORMAL : weight;

    // and here, do we really want to forbid creation of the font of the size
    // 90 (the value of wxDEFAULT)??
    m_pointSize = pointSize == wxDEFAULT || pointSize == -1
                    ? wxDEFAULT_FONT_SIZE
                    : pointSize;

    m_underlined = underlined;
    m_encoding = encoding;
    if (m_encoding == wxFONTENCODING_DEFAULT)
        m_encoding = wxFont::GetDefaultEncoding();

    SetStyle(m_style);
    SetPointSize(m_pointSize);
    SetWeight(m_weight);
}

void wxFontRefData::InitFromNative()
{
    m_pointSize = m_nativeFontInfo.GetPointSize();
    m_style = m_nativeFontInfo.GetStyle();
    m_weight = m_nativeFontInfo.GetWeight();
}

wxFontRefData::wxFontRefData( const wxFontRefData& data )
             : wxObjectRefData()
{
    m_pointSize = data.m_pointSize;
    m_family = data.m_family;
    m_style = data.m_style;
    m_weight = data.m_weight;

    m_underlined = data.m_underlined;

    m_faceName = data.m_faceName;
    m_encoding = data.m_encoding;

    m_nativeFontInfo.FromString(data.m_nativeFontInfo.ToString());
}

wxFontRefData::wxFontRefData(int size, int family, int style,
                             int weight, bool underlined,
                             const wxString& faceName,
                             wxFontEncoding encoding)
{
    Init(size, family, style, weight, underlined, faceName, encoding);
}

wxFontRefData::wxFontRefData(const wxString& fontname)
{
    m_nativeFontInfo.FromString(fontname);

    InitFromNative();
}

wxFontRefData::~wxFontRefData()
{
}

// ----------------------------------------------------------------------------
// wxFontRefData SetXXX()
// ----------------------------------------------------------------------------

void wxFontRefData::SetPointSize(int pointSize) {
    m_pointSize = pointSize;

    m_nativeFontInfo.SetPointSize(pointSize);
}

void wxFontRefData::SetFamily(int family) {
    m_family = family;

    m_nativeFontInfo.SetFamily((wxFontFamily)family);
}

void wxFontRefData::SetStyle(int style) {
    m_style = style;

    m_nativeFontInfo.SetStyle((wxFontStyle)style);
}

void wxFontRefData::SetWeight(int weight) {
    m_weight = weight;

    m_nativeFontInfo.SetWeight((wxFontWeight)weight);
}

void wxFontRefData::SetUnderlined(bool underlined) {
    m_underlined = underlined;

    return m_nativeFontInfo.SetUnderlined(underlined);
}

bool wxFontRefData::SetFaceName(const wxString& facename) {
    m_faceName = facename;

    return m_nativeFontInfo.SetFaceName(facename);
}

void wxFontRefData::SetEncoding(wxFontEncoding encoding) {
    m_encoding = encoding;

    return m_nativeFontInfo.SetEncoding(encoding);
}

void wxFontRefData::SetNativeFontInfo(const wxNativeFontInfo& info) {
    m_nativeFontInfo = info;

    InitFromNative();
}

// ----------------------------------------------------------------------------
// wxFont creation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

wxFont::wxFont() {
    Create("");
}

wxFont::wxFont(const wxNativeFontInfo& info) {
    Create(info.GetPointSize(),
           info.GetFamily(),
           info.GetStyle(),
           info.GetWeight(),
           info.GetUnderlined(),
           info.GetFaceName(),
           info.GetEncoding());
}

wxFont::wxFont(int size,
               int family,
               int style,
               int weight,
               bool underlined,
               const wxString& face,
               wxFontEncoding encoding)
{
    Create(size, family, style, weight, underlined, face, encoding);
}


bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& face,
                    wxFontEncoding encoding)
{
    UnRef();

    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, face, encoding);

    return true;
}

bool wxFont::Create(const wxString& fontname)
{
    if (fontname.empty()) {
        *this = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

        return true;
    }

    m_refData = new wxFontRefData(fontname);

    return true;
}

wxFont::~wxFont()
{
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const {
    wxCHECK_MSG(Ok(), 0, wxT("invalid font"));

    return M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceName() const {
    wxCHECK_MSG(Ok(), wxEmptyString, wxT("invalid font"));

    return M_FONTDATA->m_faceName;
}

int wxFont::GetFamily() const {
    wxCHECK_MSG(Ok(), 0, wxT("invalid font"));

    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const {
    wxCHECK_MSG(Ok(), 0, wxT("invalid font"));

    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const {
    wxCHECK_MSG(Ok(), 0, wxT("invalid font"));

    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const {
    wxCHECK_MSG(Ok(), false, wxT("invalid font"));

    return M_FONTDATA->m_underlined;
}

wxFontEncoding wxFont::GetEncoding() const {
    wxCHECK_MSG(Ok(), wxFONTENCODING_SYSTEM, wxT("invalid font"));

    return M_FONTDATA->m_encoding;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG(Ok(), (wxNativeFontInfo *)NULL, wxT("invalid font"));

    return &(M_FONTDATA->m_nativeFontInfo);
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize) {
    AllocExclusive();

    M_FONTDATA->SetPointSize(pointSize);
}

void wxFont::SetFamily(int family) {
    AllocExclusive();

    M_FONTDATA->SetFamily(family);
}

void wxFont::SetStyle(int style) {
    AllocExclusive();

    M_FONTDATA->SetStyle(style);
}

void wxFont::SetWeight(int weight) {
    AllocExclusive();

    M_FONTDATA->SetWeight(weight);
}

bool wxFont::SetFaceName(const wxString& faceName) {
    AllocExclusive();

    return M_FONTDATA->SetFaceName(faceName) &&
           wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined) {
    AllocExclusive();

    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding) {
    AllocExclusive();

    M_FONTDATA->SetEncoding(encoding);
}

void wxFont::DoSetNativeFontInfo(const wxNativeFontInfo& info)
{
    AllocExclusive();

    M_FONTDATA->SetNativeFontInfo(info);
}

wxObjectRefData* wxFont::CreateRefData() const {
    return new wxFontRefData;
}

wxObjectRefData* wxFont::CloneRefData(const wxObjectRefData* data) const {
    return new wxFontRefData(*wx_static_cast(const wxFontRefData*, data));
}
