/////////////////////////////////////////////////////////////////////////////
// Name:        fonts.i
// Purpose:     SWIG interface file wxFont, local, converters, etc.
//
// Author:      Robin Dunn
//
// Created:     1-Apr-2002
// RCS-ID:      $Id$
// Copyright:   (c) 2002 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module fonts

%{
#include "helpers.h"
#include <wx/fontmap.h>
#include <wx/fontenc.h>
#include <wx/fontutil.h>
#include <wx/fontenum.h>
#include <wx/intl.h>
#include <wx/encconv.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i


//---------------------------------------------------------------------------
%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyEmptyString(wxT(""));
%}
//---------------------------------------------------------------------------



enum wxFontFamily
{
    wxFONTFAMILY_DEFAULT = wxDEFAULT,
    wxFONTFAMILY_DECORATIVE = wxDECORATIVE,
    wxFONTFAMILY_ROMAN = wxROMAN,
    wxFONTFAMILY_SCRIPT = wxSCRIPT,
    wxFONTFAMILY_SWISS = wxSWISS,
    wxFONTFAMILY_MODERN = wxMODERN,
    wxFONTFAMILY_TELETYPE = wxTELETYPE,
    wxFONTFAMILY_MAX,
    wxFONTFAMILY_UNKNOWN
};

// font styles
enum wxFontStyle
{
    wxFONTSTYLE_NORMAL = wxNORMAL,
    wxFONTSTYLE_ITALIC = wxITALIC,
    wxFONTSTYLE_SLANT = wxSLANT,
    wxFONTSTYLE_MAX
};

// font weights
enum wxFontWeight
{
    wxFONTWEIGHT_NORMAL = wxNORMAL,
    wxFONTWEIGHT_LIGHT = wxLIGHT,
    wxFONTWEIGHT_BOLD = wxBOLD,
    wxFONTWEIGHT_MAX
};



// font encodings
enum wxFontEncoding
{
    wxFONTENCODING_SYSTEM = -1,     // system default
    wxFONTENCODING_DEFAULT,         // current default encoding

    // ISO8859 standard defines a number of single-byte charsets
    wxFONTENCODING_ISO8859_1,       // West European (Latin1)
    wxFONTENCODING_ISO8859_2,       // Central and East European (Latin2)
    wxFONTENCODING_ISO8859_3,       // Esperanto (Latin3)
    wxFONTENCODING_ISO8859_4,       // Baltic (old) (Latin4)
    wxFONTENCODING_ISO8859_5,       // Cyrillic
    wxFONTENCODING_ISO8859_6,       // Arabic
    wxFONTENCODING_ISO8859_7,       // Greek
    wxFONTENCODING_ISO8859_8,       // Hebrew
    wxFONTENCODING_ISO8859_9,       // Turkish (Latin5)
    wxFONTENCODING_ISO8859_10,      // Variation of Latin4 (Latin6)
    wxFONTENCODING_ISO8859_11,      // Thai
    wxFONTENCODING_ISO8859_12,      // doesn't exist currently, but put it
                                    // here anyhow to make all ISO8859
                                    // consecutive numbers
    wxFONTENCODING_ISO8859_13,      // Baltic (Latin7)
    wxFONTENCODING_ISO8859_14,      // Latin8
    wxFONTENCODING_ISO8859_15,      // Latin9 (a.k.a. Latin0, includes euro)
    wxFONTENCODING_ISO8859_MAX,

    // Cyrillic charset soup (see http://czyborra.com/charsets/cyrillic.html)
    wxFONTENCODING_KOI8,            // we don't support any of KOI8 variants
    wxFONTENCODING_ALTERNATIVE,     // same as MS-DOS CP866
    wxFONTENCODING_BULGARIAN,       // used under Linux in Bulgaria

    // what would we do without Microsoft? They have their own encodings
        // for DOS
    wxFONTENCODING_CP437,           // original MS-DOS codepage
    wxFONTENCODING_CP850,           // CP437 merged with Latin1
    wxFONTENCODING_CP852,           // CP437 merged with Latin2
    wxFONTENCODING_CP855,           // another cyrillic encoding
    wxFONTENCODING_CP866,           // and another one
        // and for Windows
    wxFONTENCODING_CP874,           // WinThai
    wxFONTENCODING_CP932,           // Japanese (shift-JIS)
    wxFONTENCODING_CP936,           // Chinese simplified (GB)
    wxFONTENCODING_CP949,           // Korean (Hangul charset)
    wxFONTENCODING_CP950,           // Chinese (traditional - Big5)
    wxFONTENCODING_CP1250,          // WinLatin2
    wxFONTENCODING_CP1251,          // WinCyrillic
    wxFONTENCODING_CP1252,          // WinLatin1
    wxFONTENCODING_CP1253,          // WinGreek (8859-7)
    wxFONTENCODING_CP1254,          // WinTurkish
    wxFONTENCODING_CP1255,          // WinHebrew
    wxFONTENCODING_CP1256,          // WinArabic
    wxFONTENCODING_CP1257,          // WinBaltic (same as Latin 7)
    wxFONTENCODING_CP12_MAX,

