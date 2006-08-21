/////////////////////////////////////////////////////////////////////////////
// Name:        _intl.i
// Purpose:     SWIG interface file for wxLocale and related classes
//
// Author:      Robin Dunn
//
// Created:     1-Apr-2002
// RCS-ID:      $Id$
// Copyright:   (c) 2002 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <locale.h>
%}

//---------------------------------------------------------------------------
%newgroup


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


//---------------------------------------------------------------------------


// wxLanguageInfo: encapsulates wxLanguage to OS native lang.desc.
//                 translation information
struct wxLanguageInfo
{
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


//---------------------------------------------------------------------------

class wxLocale
{
public:
    // ctor & dtor
    // -----------
    %extend {
        wxLocale(int language = -1,
                 int flags = wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING) {
            wxLocale* loc;
            if (language == -1)
                loc = new wxLocale();
            else
                loc = new wxLocale(language, flags);
            // Python before 2.4 needs to have LC_NUMERIC set to "C" in order
            // for the floating point conversions and such to work right.
%#if PY_VERSION_HEX < 0x02040000
            setlocale(LC_NUMERIC, "C");
%#endif
            return loc;
        }
    }

        // restores old locale
    ~wxLocale();

    %extend { 
        bool Init1(const wxString& szName,
                   const wxString& szShort = wxPyEmptyString,
                   const wxString& szLocale = wxPyEmptyString,
                   bool bLoadDefault = true,
                   bool bConvertEncoding = false) {
            bool rc = self->Init(szName, szShort, szLocale, bLoadDefault, bConvertEncoding);
            // Python before 2.4 needs to have LC_NUMERIC set to "C" in order
            // for the floating point conversions and such to work right.
%#if PY_VERSION_HEX < 0x02040000
            setlocale(LC_NUMERIC, "C");
%#endif
            return rc;
        }

        bool Init2(int language = wxLANGUAGE_DEFAULT,
                   int flags = wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING) {
            bool rc = self->Init(language, flags);
            // Python before 2.4 needs to have LC_NUMERIC set to "C" in order
            // for the floating point conversions and such to work right.
%#if PY_VERSION_HEX < 0x02040000
            setlocale(LC_NUMERIC, "C");
%#endif
            return rc;
        }
    }

    

    %pythoncode {
    def Init(self, *_args, **_kwargs):
        if type(_args[0]) in [type(''), type(u'')]:
            val = self.Init1(*_args, **_kwargs)
        else:
            val = self.Init2(*_args, **_kwargs)
        return val
    }

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

    // return True if the locale was set successfully
    bool IsOk() const;
    %pythoncode { def __nonzero__(self): return self.IsOk() };

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
    // Returns 'True' if it was successfully loaded
    bool AddCatalog(const wxString& szDomain);

    // check if the given catalog is loaded
    bool IsLoaded(const wxString& szDomain) const;

    // Retrieve the language info struct for the given language
    //
    // Returns NULL if no info found, pointer must *not* be deleted by caller
    static const wxLanguageInfo *GetLanguageInfo(int lang);

