
#include "internal/unzip.h"
#include "FileStream.h"
#include "BufferStream.h"
#include "OffsetStream.h"
#include "DecompressStream.h"
#include "ZipArchiveReader.h"


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


static std::string GetFileExtName(const std::string& filename)
{
    std::string extName;

    std::string::size_type pos = filename.find_last_of('.');
    if (pos != std::string::npos)
    {
        extName = filename.substr(pos);
        for (int i=1; i<(int)extName.size(); i++)
        {
            extName[i] |= 32;
        }
    }
    return extName;
}


ZipArchiveReader::ZipArchiveReader()
: m_errCode(Z_ERRNO)
, m_bEncrypted(false)
, m_hZip(nullptr)
, m_pZipStream(nullptr)
{

}


ZipArchiveReader::ZipArchiveReader(const std::string& filename, int hint)
: m_errCode(Z_ERRNO)
, m_bEncrypted(false)
, m_hZip(nullptr)
, m_pZipStream(nullptr)
{
    Open(filename, hint);
}


ZipArchiveReader::ZipArchiveReader(const std::string& filename, const TDecryptInputInfo& decInfo, int hint)
: m_errCode(Z_ERRNO)
, m_bEncrypted(false)
, m_hZip(nullptr)
, m_pZipStream(nullptr)
{
    Open(filename, decInfo, hint);
}


ZipArchiveReader::ZipArchiveReader(const std::string& mprxFileName, const std::string& epubPath)
: m_errCode(Z_ERRNO)
, m_bEncrypted(false)
, m_hZip(nullptr)
, m_pZipStream(nullptr)
{
    Open(mprxFileName, epubPath);
}


ZipArchiveReader::ZipArchiveReader(const std::string& mprxFileName, const std::string& epubPath, const TDecryptInputInfo& decInfo)
: m_errCode(Z_ERRNO)
, m_bEncrypted(false)
, m_hZip(nullptr)
, m_pZipStream(nullptr)
{
    Open(mprxFileName, epubPath, decInfo);
}


ZipArchiveReader::~ZipArchiveReader()
{
    Close();
}


std::string ZipArchiveReader::TryToLocateEpubItem(const std::string& mprxfile)
{
    std::string epubPath;

    if (!mprxfile.empty())
    {
        ZipArchiveReader mprxHandler;
        if (mprxHandler.Open(mprxfile, 1))
        {
            for (std::map<std::string, TZipItemInfo>::iterator it = mprxHandler.m_mapZipItemInfo.begin(); it != mprxHandler.m_mapZipItemInfo.end(); ++it)
            {
                std::string extName = GetFileExtName(it->first);
                if (extName == ".epub")
                {
                    if (it->second.compression_method == 0)
                    {
                        epubPath = it->first;
                        break;
                    }
                }
            }
        }
    }

    return epubPath;
}


bool ZipArchiveReader::Open(const std::string& filename, int hint)
{
    if (filename.empty())
        return false;

    if (hint == 0)
    {
        std::string extName = GetFileExtName(filename);
        hint = (extName == ".mprx") ? 2 : 1;
        return Open(filename, hint);
    }

    if (hint == 1)
    {
        m_bEncrypted = false;
        m_mprxFilePath.clear();
        m_epubFilePath = filename;
        return OpenZip_aux();
    }
    
    if (hint == 2)
    {
        m_bEncrypted = false;
        m_mprxFilePath = filename;
        m_epubFilePath = TryToLocateEpubItem(m_mprxFilePath);
        if (!m_epubFilePath.empty())
        {
            return OpenZip_aux();
        }
    }

    return false;
}


bool ZipArchiveReader::Open(const std::string& filename, const TDecryptInputInfo& decInfo, int hint)
{
    if (filename.empty())
        return false;

    if (hint == 0)
    {
        std::string extName = GetFileExtName(filename);
        hint = (extName == ".mprx") ? 2 : 1;
        return Open(filename, decInfo, hint);
    }

    m_bEncrypted = true;
    m_DecryptInputInfo = decInfo;

    if (hint == 1)
    {
        m_mprxFilePath.clear();
        m_epubFilePath = filename;
        return OpenZip_aux();
    }

    if (hint == 2)
    {
        m_mprxFilePath = filename;
        m_epubFilePath = TryToLocateEpubItem(m_mprxFilePath);
        if (!m_epubFilePath.empty())
        {
            return OpenZip_aux();
        }
    }

    return false;
}


bool ZipArchiveReader::Open(const std::string& mprxFileName, const std::string& epubPath)
{
    if (mprxFileName.empty() || epubPath.empty())
        return false;

    m_bEncrypted = false;
    m_mprxFilePath = mprxFileName;
    m_epubFilePath = ReplaceWin32PathSep(epubPath);

    return OpenZip_aux();
}