    wxFONTENCODING_UTF7,            // UTF-7 Unicode encoding
    wxFONTENCODING_UTF8,            // UTF-8 Unicode encoding

    // Far Eastern encodings
        // Chinese
    wxFONTENCODING_GB2312 = wxFONTENCODING_CP936, // Simplified Chinese
    wxFONTENCODING_BIG5 = wxFONTENCODING_CP950,   // Traditional Chinese

        // Japanese (see http://zsigri.tripod.com/fontboard/cjk/jis.html)
    wxFONTENCODING_SHIFT_JIS = wxFONTENCODING_CP932,  // Shift JIS
    wxFONTENCODING_EUC_JP = wxFONTENCODING_UTF8 + 1,  // Extended Unix Codepage
                                                      // for Japanese

    wxFONTENCODING_UNICODE,         // Unicode (for wxEncodingConverter only)

    wxFONTENCODING_MAX
};


//---------------------------------------------------------------------------
// wxNativeFontInfo is platform-specific font representation: this struct
// should be considered as opaque font description only used by the native
// functions, the user code can only get the objects of this type from
// somewhere and pass it somewhere else (possibly save them somewhere using
// ToString() and restore them using FromString())
struct wxNativeFontInfo
{
    wxNativeFontInfo();

    // reset to the default state
    void Init();

    // accessors and modifiers for the font elements
    int GetPointSize() const;
    wxFontStyle GetStyle() const;
    wxFontWeight GetWeight() const;
    bool GetUnderlined() const;
    wxString GetFaceName() const;
    wxFontFamily GetFamily() const;
    wxFontEncoding GetEncoding() const;

    void SetPointSize(int pointsize);
    void SetStyle(wxFontStyle style);
    void SetWeight(wxFontWeight weight);
    void SetUnderlined(bool underlined);
    void SetFaceName(wxString facename);
    void SetFamily(wxFontFamily family);
    void SetEncoding(wxFontEncoding encoding);

    // it is important to be able to serialize wxNativeFontInfo objects to be
    // able to store them (in config file, for example)
    bool FromString(const wxString& s);
    wxString ToString() const;

    %addmethods {
        wxString __str__() {
            return self->ToString();
        }
    }

    // we also want to present the native font descriptions to the user in some
    // human-readable form (it is not platform independent neither, but can
    // hopefully be understood by the user)
    bool FromUserString(const wxString& s);
    wxString ToUserString() const;
};


%{
// Fix some link errors...  Remove this when these methods get real implementations...
#if defined(__WXGTK__) || defined(__WXX11__)
#if wxUSE_PANGO
void wxNativeFontInfo::SetPointSize(int pointsize)
    { wxFAIL_MSG( _T("not implemented") ); }

void wxNativeFontInfo::SetStyle(wxFontStyle style)
    { wxFAIL_MSG( _T("not implemented") ); }

void wxNativeFontInfo::SetWeight(wxFontWeight weight)
    { wxFAIL_MSG( _T("not implemented") ); }

void wxNativeFontInfo::SetUnderlined(bool WXUNUSED(underlined))
    { wxFAIL_MSG( _T("not implemented") ); }

void wxNativeFontInfo::SetFaceName(wxString facename)
    { wxFAIL_MSG( _T("not implemented") ); }

void wxNativeFontInfo::SetFamily(wxFontFamily family)
    { wxFAIL_MSG( _T("not implemented") ); }

void wxNativeFontInfo::SetEncoding(wxFontEncoding encoding)
    { wxFAIL_MSG( _T("not implemented") ); }
#endif
#endif
%}

//---------------------------------------------------------------------------
// wxFontMapper manages user-definable correspondence between logical font
// names and the fonts present on the machine.
//
// The default implementations of all functions will ask the user if they are
// not capable of finding the answer themselves and store the answer in a
// config file (configurable via SetConfigXXX functions). This behaviour may
// be disabled by giving the value of FALSE to "interactive" parameter.
// However, the functions will always consult the config file to allow the
// user-defined values override the default logic and there is no way to
// disable this - which shouldn't be ever needed because if "interactive" was
// never TRUE, the config file is never created anyhow.
class  wxFontMapper
{
public:
    wxFontMapper();
    ~wxFontMapper();

    // return instance of the wxFontMapper singleton
    static wxFontMapper *Get();
    // set the sigleton to 'mapper' instance and return previous one
    static wxFontMapper *Set(wxFontMapper *mapper);


