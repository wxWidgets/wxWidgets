#----------------------------------------------------------------------------
# Name:         BrowseImage.py
# Purpose:      Display and Select Image Files
#
# Author:       Lorne White
#
# Version:      1.0
# Date:         January 29, 2002
# Licence:      wxWindows license
#----------------------------------------------------------------------------
# 1.0 Release
# Create list of all available image file types
# View "All Image" File Types as default filter
# Sort the file list
# Use newer "re" function for patterns
#
#----------------------------------------------------------------------------
#
# 12/08/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o Corrected a nasty bug or two - see comments below.
# o There was a duplicate ImageView.DrawImage() method. Que?
# 

#---------------------------------------------------------------------------

import  os
import  sys

import  wx

dir_path = os.getcwd()

#---------------------------------------------------------------------------

def ConvertBMP(file_nm):
    if file_nm is None:
        return None

    fl_fld = os.path.splitext(file_nm)
    ext = fl_fld[1]
    ext = ext[1:].lower()

    image = wx.Image(file_nm, wx.BITMAP_TYPE_ANY)
    return image


def GetSize(file_nm):       # for scaling image values
    image = ConvertBMP(file_nm)
    bmp = image.ConvertToBitmap()
    size = bmp.GetWidth(), bmp.GetHeight()
    return size


class ImageView(wx.Window):
    def __init__(self, parent, id=-1, pos=wx.DefaultPosition, size=wx.DefaultSize):
        wx.Window.__init__(self, parent, id, pos, size)
        self.win = parent
        self.image = None
        self.back_color = 'WHITE'
        self.border_color = 'BLACK'

        # Changed API of wx uses tuples for size and pos now.
        self.image_sizex = size[0]
        self.image_sizey = size[1]
        self.image_posx = pos[0]
        self.image_posy = pos[1]
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.DrawImage(dc)


    def SetValue(self, file_nm):    # display the selected file in the panel
        image = ConvertBMP(file_nm)
        self.image = image
        self.Refresh()

    def DrawBorder(self, dc):
        brush = wx.Brush(wx.NamedColour(self.back_color), wx.SOLID)
        dc.SetBrush(brush)
        dc.SetPen(wx.Pen(wx.NamedColour(self.border_color), 1))
        dc.DrawRectangle(0, 0, self.image_sizex, self.image_sizey)

    def DrawImage(self, dc):
        try:
            image = self.image
        except:
            return

        self.DrawBorder(dc)

        if image is None:
            return

        bmp = image.ConvertToBitmap()

        iwidth = bmp.GetWidth()   # dimensions of image file
        iheight = bmp.GetHeight()

        diffx = (self.image_sizex - iwidth)/2   # center calc
        if iwidth >= self.image_sizex -10:      # if image width fits in window adjust
            diffx = 5
            iwidth = self.image_sizex - 10

        diffy = (self.image_sizey - iheight)/2  # center calc
        if iheight >= self.image_sizey - 10:    # if image height fits in window adjust
            diffy = 5
            iheight = self.image_sizey - 10

        image.Rescale(iwidth, iheight)      # rescale to fit the window
        image.ConvertToBitmap()
        bmp = image.ConvertToBitmap()
        dc.DrawBitmap(bmp, diffx, diffy)        # draw the image to window