bool ZipArchiveReader::Open(const std::string& mprxFileName, const std::string& epubPath, const TDecryptInputInfo& decInfo)
{
    if (mprxFileName.empty() || epubPath.empty())
        return false;

    m_bEncrypted = true;
    m_mprxFilePath = mprxFileName;
    m_epubFilePath = ReplaceWin32PathSep(epubPath);
    m_DecryptInputInfo = decInfo;

    return OpenZip_aux();
}


bool ZipArchiveReader::OpenZip_aux()
{
    Close();

    m_errCode = Z_OK;

    if (!InitStream())
    {
        return false;
    }

    if (!InitZipArchive())
    {
        return false;
    }

    return true;
}


bool ZipArchiveReader::IsOpen() const
{
    return m_errCode == Z_OK;
}


void ZipArchiveReader::Close()
{
    m_errCode = Z_ERRNO;

    for (std::map<std::string, TZipItemInfo>::iterator it = m_mapZipItemInfo.begin(); it != m_mapZipItemInfo.end(); ++it)
    {
        if (it->second.item_stream)
        {
            delete it->second.item_stream;
            it->second.item_stream = nullptr;
        }
    }

    m_mapZipItemInfo.clear();

    if (m_hZip)
    {
        unzClose(m_hZip);
        m_hZip = nullptr;
    }

    if (m_pZipStream)
    {
        delete m_pZipStream;
        m_pZipStream = nullptr;
    }
}



bool ZipArchiveReader::InitStream()
{
    if (!m_mprxFilePath.empty())
    {
        UINT64 epubSize = 0;
        UINT64 epubOffset = 0;

        bool flag = false;

        {
            ZipArchiveReader mprxHandler;
            if (mprxHandler.Open(m_mprxFilePath, 1))
            {
                if (mprxHandler.ExistItem(m_epubFilePath) &&
                   !mprxHandler.IsItemCompressed(m_epubFilePath))
                {
                    flag = true;
                    epubOffset = mprxHandler.GetItemDataOffset(m_epubFilePath);
                    epubSize = mprxHandler.GetItemDataSize(m_epubFilePath);
                }
            }
        }

        if (!flag)
        {
            m_errCode = Z_EPUB_NOT_FOUND;
            return false;
        }

        m_pZipStream = new FileStream(m_mprxFilePath);
        m_pZipStream = new OffsetStream(m_pZipStream, epubOffset, epubSize);
    }
    else
    {
        m_pZipStream = new FileStream(m_epubFilePath);
    }

    if (m_bEncrypted)
    {
        switch (m_DecryptInputInfo.algoType)
        {
#if 0
        case AES_ECB:
        case SM4_ECB:
            m_pZipStream = new ECBDecryptStream(m_pZipStream, m_DecryptInputInfo);
            break;
        case AES_CTR:
        case SM4_CTR:
            m_pZipStream = new CTRDecryptStream(m_pZipStream, m_DecryptInputInfo);
            break;
        case AES_CBC:
        case SM4_CBC:
            m_pZipStream = new CBCDecryptStream(m_pZipStream, m_DecryptInputInfo);
            break;
#endif
        default:
            m_errCode = Z_ENCRYPT_NOT_SUPPORT;
            return false;
        }
    }

    if (m_pZipStream)
    {
        m_pZipStream->SetFlagBit(IStreamBase::STREAM_FLAG_SELF_DROP);
        m_pZipStream->SetFlagBit(IStreamBase::STREAM_FLAG_SELF_CLOSE);
        return true;
    }

    return false;
}



bool ZipArchiveReader::InitZipArchive()
{
    zlib_filefunc64_def ffunc;

    void fill_stream_filefunc(zlib_filefunc64_def*, voidpf);
    fill_stream_filefunc(&ffunc, m_pZipStream);

    m_hZip = unzOpen2_64("", &ffunc);
    if (!m_hZip)
    {
        m_errCode = Z_ZIP_OPEN_ERROR;
        return false;
    }

    if (!GetZipItemInfo())
    {
        m_errCode = Z_ZIP_ITEM_ERROR;
        return false;
    }

    return true;
}