    // find an alternative for the given encoding (which is supposed to not be
    // available on this system). If successful, return TRUE and rwxFontEcoding
    // that can be used it wxFont ctor otherwise return FALSE
    //bool GetAltForEncoding(wxFontEncoding encoding,
    //                       wxFontEncoding *alt_encoding,
    //                       const wxString& facename = wxPyEmptyString,
    //                       bool interactive = TRUE);


    // Find an alternative for the given encoding (which is supposed to not be
    // available on this system). If successful, returns the encoding otherwise
    // returns None.
    %addmethods {
        PyObject* GetAltForEncoding(wxFontEncoding encoding,
                                    const wxString& facename = wxPyEmptyString,
                                    bool interactive = TRUE) {
            wxFontEncoding alt_enc;
            if (self->GetAltForEncoding(encoding, &alt_enc, facename, interactive))
                return PyInt_FromLong(alt_enc);
            else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
    }


    // checks whether given encoding is available in given face or not.
    // If no facename is given,
    bool IsEncodingAvailable(wxFontEncoding encoding,
                             const wxString& facename = wxPyEmptyString);

    // returns the encoding for the given charset (in the form of RFC 2046) or
    // wxFONTENCODING_SYSTEM if couldn't decode it
    wxFontEncoding CharsetToEncoding(const wxString& charset,
                                     bool interactive = TRUE);

    // return internal string identifier for the encoding (see also
    // GetEncodingDescription())
    static wxString GetEncodingName(wxFontEncoding encoding);

    // return user-readable string describing the given encoding
    //
    // NB: hard-coded now, but might change later (read it from config?)
    static wxString GetEncodingDescription(wxFontEncoding encoding);

    // the parent window for modal dialogs
    void SetDialogParent(wxWindow *parent);

    // the title for the dialogs (note that default is quite reasonable)
    void SetDialogTitle(const wxString& title);

    // functions which allow to configure the config object used: by default,
    // the global one (from wxConfigBase::Get() will be used) and the default
    // root path for the config settings is the string returned by
    // GetDefaultConfigPath()


    // set the config object to use (may be NULL to use default)
    void SetConfig(wxConfigBase *config);

    // set the root config path to use (should be an absolute path)
    void SetConfigPath(const wxString& prefix);

    // return default config path
    static wxString GetDefaultConfigPath();
};



//---------------------------------------------------------------------------

class wxFont : public wxObject {
public:
    wxFont( int pointSize, int family, int style, int weight,
            int underline=FALSE, const wxString& faceName = wxPyEmptyString,
            wxFontEncoding encoding=wxFONTENCODING_DEFAULT);

    %name(wxFontFromNativeInfo)wxFont(const wxNativeFontInfo& info);
    %addmethods {
        %new wxFont* wxFontFromNativeInfoString(const wxString& info) {
            wxNativeFontInfo nfi;
            nfi.FromString(info);
            return new wxFont(nfi);
        }
    }

    ~wxFont();

    bool Ok() const;
    int GetPointSize() const;
    int GetFamily() const;
    int GetStyle() const;
    int GetWeight() const;
    bool GetUnderlined() const;
    wxString GetFaceName() const;
    wxFontEncoding GetEncoding() const;

    bool IsFixedWidth();

    wxNativeFontInfo* GetNativeFontInfo() const;
    wxString GetNativeFontInfoDesc() const;
    wxString GetNativeFontInfoUserDesc() const;

    void SetPointSize(int pointSize);
    void SetFamily(int family);
    void SetStyle(int style);
    void SetWeight(int weight);
    void SetFaceName(const wxString& faceName);
    void SetUnderlined(bool underlined);
    void SetEncoding(wxFontEncoding encoding);
    void SetNativeFontInfo(const wxNativeFontInfo& info);
    // void SetNativeFontInfo(const wxString& info);
    void SetNativeFontInfoUserDesc(const wxString& info);

    wxString GetFamilyString() const;
    wxString GetStyleString() const;
    wxString GetWeightString() const;

    void SetNoAntiAliasing( bool no = TRUE );
    bool GetNoAntiAliasing();

    static wxFontEncoding GetDefaultEncoding();
    static void SetDefaultEncoding(wxFontEncoding encoding);

    %pragma(python) addtoclass = "def __nonzero__(self): return self.Ok()"
};


class wxFontList : public wxObject {
public:

