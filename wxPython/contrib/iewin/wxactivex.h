#ifndef WX_ACTIVE_X
#define WX_ACTIVE_X
#pragma warning( disable : 4101 4786)
#pragma warning( disable : 4786)


#include <wx/setup.h>
#include <wx/wx.h>
#include <wx/variant.h>
#include <oleidl.h>
#include <exdisp.h>
#include <docobj.h>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

//////////////////////////////////////////
// wxAutoOleInterface<Interface>
// Template class for smart interface handling
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

    ~wxAutoOleInterface()
    {
    	Free();
    };


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



    inline operator I *() const {return m_interface;}
    inline I* operator ->() {return m_interface;}
	inline I** GetRef()	{return &m_interface;}

	inline bool Ok() const	{return m_interface != NULL;}
};


wxString OLEHResultToString(HRESULT hr);
wxString GetIIDName(REFIID riid);

//#define __WXOLEDEBUG


#ifdef __WXOLEDEBUG
    #define WXOLE_TRACE(str) {OutputDebugString(str);OutputDebugString("\r\n");}
    #define WXOLE_TRACEOUT(stuff)\
    {\
        ostringstream os;\
        os << stuff << ends;\
        WXOLE_TRACE(os.str().c_str());\
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

// Auto Initialisation
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
        return

#define END_OLE_TABLE\
	}



class wxActiveX : public wxWindow {
public:
    ////////////////////////////////////////
    // type stuff
	class ParamX // refer to ELEMDESC, IDLDESC in MSDN
	{
	public:
		USHORT	    flags;
        bool isPtr, isSafeArray;
		VARTYPE	    vt;
        wxString    name;

		inline bool IsIn() const		{return (flags & IDLFLAG_FIN) != 0;}
		inline bool IsOut() const		{return (flags & IDLFLAG_FOUT) != 0;}
		inline bool IsRetVal() const	{return (flags & IDLFLAG_FRETVAL) != 0;}
	};

	typedef vector<ParamX>	ParamXArray;

    class FuncX // refer to FUNCDESC in MSDN
    {
    public:
        wxString    name;
        MEMBERID    memid;
		bool		hasOut;

		ParamXArray	params;
    };

    typedef vector<FuncX> FuncXArray;
    typedef map<MEMBERID, int>  MemberIdList;

    wxActiveX(wxWindow * parent, REFCLSID clsid, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxPanelNameStr);
    wxActiveX(wxWindow * parent, wxString progId, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxPanelNameStr);
	virtual ~wxActiveX();

	void CreateActiveX(REFCLSID clsid);
    void CreateActiveX(LPOLESTR progId);

    // expose type info
    inline int GetEventCount() const {return m_events.size();}
    const FuncX& GetEvent(int idx) const;

	HRESULT ConnectAdvise(REFIID riid, IUnknown *eventSink);

	void OnSize(wxSizeEvent&);
    void OnPaint(wxPaintEvent& event);
    void OnMouse(wxMouseEvent& event);
	void OnSetFocus(wxFocusEvent&);
    void OnKillFocus(wxFocusEvent&);

	DECLARE_EVENT_TABLE();

protected:
    friend class FrameSite;
    friend class wxActiveXEvents;

	typedef wxAutoOleInterface<IConnectionPoint>	wxOleConnectionPoint;
	typedef pair<wxOleConnectionPoint, DWORD>		wxOleConnection;
	typedef vector<wxOleConnection>					wxOleConnectionArray;

    wxAutoOleInterface<IOleClientSite>      m_clientSite;
    wxAutoOleInterface<IUnknown>            m_ActiveX;
	wxAutoOleInterface<IOleObject>			m_oleObject;
	wxAutoOleInterface<IOleInPlaceObject>	m_oleInPlaceObject;
    wxAutoOleInterface<IOleInPlaceActiveObject>

                                            m_oleInPlaceActiveObject;
    wxAutoOleInterface<IOleDocumentView>	m_docView;
    wxAutoOleInterface<IViewObject>	        m_viewObject;
	HWND m_oleObjectHWND;
    bool m_bAmbientUserMode;
    DWORD m_docAdviseCookie;
	wxOleConnectionArray					m_connections;

    HRESULT AmbientPropertyChanged(DISPID dispid);

	void GetTypeInfo();
	void GetTypeInfo(ITypeInfo *ti, bool defEventSink);


    // events
    FuncXArray      m_events;
    MemberIdList    m_eventsIdx;

    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
};

// events
class wxActiveXEvent : public wxCommandEvent
{
private:
    friend class wxActiveXEvents;

    wxVariant m_params;

public:

    virtual wxEvent *Clone() const { return new wxActiveXEvent(*this); }

    wxString EventName();
    int ParamCount() const;
    wxString ParamType(int idx);
    wxString ParamName(int idx);
    wxVariant  operator[] (int idx) const;
    wxVariant& operator[] (int idx);
    wxVariant  operator[] (wxString name) const;
    wxVariant& operator[] (wxString name);
};

const wxEventType& RegisterActiveXEvent(const wxChar *eventName);
const wxEventType& RegisterActiveXEvent(DISPID event);

typedef void (wxEvtHandler::*wxActiveXEventFunction)(wxActiveXEvent&);

#define EVT_ACTIVEX(id, eventName, fn) DECLARE_EVENT_TABLE_ENTRY(RegisterActiveXEvent(wxT(eventName)), id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxActiveXEventFunction, & fn ), (wxObject *) NULL ),
#define EVT_ACTIVEX_DISPID(id, eventDispId, fn) DECLARE_EVENT_TABLE_ENTRY(RegisterActiveXEvent(eventDispId), id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxActiveXEventFunction, & fn ), (wxObject *) NULL ),

//util
bool MSWVariantToVariant(VARIANTARG& va, wxVariant& vx);
bool VariantToMSWVariant(wxVariant& vx, VARIANTARG& va);

#endif /* _IEHTMLWIN_H_ */
