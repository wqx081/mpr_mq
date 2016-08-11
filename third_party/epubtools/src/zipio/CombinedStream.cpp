
#include <cstdio>
#include <cstring>
#include "CombinedStream.h"


CombinedStream::CombinedStream(const std::vector<IStreamBase*>& streamSet)
: m_curStreamIndex(-1)
, m_innerStreamSet(streamSet)
{

}


CombinedStream::~CombinedStream()
{
    CombinedStream::Close();

    TryToFreeInnerStreams();
}


void CombinedStream::SetBasicStreamSet(const std::vector<IStreamBase*>& streamSet)
{
    CombinedStream::Close();

    TryToFreeInnerStreams();

    m_innerStreamSet = streamSet;
    m_curStreamIndex = -1;
}


bool CombinedStream::AppendStream(IStreamBase* stream, bool bUnique)
{
    if (stream)
    {
        if (bUnique)
        {
            for (std::vector<IStreamBase*>::iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
            {
                if ((*it) == stream)
                {
                    return false;
                }
            }
        }

        m_innerStreamSet.push_back(stream);
        return true;
    }
    return false;
}


bool CombinedStream::RemoveStream(IStreamBase* stream)
{
    if (stream)
    {
        for (std::vector<IStreamBase*>::iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
        {
            if ((*it) == stream)
            {
                m_innerStreamSet.erase(it);
                return true;
            }
        }
    }
    return false;
}


void CombinedStream::TryToFreeInnerStreams()
{
    for (std::vector<IStreamBase*>::iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
    {
        if (!(*it)->IsSetFlagBit(STREAM_FLAG_SELF_DROP))
        {
            delete (*it);
        }
    }
}


UINT64 CombinedStream::Size() const
{
    UINT64 nSize = 0;

    for (std::vector<IStreamBase*>::const_iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
    {
        IStreamBase* innerStream = *it;
        if (innerStream)
        {
            nSize += innerStream->Size();
        }
    }

    return nSize;
}


bool CombinedStream::Seekable() const
{
    bool bSeekable = true;

    for (std::vector<IStreamBase*>::const_iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
    {
        IStreamBase* innerStream = *it;
        if (innerStream)
        {
            bSeekable = bSeekable && innerStream->Seekable();
            if (!bSeekable)
            {
                break;
            }
        }
    }

    return bSeekable;
}


bool CombinedStream::Open(int mode)
{
    if (m_status == STREAM_INVALID)
    {
        m_status = STREAM_OPENED;

        bool bOpenSuccess = true;
        for (std::vector<IStreamBase*>::iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
        {
            IStreamBase* innerStream = *it;
            if (innerStream)
            {
                bOpenSuccess = bOpenSuccess && innerStream->Open(mode);
                if (!bOpenSuccess)
                {
                    break;
                }
            }
        }

        if (bOpenSuccess)
        {
            CombinedStream::Rewind();

            if (!CombinedStream::Error())
            {
                return true;
            }
        }

        m_status = STREAM_INVALID;
        return false;
    }

    return true;
}


INT64 CombinedStream::Tell()
{
    if (m_curStreamIndex >= 0 && m_curStreamIndex < (int)m_innerStreamSet.size())
    {
        INT64 tPos = m_innerStreamSet[m_curStreamIndex]->Tell();
        if (tPos >= 0)
        {
            UINT64 nPos = 0;
            for (int i=0; i<m_curStreamIndex; i++)
            {
                IStreamBase* innerStream = m_innerStreamSet[i];
                if (innerStream)
                {
                    nPos += innerStream->Size();
                }
            }
            return nPos + tPos;
        }
    }
    return -1;
}


void CombinedStream::Rewind()
{
    m_curStreamIndex = 0;

    for (std::vector<IStreamBase*>::iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
    {
        IStreamBase* innerStream = *it;
        if (innerStream)
        {
            innerStream->Rewind();
        }
    }
}


int CombinedStream::Seek(INT64 offset, int fromWhere)
{
    //if (!CombinedStream::Seekable())
    //    return -1;

    switch (fromWhere)
    {
    case SEEK_SET:
        {
            if (offset >= 0LL)
            {
                m_curStreamIndex = 0;
                while (m_curStreamIndex < (int)m_innerStreamSet.size())
                {
                    UINT64 nSize = m_innerStreamSet[m_curStreamIndex]->Size();
                    if ((UINT64)offset <= nSize)
                    {
                        return m_innerStreamSet[m_curStreamIndex]->Seek(offset, SEEK_SET);
                    }
                    else
                    {
                        offset -= nSize;
                        m_curStreamIndex++;
                    }
                }
            }
            return -1;
        }
    case SEEK_CUR:
        {
            if (m_curStreamIndex >= 0 && m_curStreamIndex < (int)m_innerStreamSet.size())
            {
                INT64 nPos = m_innerStreamSet[m_curStreamIndex]->Tell();
                if (nPos < 0)
                    return -1;

                if (offset > 0LL)
                {
                    offset += nPos;
                    while (m_curStreamIndex < (int)m_innerStreamSet.size())
                    {
                        UINT64 nSize = m_innerStreamSet[m_curStreamIndex]->Size();
                        if ((UINT64)offset <= nSize)
                        {
                            return m_innerStreamSet[m_curStreamIndex]->Seek(offset, SEEK_SET);
                        }
                        else
                        {
                            offset -= nSize;
                            m_curStreamIndex++;
                        }
                    }
                }
                else if (offset < 0LL)
                {
                    INT64 nSize = m_innerStreamSet[m_curStreamIndex]->Size();

                    offset -= (nSize - nPos);
                    while (m_curStreamIndex >= 0)
                    {
                        nSize = m_innerStreamSet[m_curStreamIndex]->Size();
                        if (nSize + offset >= 0)
                        {
                            return m_innerStreamSet[m_curStreamIndex]->Seek(offset, SEEK_END);
                        }
                        else
                        {
                            offset += nSize;
                            m_curStreamIndex--;
                        }
                    }
                }
                else
                {
                    return 0;
                }
            }
            return -1;
        }
    case SEEK_END:
        {
            if (offset <= 0LL)
            {
                m_curStreamIndex = m_innerStreamSet.size() - 1;
                while (m_curStreamIndex >= 0)
                {
                    INT64 nSize = m_innerStreamSet[m_curStreamIndex]->Size();
                    if (nSize + offset >= 0)
                    {
                        return m_innerStreamSet[m_curStreamIndex]->Seek(offset, SEEK_END);
                    }
                    else
                    {
                        offset += nSize;
                        m_curStreamIndex--;
                    }
                }
            }
            return -1;
        }
    default:
        return -1;
    }
}


int CombinedStream::Read(void* buffer, int bufSize)
{
    if (!buffer || bufSize <= 0)
        return -1;

    if (CombinedStream::Error()) //ERROR
        return -1;

    if (CombinedStream::Eof()) // EOF
        return 0;

    int nTotalRead = 0;
    int nRestRead = bufSize;

    while (nRestRead > 0)
    {
        IStreamBase* curInnerStream = m_innerStreamSet[m_curStreamIndex];
        //ASSERT(curInnerStream != nullptr);

        int nThisRead = curInnerStream->Read((char*)buffer + nTotalRead, nRestRead);

        if (nThisRead < 0) // error ?
            return -1;

        nRestRead  -= nThisRead;
        nTotalRead += nThisRead;

        if (curInnerStream->Eof())
        {
            if (++m_curStreamIndex < (int)m_innerStreamSet.size())
            {
                m_innerStreamSet[m_curStreamIndex]->Rewind();
            }
            else
            {
                break;
            }
        }
    }

    return nTotalRead;
}


void CombinedStream::Close()
{
    for (std::vector<IStreamBase*>::iterator it = m_innerStreamSet.begin(); it != m_innerStreamSet.end(); ++it)
    {
        IStreamBase* innerStream = *it;
        if (innerStream && !innerStream->IsSetFlagBit(STREAM_FLAG_SELF_CLOSE))
        {
            innerStream->Close();
        }
    }

    m_status = STREAM_INVALID;
}


bool CombinedStream::Eof() const
{
    if (m_curStreamIndex == m_innerStreamSet.size() - 1)
    {
        return m_innerStreamSet[m_curStreamIndex]->Eof();
    }

    return false;
}


bool CombinedStream::Error() const
{
    int nStreamCount = m_innerStreamSet.size();

    return  m_status == STREAM_INVALID       ||
            nStreamCount == 0                ||
            m_curStreamIndex < 0             ||
            m_curStreamIndex >= nStreamCount ||
            m_innerStreamSet[m_curStreamIndex]->Error();
}