    void AddFont(wxFont* font);
    wxFont * FindOrCreateFont(int point_size, int family, int style, int weight,
                              bool underline = FALSE, const wxString& facename = wxPyEmptyString,
                              wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    void RemoveFont(wxFont *font);

    int GetCount();
};


//----------------------------------------------------------------------
// wxFontEnumerator

%{
class wxPyFontEnumerator : public wxFontEnumerator {
public:
    wxPyFontEnumerator() {}
    ~wxPyFontEnumerator() {}

    DEC_PYCALLBACK_BOOL_STRING(OnFacename);
    DEC_PYCALLBACK_BOOL_STRINGSTRING(OnFontEncoding);

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_STRING(wxPyFontEnumerator, wxFontEnumerator, OnFacename);
IMP_PYCALLBACK_BOOL_STRINGSTRING(wxPyFontEnumerator, wxFontEnumerator, OnFontEncoding);

%}

%name(wxFontEnumerator) class wxPyFontEnumerator {
public:
    wxPyFontEnumerator();
    ~wxPyFontEnumerator();
    void _setCallbackInfo(PyObject* self, PyObject* _class, bool incref);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxFontEnumerator, 0)"

    bool EnumerateFacenames(
        wxFontEncoding encoding = wxFONTENCODING_SYSTEM, // all
        bool fixedWidthOnly = FALSE);
    bool EnumerateEncodings(const wxString& facename = wxPyEmptyString);

    //wxArrayString* GetEncodings();
    //wxArrayString* GetFacenames();
    %addmethods {
        PyObject* GetEncodings() {
            wxArrayString* arr = self->GetEncodings();
            return wxArrayString2PyList_helper(*arr);
        }

        PyObject* GetFacenames() {
            wxArrayString* arr = self->GetFacenames();
            return wxArrayString2PyList_helper(*arr);
        }
    }
};

//---------------------------------------------------------------------------
// wxLocale.  Not really font related, but close enough


enum wxLanguage
{
    // user's default/preffered language as got from OS:
    wxLANGUAGE_DEFAULT,
    // unknown language, if wxLocale::GetSystemLanguage fails:
    wxLANGUAGE_UNKNOWN,

