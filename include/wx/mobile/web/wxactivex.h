/*! \file wxactivex.h
    \brief implements wxActiveXOld window class and OLE tools
*/

#ifndef WX_ACTIVE_X
#define WX_ACTIVE_X
#pragma warning( disable : 4101 4786)
#pragma warning( disable : 4786)


#include <wx/setup.h>
#include <wx/wx.h>
#include <wx/variant.h>
#include <wx/datetime.h>

#ifdef new
#undef new
#endif

#include "wx/beforestd.h"

#include <oleidl.h>
#include <exdisp.h>
#include <docobj.h>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

// \brief wxActiveXOld Namespace for stuff I want to keep out of other tools way.
namespace NS_wxActiveXOld
{
    // STL utilty class.
    // specific to wxActiveXOld, for creating
    // case insenstive maps etc
    struct less_wxStringI
    {
        bool operator()(const wxString& x, const wxString& y) const
        {
            return x.CmpNoCase(y) < 0;
        };
    };
};


//////////////////////////////////////////
// Template class for smart interface handling.
// - Automatically dereferences ole interfaces
// - Smart Copy Semantics
// - Can Create Interfaces
// - Can query for other interfaces
template <class I> class wxAutoOleInterface
{
    protected:
    I *m_interface;

    public:
    // takes ownership of an existing interface
    // Assumed to already have a AddRef() applied
    explicit wxAutoOleInterface(I *pInterface = NULL) : m_interface(pInterface) {}

    // queries for an interface
    wxAutoOleInterface(REFIID riid, IUnknown *pUnk) : m_interface(NULL)
    {
        QueryInterface(riid, pUnk);
    };
    // queries for an interface
    wxAutoOleInterface(REFIID riid, IDispatch *pDispatch) : m_interface(NULL)
    {
        QueryInterface(riid, pDispatch);
    };

    // Creates an Interface
    wxAutoOleInterface(REFCLSID clsid, REFIID riid) : m_interface(NULL)
    {
        CreateInstance(clsid, riid);
    };

    // copy constructor
    wxAutoOleInterface(const wxAutoOleInterface<I>& ti) : m_interface(NULL)
    {
        operator = (ti);
    }

    // assignment operator
    wxAutoOleInterface<I>& operator = (const wxAutoOleInterface<I>& ti)
    {
        if (ti.m_interface)
            ti.m_interface->AddRef();
        Free();
        m_interface = ti.m_interface;
        return *this;
    }

    // takes ownership of an existing interface
    // Assumed to already have a AddRef() applied
    wxAutoOleInterface<I>& operator = (I *&ti)
    {
        Free();
        m_interface = ti;
        return *this;
    }

    // invokes Free()
    ~wxAutoOleInterface()
    {
        Free();
    };


    // Releases interface (i.e decrements refCount)
    inline void Free()
    {
        if (m_interface)
            m_interface->Release();
        m_interface = NULL;
    };

    // queries for an interface
    HRESULT QueryInterface(REFIID riid, IUnknown *pUnk)
    {
        Free();
        wxASSERT(pUnk != NULL);
        return pUnk->QueryInterface(riid, (void **) &m_interface);
    };

    // Create a Interface instance
    HRESULT CreateInstance(REFCLSID clsid, REFIID riid)
    {
        Free();
        return CoCreateInstance(clsid, NULL, CLSCTX_ALL, riid, (void **) &m_interface);
    };


    // returns the interface pointer
    inline operator I *() const {return m_interface;}

    // returns the dereferenced interface pointer
    inline I* operator ->() {return m_interface;}
    // returns a pointer to the interface pointer
    inline I** GetRef()    {return &m_interface;}
    // returns true if we have a valid interface pointer
    inline bool Ok() const    {return m_interface != NULL;}
};


// \brief Converts a std HRESULT to its error code.
// Hardcoded, by no means a definitive list.
wxString OLEHResultToString(HRESULT hr);
// \brief Returns the string description of a IID.
// Hardcoded, by no means a definitive list.
wxString GetIIDName(REFIID riid);

//#define __WXOLEDEBUG


#ifdef __WXOLEDEBUG
    #define WXOLE_TRACE(str) {OutputDebugString(str);OutputDebugString("\r\n");}
    #define WXOLE_TRACEOUT(stuff)\
    {\
        wxString os;\
        os << stuff << "\r\n";\
        WXOLE_TRACE(os.mb_str());\
    }

    #define WXOLE_WARN(__hr,msg)\
    {\
        if (__hr != S_OK)\
        {\
            wxString s = "*** ";\
            s += msg;\
            s += " : "+ OLEHResultToString(__hr);\
            WXOLE_TRACE(s.c_str());\
        }\
    }
