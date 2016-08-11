
#ifndef OFFSETSTREAM_H_1ERWRIERDIFS_2374734JHFD_
#define OFFSETSTREAM_H_1ERWRIERDIFS_2374734JHFD_


#include "IStreamBase.h"



class OffsetStream : public IStreamBase
{
public:

    explicit OffsetStream(IStreamBase* basicIO = nullptr, UINT64 offset = 0, UINT64 size = 0);

    ~OffsetStream();


    void SetBasicIOandPos(IStreamBase* basicIO, UINT64 offset, UINT64 size);



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


    UINT64                  m_nSize;

    UINT64                  m_nOffset;

    IStreamBase*            m_innerStream;
};


#endif // OFFSETSTREAM_H_1ERWRIERDIFS_2374734JHFD_


