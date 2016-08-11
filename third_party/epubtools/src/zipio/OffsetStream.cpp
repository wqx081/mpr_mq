
#include <cstdio>
#include <cstring>
#include "OffsetStream.h"


OffsetStream::OffsetStream(IStreamBase* basicIO, UINT64 offset, UINT64 size)
: m_innerStream(basicIO)
, m_nOffset(offset)
, m_nSize(size)
{

}


OffsetStream::~OffsetStream()
{
    OffsetStream::Close();

    if (m_innerStream && !m_innerStream->IsSetFlagBit(STREAM_FLAG_SELF_DROP))
    {
        delete m_innerStream;
        m_innerStream = nullptr;
    }
}


void OffsetStream::SetBasicIOandPos(IStreamBase* basicIO, UINT64 offset, UINT64 size)
{
    OffsetStream::Close();

    if (m_innerStream && !m_innerStream->IsSetFlagBit(STREAM_FLAG_SELF_DROP))
    {
        delete m_innerStream;
    }

    m_innerStream = basicIO;
    m_nOffset = offset;
    m_nSize = size;
}


UINT64 OffsetStream::Size() const
{
    return m_nSize;
}


bool OffsetStream::Seekable() const
{
    if (m_innerStream)
    {
        return m_innerStream->Seekable();
    }

    return false;
}


bool OffsetStream::Open(int mode)
{
    if (m_status == STREAM_INVALID)
    {
        m_status = STREAM_OPENED;

        if (m_innerStream->Open(mode))
        {
            if (!OffsetStream::Error())
            {
                OffsetStream::Rewind();
                return true;
            }
        }

        m_status = STREAM_INVALID;
        return false;
    }

    return true;
}


INT64 OffsetStream::Tell()
{
    if (m_innerStream)
    {
        INT64 pos = m_innerStream->Tell();

        if (pos != -1)
        {
            return pos - m_nOffset;
        }
    }

    return -1;
}


void OffsetStream::Rewind()
{
    if (m_innerStream)
    {
        m_innerStream->Seek(m_nOffset, SEEK_SET);
    }
}


int OffsetStream::Seek(INT64 offset, int fromWhere)
{
    if (!OffsetStream::Seekable())
        return -1;

    if (m_innerStream)
    {
        INT64 pos = m_nOffset;

        switch (fromWhere)
        {
        case SEEK_SET:
            pos += offset;
            break;
        case SEEK_CUR:
            pos = m_innerStream->Tell();
            if (pos == -1)
                return -1;
            pos += offset;
            break;
        case SEEK_END:
            pos += m_nSize + offset;
            break;
        default:
            return -1;
        }

        return m_innerStream->Seek(pos, SEEK_SET);
    }

    return -1;
}


int OffsetStream::Read(void* buffer, int bufSize)
{
    if (!buffer || bufSize <= 0)
        return -1;

    if (OffsetStream::Error()) //ERROR
        return -1;

    if (OffsetStream::Eof()) // EOF
        return 0;

    INT64 pos = m_innerStream->Tell();

    if ((UINT64)pos < m_nOffset)
        return -1;

    if ((UINT64)pos >= m_nOffset + m_nSize)
        return 0;

    int nRead = (int)(m_nOffset + m_nSize - pos);

    if (nRead > bufSize)
    {
        nRead = bufSize;
    }

    return m_innerStream->Read(buffer, nRead);
}


void OffsetStream::Close()
{
    if (m_innerStream)
    {
        if (!m_innerStream->IsSetFlagBit(STREAM_FLAG_SELF_CLOSE))
        {
            m_innerStream->Close();
        }
    }

    m_status = STREAM_INVALID;
}


bool OffsetStream::Eof() const
{
    if (m_innerStream)
    {
        INT64 pos = m_innerStream->Tell();

        if (pos == -1)
            return false;

        if ((UINT64)pos >= m_nOffset + m_nSize)
            return true;
    }

    return false;
}


bool OffsetStream::Error() const
{
    return  m_status == STREAM_INVALID  ||
            m_nSize == 0                ||
            m_innerStream == nullptr    ||
            m_innerStream->Error()      ||
            m_nOffset + m_nSize > m_innerStream->Size();
}

