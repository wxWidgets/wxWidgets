#ifndef _SRC_STC_PLATWX_H_
#define _SRC_STC_PLATWX_H_

#include "wx/defs.h"

#if wxUSE_STC

#include "wx/imaglist.h"
#include "Platform.h"

class wxStyledTextCtrl;
class wxSTCListBox;
class wxSTCListBoxVisualData;



wxRect wxRectFromPRectangle(PRectangle prc);
PRectangle PRectangleFromwxRect(wxRect rc);
wxColour wxColourFromCD(const ColourDesired& ca);

class ListBoxImpl : public ListBox {
private:
    wxSTCListBox*           m_listBox;
    wxSTCListBoxVisualData* m_visualData;

public:
    ListBoxImpl();
    ~ListBoxImpl();
    static ListBox *Allocate();

    virtual void SetFont(Font &font) wxOVERRIDE;
    virtual void Create(Window &parent, int ctrlID, Point location_, int lineHeight_, bool unicodeMode_, int technology_) wxOVERRIDE;
    virtual void SetAverageCharWidth(int width) wxOVERRIDE;
    virtual void SetVisibleRows(int rows) wxOVERRIDE;
    virtual int GetVisibleRows() const wxOVERRIDE;
    virtual PRectangle GetDesiredRect() wxOVERRIDE;
    virtual int CaretFromEdge() wxOVERRIDE;
    virtual void Clear() wxOVERRIDE;
    virtual void Append(char *s, int type = -1) wxOVERRIDE;
    virtual int Length() wxOVERRIDE;
    virtual void Select(int n) wxOVERRIDE;
    virtual int GetSelection() wxOVERRIDE;
    virtual int Find(const char *prefix) wxOVERRIDE;
    virtual void GetValue(int n, char *value, int len) wxOVERRIDE;
    virtual void RegisterImage(int type, const char *xpm_data) wxOVERRIDE;
            void RegisterImageHelper(int type, const wxBitmap& bmp);
    virtual void RegisterRGBAImage(int type, int width, int height, const unsigned char *pixelsImage) wxOVERRIDE;
    virtual void ClearRegisteredImages() wxOVERRIDE;
    virtual void SetDoubleClickAction(CallBackAction, void *) wxOVERRIDE;
    virtual void SetList(const char* list, char separator, char typesep) wxOVERRIDE;
            void SetListInfo(int*, int*, int*);
};


//----------------------------------------------------------------------
// wxSTCPopupWindow

#if defined(__WXOSX_COCOA__) || defined(__WXMSW__) || defined(__WXGTK__)
    #define wxSTC_POPUP_IS_CUSTOM 1
#else
    #define wxSTC_POPUP_IS_CUSTOM 0
#endif

// Define the base class used for wxSTCPopupWindow.
#ifdef __WXOSX_COCOA__

    #include "wx/nonownedwnd.h"
    #define wxSTC_POPUP_IS_FRAME 0

    class wxSTCPopupBase:public wxNonOwnedWindow
    {
    public:
        wxSTCPopupBase(wxWindow*);
        virtual ~wxSTCPopupBase();
        virtual bool Show(bool show=true) wxOVERRIDE;

    protected:
        virtual void DoSetSize(int, int, int, int, int) wxOVERRIDE;
        void SetSTCCursor(int);
        void RestoreSTCCursor();
        void OnMouseEnter(wxMouseEvent&);
        void OnMouseLeave(wxMouseEvent&);
        void OnParentDestroy(wxWindowDestroyEvent& event);

    private:
        WX_NSWindow       m_nativeWin;
        wxStyledTextCtrl* m_stc;
        bool              m_cursorSetByPopup;
        int               m_prevCursor;
    };

#elif wxUSE_POPUPWIN

    #include "wx/popupwin.h"
    #define wxSTC_POPUP_IS_FRAME 0

    class wxSTCPopupBase:public wxPopupWindow
    {
    public:
        wxSTCPopupBase(wxWindow*);
        #ifdef __WXGTK__
            virtual ~wxSTCPopupBase();
        #elif defined(__WXMSW__)
            virtual bool Show(bool show=true) wxOVERRIDE;
            virtual bool MSWHandleMessage(WXLRESULT *result, WXUINT message,
                                          WXWPARAM wParam, WXLPARAM lParam)
                                          wxOVERRIDE;
        #endif
    };

