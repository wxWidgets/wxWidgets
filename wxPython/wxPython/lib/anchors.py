#----------------------------------------------------------------------
# Name:        wxPython.lib.anchors
# Purpose:     A class that provides an easy to use interface over layout
#              constraints for anchored layout.
#
# Author:      Riaan Booysen
#
# Created:     15-Dec-2000
# RCS-ID:      $Id$
# Copyright:   (c) 2000 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

from wxPython.wx import wxLayoutConstraints, wxTop, wxLeft, wxBottom, wxRight, \
                        wxHeight, wxWidth

class LayoutAnchors(wxLayoutConstraints):
    """ A class that implements Delphi's Anchors with wxLayoutConstraints.

        Anchored sides maintain the distance from the edge of the
        control to the same edge of the parent.
        When neither side is selected, the control keeps the same
        relative position to both sides.

        The current position and size of the control and it's parent
        is used when setting up the constraints. To change the size or
        position of an already anchored control, set the constraints to
        None, reposition or resize and reapply the anchors.

        Examples:

        Let's anchor the right and bottom edge of a control and
        resize it's parent.

        ctrl.SetConstraints(LayoutAnchors(ctrl, left=0, top=0, right=1, bottom=1))

        +=========+         +===================+
        | +-----+ |         |                   |
        | |     * |   ->    |                   |
        | +--*--+ |         |           +-----+ |
        +---------+         |           |     * |
                            |           +--*--+ |
                            +-------------------+
        * = anchored edge

        When anchored on both sides the control will stretch horizontally.

        ctrl.SetConstraints(LayoutAnchors(ctrl, 1, 0, 1, 1))

        +=========+         +===================+
        | +-----+ |         |                   |
        | *     * |   ->    |                   |
        | +--*--+ |         | +---------------+ |
        +---------+         | *     ctrl      * |
                            | +-------*-------+ |
                            +-------------------+
        * = anchored edge
    """
    def __init__(self, control, left = 1, top = 1, right = 0, bottom = 0):
        wxLayoutConstraints.__init__(self)
        parent = control.GetParent()
        if not parent: return

        pPos, pSize = parent.GetPosition(), parent.GetClientSize()
        cPos, cSize = control.GetPosition(), control.GetSize()

        self.setConstraintSides(self.left, wxLeft, left,
                                self.right, wxRight, right,
                                self.width, wxWidth, self.centreX,
                                cPos.x, cSize.x, pSize.x, parent)

        self.setConstraintSides(self.top, wxTop, top,
                                self.bottom, wxBottom, bottom,
                                self.height, wxHeight, self.centreY,
                                cPos.y, cSize.y, pSize.y, parent)

    def setConstraintSides(self, side1, side1Edge, side1Anchor,
                                 side2, side2Edge, side2Anchor,
                                 size, sizeEdge, centre,
                                 cPos, cSize, pSize, parent):
        if side2Anchor:
            side2.SameAs(parent, side2Edge, pSize - (cPos + cSize))
        if side1Anchor:
            side1.SameAs(parent, side1Edge, cPos)
            if not side2Anchor:
                size.AsIs()
        else:
            size.AsIs()
            if not side2Anchor:
                centre.PercentOf(parent, sizeEdge,
                                 int(((cPos + cSize / 2.0) / pSize)*100))
