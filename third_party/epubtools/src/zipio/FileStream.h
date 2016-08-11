
#ifndef FILESTREAM_H_1ERWRIERDIFS_2374734JHFD_
#define FILESTREAM_H_1ERWRIERDIFS_2374734JHFD_

#include <cstdio>
#include <string>
#include "IStreamBase.h"


class FileStream : public IStreamBase
{
public:

    explicit FileStream(const std::string& fileName = "");

    ~FileStream();


    void SetFileName(const char* fileName);


    UINT64  Size() const override;

    bool    Seekable() const override;


    bool    Open(int mode) override;

    INT64   Tell() override;

    void    Rewind() override;

    int     Seek(INT64 offset, int fromWhere) override;

    int     Read(void* buffer, int bufSize) override;

    int     Write(void* buffer, int bufSize) override;

    void    Close() override;

    bool    Eof() const override;

    bool    Error() const override;

private:

    std::string     m_filePath;

    FILE*           m_hFile;

    UINT64          m_nSize;
};


#endif // FILESTREAM_H_1ERWRIERDIFS_2374734JHFD_


