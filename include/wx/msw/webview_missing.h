/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/webview_missing.h
// Purpose:     Defintions / classes commonly missing used by wxWebViewIE
// Author:      Steven Lamerton
// Id:          $Id$
// Copyright:   (c) 2012 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
 * Classes and definitions used by wxWebViewIE vary in their
 * completeness between compilers and versions of compilers.
 * We implement our own versions here which should work
 * for all compilers. The definitions are taken from the
 * mingw-w64 headers which are public domain.
*/

/* urlmon.h */

struct IHTMLElement;
struct IHTMLDocument2;

#ifndef REFRESH_NORMAL
#define REFRESH_NORMAL 0
#endif

#ifndef REFRESH_COMPLETELY
#define REFRESH_COMPLETELY 3
#endif

typedef enum __wxMIDL_IBindStatusCallback_0006
{
    wxBSCF_FIRSTDATANOTIFICATION = 0x1,
    wxBSCF_INTERMEDIATEDATANOTIFICATION = 0x2,
    wxBSCF_LASTDATANOTIFICATION = 0x4,
    wxBSCF_DATAFULLYAVAILABLE = 0x8,
    wxBSCF_AVAILABLEDATASIZEUNKNOWN = 0x10
}   wxBSCF;

EXTERN_C const IID CLSID_FileProtocol;

typedef struct _tagwxBINDINFO
{
    ULONG cbSize;
    LPWSTR szExtraInfo;
    STGMEDIUM stgmedData;
    DWORD grfBindInfoF;
    DWORD dwBindVerb;
    LPWSTR szCustomVerb;
    DWORD cbstgmedData;
    DWORD dwOptions;
    DWORD dwOptionsFlags;
    DWORD dwCodePage;
    SECURITY_ATTRIBUTES securityAttributes;
    IID iid;
    IUnknown *pUnk;
    DWORD dwReserved;
}   wxBINDINFO;

typedef struct _tagwxPROTOCOLDATA
{
    DWORD grfFlags;
    DWORD dwState;
    LPVOID pData;
    ULONG cbData;
}   wxPROTOCOLDATA;

class wxIInternetBindInfo : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL GetBindInfo(DWORD *grfBINDF, wxBINDINFO *pbindinfo) = 0;
    virtual HRESULT wxSTDCALL GetBindString(ULONG ulStringType, LPOLESTR *ppwzStr,
                                            ULONG cEl, ULONG *pcElFetched) = 0;
};

class wxIInternetProtocolSink : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL Switch(wxPROTOCOLDATA *pProtocolData) = 0;
    virtual HRESULT wxSTDCALL ReportProgress(ULONG ulStatusCode,
            LPCWSTR szStatusText) = 0;
    virtual HRESULT wxSTDCALL ReportData(DWORD grfBSCF, ULONG ulProgress,
                                         ULONG ulProgressMax) = 0;
    virtual HRESULT wxSTDCALL ReportResult(HRESULT hrResult, DWORD dwError,
                                           LPCWSTR szResult) = 0;
};

class wxIInternetProtocolRoot : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL Start(LPCWSTR szUrl, wxIInternetProtocolSink *pOIProtSink,
                                    wxIInternetBindInfo *pOIBindInfo, DWORD grfPI,
                                    HANDLE_PTR dwReserved) = 0;
    virtual HRESULT wxSTDCALL Continue(wxPROTOCOLDATA *pProtocolData) = 0;
    virtual HRESULT wxSTDCALL Abort(HRESULT hrReason, DWORD dwOptions) = 0;
    virtual HRESULT wxSTDCALL Terminate(DWORD dwOptions) = 0;
    virtual HRESULT wxSTDCALL Suspend(void) = 0;
    virtual HRESULT wxSTDCALL Resume(void) = 0;
};


class wxIInternetProtocol : public wxIInternetProtocolRoot
{
public:
    virtual HRESULT wxSTDCALL Read(void *pv, ULONG cb, ULONG *pcbRead) = 0;
    virtual HRESULT wxSTDCALL Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
                                   ULARGE_INTEGER *plibNewPosition) = 0;
    virtual HRESULT wxSTDCALL LockRequest(DWORD dwOptions) = 0;
    virtual HRESULT wxSTDCALL UnlockRequest(void) = 0;
};


class wxIInternetSession : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL RegisterNameSpace(IClassFactory *pCF, REFCLSID rclsid,
            LPCWSTR pwzProtocol,
            ULONG cPatterns,
            const LPCWSTR *ppwzPatterns,
            DWORD dwReserved) = 0;
    virtual HRESULT wxSTDCALL UnregisterNameSpace(IClassFactory *pCF,
            LPCWSTR pszProtocol) = 0;
    virtual HRESULT wxSTDCALL RegisterMimeFilter(IClassFactory *pCF,
            REFCLSID rclsid,
            LPCWSTR pwzType) = 0;
    virtual HRESULT wxSTDCALL UnregisterMimeFilter(IClassFactory *pCF,
            LPCWSTR pwzType) = 0;
    virtual HRESULT wxSTDCALL CreateBinding(LPBC pBC, LPCWSTR szUrl,
                                            IUnknown *pUnkOuter, IUnknown **ppUnk,
                                            wxIInternetProtocol **ppOInetProt,
                                            DWORD dwOption) = 0;
    virtual HRESULT wxSTDCALL SetSessionOption(DWORD dwOption, LPVOID pBuffer,
            DWORD dwBufferLength,
            DWORD dwReserved) = 0;
    virtual HRESULT wxSTDCALL GetSessionOption(DWORD dwOption, LPVOID pBuffer,
            DWORD *pdwBufferLength,
            DWORD dwReserved) = 0;
};

