///////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/filefn.cpp
// Purpose:     File system functions for classic Mac OS (Retro68)
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif

#include "wx/filefn.h"
#include "wx/retro68/crt.h"
#include "wx/retro68/private.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

// ============================================================================
// helpers
// ============================================================================

void wxMacStringToPascal(const wxString& from, StringPtr to)
{
    const wxCharBuffer buf = from.mb_str(wxConvLibc);

    const char* str;
    size_t len;
    if ( buf.data() )
    {
        str = buf.data();
        len = buf.length();
    }
    else // Conversion failed, use lossy conversion.
    {
        wxString ascii;
        for ( wxString::const_iterator it = from.begin(); it != from.end(); ++it )
            ascii += (*it).IsAscii() ? *it : wxUniChar('?');

        const wxCharBuffer bufAscii = ascii.ToAscii();
        const size_t lenAscii = bufAscii.length() > 255 ? 255 : bufAscii.length();
        memcpy(to + 1, bufAscii.data(), lenAscii);
        to[0] = static_cast<unsigned char>(lenAscii);
        return;
    }

    if ( len > 255 )
        len = 255;

    to[0] = static_cast<unsigned char>(len);
    memcpy(to + 1, str, len);
}

wxString wxMacMakeStringFromPascal(ConstStringPtr from)
{
    return wxString(reinterpret_cast<const char*>(from + 1), wxConvLibc, from[0]);
}

bool wxMacCToPascal(const char* from, StringPtr to)
{
    const size_t len = strlen(from);
    if ( len > 255 )
        return false;

    to[0] = static_cast<unsigned char>(len);
    memcpy(to + 1, from, len);

    return true;
}

int wxMacSetErrno(OSErr err)
{
    switch ( err )
    {
        case noErr:
            return 0;

        case fnfErr:
        case dirNFErr:
        case nsvErr:
        case bdNamErr:
        case paramErr:
            errno = ENOENT;
            break;

        case dupFNErr:
            errno = EEXIST;
            break;

        case fBsyErr:
            errno = EBUSY;
            break;

        case wPrErr:
        case vLckdErr:
        case fLckdErr:
        case permErr:
        case wrPermErr:
        case -5000: // afpAccessDenied
            errno = EACCES;
            break;

        case dskFulErr:
            errno = ENOSPC;
            break;

        case memFullErr:
            errno = ENOMEM;
            break;

        case badMovErr:
            errno = EXDEV;
            break;

        case tmfoErr:
            errno = EMFILE;
            break;

        default:
            errno = EIO;
    }

    return -1;
}

OSErr wxMacPathToFSSpec(const char* path, FSSpec* spec)
{
    if ( !path || !*path )
        return bdNamErr;

    Str255 pname;
    if ( !wxMacCToPascal(path, pname) )
        return bdNamErr;

    return FSMakeFSSpec(0, 0, pname, spec);
}

OSErr wxMacPathToFSSpec(const wxString& path, FSSpec* spec)
{
    const wxCharBuffer buf = path.mb_str(wxConvFile);
    if ( !buf.data() )
        return bdNamErr;

    return wxMacPathToFSSpec(buf.data(), spec);
}

OSErr wxMacGetCatInfo(const FSSpec& spec, CInfoPBRec& pb, Str255 name)
{
    memcpy(name, spec.name, spec.name[0] + 1);

    memset(&pb, 0, sizeof(pb));
    pb.hFileInfo.ioNamePtr = name;
    pb.hFileInfo.ioVRefNum = spec.vRefNum;
    pb.hFileInfo.ioDirID = spec.parID;
    pb.hFileInfo.ioFDirIndex = 0;

    return PBGetCatInfoSync(&pb);
}

wxString wxMacDirToPath(short vRefNum, long dirID)
{
    wxString path;

    Str255 name;
    CInfoPBRec pb;
    for ( ;; )
    {
        memset(&pb, 0, sizeof(pb));
        name[0] = 0;
        pb.dirInfo.ioNamePtr = name;
        pb.dirInfo.ioVRefNum = vRefNum;
        pb.dirInfo.ioDrDirID = dirID;
        // Negative index means to get information about the directory itself.
        pb.dirInfo.ioFDirIndex = -1;

        if ( PBGetCatInfoSync(&pb) != noErr )
            return wxString();

        path.Prepend(wxMacMakeStringFromPascal(name) + wxFILE_SEP_PATH);

        if ( dirID == wxMAC_FS_ROOT_DIR_ID )
            break;

        dirID = pb.dirInfo.ioDrParID;
    }

    return path;
}

