/////////////////////////////////////////////////////////////////////////////
// Name:        _font.i
// Purpose:     SWIG interface file for wxFont and related classes
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
#include <wx/fontutil.h>
#include <wx/fontmap.h>
#include <wx/fontenum.h>
%}

//---------------------------------------------------------------------------
%newgroup


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
    wxFONTFAMILY_UNKNOWN = wxFONTFAMILY_MAX
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


// the font flag bits for the new font ctor accepting one combined flags word
enum
{
    // no special flags: font with default weight/slant/anti-aliasing
    wxFONTFLAG_DEFAULT          = 0,

    // slant flags (default: no slant)
    wxFONTFLAG_ITALIC           = 1 << 0,
    wxFONTFLAG_SLANT            = 1 << 1,

    // weight flags (default: medium)
    wxFONTFLAG_LIGHT            = 1 << 2,
    wxFONTFLAG_BOLD             = 1 << 3,

    // anti-aliasing flag: force on or off (default: the current system default)
    wxFONTFLAG_ANTIALIASED      = 1 << 4,
    wxFONTFLAG_NOT_ANTIALIASED  = 1 << 5,

    // underlined/strikethrough flags (default: no lines)
    wxFONTFLAG_UNDERLINED       = 1 << 6,
    wxFONTFLAG_STRIKETHROUGH    = 1 << 7,

    // the mask of all currently used flags
    wxFONTFLAG_MASK = wxFONTFLAG_ITALIC             |
                      wxFONTFLAG_SLANT              |
                      wxFONTFLAG_LIGHT              |
                      wxFONTFLAG_BOLD               |
                      wxFONTFLAG_ANTIALIASED        |
                      wxFONTFLAG_NOT_ANTIALIASED    |
                      wxFONTFLAG_UNDERLINED         |
                      wxFONTFLAG_STRIKETHROUGH
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
    wxFONTENCODING_KOI8,            // KOI8 Russian
    wxFONTENCODING_KOI8_U,          // KOI8 Ukrainian
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
    wxFONTENCODING_EUC_JP,          // Extended Unix Codepage for Japanese
    wxFONTENCODING_UTF16BE,         // UTF-16 Big Endian Unicode encoding
    wxFONTENCODING_UTF16LE,         // UTF-16 Little Endian Unicode encoding
    wxFONTENCODING_UTF32BE,         // UTF-32 Big Endian Unicode encoding
    wxFONTENCODING_UTF32LE,         // UTF-32 Little Endian Unicode encoding

    wxFONTENCODING_MACROMAN,        // the standard mac encodings
    wxFONTENCODING_MACJAPANESE,
    wxFONTENCODING_MACCHINESETRAD,
    wxFONTENCODING_MACKOREAN,
    wxFONTENCODING_MACARABIC,
    wxFONTENCODING_MACHEBREW,
    wxFONTENCODING_MACGREEK,
    wxFONTENCODING_MACCYRILLIC,
    wxFONTENCODING_MACDEVANAGARI,
    wxFONTENCODING_MACGURMUKHI,
    wxFONTENCODING_MACGUJARATI,
    wxFONTENCODING_MACORIYA,
    wxFONTENCODING_MACBENGALI,
    wxFONTENCODING_MACTAMIL,
    wxFONTENCODING_MACTELUGU,
    wxFONTENCODING_MACKANNADA,
    wxFONTENCODING_MACMALAJALAM,
    wxFONTENCODING_MACSINHALESE,
    wxFONTENCODING_MACBURMESE,
    wxFONTENCODING_MACKHMER,
    wxFONTENCODING_MACTHAI,
    wxFONTENCODING_MACLAOTIAN,
    wxFONTENCODING_MACGEORGIAN,
    wxFONTENCODING_MACARMENIAN,
    wxFONTENCODING_MACCHINESESIMP,
    wxFONTENCODING_MACTIBETAN,
    wxFONTENCODING_MACMONGOLIAN,
    wxFONTENCODING_MACETHIOPIC,
    wxFONTENCODING_MACCENTRALEUR,
    wxFONTENCODING_MACVIATNAMESE,
    wxFONTENCODING_MACARABICEXT,
    wxFONTENCODING_MACSYMBOL,
    wxFONTENCODING_MACDINGBATS,
    wxFONTENCODING_MACTURKISH,
    wxFONTENCODING_MACCROATIAN,
    wxFONTENCODING_MACICELANDIC,
    wxFONTENCODING_MACROMANIAN,
    wxFONTENCODING_MACCELTIC,
    wxFONTENCODING_MACGAELIC,
    wxFONTENCODING_MACKEYBOARD,