/* end of urlmon.h */

/* mshtmhst.h */

typedef enum _tagwxDOCHOSTUIFLAG
{
    DOCHOSTUIFLAG_DIALOG = 0x1,
    DOCHOSTUIFLAG_DISABLE_HELP_MENU = 0x2,
    DOCHOSTUIFLAG_NO3DBORDER = 0x4,
    DOCHOSTUIFLAG_SCROLL_NO = 0x8,
    DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE = 0x10,
    DOCHOSTUIFLAG_OPENNEWWIN = 0x20,
    DOCHOSTUIFLAG_DISABLE_OFFSCREEN = 0x40,
    DOCHOSTUIFLAG_FLAT_SCROLLBAR = 0x80,
    DOCHOSTUIFLAG_DIV_BLOCKDEFAULT = 0x100,
    DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY = 0x200,
    DOCHOSTUIFLAG_OVERRIDEBEHAVIORFACTORY = 0x400,
    DOCHOSTUIFLAG_CODEPAGELINKEDFONTS = 0x800,
    DOCHOSTUIFLAG_URL_ENCODING_DISABLE_UTF8 = 0x1000,
    DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8 = 0x2000,
    DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE = 0x4000,
    DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION = 0x10000,
    DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION = 0x20000,
    DOCHOSTUIFLAG_THEME = 0x40000,
    DOCHOSTUIFLAG_NOTHEME = 0x80000,
    DOCHOSTUIFLAG_NOPICS = 0x100000,
    DOCHOSTUIFLAG_NO3DOUTERBORDER = 0x200000,
    DOCHOSTUIFLAG_DISABLE_EDIT_NS_FIXUP = 0x400000,
    DOCHOSTUIFLAG_LOCAL_MACHINE_ACCESS_CHECK = 0x800000,
    DOCHOSTUIFLAG_DISABLE_UNTRUSTEDPROTOCOL = 0x1000000
} DOCHOSTUIFLAG;

typedef struct _tagwxDOCHOSTUIINFO
{
    ULONG cbSize;
    DWORD dwFlags;
    DWORD dwDoubleClick;
    OLECHAR *pchHostCss;
    OLECHAR *pchHostNS;
} DOCHOSTUIINFO;

class wxIDocHostUIHandler : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL ShowContextMenu(DWORD dwID, POINT *ppt,
            IUnknown *pcmdtReserved,
            IDispatch *pdispReserved) = 0;

    virtual HRESULT wxSTDCALL GetHostInfo(DOCHOSTUIINFO *pInfo) = 0;

    virtual HRESULT wxSTDCALL ShowUI(DWORD dwID,
                                     IOleInPlaceActiveObject *pActiveObject,
                                     IOleCommandTarget *pCommandTarget,
                                     IOleInPlaceFrame *pFrame,
                                     IOleInPlaceUIWindow *pDoc) = 0;

    virtual HRESULT wxSTDCALL HideUI(void) = 0;

    virtual HRESULT wxSTDCALL UpdateUI(void) = 0;

    virtual HRESULT wxSTDCALL EnableModeless(BOOL fEnable) = 0;

    virtual HRESULT wxSTDCALL OnDocWindowActivate(BOOL fActivate) = 0;

    virtual HRESULT wxSTDCALL OnFrameWindowActivate(BOOL fActivate) = 0;

    virtual HRESULT wxSTDCALL ResizeBorder(LPCRECT prcBorder,
                                           IOleInPlaceUIWindow *pUIWindow,
                                           BOOL fRameWindow) = 0;

    virtual HRESULT wxSTDCALL TranslateAccelerator(LPMSG lpMsg,
            const GUID *pguidCmdGroup,
            DWORD nCmdID) = 0;

    virtual HRESULT wxSTDCALL GetOptionKeyPath(LPOLESTR *pchKey,
            DWORD dw) = 0;

    virtual HRESULT wxSTDCALL GetDropTarget(IDropTarget *pDropTarget,
                                            IDropTarget **ppDropTarget) = 0;

    virtual HRESULT wxSTDCALL GetExternal(IDispatch **ppDispatch) = 0;

    virtual HRESULT wxSTDCALL TranslateUrl(DWORD dwTranslate,
                                           OLECHAR *pchURLIn,
                                           OLECHAR **ppchURLOut) = 0;

    virtual HRESULT wxSTDCALL FilterDataObject(IDataObject *pDO,
            IDataObject **ppDORet) = 0;
};

/* end of mshtmhst.h */

/* mshtml.h */

#include <mshtml.h>

#if defined(__MINGW32__) || defined(__VISUALC6__)

typedef enum _tagwxPOINTER_GRAVITY
{
    POINTER_GRAVITY_Left = 0,
    POINTER_GRAVITY_Right = 1,
    POINTER_GRAVITY_Max = 2147483647
} POINTER_GRAVITY;

