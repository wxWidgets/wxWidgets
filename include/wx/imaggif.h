/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.h
// Purpose:     wxImage handler for GIFs (read-only)
// Author:      Vaclav Slavik (of this header only)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGGIF_H_
#define _WX_IMAGGIF_H_

#ifdef __GNUG__
#pragma interface "imaggif.h"
#endif

#include <wx/setup.h>
#include <wx/object.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/stream.h>
#include <wx/image.h>

//-----------------------------------------------------------------------------
// wxGIFHandler
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxGIFHandler : public wxImageHandler
{
  DECLARE_DYNAMIC_CLASS(wxGIFHandler)

public:

  inline wxGIFHandler()
  {
      m_name = "GIF file";
      m_extension = "gif";
      m_type = wxBITMAP_TYPE_GIF;
  };

  virtual bool LoadFile( wxImage *image, wxInputStream& stream );
  virtual bool SaveFile( wxImage *image, wxOutputStream& stream );
};

#endif
    // _WX_IMAGGIF_H_

