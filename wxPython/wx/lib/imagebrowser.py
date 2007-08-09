#----------------------------------------------------------------------------
# Name:         BrowseImage.py
# Purpose:      Display and Select Image Files
#
# Original Author:       Lorne White
#
# Version:      2.0
# Date:         June 16, 2007
# Licence:      wxWindows license
#----------------------------------------------------------------------------
# 2.0 Release - Bill Baxter (wbaxter@gmail.com)
# Date:         June 16, 2007
# o Changed to use sizers instead of fixed placement.
# o Made dialog resizeable
# o Added a splitter between file list and view pane
# o Made directory path editable
# o Added an "up" button" to go to the parent dir
# o Changed to show directories in file list
# o Don't select images on double click any more
# o Added a 'broken image' display for files that wx fails to identify
# o Redesigned appearance -- using bitmap buttons now, and rearranged things
# o Fixed display of masked gifs
# o Fixed zooming logic to show shrunken images at correct aspect ratio
# o Added different background modes for preview (white/grey/dark/checkered)
# o Added framing modes for preview (no frame/box frame/tinted border)
# 
#----------------------------------------------------------------------------
#
# 12/08/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o Corrected a nasty bug or two - see comments below.
# o There was a duplicate ImageView.DrawImage() method. Que?
# 
#----------------------------------------------------------------------------
# 1.0 Release - Lorne White
# Date:         January 29, 2002
# Create list of all available image file types
# View "All Image" File Types as default filter
# Sort the file list
# Use newer "re" function for patterns
#

#---------------------------------------------------------------------------

import  os
import  sys
import  wx

#---------------------------------------------------------------------------

BAD_IMAGE = -1
ID_WHITE_BG = wx.NewId()
ID_BLACK_BG = wx.NewId()
ID_GREY_BG = wx.NewId()
ID_CHECK_BG = wx.NewId()
ID_NO_FRAME = wx.NewId()
ID_BOX_FRAME = wx.NewId()
ID_CROP_FRAME = wx.NewId()

def ConvertBMP(file_nm):
    if file_nm is None:
        return None

    fl_fld = os.path.splitext(file_nm)
    ext = fl_fld[1]
    ext = ext[1:].lower()

    # Don't try to create it directly because wx throws up
    # an annoying messasge dialog if the type isn't supported.
    if wx.Image.CanRead(file_nm):
        image = wx.Image(file_nm, wx.BITMAP_TYPE_ANY)
        return image

    # BAD_IMAGE means a bad image, None just means no image (i.e. directory)
    return BAD_IMAGE


def GetCheckeredBitmap(blocksize=8,ntiles=4,rgb0='\xFF', rgb1='\xCC'):
    """Creates a square RGB checkered bitmap using the two specified colors.

    Inputs:
    - blocksize:  the number of pixels in each solid color square
    - ntiles:  the number of tiles along width and height.  Each tile is 2x2 blocks.
    - rbg0,rgb1:  the first and second colors, as 3-byte strings.
                  If only 1 byte is provided, it is treated as a grey value.

    The bitmap returned will have width = height = blocksize*ntiles*2
    """
    size = blocksize*ntiles*2

    if len(rgb0)==1:
        rgb0 = rgb0 * 3
    if len(rgb1)==1:
        rgb1 = rgb1 * 3

    strip0 = (rgb0*blocksize + rgb1*blocksize)*(ntiles*blocksize)
    strip1 = (rgb1*blocksize + rgb0*blocksize)*(ntiles*blocksize)
    band = strip0 + strip1
    data = band * ntiles
    return wx.BitmapFromBuffer(size, size, data)

def GetNamedBitmap(name):
    return IMG_CATALOG[name].getBitmap()


