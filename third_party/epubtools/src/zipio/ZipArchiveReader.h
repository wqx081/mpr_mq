
#ifndef ZIPARCHIVEREADER_H_1ERWRIERDIFS_2374734JHFD_
#define ZIPARCHIVEREADER_H_1ERWRIERDIFS_2374734JHFD_

#include <map>
#include <string>
#include "zlib.h"
#include "BasicDef.h"
#include "IStreamBase.h"


//typedef void *HZipItem;
typedef void **HZipItem;

struct TZipItemInfo
{
    uLong           version;
    uLong           flag;
    uLong           compression_method;
    uLong           crc;
    UINT64          index;
    UINT64          offset_in_central_dir;
    UINT64          data_offset;
    UINT64          compressed_size;
    UINT64          uncompressed_size;
    std::string     filename;
    IStreamBase*    item_stream;
};

class ZipArchiveReader
{
public:

    ZipArchiveReader();

    explicit ZipArchiveReader(const std::string& filename, int hint = 0);

    ZipArchiveReader(const std::string& filename, const TDecryptInputInfo& decInfo, int hint = 0);

    ZipArchiveReader(const std::string& mprxFileName, const std::string& epubPath);

    ZipArchiveReader(const std::string& mprxFileName, const std::string& epubPath, const TDecryptInputInfo& decInfo);

    ~ZipArchiveReader();


    /*
    ** hint 参数取值含义如下:
    ** 0: 视参数filename的扩展名而定:
    **    若参数filename的扩展名为 .epub/.zip，则处理同 hint == 1 的情形；
    **    若参数filename的扩展名为 .mprx，则处理同 hint == 2 的情形；
    ** 1: 将参数filename所指文件当成 epub/zip文件处理；
    ** 2: 将参数filename所指文件当成嵌套的zip格式 即 mprx 文件处理，并尝试定位mprx内的第一个epub文件项，并以此为目标zip文件打开之；
    ** 
    ** 如果mprx文件内含多个epub文件，且期望打开的不是第一个epub项时，应使用open的第3、4个重载方法，显式指定zip内epub项路径；
    ** 其他成员方法的hint同名参数含义与此相同(缺省值皆为0)；
    */
    bool Open(const std::string& filename, int hint = 0);

    bool Open(const std::string& filename, const TDecryptInputInfo& decInfo, int hint = 0);

    bool Open(const std::string& mprxFileName, const std::string& epubPath);

    bool Open(const std::string& mprxFileName, const std::string& epubPath, const TDecryptInputInfo& decInfo);

    bool IsOpen() const;

    void Close();


    /*
    ** 该方法的调用时机:
    ** 当利用 ZipArchiveWriter 往zip里面添加了新的文件项时
    ** 需要为 ZipArchiveReader 重建 item info table.
    ** 原因是ItemInfoTable是在zip打开之初静态创建的。
    */
    bool RebuildItemTable() { return OpenZip_aux(); }



    UINT64 NumberOfItems() const;

    bool ExistItem(const std::string& itemPath) const;

    bool IsItemCompressed(const std::string& itemPath) const;



    UINT64 ItemSize(const std::string& itemPath);

    UINT64 ItemSize(HZipItem hItem);


    INT64 ItemIndex(const std::string& itemPath);

    INT64 ItemIndex(HZipItem hItem);



    HZipItem OpenItem(const std::string& itemPath);

    int ReadItem(HZipItem hItem, void* buffer, int maxBufLen);

    void CloseItem(HZipItem hItem);

    void RewindItem(HZipItem hItem);



    int ReadAllForItem(const std::string& itemPath, void* buffer, int maxBufLen);

    int ReadAllForItem(HZipItem hItem, void* buffer, int maxBufLen);



    // 下面对zip归档文件的修改操作移至 ZipArchiveWriter 中.

    int AddDirItem(const std::string& dirPath);

    int DelDirItem(const std::string& dirPath);


    int AddFileItem(const std::string& sourceFile, const std::string& itemPath, int compressFlag);

    int AddFileItem(void* sourceBuf, int srclen, const std::string& itemPath, int compressFlag);

    int DelFileItem(const std::string& itemPath);


    int DelItem(const std::string& itemPath);

    int DelItem(UINT64 itemIndex);


private:

    bool OpenZip_aux();

    bool InitStream();

    bool InitZipArchive();

    bool GetZipItemInfo();

    std::string TryToLocateEpubItem(const std::string& mprxfile);

    UINT64 GetItemDataOffset(const std::string& itemPath);
    UINT64 GetItemDataSize(const std::string& itemPath);


    int                     m_errCode;

    bool                    m_bEncrypted;
    TDecryptInputInfo       m_DecryptInputInfo;
    std::string             m_mprxFilePath;
    std::string             m_epubFilePath;

    void*                   m_hZip;
    IStreamBase*            m_pZipStream;
    UINT64                  m_nNumberOfItems;

    std::map<std::string, TZipItemInfo>  m_mapZipItemInfo;
};


#endif // ZIPARCHIVEREADER_H_1ERWRIERDIFS_2374734JHFD_


