/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/bmpbtncmn.cpp
// Purpose:     wxBitmapButton common code
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_BMPBUTTON

#include "wx/bmpbuttn.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
#endif

#include "wx/artprov.h"
#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxBitmapButtonStyle )
wxBEGIN_FLAGS( wxBitmapButtonStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxBU_AUTODRAW)
    wxFLAGS_MEMBER(wxBU_LEFT)
    wxFLAGS_MEMBER(wxBU_RIGHT)
    wxFLAGS_MEMBER(wxBU_TOP)
    wxFLAGS_MEMBER(wxBU_BOTTOM)
wxEND_FLAGS( wxBitmapButtonStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxBitmapButton, wxButton, "wx/bmpbuttn.h");

wxBEGIN_PROPERTIES_TABLE(wxBitmapButton)
    wxPROPERTY_FLAGS( WindowStyle, wxBitmapButtonStyle, long, \
                      SetWindowStyleFlag, GetWindowStyleFlag, \
                      wxEMPTY_PARAMETER_VALUE, 0 /*flags*/, wxT("Helpstring"), \
                      wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxBitmapButton)

wxCONSTRUCTOR_5( wxBitmapButton, wxWindow*, Parent, wxWindowID, Id, \
                 wxBitmapBundle, BitmapBundle, wxPoint, Position, wxSize, Size )

/*
TODO PROPERTIES :

long "style" , wxBU_AUTODRAW
bool "default" , 0
bitmap "selected" ,
bitmap "focus" ,
bitmap "disabled" ,
*/

namespace
{

#ifdef wxHAS_DRAW_TITLE_BAR_BITMAP

// Define a specialized bundle that can render the close button at any size.
class wxBitmapBundleImplCloseButton : public wxBitmapBundleImpl
{
public:
    wxBitmapBundleImplCloseButton(wxWindow* win,
                                  const wxSize& size,
                                  const wxColour& colBg,
                                  int flags)
        : m_win(win), m_sizeDef(size), m_colBg(colBg), m_flags(flags)
    {
    }

    virtual wxSize GetDefaultSize() const wxOVERRIDE
    {
        return m_sizeDef;
    }

    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const wxOVERRIDE
    {
        // We can render the bitmap at any scale.
        return m_sizeDef*scale;
    }

    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE
    {
        wxBitmap bmp;
        bmp.Create(size.x, size.y, wxBITMAP_SCREEN_DEPTH);
        wxMemoryDC dc(bmp);
        dc.SetBackground(m_colBg);
        dc.Clear();
        wxRendererNative::Get().DrawTitleBarBitmap
            (
                m_win, dc, size,
                wxTITLEBAR_BUTTON_CLOSE, m_flags
            );
        return bmp;
    }

private:
    wxWindow* const m_win;
    const wxSize m_sizeDef;
    const wxColour m_colBg;
    const int m_flags;

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplCloseButton);
};

wxBitmapBundle
GetCloseButtonBitmap(wxWindow* win,
                     const wxSize& size,
                     const wxColour& colBg,
                     int flags = 0)
{
    return wxBitmapBundle::FromImpl(new wxBitmapBundleImplCloseButton
                                        (
                                            win, size, colBg, flags
                                        ));
}

#endif // wxHAS_DRAW_TITLE_BAR_BITMAP

} // anonymous namespace

bool
wxBitmapButton::CreateCloseButton(wxWindow* parent,
                                  wxWindowID winid,
                                  const wxString& name)
{
    wxCHECK_MSG( parent, false, wxS("Must have a valid parent") );

    const wxColour colBg = parent->GetBackgroundColour();

    wxBitmapBundle bb;
#ifdef wxHAS_DRAW_TITLE_BAR_BITMAP
    const wxSize sizeBmp = wxArtProvider::GetDIPSizeHint(wxART_BUTTON);
    bb = GetCloseButtonBitmap(parent, sizeBmp, colBg);
#else // !wxHAS_DRAW_TITLE_BAR_BITMAP
    bb = wxArtProvider::GetBitmapBundle(wxART_CLOSE, wxART_BUTTON);
#endif // wxHAS_DRAW_TITLE_BAR_BITMAP

    if ( !Create(parent, winid, bb,
                 wxDefaultPosition, wxDefaultSize,
                 wxBORDER_NONE, wxDefaultValidator, name) )
        return false;

#ifdef wxHAS_DRAW_TITLE_BAR_BITMAP
    SetBitmapPressed(
        GetCloseButtonBitmap(parent, sizeBmp, colBg, wxCONTROL_PRESSED));

    SetBitmapCurrent(
        GetCloseButtonBitmap(parent, sizeBmp, colBg, wxCONTROL_CURRENT));
#endif // wxHAS_DRAW_TITLE_BAR_BITMAP

    // The button should blend with its parent background.
    SetBackgroundColour(colBg);

    return true;
}

/* static */
wxBitmapButton*
wxBitmapButtonBase::NewCloseButton(wxWindow* parent,
                                   wxWindowID winid,
                                   const wxString& name)
{
    wxBitmapButton* const button = new wxBitmapButton();

    button->CreateCloseButton(parent, winid, name);

    return button;
}

#endif // wxUSE_BMPBUTTON