#else
    #define WXOLE_TRACE(str)
    #define WXOLE_TRACEOUT(stuff)
    #define WXOLE_WARN(_proc,msg) {_proc;}
#endif

class wxOleInit
{
    public:
    static IMalloc *GetIMalloc();

    wxOleInit();
    ~wxOleInit();
};

#define DECLARE_OLE_UNKNOWN(cls)\
    private:\
    class TAutoInitInt\
    {\
        public:\
        LONG l;\
        TAutoInitInt() : l(0) {}\
    };\
    TAutoInitInt refCount, lockCount;\
    wxOleInit oleInit;\
    static void _GetInterface(cls *self, REFIID iid, void **_interface, const char *&desc);\
    public:\
    LONG GetRefCount();\
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);\
    ULONG STDMETHODCALLTYPE AddRef();\
    ULONG STDMETHODCALLTYPE Release();\
    ULONG STDMETHODCALLTYPE AddLock();\
    ULONG STDMETHODCALLTYPE ReleaseLock()

#define DEFINE_OLE_TABLE(cls)\
    LONG cls::GetRefCount() {return refCount.l;}\
    HRESULT STDMETHODCALLTYPE cls::QueryInterface(REFIID iid, void ** ppvObject)\
    {\
        if (! ppvObject)\
        {\
            WXOLE_TRACE("*** NULL POINTER ***");\
            return E_FAIL;\
        };\
        const char *desc = NULL;\
        cls::_GetInterface(this, iid, ppvObject, desc);\
        if (! *ppvObject)\
        {\
            WXOLE_TRACEOUT("<" << GetIIDName(iid).c_str() << "> Not Found");\
            return E_NOINTERFACE;\
        };\
        WXOLE_TRACEOUT("QI : <" << desc <<">");\
        ((IUnknown * )(*ppvObject))->AddRef();\
        return S_OK;\
    };\
    ULONG STDMETHODCALLTYPE cls::AddRef()\
    {\
        WXOLE_TRACEOUT(# cls << "::Add ref(" << refCount.l << ")");\
        InterlockedIncrement(&refCount.l);\
        return refCount.l;\
    };\
    ULONG STDMETHODCALLTYPE cls::Release()\
    {\
        if (refCount.l > 0)\
        {\
            InterlockedDecrement(&refCount.l);\
            WXOLE_TRACEOUT(# cls << "::Del ref(" << refCount.l << ")");\
            if (refCount.l == 0)\
            {\
                delete this;\
                return 0;\
            };\
            return refCount.l;\
        }\
        else\
            return 0;\
    }\
    ULONG STDMETHODCALLTYPE cls::AddLock()\
    {\
        WXOLE_TRACEOUT(# cls << "::Add Lock(" << lockCount.l << ")");\
        InterlockedIncrement(&lockCount.l);\
        return lockCount.l;\
    };\
    ULONG STDMETHODCALLTYPE cls::ReleaseLock()\
    {\
        if (lockCount.l > 0)\
        {\
            InterlockedDecrement(&lockCount.l);\
            WXOLE_TRACEOUT(# cls << "::Del Lock(" << lockCount.l << ")");\
            return lockCount.l;\
        }\
        else\
            return 0;\
    }\
    DEFINE_OLE_BASE(cls)

#define DEFINE_OLE_BASE(cls)\
    void cls::_GetInterface(cls *self, REFIID iid, void **_interface, const char *&desc)\
    {\
        *_interface = NULL;\
        desc = NULL;

#define OLE_INTERFACE(_iid, _type)\
    if (IsEqualIID(iid, _iid))\
    {\
        WXOLE_TRACE("Found Interface <" # _type ">");\
        *_interface = (IUnknown *) (_type *) self;\
        desc = # _iid;\
        return;\
    }

#define OLE_IINTERFACE(_face) OLE_INTERFACE(IID_##_face, _face)

#define OLE_INTERFACE_CUSTOM(func)\
    if (func(self, iid, _interface, desc))\
    {\
        return;\
    }

#define END_OLE_TABLE\
    }


// Main class for embedding a ActiveX control.
// Use by itself or derive from it
// \note The utility program (wxie) can generate a list of events, methods & properties
// for a control.
// First display the control (File|Display),
// then get the type info (ActiveX|Get Type Info) - these are copied to the clipboard.
// Eventually this will be expanded to autogenerate
// wxWindows source files for a control with all methods etc encapsulated.
// \par Usage:
//     construct using a ProgId or class id
//     \code new wxActiveXOld(parent, CLSID_WebBrowser, id, pos, size, style, name)\endcode
//     \code new wxActiveXOld(parent, "ShockwaveFlash.ShockwaveFlash", id, pos, size, style, name)\endcode
// \par Properties
// Properties can be set using \c SetProp() and set/retrieved using \c Prop()
//         \code SetProp(name, wxVariant(x)) \endcode or
//         \code wxString Prop("<name>") = x\endcode
//         \code wxString result = Prop("<name>")\endcode
//         \code flash_ctl.Prop("movie") = "file:///movies/test.swf";\endcode
//         \code flash_ctl.Prop("Playing") = false;\endcode
//         \code wxString current_movie = flash_ctl.Prop("movie");\endcode
// \par Methods
// Methods are invoked with \c CallMethod()
// \code wxVariant result = CallMethod("<name>", args, nargs = -1)\endcode
// \code wxVariant args[] = {0L, "file:///e:/dev/wxie/bug-zap.swf"};
// wxVariant result = X->CallMethod("LoadMovie", args);\endcode
// \par events
// respond to events with the
//         \c EVT_ACTIVEX(controlId, eventName, handler) &
//         \c EVT_ACTIVEX_DISPID(controlId, eventDispId, handler) macros
// \code
// BEGIN_EVENT_TABLE(wxIEFrame, wxFrame)
//     EVT_ACTIVEX_DISPID(ID_MSHTML, DISPID_STATUSTEXTCHANGE,  OnMSHTMLStatusTextChangeX)
//     EVT_ACTIVEX(ID_MSHTML, "BeforeNavigate2",   OnMSHTMLBeforeNavigate2X)
//     EVT_ACTIVEX(ID_MSHTML, "TitleChange",       OnMSHTMLTitleChangeX)
//     EVT_ACTIVEX(ID_MSHTML, "NewWindow2",        OnMSHTMLNewWindow2X)
//     EVT_ACTIVEX(ID_MSHTML, "ProgressChange",    OnMSHTMLProgressChangeX)
// END_EVENT_TABLE()\endcode
class wxActiveXOld : public wxWindow {
public:
    // General parameter and return type infoformation for Events, Properties and Methods.
    // refer to ELEMDESC, IDLDESC in MSDN
    class ParamX
    {
    public:
        USHORT        flags;
        bool isPtr, isSafeArray;
        VARTYPE        vt;
        wxString    name;

        ParamX() : vt(VT_EMPTY) {}
        inline bool IsIn() const        {return (flags & IDLFLAG_FIN) != 0;}
        inline bool IsOut() const        {return (flags & IDLFLAG_FOUT) != 0;}
        inline bool IsRetVal() const    {return (flags & IDLFLAG_FRETVAL) != 0;}
    };
    typedef vector<ParamX>    ParamXArray;

    // Type & Parameter info for Events and Methods.
    // refer to FUNCDESC in MSDN
    class FuncX
    {
    public:
        wxString    name;
        MEMBERID    memid;
        bool        hasOut;

        ParamX      retType;
        ParamXArray    params;
    };

    // Type info for properties.
    class PropX
    {
    public:
        wxString    name;
        MEMBERID    memid;
        ParamX      type;
        ParamX      arg;
        bool        putByRef;

        PropX() : putByRef (false) {}
        inline bool CanGet() const {return type.vt != VT_EMPTY;}
        inline bool CanSet() const {return arg.vt != VT_EMPTY;}
    };

    // Create using clsid.
    wxActiveXOld(wxWindow * parent, REFCLSID clsid, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxPanelNameStr);
    // create using progid.
    wxActiveXOld(wxWindow * parent, wxString progId, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxPanelNameStr);
    virtual ~wxActiveXOld();

    // Number of events defined for this control.
    inline int GetEventCount() const {return m_events.size();}
    // returns event description by index.
    // throws exception for invalid index
    const FuncX& GetEventDesc(int idx) const;

    // Number of properties defined for this control.
    inline int GetPropCount() const {return m_props.size();}
    // returns property description by index.
    // throws exception for invalid index
    const PropX& GetPropDesc(int idx) const;
    // returns property description by name.
    // throws exception for invalid name
    const PropX& GetPropDesc(wxString name) const;

    // Number of methods defined for this control.
    inline int GetMethodCount() const {return m_methods.size();}
    // returns method description by name.
    // throws exception for invalid index
    const FuncX& GetMethodDesc(int idx) const;
    // returns method description by name.
    // throws exception for invalid name
    const FuncX& GetMethodDesc(wxString name) const;

    // Set property VARIANTARG value by MEMBERID.
    void SetProp(MEMBERID name, VARIANTARG& value);
    // Set property using wxVariant by name.
    void SetProp(const wxString &name, const wxVariant &value);

    class wxPropertySetter
    {
    public:
        wxActiveXOld *m_ctl;
        wxString m_propName;

        wxPropertySetter(wxActiveXOld *ctl, wxString propName) :
            m_ctl(ctl), m_propName(propName) {}

        inline const wxPropertySetter& operator = (wxVariant v) const
        {
            m_ctl->SetProp(m_propName, v);
            return *this;
        };

        inline operator wxVariant() const   {return m_ctl->GetPropAsWxVariant(m_propName);};
        inline operator wxString() const    {return m_ctl->GetPropAsString(m_propName);};
        inline operator char() const        {return m_ctl->GetPropAsChar(m_propName);};
        inline operator long() const        {return m_ctl->GetPropAsLong(m_propName);};
        inline operator bool() const        {return m_ctl->GetPropAsBool(m_propName);};
        inline operator double() const      {return m_ctl->GetPropAsDouble(m_propName);};
        inline operator wxDateTime() const  {return m_ctl->GetPropAsDateTime(m_propName);};
        inline operator void *() const      {return m_ctl->GetPropAsPointer(m_propName);};
    };

    // \fn inline wxPropertySetter Prop(wxString name) {return wxPropertySetter(this, name);}
    // \param name Property name to read/set
    // \return wxPropertySetter, which has overloads for setting/getting the property
    // \brief Generic Get/Set Property by name.
    // Automatically handles most types
    // \par Usage:
    //     - Prop("\<name\>") =  \<value\>
    //     - var = Prop("\<name\>")
    //     - e.g:
    //         - \code flash_ctl.Prop("movie") = "file:///movies/test.swf";\endcode
    //         - \code flash_ctl.Prop("Playing") = false;\endcode
    //         - \code wxString current_movie = flash_ctl.Prop("movie");\endcode
    // \exception raises exception if \<name\> is invalid
    // \note Have to add a few more type conversions yet ...
    inline wxPropertySetter Prop(wxString name) {return wxPropertySetter(this, name);}

    VARIANT GetPropAsVariant(MEMBERID name);
    VARIANT GetPropAsVariant(const wxString& name);
    wxVariant GetPropAsWxVariant(const wxString& name);
    wxString GetPropAsString(const wxString& name);
    char GetPropAsChar(const wxString& name);
    long GetPropAsLong(const wxString& name);
    bool GetPropAsBool(const wxString& name);
    double GetPropAsDouble(const wxString& name);
    wxDateTime GetPropAsDateTime(const wxString& name);
    void *GetPropAsPointer(const wxString& name);

    // methods
    // VARIANTARG form is passed straight to Invoke,
    // so args in *REVERSE* order
    VARIANT CallMethod(MEMBERID name, VARIANTARG args[], int argc);
    VARIANT CallMethod(wxString name, VARIANTARG args[] = NULL, int argc = -1);
    // args are in *NORMAL* order
    // args can be a single wxVariant or an array
    // \fn wxVariant CallMethod(wxString name, wxVariant args[], int nargs = -1);
    // \param name name of method to call
    // \param args array of wxVariant's, defaults to NULL (no args)
    // \param nargs number of arguments passed via args. Defaults to actual number of args for the method
    // \return wxVariant
    // \brief Call a method of the ActiveX control.
    // Automatically handles most types
    // \par Usage:
    //     - result = CallMethod("\<name\>", args, nargs)
    //     - e.g.
    //     - \code
    //     wxVariant args[] = {0L, "file:///e:/dev/wxie/bug-zap.swf"};
    //     wxVariant result = X->CallMethod("LoadMovie", args);\endcode
    // \exception raises exception if \<name\> is invalid
    // \note Since wxVariant has built in type conversion, most the std types can be passed easily
    wxVariant CallMethod(wxString name, wxVariant args[], int nargs = -1);

    HRESULT ConnectAdvise(REFIID riid, IUnknown *eventSink);

    void OnSize(wxSizeEvent&);
    void OnPaint(wxPaintEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnSetFocus(wxFocusEvent&);
    void OnKillFocus(wxFocusEvent&);

    DECLARE_EVENT_TABLE();

protected:
    friend class wxActiveXOldFrameSite;
    friend class wxActiveXOldEvents;


    typedef vector<FuncX>                                       FuncXArray;
    typedef vector<PropX>                                       PropXArray;
    typedef map<MEMBERID, int>                                    MemberIdMap;
    typedef map<wxString, int, NS_wxActiveXOld::less_wxStringI>    NameMap;

    typedef wxAutoOleInterface<IConnectionPoint>    wxOleConnectionPoint;
    typedef pair<wxOleConnectionPoint, DWORD>        wxOleConnection;
    typedef vector<wxOleConnection>                    wxOleConnectionArray;

    wxAutoOleInterface<IDispatch>            m_Dispatch;
    wxAutoOleInterface<IOleClientSite>      m_clientSite;
    wxAutoOleInterface<IUnknown>            m_ActiveX;
    wxAutoOleInterface<IOleObject>            m_oleObject;
    wxAutoOleInterface<IOleInPlaceObject>    m_oleInPlaceObject;
    wxAutoOleInterface<IOleInPlaceActiveObject>

                                            m_oleInPlaceActiveObject;
    wxAutoOleInterface<IOleDocumentView>    m_docView;
    wxAutoOleInterface<IViewObject>            m_viewObject;
    HWND m_oleObjectHWND;
    bool m_bAmbientUserMode;
    DWORD m_docAdviseCookie;
    wxOleConnectionArray                    m_connections;

    void CreateActiveX(REFCLSID clsid);
    void CreateActiveX(LPOLESTR progId);
    HRESULT AmbientPropertyChanged(DISPID dispid);

    void GetTypeInfo();
    void GetTypeInfo(ITypeInfo *ti, bool defInterface, bool defEventSink);


    // events
    FuncXArray        m_events;
    MemberIdMap     m_eventMemberIds;

    // properties
    PropXArray        m_props;
    NameMap         m_propNames;

    // Methods
    FuncXArray      m_methods;
    NameMap         m_methodNames;

    wxActiveXOldFrameSite        *m_frameSite;
    long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
    virtual bool MSWTranslateMessage(WXMSG* pMsg);
};

// events
class wxActiveXOldEvent : public wxCommandEvent
{
private:
    friend class wxActiveXOldEvents;

    wxVariant m_params;

public:

    virtual wxEvent *Clone() const { return new wxActiveXOldEvent(*this); }

    wxString EventName();
    int ParamCount() const;
    wxString ParamType(int idx);
    wxString ParamName(int idx);
    wxVariant& operator[] (int idx);
    wxVariant& operator[] (wxString name);
};

const wxEventType& RegisterActiveXEvent(const wxChar *eventName);
const wxEventType& RegisterActiveXEvent(DISPID event);

typedef void (wxEvtHandler::*wxActiveXOldEventFunction)(wxActiveXOldEvent&);

// \def EVT_ACTIVEX(id, eventName, fn)
// \brief Event handle for events by name
#define EVT_ACTIVEX(id, eventName, fn) DECLARE_EVENT_TABLE_ENTRY(RegisterActiveXEvent(wxT(eventName)), id, -1, (wxObjectEventFunction) (wxEventFunction) (wxActiveXOldEventFunction) & fn, (wxObject *) NULL ),
// \def EVT_ACTIVEX_DISPID(id, eventDispId, fn)
// \brief Event handle for events by DISPID (dispath id)
#define EVT_ACTIVEX_DISPID(id, eventDispId, fn) DECLARE_EVENT_TABLE_ENTRY(RegisterActiveXEvent(eventDispId), id, -1, (wxObjectEventFunction) (wxEventFunction) (wxActiveXOldEventFunction) & fn, (wxObject *) NULL ),

//util
bool wxDateTimeToVariant(wxDateTime dt, VARIANTARG& va);
bool VariantToWxDateTime(VARIANTARG va, wxDateTime& dt);
// \relates wxActiveXOld
// \fn bool MSWVariantToVariant(VARIANTARG& va, wxVariant& vx);
// \param va VARAIANTARG to convert from
// \param vx Destination wxVariant
// \return success/failure (true/false)
// \brief Convert MSW VARIANTARG to wxVariant.
// Handles basic types, need to add:
// - VT_ARRAY | VT_*
// - better support for VT_UNKNOWN (currently treated as void *)
// - better support for VT_DISPATCH (currently treated as void *)
bool MSWVariantToVariant(VARIANTARG& va, wxVariant& vx);
// \relates wxActiveXOld
// \fn bool VariantToMSWVariant(const wxVariant& vx, VARIANTARG& va);
// \param vx wxVariant to convert from
// \param va Destination VARIANTARG
// \return success/failure (true/false)
// \brief Convert wxVariant to MSW VARIANTARG.
// Handles basic types, need to add:
// - VT_ARRAY | VT_*
// - better support for VT_UNKNOWN (currently treated as void *)
// - better support for VT_DISPATCH (currently treated as void *)
bool VariantToMSWVariant(const wxVariant& vx, VARIANTARG& va);

#endif /* _IEHTMLWIN_H_ */