    wxLANGUAGE_ABKHAZIAN,
    wxLANGUAGE_AFAR,
    wxLANGUAGE_AFRIKAANS,
    wxLANGUAGE_ALBANIAN,
    wxLANGUAGE_AMHARIC,
    wxLANGUAGE_ARABIC,
    wxLANGUAGE_ARABIC_ALGERIA,
    wxLANGUAGE_ARABIC_BAHRAIN,
    wxLANGUAGE_ARABIC_EGYPT,
    wxLANGUAGE_ARABIC_IRAQ,
    wxLANGUAGE_ARABIC_JORDAN,
    wxLANGUAGE_ARABIC_KUWAIT,
    wxLANGUAGE_ARABIC_LEBANON,
    wxLANGUAGE_ARABIC_LIBYA,
    wxLANGUAGE_ARABIC_MOROCCO,
    wxLANGUAGE_ARABIC_OMAN,
    wxLANGUAGE_ARABIC_QATAR,
    wxLANGUAGE_ARABIC_SAUDI_ARABIA,
    wxLANGUAGE_ARABIC_SUDAN,
    wxLANGUAGE_ARABIC_SYRIA,
    wxLANGUAGE_ARABIC_TUNISIA,
    wxLANGUAGE_ARABIC_UAE,
    wxLANGUAGE_ARABIC_YEMEN,
    wxLANGUAGE_ARMENIAN,
    wxLANGUAGE_ASSAMESE,
    wxLANGUAGE_AYMARA,
    wxLANGUAGE_AZERI,
    wxLANGUAGE_AZERI_CYRILLIC,
    wxLANGUAGE_AZERI_LATIN,
    wxLANGUAGE_BASHKIR,
    wxLANGUAGE_BASQUE,
    wxLANGUAGE_BELARUSIAN,
    wxLANGUAGE_BENGALI,
    wxLANGUAGE_BHUTANI,
    wxLANGUAGE_BIHARI,
    wxLANGUAGE_BISLAMA,
    wxLANGUAGE_BRETON,
    wxLANGUAGE_BULGARIAN,
    wxLANGUAGE_BURMESE,
    wxLANGUAGE_CAMBODIAN,
    wxLANGUAGE_CATALAN,
    wxLANGUAGE_CHINESE,
    wxLANGUAGE_CHINESE_SIMPLIFIED,
    wxLANGUAGE_CHINESE_TRADITIONAL,
    wxLANGUAGE_CHINESE_HONGKONG,
    wxLANGUAGE_CHINESE_MACAU,
    wxLANGUAGE_CHINESE_SINGAPORE,
    wxLANGUAGE_CHINESE_TAIWAN,
    wxLANGUAGE_CORSICAN,
    wxLANGUAGE_CROATIAN,
    wxLANGUAGE_CZECH,
    wxLANGUAGE_DANISH,
    wxLANGUAGE_DUTCH,
    wxLANGUAGE_DUTCH_BELGIAN,
    wxLANGUAGE_ENGLISH,
    wxLANGUAGE_ENGLISH_UK,
    wxLANGUAGE_ENGLISH_US,
    wxLANGUAGE_ENGLISH_AUSTRALIA,
    wxLANGUAGE_ENGLISH_BELIZE,
    wxLANGUAGE_ENGLISH_BOTSWANA,
    wxLANGUAGE_ENGLISH_CANADA,
    wxLANGUAGE_ENGLISH_CARIBBEAN,
    wxLANGUAGE_ENGLISH_DENMARK,
    wxLANGUAGE_ENGLISH_EIRE,
    wxLANGUAGE_ENGLISH_JAMAICA,
    wxLANGUAGE_ENGLISH_NEW_ZEALAND,
    wxLANGUAGE_ENGLISH_PHILIPPINES,
    wxLANGUAGE_ENGLISH_SOUTH_AFRICA,
    wxLANGUAGE_ENGLISH_TRINIDAD,
    wxLANGUAGE_ENGLISH_ZIMBABWE,
    wxLANGUAGE_ESPERANTO,
    wxLANGUAGE_ESTONIAN,
    wxLANGUAGE_FAEROESE,
    wxLANGUAGE_FARSI,
    wxLANGUAGE_FIJI,
    wxLANGUAGE_FINNISH,
    wxLANGUAGE_FRENCH,
    wxLANGUAGE_FRENCH_BELGIAN,
    wxLANGUAGE_FRENCH_CANADIAN,
    wxLANGUAGE_FRENCH_LUXEMBOURG,
    wxLANGUAGE_FRENCH_MONACO,
    wxLANGUAGE_FRENCH_SWISS,
    wxLANGUAGE_FRISIAN,
    wxLANGUAGE_GALICIAN,
    wxLANGUAGE_GEORGIAN,
    wxLANGUAGE_GERMAN,
    wxLANGUAGE_GERMAN_AUSTRIAN,
    wxLANGUAGE_GERMAN_BELGIUM,
    wxLANGUAGE_GERMAN_LIECHTENSTEIN,
    wxLANGUAGE_GERMAN_LUXEMBOURG,
    wxLANGUAGE_GERMAN_SWISS,
    wxLANGUAGE_GREEK,
    wxLANGUAGE_GREENLANDIC,
    wxLANGUAGE_GUARANI,
    wxLANGUAGE_GUJARATI,
    wxLANGUAGE_HAUSA,
    wxLANGUAGE_HEBREW,
    wxLANGUAGE_HINDI,
    wxLANGUAGE_HUNGARIAN,
    wxLANGUAGE_ICELANDIC,
    wxLANGUAGE_INDONESIAN,
    wxLANGUAGE_INTERLINGUA,
    wxLANGUAGE_INTERLINGUE,
    wxLANGUAGE_INUKTITUT,
    wxLANGUAGE_INUPIAK,
    wxLANGUAGE_IRISH,
    wxLANGUAGE_ITALIAN,
    wxLANGUAGE_ITALIAN_SWISS,
    wxLANGUAGE_JAPANESE,
    wxLANGUAGE_JAVANESE,
    wxLANGUAGE_KANNADA,
    wxLANGUAGE_KASHMIRI,
    wxLANGUAGE_KASHMIRI_INDIA,
    wxLANGUAGE_KAZAKH,
    wxLANGUAGE_KERNEWEK,
    wxLANGUAGE_KINYARWANDA,
    wxLANGUAGE_KIRGHIZ,
    wxLANGUAGE_KIRUNDI,
    wxLANGUAGE_KONKANI,
    wxLANGUAGE_KOREAN,
    wxLANGUAGE_KURDISH,
    wxLANGUAGE_LAOTHIAN,
    wxLANGUAGE_LATIN,
    wxLANGUAGE_LATVIAN,
    wxLANGUAGE_LINGALA,
    wxLANGUAGE_LITHUANIAN,
    wxLANGUAGE_MACEDONIAN,
    wxLANGUAGE_MALAGASY,
    wxLANGUAGE_MALAY,
    wxLANGUAGE_MALAYALAM,
    wxLANGUAGE_MALAY_BRUNEI_DARUSSALAM,
    wxLANGUAGE_MALAY_MALAYSIA,
    wxLANGUAGE_MALTESE,
    wxLANGUAGE_MANIPURI,
    wxLANGUAGE_MAORI,
    wxLANGUAGE_MARATHI,
    wxLANGUAGE_MOLDAVIAN,
    wxLANGUAGE_MONGOLIAN,
    wxLANGUAGE_NAURU,
    wxLANGUAGE_NEPALI,
    wxLANGUAGE_NEPALI_INDIA,
    wxLANGUAGE_NORWEGIAN_BOKMAL,
    wxLANGUAGE_NORWEGIAN_NYNORSK,
    wxLANGUAGE_OCCITAN,
    wxLANGUAGE_ORIYA,
    wxLANGUAGE_OROMO,
    wxLANGUAGE_PASHTO,
    wxLANGUAGE_POLISH,
    wxLANGUAGE_PORTUGUESE,
    wxLANGUAGE_PORTUGUESE_BRAZILIAN,
    wxLANGUAGE_PUNJABI,
    wxLANGUAGE_QUECHUA,
    wxLANGUAGE_RHAETO_ROMANCE,
    wxLANGUAGE_ROMANIAN,
    wxLANGUAGE_RUSSIAN,
    wxLANGUAGE_RUSSIAN_UKRAINE,
    wxLANGUAGE_SAMOAN,
    wxLANGUAGE_SANGHO,
    wxLANGUAGE_SANSKRIT,
    wxLANGUAGE_SCOTS_GAELIC,
    wxLANGUAGE_SERBIAN,
    wxLANGUAGE_SERBIAN_CYRILLIC,
    wxLANGUAGE_SERBIAN_LATIN,
    wxLANGUAGE_SERBO_CROATIAN,
    wxLANGUAGE_SESOTHO,
    wxLANGUAGE_SETSWANA,
    wxLANGUAGE_SHONA,
    wxLANGUAGE_SINDHI,
    wxLANGUAGE_SINHALESE,
    wxLANGUAGE_SISWATI,
    wxLANGUAGE_SLOVAK,
    wxLANGUAGE_SLOVENIAN,
    wxLANGUAGE_SOMALI,
    wxLANGUAGE_SPANISH,
    wxLANGUAGE_SPANISH_ARGENTINA,
    wxLANGUAGE_SPANISH_BOLIVIA,
    wxLANGUAGE_SPANISH_CHILE,
    wxLANGUAGE_SPANISH_COLOMBIA,
    wxLANGUAGE_SPANISH_COSTA_RICA,
    wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC,
    wxLANGUAGE_SPANISH_ECUADOR,
    wxLANGUAGE_SPANISH_EL_SALVADOR,
    wxLANGUAGE_SPANISH_GUATEMALA,
    wxLANGUAGE_SPANISH_HONDURAS,
    wxLANGUAGE_SPANISH_MEXICAN,
    wxLANGUAGE_SPANISH_MODERN,
    wxLANGUAGE_SPANISH_NICARAGUA,
    wxLANGUAGE_SPANISH_PANAMA,
    wxLANGUAGE_SPANISH_PARAGUAY,
    wxLANGUAGE_SPANISH_PERU,
    wxLANGUAGE_SPANISH_PUERTO_RICO,
    wxLANGUAGE_SPANISH_URUGUAY,
    wxLANGUAGE_SPANISH_US,
    wxLANGUAGE_SPANISH_VENEZUELA,
    wxLANGUAGE_SUNDANESE,
    wxLANGUAGE_SWAHILI,
    wxLANGUAGE_SWEDISH,
    wxLANGUAGE_SWEDISH_FINLAND,
    wxLANGUAGE_TAGALOG,
    wxLANGUAGE_TAJIK,
    wxLANGUAGE_TAMIL,
    wxLANGUAGE_TATAR,
    wxLANGUAGE_TELUGU,
    wxLANGUAGE_THAI,
    wxLANGUAGE_TIBETAN,
    wxLANGUAGE_TIGRINYA,
    wxLANGUAGE_TONGA,
    wxLANGUAGE_TSONGA,
    wxLANGUAGE_TURKISH,
    wxLANGUAGE_TURKMEN,
    wxLANGUAGE_TWI,
    wxLANGUAGE_UIGHUR,
    wxLANGUAGE_UKRAINIAN,
    wxLANGUAGE_URDU,
    wxLANGUAGE_URDU_INDIA,
    wxLANGUAGE_URDU_PAKISTAN,
    wxLANGUAGE_UZBEK,
    wxLANGUAGE_UZBEK_CYRILLIC,
    wxLANGUAGE_UZBEK_LATIN,
    wxLANGUAGE_VIETNAMESE,
    wxLANGUAGE_VOLAPUK,
    wxLANGUAGE_WELSH,
    wxLANGUAGE_WOLOF,
    wxLANGUAGE_XHOSA,
    wxLANGUAGE_YIDDISH,
    wxLANGUAGE_YORUBA,
    wxLANGUAGE_ZHUANG,
    wxLANGUAGE_ZULU,

