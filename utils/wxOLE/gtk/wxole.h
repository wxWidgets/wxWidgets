/////////////////////////////////////////////////////////////////////////////
// Name:        wxole.h
// Purpose:     wxOLE
// Author:      Robert Roebling
// Modified by:
// Created:     17/8/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "wxole.h"
#endif

#ifndef _WX_OLE_H_
#define _WX_OLE_H_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/frame.h"
#include "wx/stream.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar *wxOleNameStr;

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------

class wxOleServerEnv;
class wxOleServer;
class wxOleControl;

//---------------------------------------------------------------------------
// wxOleServerEnv
//---------------------------------------------------------------------------

class wxOleServerEnvPrivate;

class wxOleServerEnv : public wxObject
{
  DECLARE_CLASS(wxOleServerEnv)
   
public:
  
  wxOleServerEnv( const wxString &name, const wxString &version );
  ~wxOleServerEnv();

private:

  wxString                 m_serverName;
  wxString                 m_serverVersion;
  
protected:
  wxOleServerEnvPrivate   *m_priv;      
};

//---------------------------------------------------------------------------
// wxOleServer
//---------------------------------------------------------------------------

class wxOleServerPrivate;

class wxOleServer : public wxObject
{
  DECLARE_CLASS(wxOleServer)
   
public:
  
  wxOleServer( const wxString &id );
  ~wxOleServer();
  
  virtual wxOleControl *CreateOleControl();
  
private:

  wxString                 m_ID;
  
protected:
  wxOleServerPrivate      *m_priv;
};

//---------------------------------------------------------------------------
// wxOleControl
//---------------------------------------------------------------------------

class wxOleControl : public wxFrame
{
  DECLARE_CLASS(wxOleControl)
  
public:
  
    wxOleControl( wxWindowID id, long style = 0, const wxString &name = wxOleNameStr );
    ~wxOleControl();
    bool Create( wxWindowID id, long style = 0, const wxString &name = wxOleNameStr );

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    virtual void DoSetClientSize(int width, int height);
};

#endif
