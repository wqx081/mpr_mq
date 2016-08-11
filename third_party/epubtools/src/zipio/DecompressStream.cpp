
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "DecompressStream.h"


DecompressStream::DecompressStream(IStreamBase* basicIO, TZipItemInfo* zipItemInfo)
: m_innerStream(basicIO)
, m_zipItemInfo(zipItemInfo)
, m_inflateBuffer(nullptr)
, m_rest_uncompressed_size(0)
{

}


DecompressStream::~DecompressStream()
{
    DecompressStream::Close();

    if (m_innerStream && !m_innerStream->IsSetFlagBit(STREAM_FLAG_SELF_DROP))
    {
        delete m_innerStream;
        m_innerStream = nullptr;
    }

    if (m_inflateBuffer)
    {
        free(m_inflateBuffer);
        m_inflateBuffer = nullptr;
    }
}


void DecompressStream::SetBasicStreamAndItemInfo(IStreamBase* basicIO, TZipItemInfo* zipItemInfo)
{
    DecompressStream::Close();

    if (m_innerStream && !m_innerStream->IsSetFlagBit(STREAM_FLAG_SELF_DROP))
    {
        delete m_innerStream;
    }

    m_innerStream = basicIO;
    m_zipItemInfo = zipItemInfo;
}


UINT64 DecompressStream::Size() const
{
    if (m_innerStream && m_zipItemInfo)
    {
        return m_zipItemInfo->uncompressed_size;
    }

    return 0;
}


bool DecompressStream::Seekable() const
{
    return false/*m_innerStream->Seekable()*/;
}


bool DecompressStream::Open(int mode)
{
    if (m_status == STREAM_INVALID)
    {
        m_status = STREAM_OPENED;

        if (m_innerStream->Open(mode))
        {
            if (!DecompressStream::Error())
            {
                m_inflateStream.zalloc = (alloc_func)0;
                m_inflateStream.zfree = (free_func)0;
                m_inflateStream.opaque = (voidpf)0;
                m_inflateStream.next_in = (Bytef*)0;
                m_inflateStream.avail_in = (uInt)0;
                m_inflateStream.total_in = 0;
                m_inflateStream.total_out = 0;

                if (inflateInit2(&m_inflateStream, -MAX_WBITS) == Z_OK)
                {
                    DecompressStream::Rewind();
                    return true;
                }
            }
        }

        m_status = STREAM_INVALID;
        return false;
    }

    return true;
}


INT64 DecompressStream::Tell()
{
    if (m_innerStream && m_zipItemInfo)
    {
        return m_zipItemInfo->uncompressed_size - m_rest_uncompressed_size;
    }

    return -1;
}


void DecompressStream::Rewind()
{
    if (m_innerStream && m_zipItemInfo)
    {
        m_innerStream->Rewind();

        m_inflateStream.next_in = (Bytef*)0;
        m_inflateStream.avail_in = (uInt)0;
        m_inflateStream.total_in = 0;
        m_inflateStream.total_out = 0;

        m_rest_uncompressed_size = m_zipItemInfo->uncompressed_size;
    }
}


int DecompressStream::Seek(INT64 /*offset*/, int /*fromWhere*/)
{
    return -1;
}


int DecompressStream::Read(void* buffer, int bufSize)
{
    if (!buffer || bufSize <= 0)
        return -1;

    if (DecompressStream::Error()) //ERROR
        return -1;

    if (DecompressStream::Eof()) // EOF
        return 0;

    if (!m_inflateBuffer)
    {
        m_inflateBuffer = (char*)malloc(UNZ_BUFSIZE);
        //ASSERT(m_inflateBuffer);
    }

    m_inflateStream.next_out = (Bytef*)buffer;
    m_inflateStream.avail_out = (uInt)bufSize;

    if (m_rest_uncompressed_size < bufSize)
    {
        m_inflateStream.avail_out = (uInt)m_rest_uncompressed_size;
    }

    int nTotalRead = 0;

    while (m_inflateStream.avail_out > 0)
    {
        if (m_inflateStream.avail_in == 0)
        {
            int nThisRead = m_innerStream->Read(m_inflateBuffer, UNZ_BUFSIZE);

            if (nThisRead == 0) // EOF ?
                return nTotalRead;
            else if (nThisRead < 0) // ERROR ?
                return -1;

            m_inflateStream.next_in = (Bytef*)m_inflateBuffer;
            m_inflateStream.avail_in = (uInt)nThisRead;
        }

        UINT64 uTotalOutBefore = m_inflateStream.total_out;
        //const Bytef *bufBefore = m_inflateStream.next_out;

        int err = inflate(&m_inflateStream, Z_SYNC_FLUSH);

        //if ((err >= 0) && (m_inflateStream.msg != nullptr))
        //    err = Z_DATA_ERROR;

        UINT64 uOutThis = m_inflateStream.total_out - uTotalOutBefore;

        //m_zipItemInfo->crc = crc32(m_zipItemInfo->crc, bufBefore, (uInt)(uOutThis));

        m_rest_uncompressed_size -= uOutThis;

        nTotalRead += (int)uOutThis;

        if (err == Z_STREAM_END)
            return nTotalRead;

        if (err != Z_OK)
            return -1;
    }

    return nTotalRead;
}


void DecompressStream::Close()
{
    if (m_innerStream)
    {
        if (!m_innerStream->IsSetFlagBit(STREAM_FLAG_SELF_CLOSE))
        {
            m_innerStream->Close();
        }
    }

    if (m_inflateBuffer)
    {
        free(m_inflateBuffer);
        m_inflateBuffer = nullptr;
    }

    if (m_status != STREAM_INVALID)
    {
        m_status = STREAM_INVALID;
        inflateEnd(&m_inflateStream);
    }
}


bool DecompressStream::Eof() const
{
    return m_rest_uncompressed_size == 0;
}


bool DecompressStream::Error() const
{
    return  m_status == STREAM_INVALID                      ||
            m_zipItemInfo == nullptr                        ||
            m_zipItemInfo->compression_method != Z_DEFLATED ||
            m_innerStream == nullptr                        ||
            m_innerStream->Error()                          ||
            m_innerStream->Size() != m_zipItemInfo->compressed_size;
}

