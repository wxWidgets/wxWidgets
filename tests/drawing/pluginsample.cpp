///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/pluginsample.cpp
// Purpose:     Sample plugin for the wxGraphicsContext test
// Author:      Armel Asselin
// Created:     2014-02-21
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "plugin.h"

#if wxUSE_TEST_GC_DRAWING

// ----------------------------------------------------------------------------
// plugin implementation
// ----------------------------------------------------------------------------

class SampleDrawingTestGCFactory: public DrawingTestGCFactory {
public:
    SampleDrawingTestGCFactory() {
        wxImage::AddHandler (new wxBMPHandler());
    }

    virtual ~SampleDrawingTestGCFactory() {
    }
    virtual wxString GetIdForFileName () const wxOVERRIDE { return "sample-plg"; }
    virtual wxString GetExtensionForFileName () const wxOVERRIDE { return "bmp"; }

    // Bitmaps are handled by wx code they should be binarily equal
    virtual bool UseImageComparison() const wxOVERRIDE { return false; }

    // We use wxGraphicsContext, its implementation is not platform independent
    // and returns may slightly vary
    virtual bool PlatformIndependent() const wxOVERRIDE { return false; }

    virtual wxGraphicsContext *BuildNewContext (wxSize expectedSize,
        double WXUNUSED(pointsPerInch), const wxFileName &targetFileName) wxOVERRIDE {
        m_image = new wxImage (expectedSize);
        m_image->InitAlpha();

        m_targetFileName = targetFileName.GetFullPath();

        // we should probably pass the number of points per inches somewhere...
        //  but I don't see where yet...
        return wxGraphicsContext::Create(*m_image);
    }

    // Let's the opportunity to actually save the context and associated data
    // If saving requires deleting the wxGraphicsContext object the
    //  implementer is free to do it but @c gc must then be nulled
    virtual void SaveBuiltContext (wxGraphicsContext *&gc) wxOVERRIDE {
        wxDELETE(gc);

        m_image->SaveFile (m_targetFileName);
    }

    // Cleans @c gc and internal data if any
    virtual void CleanUp (wxGraphicsContext *gc) wxOVERRIDE {
        delete gc;
        m_targetFileName.Empty();
        wxDELETE(m_image);
    }

    wxImage *m_image;
    wxString m_targetFileName;
};

extern "C" WXEXPORT DrawingTestGCFactory * CreateDrawingTestLifeCycle()
{
    return new SampleDrawingTestGCFactory;
}

extern "C" WXEXPORT void DestroyDrawingTestLifeCycle (DrawingTestGCFactory* lc)
{
    delete lc;
}

#endif // wxUSE_TEST_GC_DRAWING
