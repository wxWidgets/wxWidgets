/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#ifdef __GNUG__
    #pragma interface "control.h"
#endif

#include "wx/window.h"
#include "wx/list.h"

// General item class
class WXDLLEXPORT wxControl : public wxWindow
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
   wxControl();
   virtual ~wxControl();

   // Simulates an event
   bool Command(wxCommandEvent& event) { return ProcessCommand(event); }

   // Calls the callback and appropriate event handlers
   bool ProcessCommand(wxCommandEvent& event);

   // MSW-specific
#ifdef __WIN95__
   virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // Win95

   // For ownerdraw items
   virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *WXUNUSED(item)) { return FALSE; };
   virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *WXUNUSED(item)) { return FALSE; };

   wxList& GetSubcontrols() { return m_subControls; }

   void OnEraseBackground(wxEraseEvent& event);

#if WXWIN_COMPATIBILITY
   virtual void SetButtonColour(const wxColour& WXUNUSED(col)) { }
   wxColour* GetButtonColour() const { return NULL; }

   inline virtual void SetLabelFont(const wxFont& font);
   inline virtual void SetButtonFont(const wxFont& font);
   inline wxFont& GetLabelFont() const;
   inline wxFont& GetButtonFont() const;

   // Adds callback
   inline void Callback(const wxFunction function);

   wxFunction GetCallback() { return m_callback; }

protected:
   wxFunction       m_callback;     // Callback associated with the window
#endif // WXWIN_COMPATIBILITY

protected:
   // For controls like radiobuttons which are really composite
   wxList           m_subControls;

    virtual wxSize DoGetBestSize();

private:
   DECLARE_EVENT_TABLE()
};


#if WXWIN_COMPATIBILITY
    inline void wxControl::Callback(const wxFunction f) { m_callback = f; };
    inline wxFont& wxControl::GetLabelFont() const { return GetFont() ; }
    inline wxFont& wxControl::GetButtonFont() const { return GetFont() ; }
    inline void wxControl::SetLabelFont(const wxFont& font) { SetFont(font); }
    inline void wxControl::SetButtonFont(const wxFont& font) { SetFont(font); }
#endif // WXWIN_COMPATIBILITY

#endif
    // _WX_CONTROL_H_
