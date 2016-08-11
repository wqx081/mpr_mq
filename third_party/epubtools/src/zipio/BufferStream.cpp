
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "BufferStream.h"


BufferStream::BufferStream(unsigned char* buffer, UINT64 bufSize, TFreePolicy freePolicy)
: m_buffer(buffer)
, m_nBufSize(bufSize)
, m_fnBufFreePolicy(freePolicy)
, m_nCurPos(0)
{

}


BufferStream::~BufferStream()
{
    BufferStream::Close();

    if (m_fnBufFreePolicy)
    {
        (*m_fnBufFreePolicy)(m_buffer, m_nBufSize);
    }
    else if (IsSetFlagBit(STREAM_FLAG_BUFFER_FREE))
    {
        free(m_buffer);
    }
}


void BufferStream::SetBuffer(unsigned char* buffer, UINT64 bufSize, TFreePolicy freePolicy)
{
    BufferStream::Close();

    if (m_fnBufFreePolicy)
    {
        (*m_fnBufFreePolicy)(m_buffer, m_nBufSize);
    }
    else if (IsSetFlagBit(STREAM_FLAG_BUFFER_FREE))
    {
        free(m_buffer);
    }

    m_nCurPos = 0;
    m_buffer = buffer;
    m_nBufSize = bufSize;
    m_fnBufFreePolicy = freePolicy;
}


UINT64 BufferStream::Size() const
{
    return m_nBufSize;
}


bool BufferStream::Seekable() const
{
    return true;
}


bool BufferStream::Open(int /*mode*/)
{
    if (m_status == STREAM_INVALID)
    {
        m_status = STREAM_OPENED;

        if (!BufferStream::Error())
        {
            BufferStream::Rewind();
            return true;
        }

        m_status = STREAM_INVALID;
        return false;
    }

    return true;
}


INT64 BufferStream::Tell()
{
    return m_nCurPos;
}


void BufferStream::Rewind()
{
    m_nCurPos = 0;
}


int BufferStream::Seek(INT64 offset, int fromWhere)
{
    if (!BufferStream::Seekable())
        return -1;

    switch (fromWhere)
    {
    case SEEK_SET:
        m_nCurPos = offset;
        break;
    case SEEK_CUR:
        m_nCurPos += offset;
        break;
    case SEEK_END:
        m_nCurPos = m_nBufSize + offset;
        break;
    default:
        return -1;
    }

    return 0;
}


int BufferStream::Read(void* buffer, int bufSize)
{
    if (!buffer || bufSize <= 0)
        return -1;

    if (BufferStream::Error()) // ERROR
        return -1;

    if (BufferStream::Eof()) // EOF
        return 0;

    int nRead = (int)(m_nBufSize - m_nCurPos);

    if (nRead > bufSize)
    {
        nRead = bufSize;
    }

    memcpy(buffer, m_buffer + m_nCurPos, nRead);

    m_nCurPos += nRead;

    return nRead;
}


void BufferStream::Close()
{
    m_status = STREAM_INVALID;
}


bool BufferStream::Eof() const
{
    return m_nCurPos >= m_nBufSize;
}


bool BufferStream::Error() const
{
    return  m_status == STREAM_INVALID ||
            m_buffer == nullptr        ||
            m_nBufSize == 0;
}

