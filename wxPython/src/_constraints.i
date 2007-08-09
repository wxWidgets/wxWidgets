/////////////////////////////////////////////////////////////////////////////
// Name:        _constraints.i
// Purpose:     SWIG interface defs for the layout constraints
//
// Author:      Robin Dunn
//
// Created:     3-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

//---------------------------------------------------------------------------
%newgroup;


enum wxEdge
{
    wxLeft, wxTop, wxRight, wxBottom, wxWidth, wxHeight,
    wxCentre, wxCenter = wxCentre, wxCentreX, wxCentreY
};

enum wxRelationship
{
    wxUnconstrained = 0,
    wxAsIs,
    wxPercentOf,
    wxAbove,
    wxBelow,
    wxLeftOf,
    wxRightOf,
    wxSameAs,
    wxAbsolute
};


DocStr(wxIndividualLayoutConstraint,
"Objects of this class are stored in the `wx.LayoutConstraints` class as
one of eight possible constraints that a window can be involved in.
You will never need to create an instance of
wx.IndividualLayoutConstraint, rather you should create a
`wx.LayoutConstraints` instance and use the individual contstraints
that it contains.", "

Constraints are initially set to have the relationship
wx.Unconstrained, which means that their values should be calculated
by looking at known constraints.

The Edge specifies the type of edge or dimension of a window.

Edges
------
    ==================  ==============================================
    wx.Left             The left edge.
    wx.Top              The top edge.
    wx.Right            The right edge.
    wx.Bottom           The bottom edge.
    wx.CentreX          The x-coordinate of the centre of the window.
    wx.CentreY          The y-coordinate of the centre of the window. 
    ==================  ==============================================


The Relationship specifies the relationship that this edge or
dimension has with another specified edge or dimension. Normally, the
user doesn't use these directly because functions such as Below and
RightOf are a convenience for using the more general Set function.

Relationships
-------------
    ==================  ==============================================
    wx.Unconstrained    The edge or dimension is unconstrained
                        (the default for edges.)
    wx.AsIs             The edge or dimension is to be taken from the current
                        window position or size (the default for dimensions.)
    wx.Above            The edge should be above another edge.
    wx.Below            The edge should be below another edge.
    wx.LeftOf           The edge should be to the left of another edge.
    wx.RightOf          The edge should be to the right of another edge.
    wx.SameAs           The edge or dimension should be the same as another edge
                        or dimension.
    wx.PercentOf        The edge or dimension should be a percentage of another
                        edge or dimension.
    wx.Absolute         The edge or dimension should be a given absolute value.
    ==================  ==============================================

:see: `wx.LayoutConstraints`, `wx.Window.SetConstraints`
");


class wxIndividualLayoutConstraint : public wxObject
{
public:
//    wxIndividualLayoutConstraint();
//    ~wxIndividualLayoutConstraint();

    DocDeclStr(
        void , Set(wxRelationship rel, wxWindow *otherW, wxEdge otherE,
                   int val = 0, int marg = wxLAYOUT_DEFAULT_MARGIN),
        "Sets the properties of the constraint. Normally called by one of the
convenience functions such as Above, RightOf, SameAs.", "");
    

     DocDeclStr(
        void , LeftOf(wxWindow *sibling, int marg = 0),
        "Constrains this edge to be to the left of the given window, with an
optional margin. Implicitly, this is relative to the left edge of the
other window.", "");
    
    DocDeclStr(
        void , RightOf(wxWindow *sibling, int marg = 0),
        "Constrains this edge to be to the right of the given window, with an
optional margin. Implicitly, this is relative to the right edge of the
other window.", "");
    
    DocDeclStr(
        void , Above(wxWindow *sibling, int marg = 0),
        "Constrains this edge to be above the given window, with an optional
margin. Implicitly, this is relative to the top edge of the other
window.", "");
    
    DocDeclStr(
        void , Below(wxWindow *sibling, int marg = 0),
        "Constrains this edge to be below the given window, with an optional
margin. Implicitly, this is relative to the bottom edge of the other
window.", "");

    DocDeclStr(
        void , SameAs(wxWindow *otherW, wxEdge edge, int marg = 0),
        "Constrains this edge or dimension to be to the same as the edge of the
given window, with an optional margin.", "");
    

    DocDeclStr(
        void , PercentOf(wxWindow *otherW, wxEdge wh, int per),
        "Constrains this edge or dimension to be to a percentage of the given
window, with an optional margin.", "");
    

    DocDeclStr(
        void , Absolute(int val),
        "Constrains this edge or dimension to be the given absolute value.", "");

    DocDeclStr(
        void , Unconstrained(),
        "Sets this edge or dimension to be unconstrained, that is, dependent on
other edges and dimensions from which this value can be deduced.", "");
    

    DocDeclStr(
        void , AsIs(),
        "Sets this edge or constraint to be whatever the window's value is at
the moment. If either of the width and height constraints are *as is*,
the window will not be resized, but moved instead. This is important
when considering panel items which are intended to have a default
size, such as a button, which may take its size from the size of the
button label.", "");
    

    DocDeclStr(
        wxWindow *, GetOtherWindow(),
        "", "");
        
    DocDeclStr(
        wxEdge , GetMyEdge() const,
        "", "");
    
    DocDeclStr(
        void , SetEdge(wxEdge which),
        "", "");
    
    DocDeclStr(
        void , SetValue(int v),
        "", "");
    
    DocDeclStr(
        int , GetMargin(),
        "", "");
    
    DocDeclStr(
        void , SetMargin(int m),
        "", "");
    
    DocDeclStr(
        int , GetValue() const,
        "", "");
    
    DocDeclStr(
        int , GetPercent() const,
        "", "");
    
    DocDeclStr(
        int , GetOtherEdge() const,
        "", "");
    
    DocDeclStr(
        bool , GetDone() const,
        "", "");
    
    DocDeclStr(
        void , SetDone(bool d),
        "", "");
    
    DocDeclStr(
        wxRelationship , GetRelationship(),
        "", "");
    
    DocDeclStr(
        void , SetRelationship(wxRelationship r),
        "", "");
    

    DocDeclStr(
        bool , ResetIfWin(wxWindow *otherW),
        "Reset constraint if it mentions otherWin", "");
    

    DocDeclStr(
        bool , SatisfyConstraint(wxLayoutConstraints *constraints, wxWindow *win),
        "Try to satisfy constraint", "");
    

    DocDeclStr(
        int , GetEdge(wxEdge which, wxWindow *thisWin, wxWindow *other) const,
        "Get the value of this edge or dimension, or if this\n"
        "is not determinable, -1.", "");

    %property(Done, GetDone, SetDone, doc="See `GetDone` and `SetDone`");
    %property(Margin, GetMargin, SetMargin, doc="See `GetMargin` and `SetMargin`");
    %property(MyEdge, GetMyEdge, doc="See `GetMyEdge`");
    %property(OtherEdge, GetOtherEdge, doc="See `GetOtherEdge`");
    %property(OtherWindow, GetOtherWindow, doc="See `GetOtherWindow`");
    %property(Percent, GetPercent, doc="See `GetPercent`");
    %property(Relationship, GetRelationship, SetRelationship, doc="See `GetRelationship` and `SetRelationship`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
    
};


DocStr(wxLayoutConstraints,
"**Note:** constraints are now deprecated and you should use sizers
instead.

Objects of this class can be associated with a window to define its
layout constraints, with respect to siblings or its parent.

The class consists of the following eight constraints of class
wx.IndividualLayoutConstraint, some or all of which should be accessed
directly to set the appropriate constraints.

    * left: represents the left hand edge of the window
    * right: represents the right hand edge of the window
    * top: represents the top edge of the window
    * bottom: represents the bottom edge of the window
    * width: represents the width of the window
    * height: represents the height of the window
    * centreX: represents the horizontal centre point of the window
    * centreY: represents the vertical centre point of the window 

Most constraints are initially set to have the relationship
wxUnconstrained, which means that their values should be calculated by
looking at known constraints. The exceptions are width and height,
which are set to wxAsIs to ensure that if the user does not specify a
constraint, the existing width and height will be used, to be
compatible with panel items which often have take a default size. If
the constraint is ``wx.AsIs``, the dimension will not be changed.

:see: `wx.IndividualLayoutConstraint`, `wx.Window.SetConstraints`
", "");
       

class wxLayoutConstraints : public wxObject
{
public:
    %immutable;

    // Edge constraints
    wxIndividualLayoutConstraint left;
    wxIndividualLayoutConstraint top;
    wxIndividualLayoutConstraint right;
    wxIndividualLayoutConstraint bottom;

    // Size constraints
    wxIndividualLayoutConstraint width;
    wxIndividualLayoutConstraint height;
    
    // Centre constraints
    wxIndividualLayoutConstraint centreX;
    wxIndividualLayoutConstraint centreY;

    %mutable;

    DocCtorStr(
        wxLayoutConstraints(),
        "", "");

    ~wxLayoutConstraints();
    
    DocDeclA(
        bool, SatisfyConstraints(wxWindow *win, int *OUTPUT),
        "SatisfyConstraints(Window win) -> (areSatisfied, noChanges)");

    bool AreSatisfied() const;
};

//---------------------------------------------------------------------------