#else

    #include "wx/frame.h"
    #define wxSTC_POPUP_IS_FRAME 1

    class wxSTCPopupBase:public wxFrame
    {
    public:
        wxSTCPopupBase(wxWindow*);
        #ifdef __WXMSW__
            virtual bool Show(bool show=true) wxOVERRIDE;
            virtual bool MSWHandleMessage(WXLRESULT *result, WXUINT message,
                                          WXWPARAM wParam, WXLPARAM lParam)
                                          wxOVERRIDE;
        #elif !wxSTC_POPUP_IS_CUSTOM
            virtual bool Show(bool show=true) wxOVERRIDE;
            void ActivateParent();
        #endif
    };

#endif // __WXOSX_COCOA__

class wxSTCPopupWindow:public wxSTCPopupBase
{
public:
    wxSTCPopupWindow(wxWindow*);
    virtual ~wxSTCPopupWindow();
    virtual bool Destroy() wxOVERRIDE;
    virtual bool AcceptsFocus() const wxOVERRIDE;

protected:
    virtual void DoSetSize(int x, int y, int width, int height,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
    void OnParentMove(wxMoveEvent& event);
    #if defined(__WXOSX_COCOA__) || (defined(__WXGTK__)&&!wxSTC_POPUP_IS_FRAME)
        void OnIconize(wxIconizeEvent& event);
    #elif !wxSTC_POPUP_IS_CUSTOM
        void OnFocus(wxFocusEvent& event);
    #endif

private:
    wxPoint   m_relPos;
    wxPoint   m_absPos;
    wxWindow* m_tlw;
};


//----------------------------------------------------------------------
// SurfaceData

class SurfaceData
{
public:
    virtual ~SurfaceData(){}
};

#if wxUSE_GRAPHICS_DIRECT2D

#include "wx/msw/private/graphicsd2d.h"
#include "wx/msw/private/comptr.h"

class ScintillaWX;

class SurfaceDataD2D: public SurfaceData
{
public:
    SurfaceDataD2D(ScintillaWX*);
    bool Initialised() const;
    void DiscardGraphicsResources();
    HRESULT CreateGraphicsResources();
    void SetEditorPaintAbandoned();

    ID2D1DCRenderTarget* GetRenderTarget() const { return m_pRenderTarget.get(); }
    ID2D1SolidColorBrush* GetSolidBrush() const { return m_pSolidBrush.get(); }
    ID2D1BitmapBrush* GetPatternBrush() const { return m_pPatternBrush.get(); }
    IDWriteRenderingParams* GetDefaultRenderingParams() const
        { return m_defaultRenderingParams; }
    IDWriteRenderingParams* GetCustomClearTypeRenderingParams() const
        { return m_customClearTypeRenderingParams; }

private:
    wxCOMPtr<ID2D1Factory> m_pD2DFactory;
    wxCOMPtr<IDWriteFactory> m_pDWriteFactory;
    wxCOMPtr<ID2D1DCRenderTarget> m_pRenderTarget;
    wxCOMPtr<ID2D1SolidColorBrush> m_pSolidBrush;
    wxCOMPtr<ID2D1BitmapBrush> m_pPatternBrush;
    wxCOMPtr<IDWriteRenderingParams> m_defaultRenderingParams;
    wxCOMPtr<IDWriteRenderingParams> m_customClearTypeRenderingParams;

    ScintillaWX* m_editor;
};

class SurfaceFontDataD2D: public SurfaceData
{
public:
    SurfaceFontDataD2D(const FontParameters& fp);
    bool Initialised() const;

    XYPOSITION GetAscent() const { return m_ascent; }
    XYPOSITION GetDescent() const { return m_descent; }
    XYPOSITION GetInternalLeading() const { return m_internalLeading; }
    XYPOSITION GetAverageCharWidth() const { return m_averageCharWidth; }

    D2D1_TEXT_ANTIALIAS_MODE GetFontQuality() const { return m_aaMode; }
    IDWriteTextFormat* GetFormat() const { return m_pTextFormat.get(); }

private:
    XYPOSITION m_ascent;
    XYPOSITION m_descent;
    XYPOSITION m_internalLeading;
    XYPOSITION m_averageCharWidth;
    D2D1_TEXT_ANTIALIAS_MODE m_aaMode;
    wxCOMPtr<IDWriteTextFormat> m_pTextFormat;
};

#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // wxUSE_STC

#endif // _SRC_STC_PLATWX_H_