class ImageView(wx.Window):
    def __init__(self, parent, id=-1, pos=wx.DefaultPosition, size=wx.DefaultSize, 
                 style=wx.BORDER_SUNKEN
                 ):
        wx.Window.__init__(self, parent, id, pos, size, style=style)
        
        self.image = None

        self.check_bmp = None
        self.check_dim_bmp = None

        # dark_bg is the brush/bitmap to use for painting in the whole background
        # lite_bg is the brush/bitmap/pen to use for painting the image rectangle
        self.dark_bg = None
        self.lite_bg = None

        self.border_mode = ID_CROP_FRAME
        self.SetBackgroundMode( ID_WHITE_BG )
        self.SetBorderMode( ID_NO_FRAME )

        # Changed API of wx uses tuples for size and pos now.
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_SIZE, self.OnSize)


    def SetValue(self, file_nm):    # display the selected file in the panel
        image = ConvertBMP(file_nm)
        self.image = image
        self.Refresh()
    
    def SetBackgroundMode(self, mode):
        self.bg_mode = mode
        self._updateBGInfo()

    def _updateBGInfo(self):
        bg = self.bg_mode
        border = self.border_mode

        self.dark_bg = None
        self.lite_bg = None

        if border == ID_BOX_FRAME:
            self.lite_bg = wx.BLACK_PEN

        if bg == ID_WHITE_BG:
            if border == ID_CROP_FRAME:
                self.SetBackgroundColour('LIGHT GREY')
                self.lite_bg = wx.WHITE_BRUSH
            else:
                self.SetBackgroundColour('WHITE')

        elif bg == ID_GREY_BG:
            if border == ID_CROP_FRAME:
                self.SetBackgroundColour('GREY')
                self.lite_bg = wx.LIGHT_GREY_BRUSH
            else:
                self.SetBackgroundColour('LIGHT GREY')

        elif bg == ID_BLACK_BG:
            if border == ID_BOX_FRAME:
                self.lite_bg = wx.WHITE_PEN
            if border == ID_CROP_FRAME:
                self.SetBackgroundColour('GREY')
                self.lite_bg = wx.BLACK_BRUSH
            else:
                self.SetBackgroundColour('BLACK')

        else:
            if self.check_bmp is None:
                self.check_bmp = GetCheckeredBitmap()
                self.check_dim_bmp = GetCheckeredBitmap(rgb0='\x7F', rgb1='\x66')
            if border == ID_CROP_FRAME:
                self.dark_bg = self.check_dim_bmp
                self.lite_bg = self.check_bmp
            else:
                self.dark_bg = self.check_bmp

        self.Refresh()

    def SetBorderMode(self, mode):
        self.border_mode = mode
        self._updateBGInfo()

    def OnSize(self, event):
        event.Skip()
        self.Refresh()

    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.DrawImage(dc)

    def OnEraseBackground(self, evt):
        if self.bg_mode != ID_CHECK_BG:
            evt.Skip()
            return
        dc = evt.GetDC()
        if not dc:
            dc = wx.ClientDC(self)
            rect = self.GetUpdateRegion().GetBox()
            dc.SetClippingRect(rect)
        self.PaintBackground(dc, self.dark_bg)

    def PaintBackground(self, dc, painter, rect=None):
        if painter is None:
            return
        if rect is None:
            pos = self.GetPosition()
            sz = self.GetSize()
        else:
            pos = rect.Position
            sz = rect.Size

        if type(painter)==wx.Brush:
            dc.SetPen(wx.TRANSPARENT_PEN)
            dc.SetBrush(painter)
            dc.DrawRectangle(pos.x,pos.y,sz.width,sz.height)
        elif type(painter)==wx.Pen:
            dc.SetPen(painter)
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            dc.DrawRectangle(pos.x-1,pos.y-1,sz.width+2,sz.height+2)
        else:
            self.TileBackground(dc, painter, pos.x,pos.y,sz.width,sz.height)
        

    def TileBackground(self, dc, bmp, x,y,w,h):
        """Tile bmp into the specified rectangle"""
        bw = bmp.GetWidth()
        bh = bmp.GetHeight()

        dc.SetClippingRegion(x,y,w,h)

        # adjust so 0,0 so we always match with a tiling starting at 0,0
        dx = x % bw
        x = x - dx
        w = w + dx

        dy = y % bh
        y = y - dy
        h = h + dy

        tx = x
        x2 = x+w
        y2 = y+h

        while tx < x2:
            ty = y
            while ty < y2:
                dc.DrawBitmap(bmp, tx, ty)
                ty += bh
            tx += bw

    def DrawImage(self, dc):

        if not hasattr(self,'image') or self.image is None:
            return

        wwidth,wheight = self.GetSize() 
        image = self.image
        bmp = None
        if image != BAD_IMAGE and image.IsOk():
            iwidth = image.GetWidth()   # dimensions of image file
            iheight = image.GetHeight()
        else:
            bmp = wx.ArtProvider.GetBitmap(wx.ART_MISSING_IMAGE, wx.ART_MESSAGE_BOX, (64,64))
            iwidth = bmp.GetWidth()
            iheight = bmp.GetHeight()

        # squeeze iwidth x iheight image into window, preserving aspect ratio

        xfactor = float(wwidth) / iwidth
        yfactor = float(wheight) / iheight

        if xfactor < 1.0 and xfactor < yfactor:
            scale = xfactor
        elif yfactor < 1.0 and yfactor < xfactor:
            scale = yfactor
        else:
            scale = 1.0

        owidth = int(scale*iwidth)
        oheight = int(scale*iheight)

        diffx = (wwidth - owidth)/2   # center calc
        diffy = (wheight - oheight)/2   # center calc

        if not bmp:
            if owidth!=iwidth or oheight!=iheight:
                sc_image = sc_image = image.Scale(owidth,oheight)
            else:
                sc_image = image
            bmp = sc_image.ConvertToBitmap()

        if image != BAD_IMAGE and image.IsOk():
            self.PaintBackground(dc, self.lite_bg, wx.Rect(diffx,diffy,owidth,oheight))

        dc.DrawBitmap(bmp, diffx, diffy, useMask=True)        # draw the image to window


