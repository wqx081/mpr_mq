#include "FileStream.h"
#include "TUtf8Util.h"


#ifdef _WIN32

#include <windows.h>

static std::string UTF8ToLocal(const std::string& utf8_str)
{
    std::string retStr;

    if (!utf8_str.empty())
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
        if (len > 0)
        {
            WCHAR * wStr = new WCHAR[len+1];
            if (wStr)
            {
                MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, wStr, len);

                len = WideCharToMultiByte(CP_ACP, 0, wStr, -1, nullptr, 0, nullptr, nullptr);
                if (len > 0)
                {
                    char * localStr = new char[len+1];
                    if (localStr)
                    {
                        WideCharToMultiByte(CP_ACP, 0, wStr, -1, localStr, len, nullptr, nullptr);

                        retStr = localStr;

                        delete [] localStr;
                    }
                }

                delete [] wStr;
            }
        }
    }

    return retStr;
}

#endif



FileStream::FileStream(const std::string& fileName)
: m_filePath(fileName)
, m_hFile(nullptr)
, m_nSize(0)
{

}


FileStream::~FileStream()
{
    FileStream::Close();
}


void FileStream::SetFileName(const char* fileName)
{
    FileStream::Close();

    m_filePath = fileName;
}


UINT64 FileStream::Size() const
{
    return m_nSize;
}


bool FileStream::Seekable() const
{
    return true;
}


bool FileStream::Open(int mode)
{
    if (!m_hFile)
    {
        if (!m_filePath.empty())
        {
            m_openMode = mode;

            char szMode[10] = "rb";
            if (mode & STREAM_OPEN_MODE_READ)
            {
                strcpy(szMode, "r");
                if (mode & STREAM_OPEN_MODE_WRITE)
                {
                    strcpy(szMode, "r+");
                }
            }
            else if (mode & STREAM_OPEN_MODE_WRITE)
            {
                strcpy(szMode, "w");
            }

            if (mode & STREAM_OPEN_MODE_BIN)
            {
                strcat(szMode, "b");
            }

#ifdef _WIN32
            std::wstring wfile, wmode;
            wfile = TUtf8Util::UTF8StrToUniStr(m_filePath);
            wmode = TUtf8Util::UTF8StrToUniStr(szMode);
            m_hFile = _wfopen(wfile.c_str(), wmode.c_str());
          //_wfopen_s(&m_hFile, wfile.c_str(), wmode.c_str());
#else
            m_hFile = fopen(m_filePath.c_str(), szMode);
#endif
            if (m_hFile)
            {
                fseek(m_hFile, 0, SEEK_END);
                m_nSize = ftell(m_hFile);
                fseek(m_hFile, 0, SEEK_SET);
                m_status = STREAM_OPENED;
            }
        }
    }

    return m_hFile != nullptr;
}


INT64 FileStream::Tell()
{
    if (m_hFile)
    {
        return ftell(m_hFile);
    }

    return -1;
}


void FileStream::Rewind()
{
    if (m_hFile)
    {
        rewind(m_hFile);
    }
}


int FileStream::Seek(INT64 offset, int fromWhere)
{
    if (m_hFile)
    {
        return fseek(m_hFile, (long)offset, fromWhere);
    }

    return -1;
}


int FileStream::Read(void* buffer, int bufSize)
{
    if (m_hFile)
    {
        return fread(buffer, 1, bufSize, m_hFile);
    }

    return -1;
}


int FileStream::Write(void* buffer, int bufSize)
{
    if (m_hFile)
    {
        return fwrite(buffer, 1, bufSize, m_hFile);
    }

    return -1;
}


void FileStream::Close()
{
    if (m_hFile)
    {
        fclose(m_hFile);
        m_hFile = nullptr;
        m_nSize = 0;
        m_status = STREAM_INVALID;
    }
}


bool FileStream::Eof() const
{
    if (m_hFile)
    {
        return feof(m_hFile) != 0;
    }

    return true;
}


bool FileStream::Error() const
{
    if (m_hFile)
    {
        return ferror(m_hFile) != 0;
    }

    return true;
}

