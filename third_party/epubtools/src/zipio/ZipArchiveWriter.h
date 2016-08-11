
#ifndef ZIPARCHIVEWRITER_H_1ERWRIERDIFS_2374734JHFD_
#define ZIPARCHIVEWRITER_H_1ERWRIERDIFS_2374734JHFD_

#include <map>
#include <string>
#include "zlib.h"
#include "BasicDef.h"

class IStreamBase;
class ZipArchiveWriter
{
public:

    explicit ZipArchiveWriter(const std::string& zipFileName = "", bool bZipExist = true);

    ~ZipArchiveWriter();


    bool Open(const std::string& zipFileName, bool bZipExist = true);

    bool IsOpen() const;

    void Close();


    int AddDirItem(const std::string& dirPath);

    int DelDirItem(const std::string& dirPath);


    int AddFileItem(const std::string& sourceFile, const std::string& itemPath, int compressFlag = Z_DEFLATED);

    int AddFileItem(void* sourceBuf, int srclen, const std::string& itemPath, int compressFlag = Z_DEFLATED);

    int DelFileItem(const std::string& itemPath);


    int DelItem(const std::string& itemPath);

    int DelItem(UINT64 itemIndex);


    static int BatchAddFilesIntoZip(const std::map<std::string, std::string>& mapNameInZip2SrcFilePath, const std::string& zipFileName, bool bZipExist = true);


private:

    int AddFileItem(IStreamBase* inputStream, const std::string& strNameInZip, int compressFlag);



    void*                   m_hZip;

    int                     m_errCode;
    std::string             m_zipFilePath;
};

#endif // ZIPARCHIVEWRITER_H_1ERWRIERDIFS_2374734JHFD_