wxString wxMacFSSpecToPath(const FSSpec& spec)
{
    if ( spec.parID == wxMAC_FS_ROOT_PARENT_ID )
    {
        // This is the root directory of the volume.
        return wxMacMakeStringFromPascal(spec.name) + wxFILE_SEP_PATH;
    }

    wxString path = wxMacDirToPath(spec.vRefNum, spec.parID);
    if ( path.empty() )
        return path;

    path += wxMacMakeStringFromPascal(spec.name);

    // Append the trailing separator to the directories, as is customary
    // under Mac OS, to make it possible to distinguish them from files.
    CInfoPBRec pb;
    Str255 name;
    if ( wxMacGetCatInfo(spec, pb, name) == noErr &&
            (pb.hFileInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_DIR) )
    {
        path += wxFILE_SEP_PATH;
    }

    return path;
}

wxString wxMacFindFolder(OSType folderType, bool create)
{
    short vRefNum;
    long dirID;
    if ( FindFolder(wxMAC_ON_SYSTEM_DISK, folderType, create,
                    &vRefNum, &dirID) != noErr )
    {
        return wxString();
    }

    return wxMacDirToPath(vRefNum, dirID);
}

bool wxMacGetApplicationSpec(FSSpec* spec)
{
    ProcessSerialNumber psn = { 0, kCurrentProcess };

    ProcessInfoRec info;
    memset(&info, 0, sizeof(info));
    info.processInfoLength = sizeof(info);
    info.processAppSpec = spec;

    return GetProcessInformation(&psn, &info) == noErr;
}

long wxMacGestalt(OSType selector)
{
    long response = 0;
    if ( Gestalt(selector, &response) != noErr )
        return 0;

    return response;
}

// ============================================================================
// POSIX functions replacements
// ============================================================================

namespace
{

// Get the FSSpec and catalog information for the existing file or directory,
// setting errno and returning false if it doesn't exist.
bool GetExistingInfo(const char* path, FSSpec& spec, CInfoPBRec& pb, Str255 name)
{
    OSErr err = wxMacPathToFSSpec(path, &spec);
    if ( err == noErr )
        err = wxMacGetCatInfo(spec, pb, name);

    if ( err != noErr )
    {
        wxMacSetErrno(err);
        return false;
    }

    return true;
}

inline bool IsDir(const CInfoPBRec& pb)
{
    return (pb.hFileInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_DIR) != 0;
}

inline bool IsLocked(const CInfoPBRec& pb)
{
    return (pb.hFileInfo.ioFlAttrib & wxMAC_IOFL_ATTRIB_LOCKED) != 0;
}

bool IsVolumeLocked(short vRefNum)
{
    HParamBlockRec pb;
    memset(&pb, 0, sizeof(pb));
    pb.volumeParam.ioVRefNum = vRefNum;
    if ( PBHGetVInfoSync(&pb) != noErr )
        return false;

    // Bit 15 is set for the hardware-locked volumes and bit 7 for the
    // software-locked ones.
    return (pb.volumeParam.ioVAtrb & 0x8080) != 0;
}

inline time_t MacTimeToUnix(unsigned long t)
{
    return t >= wxMAC_EPOCH_OFFSET ? static_cast<time_t>(t - wxMAC_EPOCH_OFFSET)
                                   : 0;
}

inline unsigned long UnixTimeToMac(time_t t)
{
    return static_cast<unsigned long>(t) + wxMAC_EPOCH_OFFSET;
}

} // anonymous namespace

