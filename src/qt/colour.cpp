/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "colour.h"
#endif

#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class wxColourRefData: public wxObjectRefData
{
  public:
  
    wxColourRefData(void);
    ~wxColourRefData(void);
    void FreeColour(void);
  
    bool         m_hasPixel;
    
    friend wxColour;
};

wxColourRefData::wxColourRefData(void)
{
  m_hasPixel = FALSE;
};

wxColourRefData::~wxColourRefData(void)
{
  FreeColour();
};

void wxColourRefData::FreeColour(void)
{
};

//-----------------------------------------------------------------------------

#define M_COLDATA ((wxColourRefData *)m_refData)

#define SHIFT (8*(sizeof(short int)-sizeof(char)))

IMPLEMENT_DYNAMIC_CLASS(wxColour,wxGDIObject)

wxColour::wxColour(void)
{
};

wxColour::wxColour( char WXUNUSED(red), char WXUNUSED(green), char WXUNUSED(blue) )
{
  m_refData = new wxColourRefData();
};
  
wxColour::wxColour( const wxString &colourName )
{
  wxNode *node = NULL;
  if ( (wxTheColourDatabase) && (node = wxTheColourDatabase->Find(colourName)) ) 
  {
    wxColour *col = (wxColour*)node->Data();
    UnRef();
    if (col) Ref( *col );
  } 
  else 
  {
    m_refData = new wxColourRefData();
  };
};

wxColour::wxColour( const wxColour& col )
{ 
  Ref( col ); 
};

wxColour::wxColour( const wxColour* col ) 
{ 
  if (col) Ref( *col ); 
};

wxColour::~wxColour(void)
{
};

wxColour& wxColour::operator = ( const wxColour& col ) 
{ 
  if (*this == col) return (*this); 
  Ref( col ); 
  return *this; 
};

wxColour& wxColour::operator = ( const wxString& colourName ) 
{ 
  UnRef();
  wxNode *node = NULL;
  if ((wxTheColourDatabase) && (node = wxTheColourDatabase->Find(colourName)) ) 
  {
    wxColour *col = (wxColour*)node->Data();
    if (col) Ref( *col );
  } 
  else 
  {
    m_refData = new wxColourRefData();
  };
  return *this; 
};

bool wxColour::operator == ( const wxColour& col ) 
{ 
  return m_refData == col.m_refData; 
};

bool wxColour::operator != ( const wxColour& col) 
{ 
  return m_refData != col.m_refData; 
};

void wxColour::Set( const unsigned char WXUNUSED(red), const unsigned char WXUNUSED(green), 
                    const unsigned char WXUNUSED(blue) )
{
  UnRef();
  m_refData = new wxColourRefData();
};

unsigned char wxColour::Red(void) const
{
  if (!Ok()) return 0;
  return 0;
};

unsigned char wxColour::Green(void) const
{
  if (!Ok()) return 0;
  return 0;
};

unsigned char wxColour::Blue(void) const
{
  if (!Ok()) return 0;
  return 0;
};

bool wxColour::Ok(void) const
{
  return (m_refData);
  return 0;
};

int wxColour::GetPixel(void)
{
  if (!Ok()) return 0;
  return 0;
};

