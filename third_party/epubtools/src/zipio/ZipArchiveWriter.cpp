
#include <ctime>
#include "internal/zip.h"
#ifdef USEWIN32IOAPI
#include "internal/iowin32.h"
#include "TUtf8Util.h"
#endif
#include "FileStream.h"
#include "BufferStream.h"
#include "ZipArchiveWriter.h"


static std::string ReplaceWin32PathSep(const std::string& win32Path)
{
    std::string strPath(win32Path);
    for (int i=0; i<(int)strPath.size(); i++)
    {
        if (strPath[i] == '\\')
        {
            strPath[i] = '/';
        }
    }
    return strPath;
}


ZipArchiveWriter::ZipArchiveWriter(const std::string& zipFileName, bool bZipExist)
: m_errCode(Z_ERRNO)
, m_hZip(nullptr)
{
    Open(zipFileName, bZipExist);
}


ZipArchiveWriter::~ZipArchiveWriter()
{
    Close();
}


bool ZipArchiveWriter::Open(const std::string& zipFileName, bool bZipExist)
{
    if (zipFileName.empty())
        return false;

    Close();

    m_errCode = Z_OK;
    m_zipFilePath = zipFileName;

#ifdef USEWIN32IOAPI
    zlib_filefunc64_def ffunc;
    fill_win32_filefunc64W(&ffunc);
    m_hZip = zipOpen2_64(TUtf8Util::UTF8StrToUniStr(m_zipFilePath).c_str(), bZipExist ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE, nullptr, &ffunc);
#else
    m_hZip = zipOpen64(m_zipFilePath.c_str(), bZipExist ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE);
#endif

    if (!m_hZip)
    {
        m_errCode = Z_ZIP_OPEN_ERROR;
        return false;
    }

    return true;
}


bool ZipArchiveWriter::IsOpen() const
{
    return m_errCode == Z_OK;
}


void ZipArchiveWriter::Close()
{
    m_errCode = Z_ERRNO;

    if (m_hZip)
    {
        zipClose(m_hZip, nullptr);
        m_hZip = nullptr;
    }
}


int ZipArchiveWriter::AddFileItem(const std::string& sourceFile, const std::string& itemPath, int compressFlag)
{
    if (sourceFile.empty() || itemPath.empty())
        return -1;

    if (compressFlag != 0 && compressFlag != Z_DEFLATED)
        return -1;

    IStreamBase* inputStream = new FileStream(sourceFile);
    if (inputStream)
    {
        int ret = AddFileItem(inputStream, ReplaceWin32PathSep(itemPath), compressFlag);

        delete inputStream;
        inputStream = nullptr;

        return ret;
    }

    return -1;
}



int ZipArchiveWriter::AddFileItem(void* sourceBuf, int srclen, const std::string& itemPath, int compressFlag)
{
    if (!sourceBuf || srclen <= 0)
        return -1;

    if (compressFlag != 0 && compressFlag != Z_DEFLATED)
        return -1;

    IStreamBase* inputStream = new BufferStream((unsigned char*)sourceBuf, (UINT64)srclen);
    if (inputStream)
    {
        int ret = AddFileItem(inputStream, ReplaceWin32PathSep(itemPath), compressFlag);

        delete inputStream;
        inputStream = nullptr;

        return ret;
    }

    return -1;
}



int ZipArchiveWriter::AddFileItem(IStreamBase* inputStream, const std::string& strNameInZip, int compressFlag)
{
    if (!inputStream || !inputStream->Open())
        return -1;

    zip_fileinfo zi = {0};
    zi.dosDate = (uLong)time(nullptr);

    m_errCode = zipOpenNewFileInZip64(m_hZip, strNameInZip.c_str(), &zi, NULL, 0, NULL, 0, NULL, compressFlag, Z_DEFAULT_COMPRESSION, 1);
    if (m_errCode == ZIP_OK)
    {
        int nRead = 0;
        unsigned char buffer[32768];

        while (m_errCode == ZIP_OK)
        {
            nRead = inputStream->Read(buffer, sizeof(buffer)/sizeof(buffer[0]));
            if (nRead <= 0)
                break;

            m_errCode = zipWriteInFileInZip(m_hZip, buffer, nRead);
        }

        zipCloseFileInZip(m_hZip);

        if (m_errCode == ZIP_OK)
        {
            return 0;
        }
    }

    return -2;
}



int ZipArchiveWriter::BatchAddFilesIntoZip(const std::map<std::string, std::string>& mapNameInZip2SrcFilePath, const std::string& zipFileName, bool bZipExist)
{
    if (zipFileName.empty())
        return -1;

    if (mapNameInZip2SrcFilePath.empty())
        return 0;

    int ret = 0;
    int nSuccCount = 0;

    ZipArchiveWriter zipWriter;
    if (zipWriter.Open(zipFileName, bZipExist))
    {
        for (std::map<std::string, std::string>::const_iterator it = mapNameInZip2SrcFilePath.begin(); it != mapNameInZip2SrcFilePath.end(); ++it)
        {
            if (!it->first.empty())
            {
                FileStream inputStream(it->second);
                ret = zipWriter.AddFileItem(&inputStream, it->first, Z_DEFLATED);

                if (ret == 0) // OK
                {
                    nSuccCount++;
                }
                else if (ret == -1) // Input File IO error.
                {
                    // do nothing ...
                }
                else if (ret == -2) // Zip error.
                {
                    return -1;
                }
            }
        }
    }

    return nSuccCount;
}



int ZipArchiveWriter::AddDirItem(const std::string& dirPath)
{
    if (dirPath.empty())
        return -1;

    std::string folderNameInZip = ReplaceWin32PathSep(dirPath);

    std::string::size_type slen = folderNameInZip.size();

    if (folderNameInZip[slen - 1] != '/')
    {
        folderNameInZip.push_back('/');
    }

    zip_fileinfo zi = {0};
    m_errCode = zipOpenNewFileInZip64(m_hZip, folderNameInZip.c_str(), &zi, NULL, 0, NULL, 0, NULL, 0, Z_DEFAULT_COMPRESSION, 0);
    if (m_errCode == ZIP_OK)
    {
        m_errCode = zipCloseFileInZip(m_hZip);
    }

    return (m_errCode == ZIP_OK) ? 0 : -1;
}



int ZipArchiveWriter::DelFileItem(const std::string& /*itemPath*/)
{
    return -1;
}


int ZipArchiveWriter::DelDirItem(const std::string& /*dirPath*/)
{
    return -1;
}


int ZipArchiveWriter::DelItem(const std::string& /*itemPath*/)
{
    return -1;
}


int ZipArchiveWriter::DelItem(UINT64 /*itemIndex*/)
{
    return -1;
}


