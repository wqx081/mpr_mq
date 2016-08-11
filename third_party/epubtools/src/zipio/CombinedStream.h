
#ifndef COMBINEDSTREAM_H_1ERWRIERDIFS_2374734JHFD_
#define COMBINEDSTREAM_H_1ERWRIERDIFS_2374734JHFD_


#include <vector>
#include "IStreamBase.h"



class CombinedStream : public IStreamBase
{
public:

    explicit CombinedStream(const std::vector<IStreamBase*>& streamSet = std::vector<IStreamBase*>());

    ~CombinedStream();


    void SetBasicStreamSet(const std::vector<IStreamBase*>& streamSet);

    bool AppendStream(IStreamBase* stream, bool bUnique = true);

    bool RemoveStream(IStreamBase* stream);


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

    void TryToFreeInnerStreams();


    int                        m_curStreamIndex;

    std::vector<IStreamBase*>  m_innerStreamSet;
};


#endif // COMBINEDSTREAM_H_1ERWRIERDIFS_2374734JHFD_


