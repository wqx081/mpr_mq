
#include "IStreamBase.h"
#include "./internal/ioapi.h"

static voidpf ZCALLBACK stream_open_func(voidpf opaque, const void* /*filename*/, int mode)
{
    IStreamBase* streamHandler = (IStreamBase*)opaque;

    if (streamHandler)
    {
        if (streamHandler->Open(mode))
        {
            return opaque;
        }
    }

    return nullptr;
}


static uLong ZCALLBACK stream_read_func(voidpf /*opaque*/, voidpf stream, void* buf, uLong size)
{
    IStreamBase* streamHandler = (IStreamBase*)stream;

    if (streamHandler)
    {
        int nRead = streamHandler->Read(buf, size);

        if (nRead > 0)
        {
            return nRead;
        }
    }

    return 0;
}


static uLong ZCALLBACK stream_write_func(voidpf /*opaque*/, voidpf /*stream*/, const void* /*buf*/,uLong /*size*/)
{
    return 0;
}


static ZPOS64_T ZCALLBACK stream_tell_func(voidpf /*opaque*/, voidpf stream)
{
    IStreamBase* streamHandler = (IStreamBase*)stream;

    if (streamHandler)
    {
        INT64 pos = streamHandler->Tell();

        if (pos > 0)
        {
            return pos;
        }
    }

    return 0;
}


static long ZCALLBACK stream_seek_func(voidpf /*opaque*/, voidpf stream, ZPOS64_T offset, int origin)
{
    IStreamBase* streamHandler = (IStreamBase*)stream;

    if (streamHandler)
    {
        return streamHandler->Seek(offset, origin);
    }

    return -1;
}


static int ZCALLBACK stream_close_func(voidpf /*opaque*/, voidpf stream)
{
    IStreamBase* streamHandler = (IStreamBase*)stream;

    if (streamHandler)
    {
        streamHandler->Close();

        return 0;
    }

    return -1;
}


static int ZCALLBACK stream_error_func(voidpf /*opaque*/, voidpf stream)
{
    IStreamBase* streamHandler = (IStreamBase*)stream;

    if (streamHandler)
    {
        if (!streamHandler->Error())
        {
            return 0;
        }
    }

    return -1;
}


void fill_stream_filefunc(zlib_filefunc64_def*  pzlib_filefunc_def, voidpf cookie)
{
    pzlib_filefunc_def->zopen64_file = stream_open_func;
    pzlib_filefunc_def->zread_file   = stream_read_func;
    pzlib_filefunc_def->zwrite_file  = stream_write_func;
    pzlib_filefunc_def->ztell64_file = stream_tell_func;
    pzlib_filefunc_def->zseek64_file = stream_seek_func;
    pzlib_filefunc_def->zclose_file  = stream_close_func;
    pzlib_filefunc_def->zerror_file  = stream_error_func;
    pzlib_filefunc_def->opaque       = cookie;
}
