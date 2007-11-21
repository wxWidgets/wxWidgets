/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

WXDLLEXPORT_DATA(extern const wxChar) wxControlNameStr[];

// General item class
class WXDLLEXPORT wxControl : public wxControlBase
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
   wxControl();
   wxControl(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxControlNameStr)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxControlNameStr);
   virtual ~wxControl();

   // Simulates an event
   virtual void Command(wxCommandEvent& event) { ProcessCommand(event); }

   // implementation from now on
   // --------------------------

   // Calls the callback and appropriate event handlers
   bool ProcessCommand(wxCommandEvent& event);
   virtual void SetLabel(const wxString& title) ;

   wxList& GetSubcontrols() { return m_subControls; }
   
   void OnEraseBackground(wxEraseEvent& event);
   
   virtual bool             Enable(bool enable = TRUE) ;
   virtual bool             Show(bool show = TRUE) ;
   virtual void             DoSetWindowVariant( wxWindowVariant variant ) ;
 
   virtual void         MacRedrawControl () ;
   virtual void      MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool mouseStillDown ) ;
   virtual void             MacPreControlCreate( wxWindow *parent, wxWindowID id, wxString label ,
                          const wxPoint& pos,
                          const wxSize& size, long style,
                          const wxValidator& validator,
                          const wxString& name ,
                          WXRECTPTR outBounds ,
                          unsigned char* maclabel ) ;
   virtual void         MacPostControlCreate() ;
   virtual void         MacAdjustControlRect() ;
   virtual WXWidget MacGetContainerForEmbedding() ;
   virtual void      MacSuperChangedPosition() ;
   virtual void      MacSuperEnabled( bool enabled ) ;
   virtual void      MacSuperShown( bool show ) ;
   virtual bool         MacCanFocus() const ;
   virtual void         MacUpdateDimensions() ;
   void*             MacGetControlAction() { return m_macControlAction ; }
   
   virtual void       DoSetSize(int x, int y,int width, int height,int sizeFlags = wxSIZE_AUTO ) ;
   void              OnKeyDown( wxKeyEvent &event ) ;
   void                 OnMouseEvent( wxMouseEvent &event ) ;
   void              OnPaint(wxPaintEvent& event) ;
   virtual void         Refresh(bool eraseBack = TRUE, const wxRect *rect = NULL) ;
   WXWidget     GetMacControl() { return m_macControl ;}

protected:
   // For controls like radiobuttons which are really composite
   WXWidget m_macControl ;
   void*    m_macControlAction ;
   bool     m_macControlIsShown ;
   wxList   m_subControls;
   int      m_macHorizontalBorder ;
   int      m_macVerticalBorder ;

   virtual wxSize DoGetBestSize() const;

private:
   DECLARE_EVENT_TABLE()
};


wxControl *wxFindControlFromMacControl(WXWidget inControl ) ;
void wxAssociateControlWithMacControl(WXWidget inControl, wxControl *control) ;
void wxRemoveMacControlAssociation(wxControl *control) ;

#endif
    // _WX_CONTROL_H_
