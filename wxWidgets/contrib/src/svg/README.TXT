
A wxSVGFileDC is a  device context onto which graphics and text can be drawn, and the output
produced as a vector file, in the SVG format (see http://www.w3.org/TR/2001/REC-SVG-20010904/ ).
This format can be read by a range of programs, including a Netscape plugin (Adobe), full details at 
http://www.w3.org/Graphics/SVG/SVG-Implementations.htm8 

Vector formats may often be smaller than raster formats.

The intention behind wxSVGFileDC is that it can be used to produce a file corresponding 
to the screen display context, wxDC, by passing the wxSVGFileDC as a parameter instead of a wxDC. Thus,
the wxSVGFileDC is a write-only class.

As the wxSVGFileDC is a vector format, raster operations like GetPixel are unlikely to be supported.
However, the SVG specification allows for PNG format raster files to be embedded in the SVG, and so 
bitmaps, icons and blit operations into the wxSVGFileDC are supported.

A more substantial SVG library (for reading and writing) is available at 
http://www.xs4all.nl/~kholwerd/wxstuff/canvas/htmldocbook/aap.html

Please notify Chris Elliott (biol75@york.ac.uk) of any bugs. Look in wxWindows\contrib\docs\latex\svg for the 
documentation