typedef enum _tagwxELEMENT_ADJACENCY
{
    ELEM_ADJ_BeforeBegin = 0,
    ELEM_ADJ_AfterBegin = 1,
    ELEM_ADJ_BeforeEnd = 2,
    ELEM_ADJ_AfterEnd = 3,
    ELEMENT_ADJACENCY_Max = 2147483647
} ELEMENT_ADJACENCY;

typedef enum _tagwxMARKUP_CONTEXT_TYPE
{
    CONTEXT_TYPE_None = 0,
    CONTEXT_TYPE_Text = 1,
    CONTEXT_TYPE_EnterScope = 2,
    CONTEXT_TYPE_ExitScope = 3,
    CONTEXT_TYPE_NoScope = 4,
    MARKUP_CONTEXT_TYPE_Max = 2147483647
} MARKUP_CONTEXT_TYPE;

typedef enum _tagwxFINDTEXT_FLAGS
{
    FINDTEXT_BACKWARDS = 0x1,
    FINDTEXT_WHOLEWORD = 0x2,
    FINDTEXT_MATCHCASE = 0x4,
    FINDTEXT_RAW = 0x20000,
    FINDTEXT_MATCHDIAC = 0x20000000,
    FINDTEXT_MATCHKASHIDA = 0x40000000,
    FINDTEXT_MATCHALEFHAMZA = 0x80000000,
    FINDTEXT_FLAGS_Max = 2147483647
} FINDTEXT_FLAGS;

typedef enum _tagwxMOVEUNIT_ACTION
{
    MOVEUNIT_PREVCHAR = 0,
    MOVEUNIT_NEXTCHAR = 1,
    MOVEUNIT_PREVCLUSTERBEGIN = 2,
    MOVEUNIT_NEXTCLUSTERBEGIN = 3,
    MOVEUNIT_PREVCLUSTEREND = 4,
    MOVEUNIT_NEXTCLUSTEREND = 5,
    MOVEUNIT_PREVWORDBEGIN = 6,
    MOVEUNIT_NEXTWORDBEGIN = 7,
    MOVEUNIT_PREVWORDEND = 8,
    MOVEUNIT_NEXTWORDEND = 9,
    MOVEUNIT_PREVPROOFWORD = 10,
    MOVEUNIT_NEXTPROOFWORD = 11,
    MOVEUNIT_NEXTURLBEGIN = 12,
    MOVEUNIT_PREVURLBEGIN = 13,
    MOVEUNIT_NEXTURLEND = 14,
    MOVEUNIT_PREVURLEND = 15,
    MOVEUNIT_PREVSENTENCE = 16,
    MOVEUNIT_NEXTSENTENCE = 17,
    MOVEUNIT_PREVBLOCK = 18,
    MOVEUNIT_NEXTBLOCK = 19,
    MOVEUNIT_ACTION_Max = 2147483647
} MOVEUNIT_ACTION;

typedef enum _tagwxELEMENT_TAG_ID
{
    TAGID_NULL = 0, TAGID_UNKNOWN = 1, TAGID_A = 2, TAGID_ACRONYM = 3, TAGID_ADDRESS = 4, TAGID_APPLET = 5, TAGID_AREA = 6, TAGID_B = 7, TAGID_BASE = 8,
    TAGID_BASEFONT = 9, TAGID_BDO = 10, TAGID_BGSOUND = 11, TAGID_BIG = 12, TAGID_BLINK = 13, TAGID_BLOCKQUOTE = 14, TAGID_BODY = 15, TAGID_BR = 16,
    TAGID_BUTTON = 17, TAGID_CAPTION = 18, TAGID_CENTER = 19, TAGID_CITE = 20, TAGID_CODE = 21, TAGID_COL = 22, TAGID_COLGROUP = 23, TAGID_COMMENT = 24,
    TAGID_COMMENT_RAW = 25, TAGID_DD = 26, TAGID_DEL = 27, TAGID_DFN = 28, TAGID_DIR = 29, TAGID_DIV = 30, TAGID_DL = 31, TAGID_DT = 32, TAGID_EM = 33,
    TAGID_EMBED = 34, TAGID_FIELDSET = 35, TAGID_FONT = 36, TAGID_FORM = 37, TAGID_FRAME = 38, TAGID_FRAMESET = 39, TAGID_GENERIC = 40, TAGID_H1 = 41,
    TAGID_H2 = 42, TAGID_H3 = 43, TAGID_H4 = 44, TAGID_H5 = 45, TAGID_H6 = 46, TAGID_HEAD = 47, TAGID_HR = 48, TAGID_HTML = 49, TAGID_I = 50, TAGID_IFRAME = 51,
    TAGID_IMG = 52, TAGID_INPUT = 53, TAGID_INS = 54, TAGID_KBD = 55, TAGID_LABEL = 56, TAGID_LEGEND = 57, TAGID_LI = 58, TAGID_LINK = 59, TAGID_LISTING = 60,
    TAGID_MAP = 61, TAGID_MARQUEE = 62, TAGID_MENU = 63, TAGID_META = 64, TAGID_NEXTID = 65, TAGID_NOBR = 66, TAGID_NOEMBED = 67, TAGID_NOFRAMES = 68,
    TAGID_NOSCRIPT = 69, TAGID_OBJECT = 70, TAGID_OL = 71, TAGID_OPTION = 72, TAGID_P = 73, TAGID_PARAM = 74, TAGID_PLAINTEXT = 75, TAGID_PRE = 76,
    TAGID_Q = 77, TAGID_RP = 78, TAGID_RT = 79, TAGID_RUBY = 80, TAGID_S = 81, TAGID_SAMP = 82, TAGID_SCRIPT = 83, TAGID_SELECT = 84, TAGID_SMALL = 85,
    TAGID_SPAN = 86, TAGID_STRIKE = 87, TAGID_STRONG = 88, TAGID_STYLE = 89, TAGID_SUB = 90, TAGID_SUP = 91, TAGID_TABLE = 92, TAGID_TBODY = 93, TAGID_TC = 94,
    TAGID_TD = 95, TAGID_TEXTAREA = 96, TAGID_TFOOT = 97, TAGID_TH = 98, TAGID_THEAD = 99, TAGID_TITLE = 100, TAGID_TR = 101, TAGID_TT = 102, TAGID_U = 103,
    TAGID_UL = 104, TAGID_VAR = 105, TAGID_WBR = 106, TAGID_XMP = 107, TAGID_ROOT = 108, TAGID_OPTGROUP = 109, TAGID_COUNT = 110,
    TAGID_LAST_PREDEFINED = 10000, ELEMENT_TAG_ID_Max = 2147483647
} ELEMENT_TAG_ID;

