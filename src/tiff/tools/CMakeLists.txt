# CMake build for libtiff
#
# Copyright © 2015 Open Microscopy Environment / University of Dundee
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Permission to use, copy, modify, distribute, and sell this software and
# its documentation for any purpose is hereby granted without fee, provided
# that (i) the above copyright notices and this permission notice appear in
# all copies of the software and related documentation, and (ii) the names of
# Sam Leffler and Silicon Graphics may not be used in any advertising or
# publicity relating to the software without the specific, prior written
# permission of Sam Leffler and Silicon Graphics.
#
# THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
# WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
#
# IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
# ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
# LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.

extra_dist(Makefile.vc)

include_directories(${PROJECT_SOURCE_DIR}/libtiff
                    ${PROJECT_BINARY_DIR}/libtiff
                    ${PROJECT_SOURCE_DIR}/port
                    ${CMAKE_CURRENT_BINARY_DIR})

add_executable(fax2ps fax2ps.c)
target_link_libraries(fax2ps tiff port)

add_executable(fax2tiff fax2tiff.c)
target_link_libraries(fax2tiff tiff port)

add_executable(pal2rgb pal2rgb.c)
target_link_libraries(pal2rgb tiff port)

add_executable(ppm2tiff ppm2tiff.c)
target_link_libraries(ppm2tiff tiff port)

add_executable(raw2tiff raw2tiff.c)
target_link_libraries(raw2tiff tiff port)

add_executable(rgb2ycbcr rgb2ycbcr.c)
target_link_libraries(rgb2ycbcr tiff port)

add_executable(thumbnail thumbnail.c)
target_link_libraries(thumbnail tiff port)

add_executable(tiff2bw tiff2bw.c)
target_link_libraries(tiff2bw tiff port)

add_executable(tiff2pdf tiff2pdf.c)
target_link_libraries(tiff2pdf tiff port)

add_executable(tiff2ps tiff2ps.c)
target_link_libraries(tiff2ps tiff port)

add_executable(tiff2rgba tiff2rgba.c)
target_link_libraries(tiff2rgba tiff port)

add_executable(tiffcmp tiffcmp.c)
target_link_libraries(tiffcmp tiff port)

add_executable(tiffcp tiffcp.c)
target_link_libraries(tiffcp tiff port)

add_executable(tiffcrop tiffcrop.c)
target_link_libraries(tiffcrop tiff port)

add_executable(tiffdither tiffdither.c)
target_link_libraries(tiffdither tiff port)

add_executable(tiffdump tiffdump.c)
target_link_libraries(tiffdump tiff port)

add_executable(tiffinfo tiffinfo.c)
target_link_libraries(tiffinfo tiff port)

add_executable(tiffmedian tiffmedian.c)
target_link_libraries(tiffmedian tiff port)

add_executable(tiffset tiffset.c)
target_link_libraries(tiffset tiff port)

add_executable(tiffsplit tiffsplit.c)
target_link_libraries(tiffsplit tiff port)

install(TARGETS fax2ps
                fax2tiff
                pal2rgb
                ppm2tiff
                raw2tiff
                tiff2bw
                tiff2pdf
                tiff2ps
                tiff2rgba
                tiffcmp
                tiffcp
                tiffcrop
                tiffdither
                tiffdump
                tiffinfo
                tiffmedian
                tiffset
                tiffsplit
        RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}")

if(HAVE_OPENGL)
  if(OPENGL_INCLUDE_DIR)
    include_directories(${OPENGL_INCLUDE_DIR})
  endif()
  if(GLUT_INCLUDE_DIR)
    include_directories(${GLUT_INCLUDE_DIR})
  endif()

  add_executable(tiffgt tiffgt.c)
  target_link_libraries(tiffgt tiff ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

  install(TARGETS tiffgt
          RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}")
endif()
