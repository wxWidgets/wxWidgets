/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/statbmpcmn.cpp
// Purpose:     wxStaticBitmap common code
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STATBMP

#include "wx/statbmp.h"

extern WXDLLEXPORT_DATA(const char) wxStaticBitmapNameStr[] = "staticBitmap";

// ---------------------------------------------------------------------------
// XTI
// ---------------------------------------------------------------------------

wxDEFINE_FLAGS( wxStaticBitmapStyle )
wxBEGIN_FLAGS( wxStaticBitmapStyle )
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
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

wxEND_FLAGS( wxStaticBitmapStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticBitmap, wxControl, "wx/statbmp.h");

wxBEGIN_PROPERTIES_TABLE(wxStaticBitmap)
    wxPROPERTY_FLAGS( WindowStyle, wxStaticBitmapStyle, long, \
                      SetWindowStyleFlag, GetWindowStyleFlag, \
                      wxEMPTY_PARAMETER_VALUE, 0 /*flags*/, wxT("Helpstring"), \
                      wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxStaticBitmap)

wxCONSTRUCTOR_5( wxStaticBitmap, wxWindow*, Parent, wxWindowID, Id, \
                 wxBitmapBundle, BitmapBundle, wxPoint, Position, wxSize, Size )

/*
    TODO PROPERTIES :
        bitmap
*/

// ----------------------------------------------------------------------------
// wxStaticBitmap
// ----------------------------------------------------------------------------

wxStaticBitmapBase::~wxStaticBitmapBase()
{
    // this destructor is required for Darwin
}

wxSize wxStaticBitmapBase::DoGetBestSize() const
{
    if ( m_bitmapBundle.IsOk() )
        return m_bitmapBundle.GetPreferredLogicalSizeFor(this);

    // the fall back size is completely arbitrary
    return wxSize(16, 16);
}

wxBitmap wxStaticBitmapBase::GetBitmap() const
{
    return m_bitmapBundle.GetBitmapFor(this);
}

// Only wxMSW handles icons and bitmaps differently, in all the other ports
// they are exactly the same thing.
#ifdef wxICON_IS_BITMAP

void wxStaticBitmapBase::SetIcon(const wxIcon& icon)
{
    SetBitmap(icon);
}

wxIcon wxStaticBitmapBase::GetIcon() const
{
    wxIcon icon;
    icon.CopyFromBitmap(GetBitmap());
    return icon;
}

#else // !wxICON_IS_BITMAP

// Just provide the stabs for them, they're never used anyhow as they're
// overridden in wxMSW implementation of this class.
void wxStaticBitmapBase::SetIcon(const wxIcon& WXUNUSED(icon))
{
    wxFAIL_MSG(wxS("unreachable"));
}

wxIcon wxStaticBitmapBase::GetIcon() const
{
    wxFAIL_MSG(wxS("unreachable"));

    return wxIcon();
}

#endif // wxICON_IS_BITMAP/!wxICON_IS_BITMAP

#endif // wxUSE_STATBMP