    wxFONTENCODING_MACMIN = wxFONTENCODING_MACROMAN ,
    wxFONTENCODING_MACMAX = wxFONTENCODING_MACKEYBOARD ,

    wxFONTENCODING_MAX,             // highest enumerated encoding value


    // aliases for endian-dependent UTF encodings
    wxFONTENCODING_UTF16,  // native UTF-16
    wxFONTENCODING_UTF32,  // native UTF-32

    // alias for the native Unicode encoding on this platform
    // (this is used by wxEncodingConverter and wxUTFFile only for now)
    wxFONTENCODING_UNICODE = wxFONTENCODING_UTF16,

    // alternative names for Far Eastern encodings
        // Chinese
    wxFONTENCODING_GB2312 = wxFONTENCODING_CP936, // Simplified Chinese
    wxFONTENCODING_BIG5 = wxFONTENCODING_CP950,   // Traditional Chinese

        // Japanese (see http://zsigri.tripod.com/fontboard/cjk/jis.html)
    wxFONTENCODING_SHIFT_JIS = wxFONTENCODING_CP932 // Shift JIS
};

//---------------------------------------------------------------------------
%newgroup

// wxNativeFontInfo is platform-specific font representation: this struct
// should be considered as opaque font description only used by the native
// functions, the user code can only get the objects of this type from
// somewhere and pass it somewhere else (possibly save them somewhere using
// ToString() and restore them using FromString())
struct wxNativeFontInfo
{
public:
    wxNativeFontInfo();
    ~wxNativeFontInfo();

    // reset to the default state
    void Init();

    // init with the parameters of the given font
    void InitFromFont(const wxFont& font);

    // accessors and modifiers for the font elements
    int GetPointSize() const;
#ifdef __WXMSW__
    wxSize GetPixelSize() const;
#endif
    wxFontStyle GetStyle() const;
    wxFontWeight GetWeight() const;
    bool GetUnderlined() const;
    wxString GetFaceName() const;
    wxFontFamily GetFamily() const;
    wxFontEncoding GetEncoding() const;

    void SetPointSize(int pointsize);
#ifdef __WXMSW__
    void SetPixelSize(const wxSize& pixelSize);
#endif
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