    // for custom, user-defined languages:
    wxLANGUAGE_USER_DEFINED
};

// wxLanguageInfo: encapsulates wxLanguage to OS native lang.desc.
//                 translation information
class wxLanguageInfo
{
public:
    int Language;                   // wxLanguage id
    wxString CanonicalName;         // Canonical name, e.g. fr_FR
    wxString Description;           // human-readable name of the language
};

// wxLocaleCategory: the category of locale settings
enum wxLocaleCategory
{
    wxLOCALE_CAT_NUMBER,
    wxLOCALE_CAT_DATE,
    wxLOCALE_CAT_MONEY,
    wxLOCALE_CAT_MAX
};

// wxLocaleInfo: the items understood by wxLocale::GetInfo()
enum wxLocaleInfo
{
    wxLOCALE_THOUSANDS_SEP,
    wxLOCALE_DECIMAL_POINT

};

// wxLocale: encapsulates all language dependent settings, including current
//           message catalogs, date, time and currency formats (TODO) &c
enum wxLocaleInitFlags
{
    wxLOCALE_LOAD_DEFAULT  = 0x0001,     // load wxwin.mo?
    wxLOCALE_CONV_ENCODING = 0x0002      // convert encoding on the fly?
};

class wxLocale
{
public:
    // ctor & dtor
    // -----------
    wxLocale(int language = wxLANGUAGE_DEFAULT,
             int flags = wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);