bool ZipArchiveReader::GetZipItemInfo()
{
    unz_global_info64 gi;
    unzGetGlobalInfo64(m_hZip, &gi);
    m_nNumberOfItems = gi.number_entry;

    int err = UNZ_OK;
    char szFileName[512];

    err = unzGoToFirstFileEx (m_hZip, szFileName, sizeof(szFileName));

    for (uLong i=0; i<m_nNumberOfItems; i++)
    {
        if (err == UNZ_OK)
        {
            TZipItemInfo itemInfo;

            unz_file_info64* innerInfo = unzGetCurrentFileInfoSimply(m_hZip);

            if (innerInfo->compressed_size > 0) // Non-dir ?
            {
                itemInfo.data_offset = unzGetCurrentFileZStreamPosEx64(m_hZip);
            }
            else
            {
                itemInfo.data_offset = 0;
            }

            if (itemInfo.data_offset > 0 || innerInfo->compressed_size == 0)
            {
                itemInfo.item_stream        =   nullptr;
                itemInfo.version            =   innerInfo->version;
                itemInfo.flag               =   innerInfo->flag;
                itemInfo.compression_method =   innerInfo->compression_method;
                itemInfo.crc                =   innerInfo->crc;
                itemInfo.index              =   i;
                itemInfo.compressed_size    =   innerInfo->compressed_size;
                itemInfo.uncompressed_size  =   innerInfo->uncompressed_size;
                itemInfo.filename           =   szFileName;
                itemInfo.offset_in_central_dir = 0;

                m_mapZipItemInfo.insert(std::make_pair(itemInfo.filename, itemInfo));
            }
        }

        err = unzGoToNextFileEx(m_hZip, szFileName, sizeof(szFileName));
    }

    return true;
}


UINT64 ZipArchiveReader::NumberOfItems() const
{
    return m_nNumberOfItems;
}



bool ZipArchiveReader::ExistItem(const std::string& itemPath) const
{
    return m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath)) != m_mapZipItemInfo.end();
}


bool ZipArchiveReader::IsItemCompressed(const std::string& itemPath) const
{
	std::map<std::string, TZipItemInfo>::const_iterator it = m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath));

    if (it != m_mapZipItemInfo.end())
    {
        return it->second.compression_method != 0;
    }

    return false;
}


UINT64 ZipArchiveReader::ItemSize(const std::string& itemPath)
{
	std::map<std::string, TZipItemInfo>::iterator it = m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath));

    if (it != m_mapZipItemInfo.end())
    {
        return it->second.uncompressed_size;
    }

    return 0;
}


UINT64 ZipArchiveReader::GetItemDataOffset(const std::string& itemPath)
{
    std::map<std::string, TZipItemInfo>::iterator it = m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath));

    if (it != m_mapZipItemInfo.end())
    {
        return it->second.data_offset;
    }

    return 0;
}


UINT64 ZipArchiveReader::GetItemDataSize(const std::string& itemPath)
{
    std::map<std::string, TZipItemInfo>::iterator it = m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath));

    if (it != m_mapZipItemInfo.end())
    {
        return it->second.compressed_size;
    }

    return 0;
}


UINT64 ZipArchiveReader::ItemSize(HZipItem hItem)
{
    TZipItemInfo* itemInfo = reinterpret_cast<TZipItemInfo*>(hItem);

    if (itemInfo)
    {
        return itemInfo->uncompressed_size;
    }

    return 0;
}



INT64 ZipArchiveReader::ItemIndex(const std::string& itemPath)
{
    std::map<std::string, TZipItemInfo>::iterator it = m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath));

    if (it != m_mapZipItemInfo.end())
    {
        return it->second.index;
    }

    return -1;
}



INT64 ZipArchiveReader::ItemIndex(HZipItem hItem)
{
    TZipItemInfo* itemInfo = reinterpret_cast<TZipItemInfo*>(hItem);

    if (itemInfo)
    {
        return itemInfo->index;
    }

    return -1;
}



HZipItem ZipArchiveReader::OpenItem(const std::string& itemPath)
{
    std::map<std::string, TZipItemInfo>::iterator it = m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath));

    if (it != m_mapZipItemInfo.end())
    {
        TZipItemInfo& ItemInfo = it->second;

        if (!ItemInfo.item_stream)
        {
            if (ItemInfo.compression_method == 0)
            {
                ItemInfo.item_stream = new OffsetStream(m_pZipStream, ItemInfo.data_offset, ItemInfo.compressed_size);
            }
            else if (ItemInfo.compression_method == Z_DEFLATED)
            {
                IStreamBase* offsetStream = new OffsetStream(m_pZipStream, ItemInfo.data_offset, ItemInfo.compressed_size);

                ItemInfo.item_stream = new DecompressStream(offsetStream, &ItemInfo);
            }
        }

        if (ItemInfo.item_stream)
        {
            if (ItemInfo.item_stream->Open())
            {
                return (HZipItem)&ItemInfo;
            }
        }
    }

    return nullptr;
}



