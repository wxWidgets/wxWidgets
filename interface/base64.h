/////////////////////////////////////////////////////////////////////////////
// Name:        base64.h
// Purpose:     documentation for global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

//@{
/**
    These functions encode the given data using base64. The first of them is the
    raw encoding function writing the output string into provided buffer while the
    other ones return the output as wxString. There is no error return for these
    functions except for the first one which returns @c wxCONV_FAILED if the
    output buffer is too small. To allocate the buffer of the correct size, use
    wxBase64EncodedSize or call this function with
    @e dst set to @NULL -- it will then return the necessary buffer size.

    @param dst
    The output buffer, may be @NULL to retrieve the needed buffer
    size.

    @param dstLen
    The output buffer size, ignored if dst is @NULL.

    @param src
    The input buffer, must not be @NULL.

    @param srcLen
    The length of the input data.
*/
size_t wxBase64Encode(char * dst, size_t dstLen,
                      const void * src,
                      size_t srcLen);
wxString wxBase64Encode(const void * src, size_t srcLen);
wxString wxBase64Encode(const wxMemoryBuffer& buf);
//@}


/**
Returns the size of the buffer necessary to contain the data encoded in a
base64 string of length @e srcLen. This can be useful for allocating a
buffer to be passed to wxBase64Decode.
*/
size_t wxBase64DecodedSize(size_t srcLen);

/**
    Returns the length of the string with base64 representation of a buffer of
    specified size @e len. This can be useful for allocating the buffer passed
    to wxBase64Encode.
*/
size_t wxBase64EncodedSize(size_t len);

//@{
/**
    These function decode a Base64-encoded string. The first version is a raw
    decoding function and decodes the data into the provided buffer @e dst of
    the given size @e dstLen. An error is returned if the buffer is not large
    enough -- that is not at least wxBase64DecodedSize(srcLen)
    bytes. The second version allocates memory internally and returns it as
    wxMemoryBuffer and is recommended for normal use.

    The first version returns the number of bytes written to the buffer or the
    necessary buffer size if @e dst was @NULL or @c wxCONV_FAILED on
    error, e.g. if the output buffer is too small or invalid characters were
    encountered in the input string. The second version returns a buffer with the
    base64 decoded binary equivalent of the input string. In neither case is the
    buffer NUL-terminated.

    @param dst
    Pointer to output buffer, may be @NULL to just compute the
    necessary buffer size.

    @param dstLen
    The size of the output buffer, ignored if dst is
    @NULL.

    @param src
    The input string, must not be @NULL. For the version using
    wxString, the input string should contain only ASCII characters.

    @param srcLen
    The length of the input string or special value
    wxNO_LEN if the string is NUL-terminated and the length should be
    computed by this function itself.

    @param mode
    This parameter specifies the function behaviour when invalid
    characters are encountered in input. By default, any such character stops the
    decoding with error. If the mode is wxBase64DecodeMode_SkipWS, then the white
    space characters are silently skipped instead. And if it is
    wxBase64DecodeMode_Relaxed, then all invalid characters are skipped.

    @param posErr
    If this pointer is non-@NULL and an error occurs during
    decoding, it is filled with the index of the invalid character.
*/
size_t wxBase64Decode(void * dst, size_t dstLen,
                      const char * src,
                      size_t srcLen = wxNO_LEN,
                      wxBase64DecodeMode mode = wxBase64DecodeMode_Strict,
                      size_t posErr = @NULL);
wxMemoryBuffer wxBase64Decode(const char * src,
                              size_t srcLen = wxNO_LEN,
                              wxBase64DecodeMode mode = wxBase64DecodeMode_Strict,
                              size_t posErr = @NULL);
wxMemoryBuffer wxBase64Decode(const wxString& src,
                              wxBase64DecodeMode mode = wxBase64DecodeMode_Strict,
                              size_t posErr = @NULL);
//@}

