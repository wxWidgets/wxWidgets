///////////////////////////////////////////////////////////////////////////////
// Name:        motif/dataform.h
// Purpose:     declaration of the wxDataFormat class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.10.99 (extracted from motif/dataobj.h)
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOTIF_DATAFORM_H
#define _WX_MOTIF_DATAFORM_H

class wxDataFormat
{
public:
    wxDataFormat();
    wxDataFormat( wxDataFormatId type );
    wxDataFormat( const wxString &id );
    wxDataFormat( const wxChar *id );
    wxDataFormat( const wxDataFormat &format );
    wxDataFormat( const Atom atom );

    void SetType( wxDataFormatId type );
    wxDataFormatId GetType() const;

    /* the string Id identifies the format of clipboard or DnD data. a word
     * processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
     * to the clipboard - the latter with the Id "application/wxword", an
     * image manipulation program would put a wxBitmapDataObject and a
     * wxPrivateDataObject to the clipboard - the latter with "image/png". */

    wxString GetId() const;
    void SetId( const wxChar *id );

    Atom GetAtom();
    void SetAtom(Atom atom) { m_hasAtom = TRUE; m_atom = atom; }

    // implicit conversion to wxDataFormatId
    operator wxDataFormatId() const { return m_type; }

    bool operator==(wxDataFormatId type) const { return m_type == type; }
    bool operator!=(wxDataFormatId type) const { return m_type != type; }

private:
    wxDataFormatId  m_type;
    wxString    m_id;
    bool        m_hasAtom;
    Atom        m_atom;
};

#endif // _WX_MOTIF_DATAFORM_H