int wxRetro68Stat(const char* path, struct stat* st)
{
    FSSpec spec;
    CInfoPBRec pb;
    Str255 name;
    if ( !GetExistingInfo(path, spec, pb, name) )
        return -1;

    memset(st, 0, sizeof(*st));

    st->st_dev = spec.vRefNum;
    st->st_nlink = 1;

    // There are no "execute" permissions under Mac OS, so we consider that
    // the directories are always "executable" (i.e. can be entered) and only
    // the applications are executable among the files.
    mode_t perms = S_IRUSR | S_IRGRP | S_IROTH;
    if ( !IsLocked(pb) && !IsVolumeLocked(spec.vRefNum) )
        perms |= S_IWUSR | S_IWGRP | S_IWOTH;

    if ( IsDir(pb) )
    {
        st->st_ino = pb.dirInfo.ioDrDirID;
        st->st_mode = S_IFDIR | perms | S_IXUSR | S_IXGRP | S_IXOTH;
        st->st_mtime = MacTimeToUnix(pb.dirInfo.ioDrMdDat);
        st->st_ctime = MacTimeToUnix(pb.dirInfo.ioDrCrDat);
        st->st_atime = st->st_mtime;
    }
    else
    {
        st->st_ino = pb.hFileInfo.ioDirID;
        st->st_mode = S_IFREG | perms;
        if ( pb.hFileInfo.ioFlFndrInfo.fdType == 'APPL' )
            st->st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
        st->st_size = pb.hFileInfo.ioFlLgLen;
        st->st_blocks = (pb.hFileInfo.ioFlPyLen + pb.hFileInfo.ioFlRPyLen) / 512;
        st->st_blksize = 512;
        st->st_mtime = MacTimeToUnix(pb.hFileInfo.ioFlMdDat);
        st->st_ctime = MacTimeToUnix(pb.hFileInfo.ioFlCrDat);
        st->st_atime = st->st_mtime;
    }

    return 0;
}

int wxRetro68MkDir(const char* path, mode_t WXUNUSED(mode))
{
    FSSpec spec;
    OSErr err = wxMacPathToFSSpec(path, &spec);
    if ( err == noErr )
    {
        errno = EEXIST;
        return -1;
    }

    if ( err != fnfErr )
        return wxMacSetErrno(err);

    long dirID;
    return wxMacSetErrno(FSpDirCreate(&spec, smSystemScript, &dirID));
}

int wxRetro68RmDir(const char* path)
{
    FSSpec spec;
    CInfoPBRec pb;
    Str255 name;
    if ( !GetExistingInfo(path, spec, pb, name) )
        return -1;

    if ( !IsDir(pb) )
    {
        errno = ENOTDIR;
        return -1;
    }

    const OSErr err = FSpDelete(&spec);
    if ( err == fBsyErr )
    {
        // This is returned when the directory is not empty.
        errno = ENOTEMPTY;
        return -1;
    }

    return wxMacSetErrno(err);
}

int wxRetro68Remove(const char* path)
{
    FSSpec spec;
    CInfoPBRec pb;
    Str255 name;
    if ( !GetExistingInfo(path, spec, pb, name) )
        return -1;

    if ( IsDir(pb) )
        return wxRetro68RmDir(path);

    return wxMacSetErrno(FSpDelete(&spec));
}

int wxRetro68Rename(const char* oldpath, const char* newpath)
{
    FSSpec specOld;
    CInfoPBRec pb;
    Str255 name;
    if ( !GetExistingInfo(oldpath, specOld, pb, name) )
        return -1;

    FSSpec specNew;
    OSErr err = wxMacPathToFSSpec(newpath, &specNew);
    if ( err == noErr )
    {
        // As with POSIX rename(), replace the existing file, but not the
        // directory.
        CInfoPBRec pbNew;
        Str255 nameNew;
        err = wxMacGetCatInfo(specNew, pbNew, nameNew);
        if ( err != noErr )
            return wxMacSetErrno(err);

        if ( IsDir(pbNew) )
        {
            errno = EISDIR;
            return -1;
        }

        err = FSpDelete(&specNew);
        if ( err != noErr )
            return wxMacSetErrno(err);
    }
    else if ( err != fnfErr )
    {
        return wxMacSetErrno(err);
    }

    if ( specOld.vRefNum != specNew.vRefNum )
    {
        errno = EXDEV;
        return -1;
    }

    if ( specOld.parID != specNew.parID )
    {
        // Move the object to the new directory first.
        CMovePBRec pbMove;
        memset(&pbMove, 0, sizeof(pbMove));
        pbMove.ioNamePtr = specOld.name;
        pbMove.ioVRefNum = specOld.vRefNum;
        pbMove.ioDirID = specOld.parID;
        pbMove.ioNewDirID = specNew.parID;
        err = PBCatMoveSync(&pbMove);
        if ( err != noErr )
            return wxMacSetErrno(err);

        specOld.parID = specNew.parID;
    }

    if ( !EqualString(specOld.name, specNew.name, true, true) )
    {
        err = FSpRename(&specOld, specNew.name);
        if ( err != noErr )
            return wxMacSetErrno(err);
    }

    return 0;
}

