/////////////////////////////////////////////////////////////////////////////
// Name:        mactext.h
// Purpose:     private wxMacTextControl base class
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// RCS-ID:      $Id: 
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_PRIVATE_MACTEXT_H_
#define _WX_MAC_PRIVATE_MACTEXT_H_

#include "wx/mac/private.h"

// common interface for all implementations
class wxMacTextControl : public wxMacControl
{
public :
    wxMacTextControl( wxTextCtrl *peer ) ;
    virtual ~wxMacTextControl() ;

    virtual wxString GetStringValue() const = 0 ;
    virtual void SetStringValue( const wxString &val ) = 0 ;
    virtual void SetSelection( long from, long to ) = 0 ;
    virtual void GetSelection( long* from, long* to ) const = 0 ;
    virtual void WriteText( const wxString& str ) = 0 ;

    virtual void SetStyle( long start, long end, const wxTextAttr& style ) ;
    virtual void Copy() ;
    virtual void Cut() ;
    virtual void Paste() ;
    virtual bool CanPaste() const ;
    virtual void SetEditable( bool editable ) ;
    virtual wxTextPos GetLastPosition() const ;
    virtual void Replace( long from, long to, const wxString &str ) ;
    virtual void Remove( long from, long to ) ;


    virtual bool HasOwnContextMenu() const
        { return false ; }

    virtual bool SetupCursor( const wxPoint& WXUNUSED(pt) )
        { return false ; }

    virtual void Clear() ;
    virtual bool CanUndo() const;
    virtual void Undo() ;
    virtual bool CanRedo() const;
    virtual void Redo() ;
    virtual int GetNumberOfLines() const ;
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const ;
    virtual void ShowPosition(long WXUNUSED(pos)) ;
    virtual int GetLineLength(long lineNo) const ;
    virtual wxString GetLineText(long lineNo) const ;
    virtual void CheckSpelling(bool WXUNUSED(check)) { }

} ;

class wxMacUnicodeTextControl : public wxMacTextControl
{
public :
    wxMacUnicodeTextControl( wxTextCtrl *wxPeer ) ;
    wxMacUnicodeTextControl( wxTextCtrl *wxPeer,
                             const wxString& str,
                             const wxPoint& pos,
                             const wxSize& size, long style ) ;
    virtual ~wxMacUnicodeTextControl();

    virtual bool Create( wxTextCtrl *wxPeer,
                         const wxString& str,
                         const wxPoint& pos,
                         const wxSize& size, long style ) ;
    virtual void VisibilityChanged(bool shown);
    virtual wxString GetStringValue() const ;
    virtual void SetStringValue( const wxString &str) ;
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual bool CanPaste() const;
    virtual void SetEditable(bool editable) ;
    virtual void GetSelection( long* from, long* to) const ;
    virtual void SetSelection( long from , long to ) ;
    virtual void WriteText(const wxString& str) ;

protected :
	virtual void CreateControl( wxTextCtrl* peer, const Rect* bounds, CFStringRef cfr );

    // contains the tag for the content (is different for password and non-password controls)
    OSType m_valueTag ;
public :
    ControlEditTextSelectionRec m_selection ;
};

#endif // _WX_MAC_PRIVATE_MACTEXT_H_