class ImageDialog(wx.Dialog):
    def __init__(self, parent, set_dir = None):
        wx.Dialog.__init__(self, parent, -1, "Image Browser", wx.DefaultPosition, (400, 400))

        self.x_pos = 30     # initial display positions
        self.y_pos = 20
        self.delta = 20

        size = wx.Size(80, -1)

        self.set_dir = os.getcwd()
        self.set_file = None

        if set_dir != None:
            if os.path.exists(set_dir):     # set to working directory if nothing set
                self.set_dir = set_dir

        self.dir_x = self.x_pos
        self.dir_y = self.y_pos
        self.dir = wx.StaticText(self, -1, self.set_dir, (self.dir_x, self.dir_y), (250, -1))

        self.y_pos = self.y_pos + self.delta

        btn = wx.Button(self, 12331, ' Set Directory ', (self.x_pos, self.y_pos))
        self.Bind(wx.EVT_BUTTON, self.SetDirect, btn)

        self.type_posy = self.y_pos     # save the y position for the image type combo

        self.fl_ext = '*.bmp'   # initial setting for file filtering
        self.GetFiles()     # get the file list

        self.y_pos = self.y_pos + self.delta + 10
        self.list_height = 150

    # List of Labels
        self.tb = tb = wx.ListBox(self, -1, (self.x_pos, self.y_pos), 
                                  (160, self.list_height), self.fl_list, 
                                  wx.LB_SINGLE )
        self.Bind(wx.EVT_LISTBOX, self.OnListClick, tb)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.OnListDClick, tb)

        width, height = self.tb.GetSize()
        image_posx = self.x_pos + width + 20       # positions for setting the image window
        image_posy = self.y_pos
        image_sizex = 150
        image_sizey = self.list_height

        self.fl_types = [
                    "All Images", "Bmp", "Gif", "Png", "Jpg", "Ico", "Pnm", 
                    "Pcx", "Tif", "All Files"
                    ]

        self.fl_ext_types = {
                "All Images": "All", 
                "Bmp": "*.bmp", 
                "Gif": "*.gif", 
                "Png": "*.png", 
                "Jpg": "*.jpg",
                "Ico": "*.ico", 
                "Pnm": "*.pnm", 
                "Pcx": "*.pcx", 
                "Tif": "*.tif", 
                "All Files": "*.*" 
                }

        self.set_type = self.fl_types[0]    # initial file filter setting
        self.fl_ext = self.fl_ext_types[self.set_type]

        self.sel_type = wx.ComboBox(self, -1, self.set_type, (image_posx , self.type_posy), 
                                    (150, -1), self.fl_types, wx.CB_DROPDOWN)
        self.Bind(wx.EVT_COMBOBOX, self.OnSetType, self.sel_type)

        self.image_view = ImageView( self, pos=(image_posx, image_posy), 
                                     size=(image_sizex, image_sizey))

        self.y_pos = self.y_pos + height + 20

        btn = wx.Button(self, -1, ' Select ', (100, self.y_pos), size)
        self.Bind(wx.EVT_BUTTON, self.OnOk, btn)

        wx.Button(self, wx.ID_CANCEL, 'Cancel', (250, self.y_pos), size)

        self.y_pos = self.y_pos + self.delta
        fsize = (400, self.y_pos + 50)    # resize dialog for final vertical position
        self.SetSize(fsize)

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

    def DisplayDir(self):       # display the working directory
        if self.dir:
            self.dir.SetLabel(self.set_dir)

    def OnSetType(self, event):
        val = event.GetString()      # get file type value
        self.fl_ext = self.fl_ext_types[val]
        self.ResetFiles()

    def OnListDClick(self, event):
        self.OnOk(0)

    def OnListClick(self, event):
        val = event.GetSelection()
        self.SetListValue(val)

    def SetListValue(self, val):
        file_nm = self.fl_list[val]
        self.set_file = file_val = os.path.join(self.set_dir, file_nm)
        self.image_view.SetValue(file_val)

    def SetDirect(self, event):     # set the new directory
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
        self.result = self.set_file
        self.EndModal(wx.ID_OK)


def OnFileDlg(self):
    dlg = wx.FileDialog(self, "Choose an Image File", ".", "", 
                        "Bmp (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg", wx.OPEN)

    if dlg.ShowModal() == wx.ID_OK:
        path = dlg.GetPath()
    else:
        path = None

    dlg.Destroy()
    return path

class FindFiles:
    def __init__(self, parent, dir, mask):
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
            path = path.upper()
            value = i.upper()

            if pattern.match(value) != None:
                filelist.append(i)

            self.files = filelist

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
