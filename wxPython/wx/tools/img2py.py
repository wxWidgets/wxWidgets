#----------------------------------------------------------------------
# Name:        wxPython.tools.img2py
# Purpose:     Convert an image to Python code.
#
# Author:      Robin Dunn
#
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
#
# 2/25/2007 - Gianluca Costa (archimede86@katamail.com)
#
#
# o V2.5 compatibility update 
#

"""
img2py.py  --  Convert an image to PNG format and embed it in a Python
               module with appropriate code so it can be loaded into
               a program at runtime.  The benefit is that since it is
               Python source code it can be delivered as a .pyc or
               'compiled' into the program using freeze, py2exe, etc.

Usage:

    img2py.py [options] image_file python_file

Options:

    -m <#rrggbb>   If the original image has a mask or transparency defined
                   it will be used by default.  You can use this option to
                   override the default or provide a new mask by specifying
                   a colour in the image to mark as transparent.

    -n <name>      Normally generic names (getBitmap, etc.) are used for the
                   image access functions.  If you use this option you can
                   specify a name that should be used to customize the access
                   fucntions, (getNameBitmap, etc.)

    -c             Maintain a catalog of names that can be used to reference
                   images.  Catalog can be accessed via catalog and 
                   index attributes of the module.  
                   If the -n <name> option is specified then <name>
                   is used for the catalog key and index value, otherwise
                   the filename without any path or extension is used 
                   as the key.

    -a             This flag specifies that the python_file should be appended
                   to instead of overwritten.  This in combination with -n will
                   allow you to put multiple images in one Python source file.

    -u             Don't use compression.  Leaves the data uncompressed.

    -i             Also output a function to return the image as a wxIcon.


You can also import this module from your Python scripts, and use its img2py()
function. See its docstring for more info.
"""

#
# Changes:
#    - Cliff Wells <LogiplexSoftware@earthlink.net>
#      20021206: Added catalog (-c) option.
#
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
#
# 2/25/2007 - Gianluca Costa (archimede86@katamail.com)
#        -Refactorization of the script-creation code in a specific "img2py()" function
#        -Added regex parsing instead of module importing
#        -Added some "try/finally" statements
#        -Added default values as named constants
#        -Made some parts of code a bit easier to read
#        -Updated the module docstring
#        -Corrected a bug with EmptyIcon
#
# o V2.5 compatibility update 
#
import  cPickle
import  cStringIO
import  getopt
import  glob
import  os
import  os.path
import  sys
import  tempfile
import  zlib
import  re

import  wx
import  img2img


def crunch_data(data, compressed):
    # compress it?
    if compressed:
        data = zlib.compress(data, 9)

    # convert to a printable format, so it can be in a Python source file
    data = repr(data)

    # This next bit is borrowed from PIL.  It is used to wrap the text intelligently.
    fp = cStringIO.StringIO()
    data += " "  # buffer for the +1 test
    c = i = 0
    word = ""
    octdigits = "01234567"
    hexdigits = "0123456789abcdef"
    while i < len(data):
        if data[i] != "\\":
            word = data[i]
            i += 1
        else:
            if data[i+1] in octdigits:
                for n in xrange(2, 5):
                    if data[i+n] not in octdigits:
                        break
                word = data[i:i+n]
                i += n
            elif data[i+1] == 'x':
                for n in xrange(2, 5):
                    if data[i+n] not in hexdigits:
                        break
                word = data[i:i+n]
                i += n
            else:
                word = data[i:i+2]
                i += 2

        l = len(word)
        if c + l >= 78-1:
            fp.write("\\\n")
            c = 0
        fp.write(word)
        c += l

    # return the formatted compressed data
    return fp.getvalue()


app = None
DEFAULT_APPEND = False
DEFAULT_COMPRESSED = True
DEFAULT_MASKCLR = None
DEFAULT_IMGNAME = ""
DEFAULT_ICON = False
DEFAULT_CATALOG = False

#THIS IS USED TO IDENTIFY, IN THE GENERATED SCRIPT, LINES IN THE FORM "index.append('Image name')"
indexPattern = re.compile(r"\s*index.append\('(.+)'\)\s*")