class ImagePanel(wx.Panel):
    def __init__(self, parent, id=-1, pos=wx.DefaultPosition, size=wx.DefaultSize, 
                 style=wx.NO_BORDER
                 ):
        wx.Panel.__init__(self, parent, id, pos, size, style=style)

        vbox = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(vbox)

        self.view = ImageView(self)
        vbox.Add(self.view, 1, wx.GROW|wx.ALL, 0)

        hbox_ctrls = wx.BoxSizer(wx.HORIZONTAL)
        vbox.Add(hbox_ctrls, 0, wx.ALIGN_RIGHT|wx.TOP, 4)

        bmp = GetNamedBitmap('White')
        btn = wx.BitmapButton(self, ID_WHITE_BG, bmp, style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSetImgBackground, btn)
        btn.SetToolTipString("Set background to white")
        hbox_ctrls.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT, 4)

        bmp = GetNamedBitmap('Grey')
        btn = wx.BitmapButton(self, ID_GREY_BG, bmp, style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSetImgBackground, btn)
        btn.SetToolTipString("Set background to grey")
        hbox_ctrls.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT, 4)

        bmp = GetNamedBitmap('Black')
        btn = wx.BitmapButton(self, ID_BLACK_BG, bmp, style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSetImgBackground, btn)
        btn.SetToolTipString("Set background to black")
        hbox_ctrls.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT, 4)

        bmp = GetNamedBitmap('Checked')
        btn = wx.BitmapButton(self, ID_CHECK_BG, bmp, style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSetImgBackground, btn)
        btn.SetToolTipString("Set background to chekered pattern")
        hbox_ctrls.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT, 4)


        hbox_ctrls.AddSpacer(7)

        bmp = GetNamedBitmap('NoFrame')
        btn = wx.BitmapButton(self, ID_NO_FRAME, bmp, style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSetBorderMode, btn)
        btn.SetToolTipString("No framing around image")
        hbox_ctrls.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT, 4)
        
        bmp = GetNamedBitmap('BoxFrame')
        btn = wx.BitmapButton(self, ID_BOX_FRAME, bmp, style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSetBorderMode, btn)
        btn.SetToolTipString("Frame image with a box")
        hbox_ctrls.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT, 4)

        bmp = GetNamedBitmap('CropFrame')
        btn = wx.BitmapButton(self, ID_CROP_FRAME, bmp, style=wx.BU_EXACTFIT|wx.BORDER_SIMPLE)
        self.Bind(wx.EVT_BUTTON, self.OnSetBorderMode, btn)
        btn.SetToolTipString("Frame image with a dimmed background")
        hbox_ctrls.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT, 4)


    def SetValue(self, file_nm):    # display the selected file in the panel
        self.view.SetValue(file_nm)

    def SetBackgroundMode(self, mode):
        self.view.SetBackgroundMode(mode)

    def SetBorderMode(self, mode):
        self.view.SetBorderMode(mode)

    def OnSetImgBackground(self, event):
        mode = event.GetId()
        self.SetBackgroundMode(mode)

    def OnSetBorderMode(self, event):
        mode = event.GetId()
        self.SetBorderMode(mode)



