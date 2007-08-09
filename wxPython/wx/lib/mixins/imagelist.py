#----------------------------------------------------------------------------
# Name:        wx.lib.mixins.imagelist
# Purpose:     Helpful mix-in classes for using a wxImageList
#
# Author:      Robin Dunn
#
# Created:     15-May-2001
# RCS-ID:      $Id$
# Copyright:   (c) 2001 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------------
# 12/14/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o Untested.
#

import  wx

#----------------------------------------------------------------------------

class MagicImageList:
    '''
    Mix-in to provide "magic" growing image lists
    By Mike Fletcher
    '''

    ### LAZYTREE and LISTCONTROL Methods
    DEFAULTICONSIZE = 16

    def SetupIcons(self, images=(), size=None):
        self.__size = size or self.DEFAULTICONSIZE
        self.__magicImageList = wx.ImageList (self.__size,self.__size)
        self.__magicImageListMapping = {}
        self.SetImageList (
            self.__magicImageList, {
                16:wx.IMAGE_LIST_SMALL,
                32:wx.IMAGE_LIST_NORMAL,
            }[self.__size]
        )
        for image in images:
            self.AddIcon (image)

    def GetIcons (self, node):
        '''Get icon indexes for a given node, or None if no associated icon'''
        icon = self.GetIcon( node )
        if icon:
            index = self.AddIcon (icon)
            return index, index
        return None


    ### Local methods...
    def AddIcon(self, icon, mask = wx.NullBitmap):
        '''Add an icon to the image list, or get the index if already there'''
        index = self.__magicImageListMapping.get (id (icon))
        if index is None:
            if isinstance( icon, wxIconPtr ):
                index = self.__magicImageList.AddIcon( icon )
            elif isinstance( icon, wx.BitmapPtr ):
                if isinstance( mask, wx.Colour ):
                    index = self.__magicImageList.AddWithColourMask( icon, mask )
                else:
                    index = self.__magicImageList.Add( icon, mask )
            else:
                raise ValueError("Unexpected icon object %s, "
                                 "expected wx.Icon or wx.Bitmap" % (icon))
            self.__magicImageListMapping [id (icon)] = index
        return index

    ### Customisation point...
    def GetIcon( self, node ):
        '''Get the actual icon object for a node'''
        if hasattr (node,"DIAGRAMICON"):
            return node.DIAGRAMICON



