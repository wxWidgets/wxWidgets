/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Purpose:     interface of wxIndividualLayoutConstraint
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxIndividualLayoutConstraint
    @wxheader{layout.h}

    Objects of this class are stored in the wxLayoutConstraint class
    as one of eight possible constraints that a window can be involved in.

    Constraints are initially set to have the relationship wxUnconstrained,
    which means that their values should be calculated by looking at known
    constraints.

    @library{wxcore}
    @category{winlayout}

    @see @ref overview_constraintsoverview "Overview and examples",
    wxLayoutConstraints, wxWindow::SetConstraints.
*/
class wxIndividualLayoutConstraint : public wxObject
{
public:
    /**
        Constructor. Not used by the end-user.
    */
    wxIndividualLayoutConstraint();

    /**
        Constrains this edge to be above the given window, with an
        optional margin. Implicitly, this is relative to the top edge of the other
        window.
    */
    void Above(wxWindow* otherWin, int margin = 0);

    /**
        Constrains this edge or dimension to be the given absolute value.
    */
    void Absolute(int value);

    /**
        Sets this edge or constraint to be whatever the window's value is
        at the moment. If either of the width and height constraints
        are @e as is, the window will not be resized, but moved instead.
        This is important when considering panel items which are intended
        to have a default size, such as a button, which may take its size
        from the size of the button label.
    */
    void AsIs();

    /**
        Constrains this edge to be below the given window, with an
        optional margin. Implicitly, this is relative to the bottom edge of the other
        window.
    */
    void Below(wxWindow* otherWin, int margin = 0);

    /**
        The @e wxEdge enumerated type specifies the type of edge or dimension of a
        window.
        
        wxLeft
        
        The left edge.
        
        wxTop
        
        The top edge.
        
        wxRight
        
        The right edge.
        
        wxBottom
        
        The bottom edge.
        
        wxCentreX
        
        The x-coordinate of the centre of the window.
        
        wxCentreY
        
        The y-coordinate of the centre of the window.
        
        The @e wxRelationship enumerated type specifies the relationship that
        this edge or dimension has with another specified edge or dimension. Normally,
        the user
        doesn't use these directly because functions such as @e Below and @e RightOf
        are a convenience
        for using the more general @e Set function.
        
        wxUnconstrained
        
        The edge or dimension is unconstrained (the default for edges.
        
        wxAsIs
        
        The edge or dimension is to be taken from the current window position or size
        (the
        default for dimensions.
        
        wxAbove
        
        The edge should be above another edge.
        
        wxBelow
        
        The edge should be below another edge.
        
        wxLeftOf
        
        The edge should be to the left of another edge.
        
        wxRightOf
        
        The edge should be to the right of another edge.
        
        wxSameAs
        
        The edge or dimension should be the same as another edge or dimension.
        
        wxPercentOf
        
        The edge or dimension should be a percentage of another edge or dimension.
        
        wxAbsolute
        
        The edge or dimension should be a given absolute value.
    */


    /**
        Constrains this edge to be to the left of the given window, with an
        optional margin. Implicitly, this is relative to the left edge of the other
        window.
    */
    void LeftOf(wxWindow* otherWin, int margin = 0);

    /**
        Constrains this edge or dimension to be to a percentage of the given window,
        with an
        optional margin.
    */
    void PercentOf(wxWindow* otherWin, wxEdge edge, int per);

    /**
        Constrains this edge to be to the right of the given window, with an
        optional margin. Implicitly, this is relative to the right edge of the other
        window.
    */
    void RightOf(wxWindow* otherWin, int margin = 0);

    /**
        Constrains this edge or dimension to be to the same as the edge of the given
        window, with an
        optional margin.
    */
    void SameAs(wxWindow* otherWin, wxEdge edge, int margin = 0);

    /**
        Sets the properties of the constraint. Normally called by one of the convenience
        functions such as Above, RightOf, SameAs.
    */
    void Set(wxRelationship rel, wxWindow* otherWin,
             wxEdge otherEdge, int value = 0,
             int margin = 0);

    /**
        Sets this edge or dimension to be unconstrained, that is, dependent on
        other edges and dimensions from which this value can be deduced.
    */
    void Unconstrained();
};



/**
    @class wxLayoutConstraints
    @wxheader{layout.h}

    @b Note: constraints are now deprecated and you should use sizers() instead.

    Objects of this class can be associated with a window to define its
    layout constraints, with respect to siblings or its parent.

    The class consists of the following eight constraints of class
    wxIndividualLayoutConstraint,
    some or all of which should be accessed directly to set the appropriate
    constraints.

     @b left: represents the left hand edge of the window
     @b right: represents the right hand edge of the window
     @b top: represents the top edge of the window
     @b bottom: represents the bottom edge of the window
     @b width: represents the width of the window
     @b height: represents the height of the window
     @b centreX: represents the horizontal centre point of the window
     @b centreY: represents the vertical centre point of the window

    Most constraints are initially set to have the relationship wxUnconstrained,
    which means that their values should be calculated by looking at known
    constraints.
    The exceptions are @e width and @e height, which are set to wxAsIs to
    ensure that if the user does not specify a constraint, the existing
    width and height will be used, to be compatible with panel items which often
    have take a default size. If the constraint is wxAsIs, the dimension will
    not be changed.

    @b wxPerl note: In wxPerl the constraints are accessed as

    @code
    constraint = Wx::LayoutConstraints-new();
      constraint-centreX-AsIs();
      constraint-centreY-Unconstrained();
    @endcode


    @library{wxcore}
    @category{winlayout}

    @see @ref overview_constraintsoverview "Overview and examples",
    wxIndividualLayoutConstraint, wxWindow::SetConstraints
*/
class wxLayoutConstraints : public wxObject
{
public:
    /**
        Constructor.
    */
    wxLayoutConstraints();

    /**
        wxIndividualLayoutConstraint bottom
        Constraint for the bottom edge.
    */


    /**
        wxIndividualLayoutConstraint centreX
        Constraint for the horizontal centre point.
    */


    /**
        wxIndividualLayoutConstraint centreY
        Constraint for the vertical centre point.
    */


    /**
        wxIndividualLayoutConstraint height
        Constraint for the height.
    */


    /**
        wxIndividualLayoutConstraint left
        Constraint for the left-hand edge.
    */


    /**
        wxIndividualLayoutConstraint right
        Constraint for the right-hand edge.
    */


    /**
        wxIndividualLayoutConstraint top
        Constraint for the top edge.
    */


    /**
        wxIndividualLayoutConstraint width
        Constraint for the width.
    */
};