    // Returns language name in English or empty string if the language
    // is not in database
    static wxString GetLanguageName(int lang);

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

//---------------------------------------------------------------------------

%{
class wxPyLocale : public wxLocale
{
public:
    wxPyLocale();

    wxPyLocale(const wxChar *szName,                             // name (for messages)
             const wxChar *szShort = (const wxChar *) NULL,      // dir prefix (for msg files)
             const wxChar *szLocale = (const wxChar *) NULL,     // locale (for setlocale)
             bool bLoadDefault = true,                           // preload wxstd.mo?
             bool bConvertEncoding = false);                     // convert Win<->Unix if necessary?

    wxPyLocale(int language, // wxLanguage id or custom language
             int flags = wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);
             
    ~wxPyLocale();

    virtual const wxChar *GetString(const wxChar *szOrigString,
                                    const wxChar *szDomain = NULL) const;
    virtual const wxChar *GetString(const wxChar *szOrigString,
                                    const wxChar *szOrigString2, size_t n,
                                    const wxChar *szDomain = NULL) const;
    
    virtual wxChar *GetSingularString(const wxChar *szOrigString,
                                      const wxChar *szDomain = NULL) const;
    virtual wxChar *GetPluralString(const wxChar *szOrigString,
                                    const wxChar *szOrigString2, size_t n,
                                    const wxChar *szDomain = NULL) const;

    PYPRIVATE;
private:
    DECLARE_NO_COPY_CLASS(wxPyLocale)
};

wxPyLocale::wxPyLocale() : wxLocale()
{
}

wxPyLocale::wxPyLocale(const wxChar *szName,  // name (for messages)
             const wxChar *szShort,           // dir prefix (for msg files)
             const wxChar *szLocale,          // locale (for setlocale)
             bool bLoadDefault,               // preload wxstd.mo?
             bool bConvertEncoding)           // convert Win<->Unix if necessary?
             : wxLocale(szName, szShort, szLocale, bLoadDefault, bConvertEncoding)
{
}

wxPyLocale::wxPyLocale(int language, // wxLanguage id or custom language
                       int flags) : wxLocale(language, flags)
{
}
             
wxPyLocale::~wxPyLocale()
{
}

const wxChar *wxPyLocale::GetString(const wxChar *szOrigString,
                                    const wxChar *szDomain) const 
{
    wxChar *str = GetSingularString(szOrigString, szDomain);
    return (str != NULL) ? str : wxLocale::GetString(szOrigString, szDomain);
}

const wxChar *wxPyLocale::GetString(const wxChar *szOrigString,
                                    const wxChar *szOrigString2, size_t n,
                                    const wxChar *szDomain) const
{
    wxChar *str = GetPluralString(szOrigString, szOrigString2, n, szDomain);
    return (str != NULL) ? str : wxLocale::GetString(szOrigString, szOrigString2, n, szDomain);
}

wxChar *wxPyLocale::GetSingularString(const wxChar *szOrigString,
                                      const wxChar *szDomain) const
{
    bool found;
    static wxString str;
    str = _T("error in translation"); // when the first if condition is true but the second if condition is not we do not want to return the previously queried string.
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
    if((found=wxPyCBH_findCallback(m_myInst, "GetSingularString"))) {
        PyObject* param1 = wx2PyString(szOrigString);
        PyObject* param2 = wx2PyString(szDomain);
        PyObject* ret = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OO)", param1, param2));
        Py_DECREF(param1);
        Py_DECREF(param2);
        if (ret) {
            str = Py2wxString(ret);
            Py_DECREF(ret);
        }
    }
    wxPyEndBlockThreads(blocked);
    return (found ? (wxChar*)str.c_str() : NULL);
}

wxChar *wxPyLocale::GetPluralString(const wxChar *szOrigString,
                                    const wxChar *szOrigString2, size_t n,
                                    const wxChar *szDomain) const
{
    bool found;
    static wxString str;
    str = _T("error in translation"); // when the first if condition is true but the second if condition is not we do not want to return the previously queried string.
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
    if((found=wxPyCBH_findCallback(m_myInst, "GetPluralString"))) {
        PyObject* param1 = wx2PyString(szOrigString);
        PyObject* param2 = wx2PyString(szOrigString2);
        PyObject* param4 = wx2PyString(szDomain);
        PyObject* ret = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OOiO)", param1, param2, (int)n, param4));
        Py_DECREF(param1);
        Py_DECREF(param2);
        Py_DECREF(param4);
        if( ret) {
            str = Py2wxString(ret);
            Py_DECREF(ret);
        }
    }
    wxPyEndBlockThreads(blocked);
    return (found ? (wxChar*)str.c_str() : NULL);
}
%}


class wxPyLocale : public wxLocale
{
public:
    %pythonAppend wxPyLocale "self._setCallbackInfo(self, PyLocale)"

    // ctor & dtor
    // -----------
    %extend {
        wxPyLocale(int language = -1,
                   int flags = wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING) {
            wxPyLocale* loc;
            if (language == -1)
                loc = new wxPyLocale();
            else
                loc = new wxPyLocale(language, flags);
            // Python before 2.4 needs to have LC_NUMERIC set to "C" in order
            // for the floating point conversions and such to work right.
%#if PY_VERSION_HEX < 0x02040000
            setlocale(LC_NUMERIC, "C");
%#endif
            return loc;
        }
    }
    ~wxPyLocale();
    
    void _setCallbackInfo(PyObject* self, PyObject* _class);
    
    virtual const wxChar *GetSingularString(const wxChar *szOrigString,
                                            const wxChar *szDomain = NULL) const;
    virtual const wxChar *GetPluralString(const wxChar *szOrigString,
                                          const wxChar *szOrigString2, size_t n,
                                          const wxChar *szDomain = NULL) const;
};

//---------------------------------------------------------------------------

// get the current locale object (note that it may be NULL!)
wxLocale* wxGetLocale();

// get the translation of the string in the current locale
%nokwargs wxGetTranslation;
wxString wxGetTranslation(const wxString& str);
wxString wxGetTranslation(const wxString& str, const wxString& domain);
wxString wxGetTranslation(const wxString& str, const wxString& strPlural, size_t n);
wxString wxGetTranslation(const wxString& str, const wxString& strPlural, size_t n, const wxString& domain);

//---------------------------------------------------------------------------
%newgroup


// wxEncodingConverter
//                  This class is capable of converting strings between any two
//                  8bit encodings/charsets. It can also convert from/to Unicode


%typemap(out) wxFontEncodingArray {
    $result = PyList_New(0);
    for (size_t i=0; i < $1.GetCount(); i++) {
        PyObject* number = PyInt_FromLong($1.Item(i));
        PyList_Append($result, number);
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
    // Returns False if given conversion is impossible, True otherwise
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

    // Return True if [any text in] one multibyte encoding can be
    // converted to another one losslessly.
    //
    // Do not call this with wxFONTENCODING_UNICODE, it doesn't make
    // sense (always works in one sense and always depends on the text
    // to convert in the other)
    static bool CanConvert(wxFontEncoding encIn, wxFontEncoding encOut);

    %pythoncode { def __nonzero__(self): return self.IsOk() }
};




//---------------------------------------------------------------------------

%pythoncode "_intl_ex.py"

//---------------------------------------------------------------------------