        // restores old locale
    ~wxLocale();

    %name(Init1)bool Init(const wxString& szName,
                          const wxString& szShort = wxPyEmptyString,
                          const wxString& szLocale = wxPyEmptyString,
                          bool bLoadDefault = TRUE,
                          bool bConvertEncoding = FALSE);

    %name(Init2) bool Init(int language = wxLANGUAGE_DEFAULT,
                           int flags = wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);

    %pragma(python) addtoclass = "
    def Init(self, *_args, **_kwargs):
        if type(_args[0]) in [type(''), type(u'')]:
            val = self.Init1(*_args, **_kwargs)
        else:
            val = self.Init2(*_args, **_kwargs)
        return val
    "


    // Try to get user's (or OS's) prefered language setting.
    // Return wxLANGUAGE_UNKNOWN if language-guessing algorithm failed
    static int GetSystemLanguage();

    // get the encoding used by default for text on this system, returns
    // wxFONTENCODING_SYSTEM if it couldn't be determined
    static wxFontEncoding GetSystemEncoding();

    // get the string describing the system encoding, return empty string if
    // couldn't be determined
    static wxString GetSystemEncodingName();


    // THIS ONE IS COMMENTED OUT IN src/common/intl.cpp
    // get the values of the given locale-dependent datum: the current locale
    // is used, the US default value is returned if everything else fails
    // static wxString GetInfo(wxLocaleInfo index, wxLocaleCategory cat);

    // return TRUE if the locale was set successfully
    bool IsOk() const;

    // returns locale name
    wxString GetLocale() const;

    // return current locale wxLanguage value
    int GetLanguage() const;

    // return locale name to be passed to setlocale()
    wxString GetSysName() const;

    // return 'canonical' name, i.e. in the form of xx[_YY], where xx is
    // language code according to ISO 639 and YY is country name
    // as specified by ISO 3166.
    wxString GetCanonicalName() const;

    // add a prefix to the catalog lookup path: the message catalog files will be
    // looked up under prefix/<lang>/LC_MESSAGES, prefix/LC_MESSAGES and prefix
    // (in this order).
    //
    // This only applies to subsequent invocations of AddCatalog()!
    static void AddCatalogLookupPathPrefix(const wxString& prefix);

    // add a catalog: it's searched for in standard places (current directory
    // first, system one after), but the you may prepend additional directories to
    // the search path with AddCatalogLookupPathPrefix().
    //
    // The loaded catalog will be used for message lookup by GetString().
    //
    // Returns 'true' if it was successfully loaded
    bool AddCatalog(const wxString& szDomain);

    // check if the given catalog is loaded
    bool IsLoaded(const wxString& szDomain) const;

    // Retrieve the language info struct for the given language
    //
    // Returns NULL if no info found, pointer must *not* be deleted by caller
    static const wxLanguageInfo *GetLanguageInfo(int lang);

    // Find the language for the given locale string which may be either a
    // canonical ISO 2 letter language code ("xx"), a language code followed by
    // the country code ("xx_XX") or a Windows full language name ("Xxxxx...")
    //
    // Returns NULL if no info found, pointer must *not* be deleted by caller
    static const wxLanguageInfo *FindLanguageInfo(const wxString& locale);

    // Add custom language to the list of known languages.
    // Notes: 1) wxLanguageInfo contains platform-specific data
    //        2) must be called before Init to have effect
    static void AddLanguage(const wxLanguageInfo& info);

    // retrieve the translation for a string in all loaded domains unless
    // the szDomain parameter is specified (and then only this domain is
    // searched)
    //
    // return original string if translation is not available
    // (in this case an error message is generated the first time
    //  a string is not found; use wxLogNull to suppress it)
    //
    // domains are searched in the last to first order, i.e. catalogs
    // added later override those added before.
    wxString GetString(const wxString& szOrigString,
                       const wxString& szDomain = wxPyEmptyString) const;