    %extend {
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




struct wxNativeEncodingInfo
{
    wxString facename;          // may be empty meaning "any"
    wxFontEncoding encoding;    // so that we know what this struct represents

    wxNativeEncodingInfo();
    ~wxNativeEncodingInfo();

    // this struct is saved in config by wxFontMapper, so it should know to
    // serialise itself (implemented in platform-specific code)
    bool FromString(const wxString& s);
    wxString ToString() const;
};


#ifndef __WXMSW__
// translate a wxFontEncoding into native encoding parameter (defined above),
// returning a wxNativeEncodingInfo if an (exact) match could be found, NULL
// otherwise.
%inline %{
    wxNativeEncodingInfo* wxGetNativeFontEncoding(wxFontEncoding encoding) {
        static wxNativeEncodingInfo info;
        if ( wxGetNativeFontEncoding(encoding, &info) )
            return &info;
        else
            return NULL;
    }
%}

// test for the existence of the font described by this facename/encoding,
// return True if such font(s) exist, False otherwise
bool wxTestFontEncoding(const wxNativeEncodingInfo& info);

#else

%inline %{
    wxNativeEncodingInfo* wxGetNativeFontEncoding(wxFontEncoding encoding)
        { wxPyRaiseNotImplemented(); return NULL; }

    bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
        { wxPyRaiseNotImplemented(); return false; }
%}
#endif

//---------------------------------------------------------------------------
%newgroup

// wxFontMapper manages user-definable correspondence between logical font
// names and the fonts present on the machine.
//
// The default implementations of all functions will ask the user if they are
// not capable of finding the answer themselves and store the answer in a
// config file (configurable via SetConfigXXX functions). This behaviour may
// be disabled by giving the value of False to "interactive" parameter.
// However, the functions will always consult the config file to allow the
// user-defined values override the default logic and there is no way to
// disable this - which shouldn't be ever needed because if "interactive" was
// never True, the config file is never created anyhow.
//
// This is a singleton class, font mapper objects can only be accessed using
// wxFontMapper::Get().

class  wxFontMapper
{
public:
    wxFontMapper();
    ~wxFontMapper();

    // return instance of the wxFontMapper singleton
    static wxFontMapper *Get();
    // set the sigleton to 'mapper' instance and return previous one
    static wxFontMapper *Set(wxFontMapper *mapper);


    // returns the encoding for the given charset (in the form of RFC 2046) or
    // wxFONTENCODING_SYSTEM if couldn't decode it
    //
    // interactive parameter is ignored in the base class, we behave as if it
    // were always False
    virtual wxFontEncoding CharsetToEncoding(const wxString& charset,
                                             bool interactive = true);


    // get the number of font encodings we know about
    static size_t GetSupportedEncodingsCount();

    // get the n-th supported encoding
    static wxFontEncoding GetEncoding(size_t n);

    // return internal string identifier for the encoding (see also
    // GetEncodingDescription())
    static wxString GetEncodingName(wxFontEncoding encoding);

    // return user-readable string describing the given encoding
    //
    // NB: hard-coded now, but might change later (read it from config?)
    static wxString GetEncodingDescription(wxFontEncoding encoding);

    // find the encoding corresponding to the given name, inverse of
    // GetEncodingName() and less general than CharsetToEncoding()
    //
    // returns wxFONTENCODING_MAX if the name is not a supported encoding
    static wxFontEncoding GetEncodingFromName(const wxString& name);


    // set the config object to use (may be NULL to use default)
    void SetConfig(wxConfigBase *config);

    // set the root config path to use (should be an absolute path)
    void SetConfigPath(const wxString& prefix);

    // return default config path
    static const wxString GetDefaultConfigPath();



    // Find an alternative for the given encoding (which is supposed to not be
    // available on this system). If successful, returns the encoding otherwise
    // returns None.
    %extend {
        PyObject* GetAltForEncoding(wxFontEncoding encoding,
                                    const wxString& facename = wxPyEmptyString,
                                    bool interactive = true) {
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
    // If no facename is given (default), return true if it's available in any
    // facename at all.
    bool IsEncodingAvailable(wxFontEncoding encoding,
                             const wxString& facename = wxPyEmptyString);

    // the parent window for modal dialogs
    void SetDialogParent(wxWindow *parent);

    // the title for the dialogs (note that default is quite reasonable)
    void SetDialogTitle(const wxString& title);

};



//---------------------------------------------------------------------------
%newgroup


MustHaveApp(wxFont);
MustHaveApp(wxFont::GetDefaultEncoding);
MustHaveApp(wxFont::SetDefaultEncoding);

class wxFont : public wxGDIObject {
public:
    %pythonPrepend wxFont   "if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']"

    wxFont( int pointSize, int family, int style, int weight,
            bool underline=false, const wxString& face = wxPyEmptyString,
            wxFontEncoding encoding=wxFONTENCODING_DEFAULT);
    ~wxFont();

    %RenameCtor(FontFromNativeInfo,  wxFont(const wxNativeFontInfo& info));
    %extend {
        %RenameCtor(FontFromNativeInfoString, wxFont(const wxString& info))
        {
            wxNativeFontInfo nfi;
            nfi.FromString(info);
            return new wxFont(nfi);
        }

        %RenameCtor(Font2,  wxFont(int pointSize,
                                   wxFontFamily family,
                                   int flags = wxFONTFLAG_DEFAULT,
                                   const wxString& face = wxPyEmptyString,
                                   wxFontEncoding encoding = wxFONTENCODING_DEFAULT))
        {
            return wxFont::New(pointSize, family, flags, face, encoding);
        }
    }

    // There is a real ctor for this on wxMSW, but not the others, so just use
    // the factory funciton in all cases.
    %extend {
        %RenameCtor(FontFromPixelSize, wxFont(const wxSize& pixelSize,
                                              int family,
                                              int style,
                                              int weight,
                                              bool underlined = false,
                                              const wxString& face = wxEmptyString,
                                              wxFontEncoding encoding = wxFONTENCODING_DEFAULT))
        {
            return wxFontBase::New(pixelSize, family,
                                   style, weight, underlined,
                                   face, encoding);
        }
    }

    

    // was the font successfully created?
    bool Ok() const;
    %pythoncode { def __nonzero__(self): return self.Ok() }

    // comparison
    %extend {
        bool __eq__(const wxFont* other) { return other ? (*self == *other) : false; }
        bool __ne__(const wxFont* other) { return other ? (*self != *other) : true;  }
    }

    // accessors: get the font characteristics
    virtual int GetPointSize() const;
    virtual wxSize GetPixelSize() const;
    virtual bool IsUsingSizeInPixels() const;
    virtual int GetFamily() const;
    virtual int GetStyle() const;
    virtual int GetWeight() const;
    virtual bool GetUnderlined() const;
    virtual wxString GetFaceName() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;

    virtual bool IsFixedWidth() const;

    wxString GetNativeFontInfoDesc() const;
    wxString GetNativeFontInfoUserDesc() const;

    // change the font characteristics
    virtual void SetPointSize( int pointSize );
    virtual void SetPixelSize( const wxSize& pixelSize );
    virtual void SetFamily( int family );
    virtual void SetStyle( int style );
    virtual void SetWeight( int weight );
    virtual void SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);
    void SetNativeFontInfo(const wxNativeFontInfo& info);
    %Rename(SetNativeFontInfoFromString, void, SetNativeFontInfo(const wxString& info));
    void SetNativeFontInfoUserDesc(const wxString& info);

    // translate the fonts into human-readable string (i.e. GetStyleString()
    // will return "wxITALIC" for an italic font, ...)
    wxString GetFamilyString() const;
    wxString GetStyleString() const;
    wxString GetWeightString() const;

    // Unofficial API, don't use
    virtual void SetNoAntiAliasing( bool no = true );
    virtual bool GetNoAntiAliasing() const;

    // the default encoding is used for creating all fonts with default
    // encoding parameter
    static wxFontEncoding GetDefaultEncoding() { return ms_encodingDefault; }
    static void SetDefaultEncoding(wxFontEncoding encoding);
};



//---------------------------------------------------------------------------
%newgroup

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

MustHaveApp(wxPyFontEnumerator);

%rename(FontEnumerator) wxPyFontEnumerator;
class wxPyFontEnumerator {
public:
    %pythonAppend wxPyFontEnumerator "self._setCallbackInfo(self, FontEnumerator, 0)"

    wxPyFontEnumerator();
    ~wxPyFontEnumerator();
    void _setCallbackInfo(PyObject* self, PyObject* _class, bool incref);

    bool EnumerateFacenames(
        wxFontEncoding encoding = wxFONTENCODING_SYSTEM, // all
        bool fixedWidthOnly = false);

    bool EnumerateEncodings(const wxString& facename = wxPyEmptyString);

    //wxArrayString* GetEncodings();
    //wxArrayString* GetFacenames();
    %extend {
        PyObject* GetEncodings() {
            wxArrayString* arr = self->GetEncodings();
            if (arr)
                return wxArrayString2PyList_helper(*arr);
            else
                return PyList_New(0);
        }

        PyObject* GetFacenames() {
            wxArrayString* arr = self->GetFacenames();
            if (arr)
                return wxArrayString2PyList_helper(*arr);
            else
                return PyList_New(0);
        }
    }
};



%init %{
    wxPyPtrTypeMap_Add("wxFontEnumerator", "wxPyFontEnumerator");
%}

//---------------------------------------------------------------------------