int ZipArchiveReader::ReadItem(HZipItem hItem, void* buffer, int maxBufLen)
{
    TZipItemInfo* zipItemInfo = reinterpret_cast<TZipItemInfo*>(hItem);

    if (zipItemInfo)
    {
        if (zipItemInfo->item_stream)
        {
            return zipItemInfo->item_stream->Read(buffer, maxBufLen);
        }
    }

    return -1;
}



void ZipArchiveReader::CloseItem(HZipItem hItem)
{
    TZipItemInfo* zipItemInfo = reinterpret_cast<TZipItemInfo*>(hItem);

    if (zipItemInfo)
    {
        if (zipItemInfo->item_stream)
        {
            zipItemInfo->item_stream->Close();
        }
    }
}



void ZipArchiveReader::RewindItem(HZipItem hItem)
{
    TZipItemInfo* zipItemInfo = reinterpret_cast<TZipItemInfo*>(hItem);

    if (zipItemInfo)
    {
        if (zipItemInfo->item_stream)
        {
            zipItemInfo->item_stream->Rewind();
        }
    }
}



int ZipArchiveReader::ReadAllForItem(const std::string& itemPath, void* buffer, int maxBufLen)
{
    if (!buffer || maxBufLen <= 0)
        return -1;

    std::map<std::string, TZipItemInfo>::iterator it = m_mapZipItemInfo.find(ReplaceWin32PathSep(itemPath));

    if (it != m_mapZipItemInfo.end())
    {
        TZipItemInfo& ItemInfo = it->second;

        if (ItemInfo.uncompressed_size > maxBufLen)
        {
            return 0; // buffer size not enough.
        }

        if (!ItemInfo.item_stream)
        {
            if (!OpenItem(itemPath)) {
                return -1;
            }
        } else {

            ItemInfo.item_stream->Close();
            if (!ItemInfo.item_stream->Open()) {
                return -1;
            }
        }

        int nRead = ItemInfo.item_stream->Read(buffer, (int)ItemInfo.uncompressed_size);

        ItemInfo.item_stream->Close();

        return nRead;
    }

    return -1;
}



int ZipArchiveReader::ReadAllForItem(HZipItem hItem, void* buffer, int maxBufLen)
{
    TZipItemInfo* zipItemInfo = reinterpret_cast<TZipItemInfo*>(hItem);

    if (zipItemInfo)
    {
        if (zipItemInfo->uncompressed_size > maxBufLen)
        {
            return 0;
        }

        if (zipItemInfo->item_stream)
        {
            if (!zipItemInfo->item_stream->Error())
            {
                zipItemInfo->item_stream->Rewind();

                return zipItemInfo->item_stream->Read(buffer, (int)zipItemInfo->uncompressed_size);
            }
        }
    }

    return -1;
}



int ZipArchiveReader::AddDirItem(const std::string& /*dirPath*/)
{
    return -1;
}



int ZipArchiveReader::DelDirItem(const std::string& /*dirPath*/)
{
    return -1;
}



int ZipArchiveReader::AddFileItem(const std::string& sourceFile, const std::string& itemPath, int compressFlag)
{
    if (sourceFile.empty() || itemPath.empty())
        return -1;

    IStreamBase* inputStream = nullptr;

    if (compressFlag == 0)
    {
        inputStream = new FileStream(sourceFile);
    }
    else if (compressFlag == Z_DEFLATED)
    {
        inputStream = new FileStream(sourceFile);
        //inputStream = new CompressStream(inputStream);
    }

    if (inputStream)
    {
        // write one new item into zip archive.

        delete inputStream;
        return 0;
    }

    return -1;
}



int ZipArchiveReader::AddFileItem(void* sourceBuf, int srclen, const std::string& /*itemPath*/, int compressFlag)
{
    if (!sourceBuf || srclen <= 0)
        return -1;

    IStreamBase* inputStream = nullptr;

    if (compressFlag == 0)
    {
        inputStream = new BufferStream((unsigned char*)sourceBuf, srclen);
    }
    else if (compressFlag == Z_DEFLATED)
    {
        inputStream = new BufferStream((unsigned char*)sourceBuf, srclen);
        //inputStream = new CompressStream(inputStream);
    }

    if (inputStream)
    {
        // write one new item into zip archive.

        delete inputStream;
        return 0;
    }

    return -1;
}



int ZipArchiveReader::DelFileItem(const std::string& /*itemPath*/)
{
    return -1;
}



int ZipArchiveReader::DelItem(const std::string& /*itemPath*/)
{
    return -1;
}



int ZipArchiveReader::DelItem(UINT64 /*itemIndex*/)
{
    return -1;
}