#endif

struct wxIHTMLStyle : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL put_fontFamily(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_fontFamily(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_fontStyle(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_fontStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_fontVariant(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_fontVariant(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_fontWeight(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_fontWeight(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_fontSize(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_fontSize(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_font(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_font(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_color(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_color(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_background(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_background(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_backgroundColor(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_backgroundColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_backgroundImage(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_backgroundImage(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_backgroundRepeat(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_backgroundRepeat(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_backgroundAttachment(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_backgroundAttachment(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_backgroundPosition(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_backgroundPosition(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_backgroundPositionX(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_backgroundPositionX(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_backgroundPositionY(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_backgroundPositionY(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_wordSpacing(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_wordSpacing(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_letterSpacing(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_letterSpacing(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_textDecoration(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_textDecoration(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_textDecorationNone(VARIANT_BOOL v) = 0;
    virtual HRESULT wxSTDCALL get_textDecorationNone(VARIANT_BOOL *p) = 0;
    virtual HRESULT wxSTDCALL put_textDecorationUnderline(VARIANT_BOOL v) = 0;
    virtual HRESULT wxSTDCALL get_textDecorationUnderline(VARIANT_BOOL *p) = 0;
    virtual HRESULT wxSTDCALL put_textDecorationOverline(VARIANT_BOOL v) = 0;
    virtual HRESULT wxSTDCALL get_textDecorationOverline(VARIANT_BOOL *p) = 0;
    virtual HRESULT wxSTDCALL put_textDecorationLineThrough(VARIANT_BOOL v) = 0;
    virtual HRESULT wxSTDCALL get_textDecorationLineThrough(VARIANT_BOOL *p) = 0;
    virtual HRESULT wxSTDCALL put_textDecorationBlink(VARIANT_BOOL v) = 0;
    virtual HRESULT wxSTDCALL get_textDecorationBlink(VARIANT_BOOL *p) = 0;
    virtual HRESULT wxSTDCALL put_verticalAlign(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_verticalAlign(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_textTransform(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_textTransform(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_textAlign(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_textAlign(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_textIndent(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_textIndent(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_lineHeight(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_lineHeight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_marginTop(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_marginTop(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_marginRight(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_marginRight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_marginBottom(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_marginBottom(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_marginLeft(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_marginLeft(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_margin(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_margin(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_paddingTop(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_paddingTop(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_paddingRight(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_paddingRight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_paddingBottom(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_paddingBottom(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_paddingLeft(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_paddingLeft(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_padding(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_padding(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_border(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_border(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderTop(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderTop(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderRight(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderRight(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderBottom(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderBottom(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderLeft(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderLeft(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderColor(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderColor(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderTopColor(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderTopColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderRightColor(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderRightColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderBottomColor(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderBottomColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderLeftColor(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderLeftColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderWidth(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderWidth(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderTopWidth(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderTopWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderRightWidth(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderRightWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderBottomWidth(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderBottomWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderLeftWidth(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_borderLeftWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_borderStyle(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderTopStyle(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderTopStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderRightStyle(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderRightStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderBottomStyle(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderBottomStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_borderLeftStyle(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_borderLeftStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_width(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_width(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_height(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_height(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_styleFloat(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_styleFloat(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_clear(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_clear(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_display(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_display(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_visibility(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_visibility(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_listStyleType(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_listStyleType(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_listStylePosition(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_listStylePosition(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_listStyleImage(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_listStyleImage(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_listStyle(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_listStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_whiteSpace(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_whiteSpace(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_top(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_top(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_left(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_left(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_position(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_zIndex(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_zIndex(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_overflow(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_overflow(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_pageBreakBefore(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_pageBreakBefore(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_pageBreakAfter(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_pageBreakAfter(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_cssText(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_cssText(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_pixelTop(long v) = 0;
    virtual HRESULT wxSTDCALL get_pixelTop(long *p) = 0;
    virtual HRESULT wxSTDCALL put_pixelLeft(long v) = 0;
    virtual HRESULT wxSTDCALL get_pixelLeft(long *p) = 0;
    virtual HRESULT wxSTDCALL put_pixelWidth(long v) = 0;
    virtual HRESULT wxSTDCALL get_pixelWidth(long *p) = 0;
    virtual HRESULT wxSTDCALL put_pixelHeight(long v) = 0;
    virtual HRESULT wxSTDCALL get_pixelHeight(long *p) = 0;
    virtual HRESULT wxSTDCALL put_posTop(float v) = 0;
    virtual HRESULT wxSTDCALL get_posTop(float *p) = 0;
    virtual HRESULT wxSTDCALL put_posLeft(float v) = 0;
    virtual HRESULT wxSTDCALL get_posLeft(float *p) = 0;
    virtual HRESULT wxSTDCALL put_posWidth(float v) = 0;
    virtual HRESULT wxSTDCALL get_posWidth(float *p) = 0;
    virtual HRESULT wxSTDCALL put_posHeight(float v) = 0;
    virtual HRESULT wxSTDCALL get_posHeight(float *p) = 0;
    virtual HRESULT wxSTDCALL put_cursor(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_cursor(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_clip(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_clip(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_filter(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_filter(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL setAttribute(BSTR strAttributeName, VARIANT AttributeValue, LONG lFlags = 1) = 0;
    virtual HRESULT wxSTDCALL getAttribute(BSTR strAttributeName, LONG lFlags, VARIANT *AttributeValue) = 0;
    virtual HRESULT wxSTDCALL removeAttribute(BSTR strAttributeName, LONG lFlags, VARIANT_BOOL *pfSuccess) = 0;
    virtual HRESULT wxSTDCALL toString(BSTR *String) = 0;
};

struct wxIHTMLCurrentStyle : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL get_position(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_styleFloat(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_color(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_backgroundColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_fontFamily(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_fontStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_fontVariant(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_fontWeight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_fontSize(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_backgroundImage(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_backgroundPositionX(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_backgroundPositionY(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_backgroundRepeat(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderLeftColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_borderTopColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_borderRightColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_borderBottomColor(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_borderTopStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderRightStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderBottomStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderLeftStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderTopWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_borderRightWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_borderBottomWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_borderLeftWidth(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_left(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_top(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_width(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_height(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_paddingLeft(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_paddingTop(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_paddingRight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_paddingBottom(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_textAlign(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_textDecoration(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_display(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_visibility(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_zIndex(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_letterSpacing(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_lineHeight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_textIndent(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_verticalAlign(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_backgroundAttachment(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_marginTop(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_marginRight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_marginBottom(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_marginLeft(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_clear(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_listStyleType(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_listStylePosition(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_listStyleImage(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_clipTop(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_clipRight(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_clipBottom(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_clipLeft(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_overflow(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_pageBreakBefore(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_pageBreakAfter(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_cursor(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_tableLayout(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderCollapse(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_direction(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_behavior(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL getAttribute(BSTR strAttributeName, LONG lFlags, VARIANT *AttributeValue) = 0;
    virtual HRESULT wxSTDCALL get_unicodeBidi(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_right(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_bottom(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_imeMode(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_rubyAlign(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_rubyPosition(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_rubyOverhang(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_textAutospace(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_lineBreak(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_wordBreak(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_textJustify(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_textJustifyTrim(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_textKashida(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_blockDirection(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_layoutGridChar(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_layoutGridLine(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_layoutGridMode(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_layoutGridType(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderStyle(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderColor(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_borderWidth(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_padding(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_margin(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_accelerator(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_overflowX(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_overflowY(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL get_textTransform(BSTR *p) = 0;
};


struct wxIHTMLRect : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL put_left(long v) = 0;
    virtual HRESULT wxSTDCALL get_left(long *p) = 0;
    virtual HRESULT wxSTDCALL put_top(long v) = 0;
    virtual HRESULT wxSTDCALL get_top(long *p) = 0;
    virtual HRESULT wxSTDCALL put_right(long v) = 0;
    virtual HRESULT wxSTDCALL get_right(long *p) = 0;
    virtual HRESULT wxSTDCALL put_bottom(long v) = 0;
    virtual HRESULT wxSTDCALL get_bottom(long *p) = 0;
};

struct wxIHTMLRectCollection : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL get_length(long *p) = 0;
    virtual HRESULT wxSTDCALL get__newEnum(IUnknown **p) = 0;
    virtual HRESULT wxSTDCALL item(VARIANT *pvarIndex, VARIANT *pvarResult) = 0;
};

struct wxIHTMLFiltersCollection : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL get_length(long *p) = 0;
    virtual HRESULT wxSTDCALL get__newEnum(IUnknown **p) = 0;
    virtual HRESULT wxSTDCALL item(VARIANT *pvarIndex, VARIANT *pvarResult) = 0;
};

struct wxIHTMLElementCollection : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL toString(BSTR *String) = 0;
    virtual HRESULT wxSTDCALL put_length(long v) = 0;
    virtual HRESULT wxSTDCALL get_length(long *p) = 0;
    virtual HRESULT wxSTDCALL get__newEnum(IUnknown **p) = 0;
    virtual HRESULT wxSTDCALL item(VARIANT name, VARIANT index, IDispatch **pdisp) = 0;
    virtual HRESULT wxSTDCALL tags(VARIANT tagName, IDispatch **pdisp) = 0;
};

struct wxIHTMLElement2 : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL get_scopeName(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL setCapture(VARIANT_BOOL containerCapture = -1) = 0;
    virtual HRESULT wxSTDCALL releaseCapture(void) = 0;
    virtual HRESULT wxSTDCALL put_onlosecapture(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onlosecapture(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL componentFromPoint(long x, long y, BSTR *component) = 0;
    virtual HRESULT wxSTDCALL doScroll(VARIANT component) = 0;
    virtual HRESULT wxSTDCALL put_onscroll(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onscroll(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_ondrag(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_ondrag(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_ondragend(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_ondragend(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_ondragenter(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_ondragenter(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_ondragover(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_ondragover(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_ondragleave(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_ondragleave(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_ondrop(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_ondrop(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onbeforecut(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onbeforecut(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_oncut(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_oncut(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onbeforecopy(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onbeforecopy(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_oncopy(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_oncopy(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onbeforepaste(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onbeforepaste(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onpaste(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onpaste(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_currentStyle(wxIHTMLCurrentStyle **p) = 0;
    virtual HRESULT wxSTDCALL put_onpropertychange(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onpropertychange(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL getClientRects(wxIHTMLRectCollection **pRectCol) = 0;
    virtual HRESULT wxSTDCALL getBoundingClientRect(wxIHTMLRect **pRect) = 0;
    virtual HRESULT wxSTDCALL setExpression(BSTR propname, BSTR expression, BSTR language = L"") = 0;
    virtual HRESULT wxSTDCALL getExpression(BSTR propname, VARIANT *expression) = 0;
    virtual HRESULT wxSTDCALL removeExpression(BSTR propname, VARIANT_BOOL *pfSuccess) = 0;
    virtual HRESULT wxSTDCALL put_tabIndex(short v) = 0;
    virtual HRESULT wxSTDCALL get_tabIndex(short *p) = 0;
    virtual HRESULT wxSTDCALL focus(void) = 0;
    virtual HRESULT wxSTDCALL put_accessKey(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_accessKey(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_onblur(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onblur(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onfocus(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onfocus(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onresize(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onresize(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL blur(void) = 0;
    virtual HRESULT wxSTDCALL addFilter(IUnknown *pUnk) = 0;
    virtual HRESULT wxSTDCALL removeFilter(IUnknown *pUnk) = 0;
    virtual HRESULT wxSTDCALL get_clientHeight(long *p) = 0;
    virtual HRESULT wxSTDCALL get_clientWidth(long *p) = 0;
    virtual HRESULT wxSTDCALL get_clientTop(long *p) = 0;
    virtual HRESULT wxSTDCALL get_clientLeft(long *p) = 0;
    virtual HRESULT wxSTDCALL attachEvent(BSTR event, IDispatch *pDisp, VARIANT_BOOL *pfResult) = 0;
    virtual HRESULT wxSTDCALL detachEvent(BSTR event, IDispatch *pDisp) = 0;
    virtual HRESULT wxSTDCALL get_readyState(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onreadystatechange(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onreadystatechange(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onrowsdelete(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onrowsdelete(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_onrowsinserted(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onrowsinserted(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_oncellchange(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_oncellchange(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL put_dir(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_dir(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL createControlRange(IDispatch **range) = 0;
    virtual HRESULT wxSTDCALL get_scrollHeight(long *p) = 0;
    virtual HRESULT wxSTDCALL get_scrollWidth(long *p) = 0;
    virtual HRESULT wxSTDCALL put_scrollTop(long v) = 0;
    virtual HRESULT wxSTDCALL get_scrollTop(long *p) = 0;
    virtual HRESULT wxSTDCALL put_scrollLeft(long v) = 0;
    virtual HRESULT wxSTDCALL get_scrollLeft(long *p) = 0;
    virtual HRESULT wxSTDCALL clearAttributes(void) = 0;
    virtual HRESULT wxSTDCALL mergeAttributes(IHTMLElement *mergeThis) = 0;
    virtual HRESULT wxSTDCALL put_oncontextmenu(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_oncontextmenu(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL insertAdjacentElement(BSTR where, IHTMLElement *insertedElement, IHTMLElement **inserted) = 0;
    virtual HRESULT wxSTDCALL applyElement(IHTMLElement *apply, BSTR where, IHTMLElement **applied) = 0;
    virtual HRESULT wxSTDCALL getAdjacentText(BSTR where, BSTR *text) = 0;
    virtual HRESULT wxSTDCALL replaceAdjacentText(BSTR where, BSTR newText, BSTR *oldText) = 0;
    virtual HRESULT wxSTDCALL get_canHaveChildren(VARIANT_BOOL *p) = 0;
    virtual HRESULT wxSTDCALL addBehavior(BSTR bstrUrl, VARIANT *pvarFactory, long *pCookie) = 0;
    virtual HRESULT wxSTDCALL removeBehavior(long cookie, VARIANT_BOOL *pfResult) = 0;
    virtual HRESULT wxSTDCALL get_runtimeStyle(wxIHTMLStyle **p) = 0;
    virtual HRESULT wxSTDCALL get_behaviorUrns(IDispatch **p) = 0;
    virtual HRESULT wxSTDCALL put_tagUrn(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_tagUrn(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_onbeforeeditfocus(VARIANT v) = 0;
    virtual HRESULT wxSTDCALL get_onbeforeeditfocus(VARIANT *p) = 0;
    virtual HRESULT wxSTDCALL get_readyStateValue(long *p) = 0;
    virtual HRESULT wxSTDCALL getElementsByTagName(BSTR v,
            wxIHTMLElementCollection **pelColl) = 0;
};

struct wxIHTMLTxtRange : public IDispatch
{
public:
    virtual HRESULT wxSTDCALL get_htmlText(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL put_text(BSTR v) = 0;
    virtual HRESULT wxSTDCALL get_text(BSTR *p) = 0;
    virtual HRESULT wxSTDCALL parentElement(IHTMLElement **parent) = 0;
    virtual HRESULT wxSTDCALL duplicate(wxIHTMLTxtRange **Duplicate) = 0;
    virtual HRESULT wxSTDCALL inRange(wxIHTMLTxtRange *Range, VARIANT_BOOL *InRange) = 0;
    virtual HRESULT wxSTDCALL isEqual(wxIHTMLTxtRange *Range, VARIANT_BOOL *IsEqual) = 0;
    virtual HRESULT wxSTDCALL scrollIntoView(VARIANT_BOOL fStart = -1) = 0;
    virtual HRESULT wxSTDCALL collapse(VARIANT_BOOL Start = -1) = 0;
    virtual HRESULT wxSTDCALL expand(BSTR Unit, VARIANT_BOOL *Success) = 0;
    virtual HRESULT wxSTDCALL move(BSTR Unit, long Count, long *ActualCount) = 0;
    virtual HRESULT wxSTDCALL moveStart(BSTR Unit, long Count, long *ActualCount) = 0;
    virtual HRESULT wxSTDCALL moveEnd(BSTR Unit, long Count, long *ActualCount) = 0;
    virtual HRESULT wxSTDCALL select(void) = 0;
    virtual HRESULT wxSTDCALL pasteHTML(BSTR html) = 0;
    virtual HRESULT wxSTDCALL moveToElementText(IHTMLElement *element) = 0;
    virtual HRESULT wxSTDCALL setEndPoint(BSTR how, wxIHTMLTxtRange *SourceRange) = 0;
    virtual HRESULT wxSTDCALL compareEndPoints(BSTR how, wxIHTMLTxtRange *SourceRange, long *ret) = 0;
    virtual HRESULT wxSTDCALL findText(BSTR String, long count, long Flags, VARIANT_BOOL *Success) = 0;
    virtual HRESULT wxSTDCALL moveToPoint(long x, long y) = 0;
    virtual HRESULT wxSTDCALL getBookmark(BSTR *Boolmark) = 0;
    virtual HRESULT wxSTDCALL moveToBookmark(BSTR Bookmark, VARIANT_BOOL *Success) = 0;
    virtual HRESULT wxSTDCALL queryCommandSupported(BSTR cmdID, VARIANT_BOOL *pfRet) = 0;
    virtual HRESULT wxSTDCALL queryCommandEnabled(BSTR cmdID, VARIANT_BOOL *pfRet) = 0;
    virtual HRESULT wxSTDCALL queryCommandState(BSTR cmdID, VARIANT_BOOL *pfRet) = 0;
    virtual HRESULT wxSTDCALL queryCommandIndeterm(BSTR cmdID, VARIANT_BOOL *pfRet) = 0;
    virtual HRESULT wxSTDCALL queryCommandText(BSTR cmdID, BSTR *pcmdText) = 0;
    virtual HRESULT wxSTDCALL queryCommandValue(BSTR cmdID, VARIANT *pcmdValue) = 0;
    virtual HRESULT wxSTDCALL execCommand(BSTR cmdID, VARIANT_BOOL showUI, VARIANT value, VARIANT_BOOL *pfRet) = 0;
    virtual HRESULT wxSTDCALL execCommandShowHelp(BSTR cmdID, VARIANT_BOOL *pfRet) = 0;
};

struct wxIMarkupContainer : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL OwningDoc(IHTMLDocument2 **ppDoc) = 0;
};

struct wxIMarkupPointer : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL OwningDoc(IHTMLDocument2 **ppDoc) = 0;
    virtual HRESULT wxSTDCALL Gravity(POINTER_GRAVITY *pGravity) = 0;
    virtual HRESULT wxSTDCALL SetGravity(POINTER_GRAVITY Gravity) = 0;
    virtual HRESULT wxSTDCALL Cling(BOOL *pfCling) = 0;
    virtual HRESULT wxSTDCALL SetCling(BOOL fCLing) = 0;
    virtual HRESULT wxSTDCALL Unposition(void) = 0;
    virtual HRESULT wxSTDCALL IsPositioned(BOOL *pfPositioned) = 0;
    virtual HRESULT wxSTDCALL GetContainer(wxIMarkupContainer **ppContainer) = 0;
    virtual HRESULT wxSTDCALL MoveAdjacentToElement(IHTMLElement *pElement, ELEMENT_ADJACENCY eAdj) = 0;
    virtual HRESULT wxSTDCALL MoveToPointer(wxIMarkupPointer *pPointer) = 0;
    virtual HRESULT wxSTDCALL MoveToContainer(wxIMarkupContainer *pContainer, BOOL fAtStart) = 0;
    virtual HRESULT wxSTDCALL Left(BOOL fMove, MARKUP_CONTEXT_TYPE *pContext, IHTMLElement **ppElement, long *pcch, OLECHAR *pchText) = 0;
    virtual HRESULT wxSTDCALL Right(BOOL fMove, MARKUP_CONTEXT_TYPE *pContext, IHTMLElement **ppElement, long *pcch, OLECHAR *pchText) = 0;
    virtual HRESULT wxSTDCALL CurrentScope(IHTMLElement **ppElemCurrent) = 0;
    virtual HRESULT wxSTDCALL IsLeftOf(wxIMarkupPointer *pPointerThat, BOOL *pfResult) = 0;
    virtual HRESULT wxSTDCALL IsLeftOfOrEqualTo(wxIMarkupPointer *pPointerThat, BOOL *pfResult) = 0;
    virtual HRESULT wxSTDCALL IsRightOf(wxIMarkupPointer *pPointerThat, BOOL *pfResult) = 0;
    virtual HRESULT wxSTDCALL IsRightOfOrEqualTo(wxIMarkupPointer *pPointerThat, BOOL *pfResult) = 0;
    virtual HRESULT wxSTDCALL IsEqualTo(wxIMarkupPointer *pPointerThat, BOOL *pfAreEqual) = 0;
    virtual HRESULT wxSTDCALL MoveUnit(MOVEUNIT_ACTION muAction) = 0;
    virtual HRESULT wxSTDCALL FindText(OLECHAR *pchFindText, DWORD dwFlags, wxIMarkupPointer *pIEndMatch, wxIMarkupPointer *pIEndSearch) = 0;
};

struct wxIMarkupServices : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL CreateMarkupPointer(wxIMarkupPointer **ppPointer) = 0;
    virtual HRESULT wxSTDCALL CreateMarkupContainer(wxIMarkupContainer **ppMarkupContainer) = 0;
    virtual HRESULT wxSTDCALL CreateElement(ELEMENT_TAG_ID tagID, OLECHAR *pchAttributes, IHTMLElement **ppElement) = 0;
    virtual HRESULT wxSTDCALL CloneElement(IHTMLElement *pElemCloneThis, IHTMLElement **ppElementTheClone) = 0;
    virtual HRESULT wxSTDCALL InsertElement(IHTMLElement *pElementInsert, wxIMarkupPointer *pPointerStart, wxIMarkupPointer *pPointerFinish) = 0;
    virtual HRESULT wxSTDCALL RemoveElement(IHTMLElement *pElementRemove) = 0;
    virtual HRESULT wxSTDCALL Remove(wxIMarkupPointer *pPointerStart, wxIMarkupPointer *pPointerFinish) = 0;
    virtual HRESULT wxSTDCALL Copy(wxIMarkupPointer *pPointerSourceStart, wxIMarkupPointer *pPointerSourceFinish, wxIMarkupPointer *pPointerTarget) = 0;
    virtual HRESULT wxSTDCALL Move(wxIMarkupPointer *pPointerSourceStart, wxIMarkupPointer *pPointerSourceFinish, wxIMarkupPointer *pPointerTarget) = 0;
    virtual HRESULT wxSTDCALL InsertText(OLECHAR *pchText, long cch, wxIMarkupPointer *pPointerTarget) = 0;
    virtual HRESULT wxSTDCALL ParseString(OLECHAR *pchHTML, DWORD dwFlags, wxIMarkupContainer **ppContainerResult, wxIMarkupPointer *ppPointerStart, wxIMarkupPointer *ppPointerFinish) = 0;
    virtual HRESULT wxSTDCALL ParseGlobal(HGLOBAL hglobalHTML, DWORD dwFlags, wxIMarkupContainer **ppContainerResult, wxIMarkupPointer *pPointerStart, wxIMarkupPointer *pPointerFinish) = 0;
    virtual HRESULT wxSTDCALL IsScopedElement(IHTMLElement *pElement, BOOL *pfScoped) = 0;
    virtual HRESULT wxSTDCALL GetElementTagId(IHTMLElement *pElement, ELEMENT_TAG_ID *ptagId) = 0;
    virtual HRESULT wxSTDCALL GetTagIDForName(BSTR bstrName, ELEMENT_TAG_ID *ptagId) = 0;
    virtual HRESULT wxSTDCALL GetNameForTagID(ELEMENT_TAG_ID tagId, BSTR *pbstrName) = 0;
    virtual HRESULT wxSTDCALL MovePointersToRange(wxIHTMLTxtRange *pIRange, wxIMarkupPointer *pPointerStart, wxIMarkupPointer *pPointerFinish) = 0;
    virtual HRESULT wxSTDCALL MoveRangeToPointers(wxIMarkupPointer *pPointerStart, wxIMarkupPointer *pPointerFinish, wxIHTMLTxtRange *pIRange) = 0;
    virtual HRESULT wxSTDCALL BeginUndoUnit(OLECHAR *pchTitle) = 0;
    virtual HRESULT wxSTDCALL EndUndoUnit(void) = 0;
};

/* end of mshtml.h */