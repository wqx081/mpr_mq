
#ifndef BUFFERSTREAM_H_1ERWRIERDIFS_2374734JHFD_
#define BUFFERSTREAM_H_1ERWRIERDIFS_2374734JHFD_


#include "IStreamBase.h"


typedef void (*TFreePolicy)(unsigned char* buffer, UINT64 bufSize);

class BufferStream : public IStreamBase
{
public:

    explicit BufferStream(unsigned char* buffer = nullptr, UINT64 bufSize = 0, TFreePolicy freePolicy = nullptr);

    ~BufferStream();


    void SetBuffer(unsigned char* buffer, UINT64 bufSize, TFreePolicy freePolicy = nullptr);


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

    unsigned char*          m_buffer;

    UINT64                  m_nBufSize;

    TFreePolicy             m_fnBufFreePolicy;

    UINT64                  m_nCurPos;

};


#endif // BUFFERSTREAM_H_1ERWRIERDIFS_2374734JHFD_


