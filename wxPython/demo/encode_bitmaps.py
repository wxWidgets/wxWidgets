
"""
This is a way to save the startup time when running img2py on lots of
files...
"""

import sys

from wx.tools import img2py


command_lines = [
    "   -u -i -n Mondrian bmp_source/mondrian.ico images.py",

    "-a -u -n _10s_ bmp_source/10s.gif images.py",
    "-a -u -n _01c_ bmp_source/01c.gif images.py",
    "-a -u -n _12h_ bmp_source/12h.gif images.py",
    "-a -u -n _13d_ bmp_source/13d.gif images.py",
    "-a -u -n Background bmp_source/backgrnd.png images.py",
    "-a -u -n TestStar -m #FFFFFF bmp_source/teststar.png images.py",
    "-a -u -n TestStar2 bmp_source/teststar.png images.py",
    "-a -u -n TestMask bmp_source/testmask.bmp images.py",

    "-a -u -n Test2 bmp_source/test2.bmp images.py",
    "-a -u -n Test2m -m #0000FF  bmp_source/test2.bmp images.py",
    "-a -u -n Robin bmp_source/robin.jpg images.py",

    "-a -u -n Bulb1 bmp_source/lb1.bmp images.py",
    "-a -u -n Bulb2 bmp_source/lb2.bmp images.py",

    "-a -u -n Calendar bmp_source/Calend.bmp images.py",
    "-a -u -n DbDec bmp_source/DbDec.bmp images.py",
    "-a -u -n Dec bmp_source/Dec.bmp images.py",
    "-a -u -n Pt bmp_source/Pt.bmp images.py",
    "-a -u -n DbInc bmp_source/DbInc.bmp images.py",
    "-a -u -n Inc bmp_source/Inc.bmp images.py",

    "-a -u -n New   -m #C0C0C0 bmp_source/new.bmp images.py",
    "-a -u -n Open  -m #C0C0C0 bmp_source/open.bmp images.py",
    "-a -u -n Copy  -m #C0C0C0 bmp_source/copy.bmp images.py",
    "-a -u -n Copy2  -m #C0C0C0 bmp_source/copy2.bmp images.py",
    "-a -u -n Paste -m #C0C0C0 bmp_source/paste.bmp images.py",
    "-a -u -n Tog1  -m #C0C0C0 bmp_source/tog1.bmp images.py",
    "-a -u -n Tog2  -m #C0C0C0 bmp_source/tog2.bmp images.py",

    "-a -u -n Smiles -m #FFFFFF bmp_source/smiles2.bmp images.py",

    "-a -u -n GridBG bmp_source/GridBG.gif images.py",

    "-a -u -n SmallUpArrow  -m #0000FF bmp_source/sm_up.bmp images.py",
    "-a -u -n SmallDnArrow  -m #0000FF bmp_source/sm_down.bmp images.py",

    "-a -u -n Folder1 -m #FFFFFF bmp_source/folder1.bmp images.py",
    "-a -u -n Folder2 -m #FFFFFF bmp_source/folder2.bmp images.py",
    "-a -u -n Folder3 -m #FFFFFF bmp_source/folder3.bmp images.py",
    "-a -u -n File1   -m #FFFFFF bmp_source/file1.bmp   images.py",
    "-a -u -n File2   -m #FFFFFF bmp_source/file2.bmp   images.py",

    "-a -u -n NoIcon  bmp_source/noicon.png  images.py",

    "-a -u -n WizTest1 bmp_source/wiztest1.bmp images.py",
    "-a -u -n WizTest2 bmp_source/wiztest2.bmp images.py",

    "-a -u -n Tux bmp_source/Tux.png images.py",


    "-a -u -n Blom01 bmp_source/toblom01.png images.py",
    "-a -u -n Blom02 bmp_source/toblom02.png images.py",
    "-a -u -n Blom03 bmp_source/toblom03.png images.py",
    "-a -u -n Blom04 bmp_source/toblom04.png images.py",
    "-a -u -n Blom05 bmp_source/toblom05.png images.py",
    "-a -u -n Blom06 bmp_source/toblom06.png images.py",
    "-a -u -n Blom07 bmp_source/toblom07.png images.py",
    "-a -u -n Blom08 bmp_source/toblom08.png images.py",
    "-a -u -n Blom09 bmp_source/toblom09.png images.py",
    "-a -u -n Blom10 bmp_source/toblom10.png images.py",
    "-a -u -n Blom11 bmp_source/toblom11.png images.py",
    "-a -u -n Blom12 bmp_source/toblom12.png images.py",
    "-a -u -n Blom13 bmp_source/toblom13.png images.py",
    "-a -u -n Blom14 bmp_source/toblom14.png images.py",
    "-a -u -n Blom15 bmp_source/toblom15.png images.py",

    "-a -u -n Blom08m -m #FFFFFF bmp_source/toblom08.png images.py",
    "-a -u -n Blom10m -m #FFFFFF bmp_source/toblom10.png images.py",
    "-a -u -n Blom15m -m #FFFFFF bmp_source/toblom15.png images.py",

    "-a -u -n FloatCanvas bmp_source/floatcanvas.png images.py",
    
    "-a -u -i -n WXP bmp_source/wxp.ico images.py",
    "-a -u -i -n WXPdemo bmp_source/wxpdemo.ico images.py",


    "   -u -c bmp_source/001.png throbImages.py",
    "-a -u -c bmp_source/002.png throbImages.py",
    "-a -u -c bmp_source/003.png throbImages.py",
    "-a -u -c bmp_source/004.png throbImages.py",
    "-a -u -c bmp_source/005.png throbImages.py",
    "-a -u -c bmp_source/006.png throbImages.py",
    "-a -u -c bmp_source/007.png throbImages.py",
    "-a -u -c bmp_source/008.png throbImages.py",
    "-a -u -c bmp_source/009.png throbImages.py",
    "-a -u -c bmp_source/010.png throbImages.py",
    "-a -u -c bmp_source/011.png throbImages.py",
    "-a -u -c bmp_source/012.png throbImages.py",
    "-a -u -c bmp_source/013.png throbImages.py",
    "-a -u -c bmp_source/014.png throbImages.py",
    "-a -u -c bmp_source/015.png throbImages.py",
    "-a -u -c bmp_source/016.png throbImages.py",
    "-a -u -c bmp_source/017.png throbImages.py",
    "-a -u -c bmp_source/018.png throbImages.py",
    "-a -u -c bmp_source/019.png throbImages.py",
    "-a -u -c bmp_source/020.png throbImages.py",
    "-a -u -c bmp_source/021.png throbImages.py",
    "-a -u -c bmp_source/022.png throbImages.py",
    "-a -u -c bmp_source/023.png throbImages.py",
    "-a -u -c bmp_source/024.png throbImages.py",
    "-a -u -c bmp_source/025.png throbImages.py",
    "-a -u -c bmp_source/026.png throbImages.py",
    "-a -u -c bmp_source/027.png throbImages.py",
    "-a -u -c bmp_source/028.png throbImages.py",
    "-a -u -c bmp_source/029.png throbImages.py",
    "-a -u -c bmp_source/030.png throbImages.py",

    "-a -u -c bmp_source/eclouds.png throbImages.py",
    "-a -u -c bmp_source/logo.png    throbImages.py",
    "-a -u -c bmp_source/rest.png    throbImages.py",
    ]


if __name__ == "__main__":
    for line in command_lines:
        args = line.split()
        img2py.main(args)