def img2py(image_file, python_file, append=DEFAULT_APPEND, compressed=DEFAULT_COMPRESSED, maskClr=DEFAULT_MASKCLR, imgName=DEFAULT_IMGNAME, icon=DEFAULT_ICON, catalog=DEFAULT_CATALOG):
    """
    Converts an image file to a data structure written in a Python file
    --image_file: string; the path of the source image file
    --python_file: string; the path of the destination python file
    --other arguments: they are equivalent to the command-line arguments
    """
    global app
    if not wx.GetApp():
        app = wx.PySimpleApp()
        
    # convert the image file to a temporary file
    tfname = tempfile.mktemp()
    try:
        ok, msg = img2img.convert(image_file, maskClr, None, tfname, wx.BITMAP_TYPE_PNG, ".png")
        if not ok:
            print msg
            return
            
        data = open(tfname, "rb").read()
        data = crunch_data(data, compressed)
    finally:
        if os.path.exists(tfname):
            os.remove(tfname)
    

    old_index = []
    if catalog and append:
        # check to see if catalog exists already (file may have been created
        # with an earlier version of img2py or without -c option)
        pyPath, pyFile = os.path.split(python_file)
        
        append_catalog = True
        
        sourcePy = open(python_file, "r")
        try:
            for line in sourcePy:
                
                if line == "catalog = {}\n":
                    append_catalog = False
                else:
                    lineMatcher = indexPattern.match(line)
                    if lineMatcher:
                        old_index.append(lineMatcher.groups()[0])
        finally:
            sourcePy.close()
                    
                    
        if append_catalog:
            out = open(python_file, "a")
            try:
                out.write("\n# ***************** Catalog starts here *******************")
                out.write("\n\ncatalog = {}\n")
                out.write("index = []\n\n")
                out.write("class ImageClass: pass\n\n")
            finally:
                out.close()
        
        
    
    if append:
        out = open(python_file, "a")
    else:
        out = open(python_file, "w")
        
    try:
        if catalog:
            imgPath, imgFile = os.path.split(image_file)
    
            if not imgName:
                imgName = os.path.splitext(imgFile)[0]
                print "\nWarning: -n not specified. Using filename (%s) for catalog entry." % imgName
    
        out.write("#" + "-" * 70 + "\n")
        if not append:
            out.write("# This file was generated by %s\n#\n" % sys.argv[0])
            out.write("from wx import ImageFromStream, BitmapFromImage, EmptyIcon\n")        
            if compressed:
                out.write("import cStringIO, zlib\n\n\n")
            else:
                out.write("import cStringIO\n\n\n")
    
            if catalog:
                out.write("catalog = {}\n")
                out.write("index = []\n\n")
                out.write("class ImageClass: pass\n\n")
    
        if compressed:
            out.write("def get%sData():\n"
                      "    return zlib.decompress(\n%s)\n\n"
                      % (imgName, data))
        else:
            out.write("def get%sData():\n"
                      "    return \\\n%s\n\n"
                      % (imgName, data))
    
    
        out.write("def get%sBitmap():\n"
                  "    return BitmapFromImage(get%sImage())\n\n"
                  "def get%sImage():\n"
                  "    stream = cStringIO.StringIO(get%sData())\n"
                  "    return ImageFromStream(stream)\n\n"
                  % tuple([imgName] * 4))
        if icon:
            out.write("def get%sIcon():\n"
                      "    icon = EmptyIcon()\n"
                      "    icon.CopyFromBitmap(get%sBitmap())\n"
                      "    return icon\n\n"
                      % tuple([imgName] * 2))
    
        if catalog:
            if imgName in old_index:
                print "Warning: %s already in catalog." % imgName
                print "         Only the last entry will be accessible.\n"
            old_index.append(imgName)
            out.write("index.append('%s')\n" % imgName)
            out.write("catalog['%s'] = ImageClass()\n" % imgName)
            out.write("catalog['%s'].getData = get%sData\n" % tuple([imgName] * 2))
            out.write("catalog['%s'].getImage = get%sImage\n" % tuple([imgName] * 2))
            out.write("catalog['%s'].getBitmap = get%sBitmap\n" % tuple([imgName] * 2))
            if icon:
                out.write("catalog['%s'].getIcon = get%sIcon\n" % tuple([imgName] * 2))
            out.write("\n\n")
    

        if imgName:
            n_msg = ' using "%s"' % imgName
        else:
            n_msg = ""        
            
        if maskClr:
            m_msg = " with mask %s" % maskClr 
        else:
            m_msg = ""

        print "Embedded %s%s into %s%s" % (image_file, n_msg, python_file, m_msg)
    finally:
        out.close()
        

    
def main(args):
    if not args or ("-h" in args):
        print __doc__
        return
        
    append = DEFAULT_APPEND
    compressed = DEFAULT_COMPRESSED
    maskClr = DEFAULT_MASKCLR
    imgName = DEFAULT_IMGNAME
    icon = DEFAULT_ICON
    catalog = DEFAULT_CATALOG

    try:
        opts, fileArgs = getopt.getopt(args, "auicn:m:")
    except getopt.GetoptError:
        print __doc__
        return

    for opt, val in opts:
        if opt == "-a":
            append = True
        elif opt == "-u":
            compressed = False
        elif opt == "-n":
            imgName = val
        elif opt == "-m":
            maskClr = val
        elif opt == "-i":
            icon = True
        elif opt == "-c":
            catalog = True

    if len(fileArgs) != 2:
        print __doc__
        return

    image_file, python_file = fileArgs
    img2py(image_file, python_file, append, compressed, maskClr, imgName, icon, catalog)
    

    
if __name__ == "__main__":
    main(sys.argv[1:])
