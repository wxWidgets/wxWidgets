/////////////////////////////////////////////////////////////////////////////
// Name:        docmdi.h
// Purpose:     documentation for wxDocMDIParentFrame class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxDocMDIParentFrame
    @wxheader{docmdi.h}
    
    The wxDocMDIParentFrame class provides a default top-level frame for
    applications using the document/view framework. This class can only be used for
    MDI parent frames.
    
    It cooperates with the wxView, wxDocument,
    wxDocManager and wxDocTemplates classes.
    
    See the example application in @c samples/docview.
    
    @library{wxcore}
    @category{FIXME}
    
    @seealso
    @ref overview_docviewoverview "Document/view overview", wxMDIParentFrame
*/
class wxDocMDIParentFrame : public wxMDIParentFrame
{
public:
    //@{
    /**
        Constructor.
    */
    wxDocMDIParentFrame();
        wxDocMDIParentFrame(wxDocManager* manager, wxFrame * parent,
                            wxWindowID id,
                            const wxString& title,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxDEFAULT_FRAME_STYLE,
                            const wxString& name = "frame");
    //@}

    /**
        Destructor.
    */
    ~wxDocMDIParentFrame();

    /**
        Creates the window.
    */
    bool Create(wxDocManager* manager, wxFrame * parent,
                wxWindowID id, const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = "frame");

    /**
        Deletes all views and documents. If no user input cancelled the
        operation, the frame will be destroyed and the application will exit.
        
        Since understanding how document/view clean-up takes place can be difficult,
        the implementation of this function is shown below.
    */
    void OnCloseWindow(wxCloseEvent& event);
};


/**
    @class wxDocMDIChildFrame
    @wxheader{docmdi.h}
    
    The wxDocMDIChildFrame class provides a default frame for displaying documents
    on separate windows. This class can only be used for MDI child frames.
    
    The class is part of the document/view framework supported by wxWidgets,
    and cooperates with the wxView, wxDocument,
    wxDocManager and wxDocTemplate classes.
    
    See the example application in @c samples/docview.
    
    @library{wxcore}
    @category{FIXME}
    
    @seealso
    @ref overview_docviewoverview "Document/view overview", wxMDIChildFrame
*/
class wxDocMDIChildFrame : public wxMDIChildFrame
{
public:
    /**
        Constructor.
    */
    wxDocMDIChildFrame(wxDocument* doc, wxView* view,
                       wxFrame* parent,
                       wxWindowID id,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxDEFAULT_FRAME_STYLE,
                       const wxString& name = "frame");

    /**
        Destructor.
    */
    ~wxDocMDIChildFrame();

    /**
        Returns the document associated with this frame.
    */
    wxDocument* GetDocument();

    /**
        Returns the view associated with this frame.
    */
    wxView* GetView();

    /**
        Sets the currently active view to be the frame's view. You may need
        to override (but still call) this function in order to set the keyboard
        focus for your subwindow.
    */
    void OnActivate(wxActivateEvent event);

    /**
        Closes and deletes the current view and document.
    */
    void OnCloseWindow(wxCloseEvent& event);

    /**
        Sets the document for this frame.
    */
    void SetDocument(wxDocument * doc);

    /**
        Sets the view for this frame.
    */
    void SetView(wxView * view);

    /**
        wxDocument* m_childDocument
        
        The document associated with the frame.
    */


    /**
        wxView* m_childView
        
        The view associated with the frame.
    */
};