int wxRetro68Access(const char* path, int mode)
{
    struct stat st;
    if ( wxRetro68Stat(path, &st) != 0 )
        return -1;

    if ( ((mode & W_OK) && !(st.st_mode & S_IWUSR)) ||
            ((mode & X_OK) && !(st.st_mode & S_IXUSR)) )
    {
        errno = EACCES;
        return -1;
    }

    return 0;
}

int wxRetro68Chmod(const char* path, mode_t mode)
{
    FSSpec spec;
    CInfoPBRec pb;
    Str255 name;
    if ( !GetExistingInfo(path, spec, pb, name) )
        return -1;

    // Directories can't be locked.
    if ( IsDir(pb) )
        return 0;

    // The only thing we can do is to lock or unlock the file.
    const bool lock = !(mode & S_IWUSR);
    if ( lock == IsLocked(pb) )
        return 0;

    return wxMacSetErrno(lock ? FSpSetFLock(&spec) : FSpRstFLock(&spec));
}

int wxRetro68Utime(const char* path, const struct utimbuf* times)
{
    FSSpec spec;
    CInfoPBRec pb;
    Str255 name;
    if ( !GetExistingInfo(path, spec, pb, name) )
        return -1;

    ULONGINT modTime;
    if ( times )
    {
        modTime = UnixTimeToMac(times->modtime);
    }
    else
    {
        // Note that GetDateTime() is a macro under 68K which we undefine in
        // wx/platform.h, so use this function instead.
        ReadDateTime(&modTime);
    }

    // PBSetCatInfo() uses ioDirID as input, so restore it after it was
    // overwritten by PBGetCatInfo().
    if ( IsDir(pb) )
    {
        pb.dirInfo.ioDrMdDat = modTime;
        pb.dirInfo.ioDrDirID = spec.parID;
    }
    else
    {
        pb.hFileInfo.ioFlMdDat = modTime;
        pb.hFileInfo.ioDirID = spec.parID;
    }

    return wxMacSetErrno(PBSetCatInfoSync(&pb));
}

char* wxRetro68GetCwd(char* buf, size_t size)
{
    WDPBRec pb;
    memset(&pb, 0, sizeof(pb));
    OSErr err = PBHGetVolSync(&pb);
    if ( err != noErr )
    {
        wxMacSetErrno(err);
        return nullptr;
    }

    const wxString path = wxMacDirToPath(pb.ioWDVRefNum, pb.ioWDDirID);
    const wxCharBuffer pathBuf = path.mb_str(wxConvFile);
    if ( path.empty() || !pathBuf.data() )
    {
        errno = ENOENT;
        return nullptr;
    }

    if ( pathBuf.length() >= size )
    {
        errno = ERANGE;
        return nullptr;
    }

    memcpy(buf, pathBuf.data(), pathBuf.length() + 1);

    return buf;
}

int wxRetro68ChDir(const char* path)
{
    FSSpec spec;
    CInfoPBRec pb;
    Str255 name;
    if ( !GetExistingInfo(path, spec, pb, name) )
        return -1;

    if ( !IsDir(pb) )
    {
        errno = ENOTDIR;
        return -1;
    }

    WDPBRec wdpb;
    memset(&wdpb, 0, sizeof(wdpb));
    wdpb.ioVRefNum = spec.vRefNum;
    wdpb.ioWDDirID = pb.dirInfo.ioDrDirID;

    return wxMacSetErrno(PBHSetVolSync(&wdpb));
}

int wxRetro68Eof(int fd)
{
    const off_t pos = lseek(fd, 0, SEEK_CUR);
    if ( pos == -1 )
        return -1;

    const off_t end = lseek(fd, 0, SEEK_END);
    lseek(fd, pos, SEEK_SET);

    return end == -1 ? -1 : pos >= end;
}
