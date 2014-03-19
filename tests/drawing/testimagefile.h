///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/testimagefile.h
// Purpose:     Functions to test whether the content of files or images files
//              are equal
// Author:      Armel Asselin
// Created:     2014-02-28
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_TESTS_TESTIMAGEFILE_H_
#define _WX_TESTS_TESTIMAGEFILE_H_

bool AreFilesContentsEqual(const wxString &filename, const wxString &refFilename)
{
    wxFileInputStream input(filename);
    wxFileInputStream refInput(refFilename);

    wxFileOffset refLength = refInput.GetLength();
    if (refLength != input.GetLength())
        return false;

    wxUint8 buffer[1024], refBuffer[sizeof(buffer)];

    wxFileOffset remainingLength = refLength;
    while (remainingLength != 0)
    {
        input.Read (buffer, wxMin(remainingLength, sizeof(buffer)));
        refInput.Read(refBuffer, wxMin(remainingLength, sizeof(refBuffer)));

        size_t refLastRead = refInput.LastRead();
        if (input.LastRead() != refLastRead)
            return false;

        if (memcmp (buffer, refBuffer, refLastRead) != 0)
            return false;

        remainingLength -= refLastRead;
    }

    return true;
}

#define WX_ASSERT_SAME_AS_FILE(filename1, filename2) \
    WX_ASSERT_MESSAGE(\
        ("Files \"%s\" and \"%s\" differ.",\
            filename1, filename2),\
        AreFilesContentsEqual(filename1, filename2))

bool AreImagesFilesContentsEqual(const wxString &filename,
                                 const wxString &refFilename)
{
    wxImage input(filename);
    wxImage refInput(refFilename);

    // assertion_traits<wxImage> could do part of the job or maybe the contrary
    // there should probably be somethin common at least

    if (!input.Ok() || !refInput.Ok())
        return false;
    if (input.GetSize() != refInput.GetSize())
        return false;
    if (input.HasAlpha() != refInput.HasAlpha())
        return false;
    if (input.HasMask() != refInput.HasMask())
        return false;

    long pixelsCount = input.GetSize().GetWidth() * input.GetSize().GetHeight();
    const unsigned char *data = input.GetData();
    const unsigned char *refData = refInput.GetData();
    if (memcmp (data, refData, pixelsCount*3) != 0)
        return false;

    if (input.HasAlpha())
    {
        const unsigned char *alpha = input.GetAlpha();
        const unsigned char *refAlpha = refInput.GetAlpha();
        if (memcmp (alpha, refAlpha, pixelsCount) != 0)
            return false;
    }

    if (input.HasMask() &&
        (input.GetMaskRed() != refInput.GetMaskRed() ||
         input.GetMaskGreen() != refInput.GetMaskGreen() ||
         input.GetMaskBlue() != refInput.GetMaskBlue()))
         return false;

    return true;
}

#define WX_ASSERT_SAME_AS_IMAGE_FILE(filename1, filename2) \
    WX_ASSERT_MESSAGE(\
        ("Image files \"%s\" and \"%s\" differ.",\
            filename1, filename2),\
        AreImagesFilesContentsEqual(filename1, filename2))

#endif // _WX_TESTS_TESTIMAGEFILE_H_
