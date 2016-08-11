
#ifndef DECOMPRESSSTREAM_H_1ERWRIERDIFS_2374734JHFD_
#define DECOMPRESSSTREAM_H_1ERWRIERDIFS_2374734JHFD_

#include "IStreamBase.h"
#include "ZipArchiveReader.h"


class DecompressStream : public IStreamBase
{
public:

    explicit DecompressStream(IStreamBase* basicIO = nullptr, TZipItemInfo* zipItemInfo = nullptr);

    ~DecompressStream();


    void SetBasicStreamAndItemInfo(IStreamBase* basicIO, TZipItemInfo* zipItemInfo);



    UINT64  Size() const override;

    bool    Seekable() const override;


    bool    Open(int mode) override;

    INT64   Tell() override;

    void    Rewind() override;

    int     Seek(INT64 offset, int fromWhere) override;

    int     Read(void* buffer, int bufSize) override;

    void    Close() override;

    bool    Eof() const override;

    bool    Error() const override;


private:

    IStreamBase*         m_innerStream;

    TZipItemInfo*        m_zipItemInfo;

    UINT64               m_rest_uncompressed_size;

    z_stream             m_inflateStream;  /* zLib stream structure for inflate */

    char*                m_inflateBuffer;  /* internal buffer for compressed data */
};


#endif // DECOMPRESSSTREAM_H_1ERWRIERDIFS_2374734JHFD_