class ImageDialog(wx.Dialog):
    def __init__(self, parent, set_dir = None):
        wx.Dialog.__init__(self, parent, -1, "Image Browser", wx.DefaultPosition, (400, 400),style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)

        self.set_dir = os.getcwd()
        self.set_file = None

        if set_dir != None:
            if os.path.exists(set_dir):     # set to working directory if nothing set
                self.set_dir = set_dir

        vbox_top = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(vbox_top)

        hbox_loc = wx.BoxSizer(wx.HORIZONTAL)
        vbox_top.Add(hbox_loc, 0, wx.GROW|wx.ALIGN_LEFT|wx.ALL, 0)

        loc_label = wx.StaticText( self, -1, "Folder:")
        hbox_loc.Add(loc_label, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL|wx.ADJUST_MINSIZE, 5)

        self.dir = wx.TextCtrl( self, -1, self.set_dir, style=wx.TE_RICH|wx.TE_PROCESS_ENTER)
        self.Bind(wx.EVT_TEXT_ENTER, self.OnDirectoryTextSet, self.dir)
        hbox_loc.Add(self.dir, 1, wx.GROW|wx.ALIGN_LEFT|wx.ALL, 5)

        up_bmp = wx.ArtProvider.GetBitmap(wx.ART_GO_DIR_UP, wx.ART_BUTTON, (16,16))
        btn = wx.BitmapButton(self, -1, up_bmp)
        btn.SetHelpText("Up one level")
        btn.SetToolTipString("Up one level")
        self.Bind(wx.EVT_BUTTON, self.OnUpDirectory, btn)
        hbox_loc.Add(btn, 0, wx.ALIGN_CENTER_VERTICAL|wx.RIGHT, 2)

        folder_bmp = wx.ArtProvider.GetBitmap(wx.ART_FOLDER_OPEN, wx.ART_BUTTON, (16,16))
        btn = wx.BitmapButton(self, -1, folder_bmp)
        btn.SetHelpText("Browse for a &folder...")
        btn.SetToolTipString("Browse for a folder...")
        self.Bind(wx.EVT_BUTTON, self.OnChooseDirectory, btn)
        hbox_loc.Add(btn, 0, wx.ALIGN_CENTER_VERTICAL|wx.RIGHT, 5)

        hbox_nav = wx.BoxSizer(wx.HORIZONTAL)
        vbox_top.Add(hbox_nav, 0, wx.ALIGN_LEFT|wx.ALL, 0)


        label = wx.StaticText( self, -1, "Files of type:")
        hbox_nav.Add(label, 0, wx.ALIGN_CENTER_VERTICAL|wx.LEFT, 5)

        self.fl_ext = '*.bmp'   # initial setting for file filtering
        self.GetFiles()     # get the file list

        self.fl_ext_types = (
            # display, filter
            ("All supported formats", "All"),
            ("BMP (*.bmp)", "*.bmp"),
            ("GIF (*.gif)", "*.gif"),
            ("PNG (*.png)", "*.png"),
            ("JPEG (*.jpg)", "*.jpg"),
            ("ICO (*.ico)", "*.ico"),
            ("PNM (*.pnm)", "*.pnm"),
            ("PCX (*.pcx)", "*.pcx"),
            ("TIFF (*.tif)", "*.tif"),
            ("All Files", "*.*"),
            )
        self.set_type,self.fl_ext = self.fl_ext_types[0]  # initial file filter setting
        self.fl_types = [ x[0] for x in self.fl_ext_types ]
        self.sel_type = wx.ComboBox( self, -1, self.set_type,
                                     wx.DefaultPosition, wx.DefaultSize, self.fl_types, 
                                     wx.CB_DROPDOWN )
        # after this we don't care about the order any more
        self.fl_ext_types = dict(self.fl_ext_types)

        self.Bind(wx.EVT_COMBOBOX, self.OnSetType, self.sel_type)
        hbox_nav.Add(self.sel_type, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)

        splitter = wx.SplitterWindow( self, -1, wx.DefaultPosition, wx.Size(100, 100), 0 )
        splitter.SetMinimumPaneSize(100)

        split_left = wx.Panel( splitter, -1, wx.DefaultPosition, wx.DefaultSize, 
                               wx.NO_BORDER|wx.TAB_TRAVERSAL )
        vbox_left = wx.BoxSizer(wx.VERTICAL)
        split_left.SetSizer(vbox_left)


        self.tb = tb = wx.ListBox( split_left, -1, wx.DefaultPosition, wx.DefaultSize,
                                   self.fl_list, wx.LB_SINGLE )
        self.Bind(wx.EVT_LISTBOX, self.OnListClick, tb)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.OnListDClick, tb)
        vbox_left.Add(self.tb, 1, wx.GROW|wx.ALL, 0)

        width, height = self.tb.GetSize()

        split_right = wx.Panel( splitter, -1, wx.DefaultPosition, wx.DefaultSize,
                                wx.NO_BORDER|wx.TAB_TRAVERSAL )
        vbox_right = wx.BoxSizer(wx.VERTICAL)
        split_right.SetSizer(vbox_right)

        self.image_view = ImagePanel( split_right )
        vbox_right.Add(self.image_view, 1, wx.GROW|wx.ALL, 0)

        splitter.SplitVertically(split_left, split_right, 150)
        vbox_top.Add(splitter, 1, wx.GROW|wx.ALL, 5)

        hbox_btns = wx.BoxSizer(wx.HORIZONTAL)
        vbox_top.Add(hbox_btns, 0, wx.ALIGN_RIGHT|wx.ALL, 5)

        ok_btn = wx.Button( self, wx.ID_OPEN, "", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.Bind(wx.EVT_BUTTON, self.OnOk, ok_btn)
        #ok_btn.SetDefault()
        hbox_btns.Add(ok_btn, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)

        cancel_btn = wx.Button( self, wx.ID_CANCEL, "", 
                                wx.DefaultPosition, wx.DefaultSize, 0 )
        hbox_btns.Add(cancel_btn, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)

        self.ResetFiles()


    def GetFiles(self):     # get the file list using directory and extension values
        if self.fl_ext == "All":
            all_files = []

            for ftypes in self.fl_types[1:-1]:    # get list of all available image types
                filter = self.fl_ext_types[ftypes]
                #print "filter = ", filter
                self.fl_val = FindFiles(self, self.set_dir, filter)
                all_files = all_files + self.fl_val.files   # add to list of files

            self.fl_list = all_files
        else:
            self.fl_val = FindFiles(self, self.set_dir, self.fl_ext)
            self.fl_list = self.fl_val.files


        self.fl_list.sort()     # sort the file list
        # prepend the directories
        self.fl_ndirs = len(self.fl_val.dirs)
        self.fl_list = sorted(self.fl_val.dirs) + self.fl_list

    def DisplayDir(self):       # display the working directory
        if self.dir:
            ipt = self.dir.GetInsertionPoint()
            self.dir.SetValue(self.set_dir)
            self.dir.SetInsertionPoint(ipt)

    def OnSetType(self, event):
        val = event.GetString()      # get file type value
        self.fl_ext = self.fl_ext_types[val]
        self.ResetFiles()

    def OnListDClick(self, event):
        self.OnOk('dclick')

    def OnListClick(self, event):
        val = event.GetSelection()
        self.SetListValue(val)

    def SetListValue(self, val):
        file_nm = self.fl_list[val]
        self.set_file = file_val = os.path.join(self.set_dir, file_nm)
        if val>=self.fl_ndirs:
            self.image_view.SetValue(file_val)
        else:
            self.image_view.SetValue(None)

    def OnDirectoryTextSet(self,event):
        event.Skip()
        path = event.GetString()
        if os.path.isdir(path):
            self.set_dir = path
            self.ResetFiles()
            return

        if os.path.isfile(path):
            dname,fname = os.path.split(path)
            if os.path.isdir(dname):
                self.ResetFiles()
            # try to select fname in list
            try:
                idx = self.fl_list.index(fname)
                self.tb.SetSelection(idx)
                self.SetListValue(idx)
                return
            except ValueError:
                pass

        wx.Bell()
    
    def OnUpDirectory(self, event):
        sdir = os.path.split(self.set_dir)[0]
        self.set_dir = sdir
        self.ResetFiles()
        
    def OnChooseDirectory(self, event):     # set the new directory
        dlg = wx.DirDialog(self)
        dlg.SetPath(self.set_dir)

        if dlg.ShowModal() == wx.ID_OK:
            self.set_dir = dlg.GetPath()
            self.ResetFiles()

        dlg.Destroy()

    def ResetFiles(self):   # refresh the display with files and initial image
        self.DisplayDir()
        self.GetFiles()

        # Changed 12/8/03 jmg
        #
        # o Clear listbox first
        # o THEN check to see if there are any valid files of the selected
        #   type, 
        # o THEN if we have any files to display, set the listbox up,
        #
        # OTHERWISE
        #
        # o Leave it cleared
        # o Clear the image viewer.
        #
        # This avoids a nasty assert error.
        #
        self.tb.Clear()
        
        if len(self.fl_list):
            self.tb.Set(self.fl_list)

            for idir in xrange(self.fl_ndirs):
                d = self.fl_list[idir]
                # mark directories as 'True' with client data
                self.tb.SetClientData(idir, True)
                self.tb.SetString(idir,'['+d+']')

            try:
                self.tb.SetSelection(0)
                self.SetListValue(0)
            except:
                self.image_view.SetValue(None)
        else:
            self.image_view.SetValue(None)

    def GetFile(self):
        return self.set_file

    def GetDirectory(self):
        return self.set_dir

    def OnCancel(self, event):
        self.result = None
        self.EndModal(wx.ID_CANCEL)

    def OnOk(self, event):
        if os.path.isdir(self.set_file):
            sdir = os.path.split(self.set_file)
	
            #os.path.normapth?
            if sdir and sdir[-1]=='..':
                sdir = os.path.split(sdir[0])[0]
                sdir = os.path.split(sdir)
            self.set_dir = os.path.join(*sdir)
            self.set_file = None
            self.ResetFiles()
        elif event != 'dclick':
            self.result = self.set_file
            self.EndModal(wx.ID_OK)



class FindFiles:
    def __init__(self, parent, dir, mask, with_dirs=True):
        filelist = []
        dirlist = [".."]
        self.dir = dir
        self.file = ""
        mask = mask.upper()
        pattern = self.MakeRegex(mask)

        for i in os.listdir(dir):
            if i == "." or i == "..":
                continue

            path = os.path.join(dir, i)

            if os.path.isdir(path):
                dirlist.append(i)
                continue

            path = path.upper()
            value = i.upper()

            if pattern.match(value) != None:
                filelist.append(i)


        self.files = filelist
        if with_dirs:
            self.dirs = dirlist

    def MakeRegex(self, pattern):
        import re
        f = ""  # Set up a regex for file names

        for ch in pattern:
            if ch == "*":
                f = f + ".*"
            elif ch == ".":
                f = f + "\."
            elif ch == "?":
                f = f + "."
            else:
                f = f + ch

        return re.compile(f+'$')

    def StripExt(self, file_nm):
        fl_fld = os.path.splitext(file_nm)
        fl_name = fl_fld[0]
        ext = fl_fld[1]
        return ext[1:]


#----------------------------------------------------------------------
# This part of the file was generated by C:\Python25\Scripts\img2py
# then edited slightly.

import cStringIO, zlib


IMG_CATALOG = {}

class ImageClass: pass

def getWhiteData():
    return zlib.decompress(
'x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2< \xcc\xc1\x04$\
o\x8a\x9f\xde\x00\xa4\x98\x8b\x9d<C888n?\xf4\x7f\x00\xe4*z\xba8\x86HLMNP`202\
0\xf8\xf3\xff\xbf\xfc|.77\xb5$\x1f\xa9P\x979J\x8b\x18\x18N\\d\x16\t\xfd\xfc\
\xce\x07\xa8\x98\xc1\xd3\xd5\xcfe\x9dSB\x13\x00\xcc1\x1b\xb3' )

def getWhiteBitmap():
    return wx.BitmapFromImage(getWhiteImage())

def getWhiteImage():
    stream = cStringIO.StringIO(getWhiteData())
    return wx.ImageFromStream(stream)

IMG_CATALOG['White'] = ImageClass()
IMG_CATALOG['White'].getData = getWhiteData
IMG_CATALOG['White'].getImage = getWhiteImage
IMG_CATALOG['White'].getBitmap = getWhiteBitmap


#----------------------------------------------------------------------
def getGreyData():
    return zlib.decompress(
'x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2< \xcc\xc1\x04$\
o\x8a\x9f\xde\x00\xa4\x98\x8b\x9d<C888n?\xf4\x7f\x00\xe4*y\xba8\x86HLMNP`202\
0\x98cY\xcc\xd6\xcf%,,\xac\x96\xe4#\x15\xea2Gi\x11\x03\xc3\xb6\xc7\xcc":A7%\
\x80\xaa\x19<]\xfd\\\xd69%4\x01\x00{m\x18s' )

def getGreyBitmap():
    return wx.BitmapFromImage(getGreyImage())

def getGreyImage():
    stream = cStringIO.StringIO(getGreyData())
    return wx.ImageFromStream(stream)

IMG_CATALOG['Grey'] = ImageClass()
IMG_CATALOG['Grey'].getData = getGreyData
IMG_CATALOG['Grey'].getImage = getGreyImage
IMG_CATALOG['Grey'].getBitmap = getGreyBitmap


#----------------------------------------------------------------------
def getBlackData():
    return zlib.decompress(
'x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2< \xcc\xc1\x04$\
o\x8a\x9f\xde\x00\xa4\x98\x8b\x9d<C888n?\xf4\x7f\x00\xe4\xf2y\xba8\x86HLMN\
\x90`u\x16e``\xdc\xc3\xc0h\\3\xdb\x0c(\xc3\xe0\xe9\xea\xe7\xb2\xce)\xa1\t\
\x00\xb6`\x12\x08' )

def getBlackBitmap():
    return wx.BitmapFromImage(getBlackImage())

def getBlackImage():
    stream = cStringIO.StringIO(getBlackData())
    return wx.ImageFromStream(stream)

IMG_CATALOG['Black'] = ImageClass()
IMG_CATALOG['Black'].getData = getBlackData
IMG_CATALOG['Black'].getImage = getBlackImage
IMG_CATALOG['Black'].getBitmap = getBlackBitmap


#----------------------------------------------------------------------
def getCheckedData():
    return zlib.decompress(
'x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2< \xcc\xc1\x04$\
o\x8a\x9f\xde\x00\xa4\x98\x8b\x9d<C888n?\xf4\x7f\x00\xe4\x1az\xba8\x86HLMNP`\
2020\x98cY\xcc\x16y\xe2\xc6\r\tWVeeaC\xb5\x8b\x91\x82\xdc\xccm\xde\xe7\xe7\
\xd9Zo\xc8S\xf2\x12\x0cd`\xd0\xd8\xc5&\xf6\xeb\xd5\xe5t\xa0f\x06OW?\x97uN\tM\
\x00qL\x1f\x94' )

def getCheckedBitmap():
    return wx.BitmapFromImage(getCheckedImage())

def getCheckedImage():
    stream = cStringIO.StringIO(getCheckedData())
    return wx.ImageFromStream(stream)

IMG_CATALOG['Checked'] = ImageClass()
IMG_CATALOG['Checked'].getData = getCheckedData
IMG_CATALOG['Checked'].getImage = getCheckedImage
IMG_CATALOG['Checked'].getBitmap = getCheckedBitmap


#----------------------------------------------------------------------
def getNoFrameData():
    return zlib.decompress(
"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2< \xcc\xc1\x04$\
o\x8a\x9f\xde\x00\xa4\x98\x8b\x9d<C888n?\xf4\x7f\x00\xe4\x9ay\xba8\x86HL]\
\xdb\xef\xc8\xc5\xa0 \xc04\xf7\xc5\xff\xf8m\xd1\x01.\xba\x93\x9e'\x86\xac\
\x14P\xb9\xb9O\xf0\x82\xd62\x0e\xa6\x06\xf9e\x8f;Yg\xc5F'\xd7g]\xf2\xadd;=\
\x87S\xfe\xf3\xc7\x15\x8f\x80&0x\xba\xfa\xb9\xacsJh\x02\x00\x07\xac't" )

def getNoFrameBitmap():
    return wx.BitmapFromImage(getNoFrameImage())

def getNoFrameImage():
    stream = cStringIO.StringIO(getNoFrameData())
    return wx.ImageFromStream(stream)

IMG_CATALOG['NoFrame'] = ImageClass()
IMG_CATALOG['NoFrame'].getData = getNoFrameData
IMG_CATALOG['NoFrame'].getImage = getNoFrameImage
IMG_CATALOG['NoFrame'].getBitmap = getNoFrameBitmap


#----------------------------------------------------------------------
def getBoxFrameData():
    return zlib.decompress(
"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2< \xcc\xc1\x04$\
o\x8a\x9f\xde\x00\xa4\x98\x8b\x9d<C888n?\xf4\x7f\x00\xe4:{\xba8\x86HL\x9d\
\xdbw\x91\x8bA\x81\x83\xb9\xfc\xd2\xff\xff\x9bl9\x02\x15\xd5\xdefnJ\xf6\xcb\
\xe2\xf0|\x17'\x980W\xed\xaa\xaf\xe0\xcd:\xfd\xaa\xef\xec!/\xda.]ggaH\xfcT\
\xbaRI\xca_\xef\xe6\x97\xf5\x9c;\xa2\x15\xfe\xbe^S\xbe\th\x1c\x83\xa7\xab\
\x9f\xcb:\xa7\x84&\x00k\xdd.\x08" )

def getBoxFrameBitmap():
    return wx.BitmapFromImage(getBoxFrameImage())

def getBoxFrameImage():
    stream = cStringIO.StringIO(getBoxFrameData())
    return wx.ImageFromStream(stream)

IMG_CATALOG['BoxFrame'] = ImageClass()
IMG_CATALOG['BoxFrame'].getData = getBoxFrameData
IMG_CATALOG['BoxFrame'].getImage = getBoxFrameImage
IMG_CATALOG['BoxFrame'].getBitmap = getBoxFrameBitmap


#----------------------------------------------------------------------
def getCropFrameData():
    return zlib.decompress(
"x\xda\xeb\x0c\xf0s\xe7\xe5\x92\xe2b``\xe0\xf5\xf4p\t\x02\xd2< \xcc\xc1\x04$\
o\x8a\x9f\xde\x00\xa4\x98\x8b\x9d<C888n?\xf4\x7f\x00\xe4zz\xba8\x86HL\xdd;\
\xc1\x90\xeb\x80\x03\x07K\xba\xf3\xbf\xd5\xfe\x17\xc5g;\xedh\x16i\xcf\xdc\
\xd4z\xc2\xa8G\x81GIA\x89\xafew\xbc\xf0e\x8e]\xd7\xd3\xd2\x1aT\x16\xacj8\xf3\
'\xa1\xca\xf9\xad\x85\xe3\xa4_1\xe7\xef~~\xcd\xedV\xc9\xf0\x7f#\xbftm\xb5\
\x8d\t\x03\xc8TW?\x97uN\tM\x00\x9c@0\x82" )

def getCropFrameBitmap():
    return wx.BitmapFromImage(getCropFrameImage())

def getCropFrameImage():
    stream = cStringIO.StringIO(getCropFrameData())
    return wx.ImageFromStream(stream)

IMG_CATALOG['CropFrame'] = ImageClass()
IMG_CATALOG['CropFrame'].getData = getCropFrameData
IMG_CATALOG['CropFrame'].getImage = getCropFrameImage
IMG_CATALOG['CropFrame'].getBitmap = getCropFrameBitmap
