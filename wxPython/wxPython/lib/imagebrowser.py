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

#---------------------------------------------------------------------------

import os, sys
from wxPython.wx import *
dir_path = os.getcwd()

#---------------------------------------------------------------------------

def ConvertBMP(file_nm):
    if file_nm is None:
        return None

    fl_fld = os.path.splitext(file_nm)
    ext = fl_fld[1]
    ext = ext[1:].lower()
    if ext == 'bmp':
        image = wxImage(file_nm, wxBITMAP_TYPE_BMP)
    elif ext == 'gif':
        image = wxImage(file_nm, wxBITMAP_TYPE_GIF)
    elif ext == 'png':
        image = wxImage(file_nm, wxBITMAP_TYPE_PNG)
    elif ext == 'jpg':
        image = wxImage(file_nm, wxBITMAP_TYPE_JPEG)
    elif ext == 'pcx':
        image = wxImage(file_nm, wxBITMAP_TYPE_PCX)
    elif ext == 'tif':
        image = wxImage(file_nm, wxBITMAP_TYPE_TIF)
    elif ext == 'pnm':
        image = wxImage(file_nm, wxBITMAP_TYPE_PNM)
    else:
        image = wxImage(file_nm, wxBITMAP_TYPE_ANY)

    return image

def GetSize(file_nm):       # for scaling image values
    image = ConvertBMP(file_nm)
    bmp = image.ConvertToBitmap()
    size = bmp.GetWidth(), bmp.GetHeight()
    return size

class ImageView(wxWindow):
    def __init__(self, parent, id=-1, pos=wxDefaultPosition, size=wxDefaultSize):
        wxWindow.__init__(self, parent, id, pos, size)
        self.win = parent
        self.image = None
        self.back_color = 'WHITE'
        self.border_color = 'BLACK'

        self.image_sizex = size.width
        self.image_sizey = size.height
        self.image_posx = pos.x
        self.image_posy = pos.y
        EVT_PAINT(self, self.OnPaint)

        wxInitAllImageHandlers()

    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.DrawImage(dc)

    def DrawImage(self, dc):
        dc.BeginDrawing()
        self.DrawImage(dc)
        dc.EndDrawing()

    def SetValue(self, file_nm):    # display the selected file in the panel
        image = ConvertBMP(file_nm)
        self.image = image
        self.Refresh()

    def DrawBorder(self, dc):
        brush = wxBrush(wxNamedColour(self.back_color), wxSOLID)
        dc.SetBrush(brush)
        dc.SetPen(wxPen(wxNamedColour(self.border_color), 1))
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


