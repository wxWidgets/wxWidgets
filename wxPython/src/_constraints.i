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


// wxIndividualLayoutConstraint: a constraint on window position
class wxIndividualLayoutConstraint : public wxObject
{
public:
//    wxIndividualLayoutConstraint();
//    ~wxIndividualLayoutConstraint();

    void Set(wxRelationship rel, wxWindow *otherW, wxEdge otherE, int val = 0, int marg = wxLAYOUT_DEFAULT_MARGIN);

    //
    // Sibling relationships
    //
    void LeftOf(wxWindow *sibling, int marg = 0);
    void RightOf(wxWindow *sibling, int marg = 0);
    void Above(wxWindow *sibling, int marg = 0);
    void Below(wxWindow *sibling, int marg = 0);

    //
    // 'Same edge' alignment
    //
    void SameAs(wxWindow *otherW, wxEdge edge, int marg = 0);

    // The edge is a percentage of the other window's edge
    void PercentOf(wxWindow *otherW, wxEdge wh, int per);

    //
    // Edge has absolute value
    //
    void Absolute(int val);

    //
    // Dimension is unconstrained
    //
    void Unconstrained() { relationship = wxUnconstrained; }

    //
    // Dimension is 'as is' (use current size settings)
    //
    void AsIs() { relationship = wxAsIs; }

    //
    // Accessors
    //
    wxWindow *GetOtherWindow();
    wxEdge GetMyEdge() const;
    void SetEdge(wxEdge which);
    void SetValue(int v);
    int GetMargin();
    void SetMargin(int m);
    int GetValue() const;
    int GetPercent() const;
    int GetOtherEdge() const;
    bool GetDone() const;
    void SetDone(bool d);
    wxRelationship GetRelationship();
    void SetRelationship(wxRelationship r);

    // Reset constraint if it mentions otherWin
    bool ResetIfWin(wxWindow *otherW);

    // Try to satisfy constraint
    bool SatisfyConstraint(wxLayoutConstraints *constraints, wxWindow *win);

    // Get the value of this edge or dimension, or if this
    // is not determinable, -1.
    int GetEdge(wxEdge which, wxWindow *thisWin, wxWindow *other) const;

};


// wxLayoutConstraints: the complete set of constraints for a window
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
    
    wxLayoutConstraints();

    
    DocDeclA(
        bool, SatisfyConstraints(wxWindow *win, int *OUTPUT),
        "SatisfyConstraints(Window win) -> (areSatisfied, noChanges)");

    bool AreSatisfied() const;
};

//---------------------------------------------------------------------------