    // Returns the current short name for the locale
    const wxString& GetName() const;

};



// get the current locale object (note that it may be NULL!)
wxLocale* wxGetLocale();

// get the translation of the string in the current locale
wxString wxGetTranslation(const wxString& sz);


//----------------------------------------------------------------------
// wxEncodingConverter
//                  This class is capable of converting strings between any two
//                  8bit encodings/charsets. It can also convert from/to Unicode


%typemap(python, out) wxFontEncodingArray {
    $target = PyList_New(0);
    for (size_t i=0; i < $source->GetCount(); i++) {
        PyObject* number = PyInt_FromLong($source->Item(i));
        PyList_Append($target, number);
        Py_DECREF(number);
    }
}


enum
{
    wxCONVERT_STRICT,
    wxCONVERT_SUBSTITUTE
};


enum
{
    wxPLATFORM_CURRENT = -1,

    wxPLATFORM_UNIX = 0,
    wxPLATFORM_WINDOWS,
    wxPLATFORM_OS2,
    wxPLATFORM_MAC
};


class wxEncodingConverter : public wxObject
{
public:

    wxEncodingConverter();
    ~wxEncodingConverter();


    // Initialize convertion. Both output or input encoding may
    // be wxFONTENCODING_UNICODE, but only if wxUSE_WCHAR_T is set to 1.
    //
    // All subsequent calls to Convert() will interpret it's argument
    // as a string in input_enc encoding and will output string in
    // output_enc encoding.
    //
    // You must call this method before calling Convert. You may call
    // it more than once in order to switch to another conversion
    //
    // Method affects behaviour of Convert() in case input character
    // cannot be converted because it does not exist in output encoding:
    //     wxCONVERT_STRICT --
    //              follow behaviour of GNU Recode - just copy unconvertable
    //              characters to output and don't change them (it's integer
    //              value will stay the same)
    //     wxCONVERT_SUBSTITUTE --
    //              try some (lossy) substitutions - e.g. replace
    //              unconvertable latin capitals with acute by ordinary
    //              capitals, replace en-dash or em-dash by '-' etc.
    //     both modes gurantee that output string will have same length
    //     as input string
    //
    // Returns FALSE if given conversion is impossible, TRUE otherwise
    // (conversion may be impossible either if you try to convert
    // to Unicode with non-Unicode build of wxWindows or if input
    // or output encoding is not supported.)
    bool Init(wxFontEncoding input_enc, wxFontEncoding output_enc, int method = wxCONVERT_STRICT);


    // TODO:  Need to do something about unicode mode...

    // Convert input string according to settings passed to Init.
    // Note that you must call Init before using Convert!
    wxString Convert(const wxString& input);

    // void Convert(const char* input, char* output);

//  #if wxUSE_WCHAR_T
//              void Convert(const char* input, wchar_t* output);
//              void Convert(const wchar_t* input, char* output);
//              void Convert(const wchar_t* input, wchar_t* output);
//              void Convert(wchar_t* str) { Convert(str, str); }
//  #endif



    // Return equivalent(s) for given font that are used
    // under given platform. wxPLATFORM_CURRENT means the plaform
    // this binary was compiled for
    //
    // Examples:
    //     current platform          enc    returned value
    // -----------------------------------------------------
    //     unix                   CP1250         {ISO8859_2}
    //     unix                ISO8859_2                  {}
    //     windows             ISO8859_2            {CP1250}
    //
    // Equivalence is defined in terms of convertibility:
    // 2 encodings are equivalent if you can convert text between
    // then without loosing information (it may - and will - happen
    // that you loose special chars like quotation marks or em-dashes
    // but you shouldn't loose any diacritics and language-specific
    // characters when converting between equivalent encodings).
    //
    // Convert() method is not limited to converting between
    // equivalent encodings, it can convert between arbitrary
    // two encodings!
    //
    // Remember that this function does _NOT_ check for presence of
    // fonts in system. It only tells you what are most suitable
    // encodings. (It usually returns only one encoding)
    //
    // Note that argument enc itself may be present in returned array!
    // (so that you can -- as a side effect -- detect whether the
    // encoding is native for this platform or not)
    static wxFontEncodingArray GetPlatformEquivalents(wxFontEncoding enc,
                                                      int platform = wxPLATFORM_CURRENT);

    // Similar to GetPlatformEquivalent, but this one will return ALL
    // equivalent encodings, regardless the platform, including itself.
    static wxFontEncodingArray GetAllEquivalents(wxFontEncoding enc);

    %pragma(python) addtoclass = "def __nonzero__(self): return self.IsOk()"
};


//----------------------------------------------------------------------
//----------------------------------------------------------------------

%init %{
    wxPyPtrTypeMap_Add("wxFontEnumerator", "wxPyFontEnumerator");
%}

//----------------------------------------------------------------------