class ImageDialog(wxDialog):
    def __init__(self, parent, set_dir = None):
        wxDialog.__init__(self, parent, -1, "Image Browser", wxPyDefaultPosition, wxSize(400, 400))

        self.x_pos = 30     # initial display positions
        self.y_pos = 20
        self.delta = 20

        size = wxSize(80, 25)

        self.set_dir = os.getcwd()
        self.set_file = None

        if set_dir != None:
            if os.path.exists(set_dir):     # set to working directory if nothing set
                self.set_dir = set_dir

        self.dir_x = self.x_pos
        self.dir_y = self.y_pos
        self.DisplayDir()       # display the directory value

        self.y_pos = self.y_pos + self.delta

        mID = wxNewId()
        wxButton(self, mID, ' Set Directory ', wxPoint(self.x_pos, self.y_pos), size).SetDefault()
        EVT_BUTTON(self, mID, self.SetDirect)

        self.type_posy = self.y_pos     # save the y position for the image type combo

        self.fl_ext = '*.bmp'   # initial setting for file filtering
        self.GetFiles()     # get the file list

        self.y_pos = self.y_pos + self.delta + 10

        self.list_height = 150

    # List of Labels
        mID = wxNewId()
        self.tb = tb = wxListBox(self, mID, wxPoint(self.x_pos, self.y_pos), wxSize(160, self.list_height), self.fl_list, wxLB_SINGLE)
        EVT_LISTBOX(self, mID, self.OnListClick)
        EVT_LISTBOX_DCLICK(self, mID, self.OnListDClick)

        width, height = self.tb.GetSizeTuple()
        image_posx = self.x_pos + width + 20       # positions for setting the image window
        image_posy = self.y_pos
        image_sizex = 150
        image_sizey = self.list_height

        self.fl_types = ["All Images", "Bmp", "Gif", "Png", "Jpg", "Ico", "Pnm", "Pcx", "Tif", "All Files"]
        self.fl_ext_types = { "All Images": "All", "Bmp": "*.bmp", "Gif": "*.gif", "Png": "*.png", "Jpg": "*.jpg",
                        "Ico": "*.ico", "Pnm": "*.pnm", "Pcx": "*.pcx", "Tif": "*.tif", "All Files": "*.*" }

        self.set_type = self.fl_types[0]    # initial file filter setting
        self.fl_ext = self.fl_ext_types[self.set_type]

        mID = wxNewId()
        self.sel_type = wxComboBox(self, mID, self.set_type, wxPoint(image_posx , self.type_posy), wxSize(150, -1), self.fl_types, wxCB_DROPDOWN)
        EVT_COMBOBOX(self, mID, self.OnSetType)

        self.image_view = ImageView(self, pos=wxPoint(image_posx, image_posy), size=wxSize(image_sizex, image_sizey))

        self.y_pos = self.y_pos + height + 20

        mID = wxNewId()
        wxButton(self, mID, ' Select ', wxPoint(100, self.y_pos), size).SetDefault()
        EVT_BUTTON(self, mID, self.OnOk)

        wxButton(self, wxID_CANCEL, 'Cancel', wxPoint(250, self.y_pos), size)

        self.y_pos = self.y_pos + self.delta
        fsize = wxSize(400, self.y_pos + 50)    # resize dialog for final vertical position
        self.SetSize(fsize)

        self.ResetFiles()

    def GetFiles(self):     # get the file list using directory and extension values
        if self.fl_ext == "All":
            all_files = []
            for ftypes in self.fl_types[1:-1]:    # get list of all available image types
                filter = self.fl_ext_types[ftypes]
                self.fl_val = FindFiles(self, self.set_dir, filter)
                all_files = all_files + self.fl_val.files   # add to list of files
            self.fl_list = all_files
        else:
            self.fl_val = FindFiles(self, self.set_dir, self.fl_ext)
            self.fl_list = self.fl_val.files

        self.fl_list.sort()     # sort the file list

    def DisplayDir(self):       # display the working directory
        wxStaticText(self, -1, self.set_dir, wxPoint(self.dir_x, self.dir_y), wxSize(250, -1))

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
        dlg = wxDirDialog(self)
        dlg.SetPath(self.set_dir)
        if dlg.ShowModal() == wxID_OK:
            self.set_dir = dlg.GetPath()
            self.ResetFiles()
        dlg.Destroy()

    def ResetFiles(self):   # refresh the display with files and initial image
        self.DisplayDir()
        self.GetFiles()
        self.tb.Set(self.fl_list)
        try:
            self.tb.SetSelection(0)
            self.SetListValue(0)
        except:
            self.image_view.SetValue(None)

    def GetFile(self):
        return self.set_file

    def GetDirectory(self):
        return self.set_dir

    def OnCancel(self, event):
        self.result = None
        self.EndModal(wxID_CANCEL)

    def OnOk(self, event):
        self.result = self.set_file
        self.EndModal(wxID_OK)


def OnFileDlg(self):
    dlg = wxFileDialog(self, "Choose an Image File", ".", "", "Bmp (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg", wxOPEN)
    if dlg.ShowModal() == wxID_OK:
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

        return re.compile(f)

    def StripExt(self, file_nm):
        fl_fld = os.path.splitext(file_nm)
        fl_name = fl_fld[0]
        ext = fl_fld[1]
        return ext[1:]
