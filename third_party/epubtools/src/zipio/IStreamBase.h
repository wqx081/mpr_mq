
#ifndef STREAMBASE_H_1ERWRIERDIFS_2374734JHFD_
#define STREAMBASE_H_1ERWRIERDIFS_2374734JHFD_

#include "BasicDef.h"


#ifndef WIN32
#define nullptr NULL
#define override
#endif

class IStreamBase
{
public:

    IStreamBase();

    virtual ~IStreamBase();


    virtual  UINT64  Size() const = 0;

    virtual  bool    Seekable() const = 0;


    virtual  bool    Open(int mode = 9) = 0;

    virtual  INT64   Tell() = 0;

    virtual  void    Rewind() = 0;

    virtual  int     Seek(INT64 offset, int fromWhere) = 0;

    virtual  int     Read(void* buffer, int bufSize) = 0;

    virtual  int     Write(void* buffer, int bufSize);

    virtual  void    Close() = 0;

    virtual  bool    Eof() const = 0;

    virtual  bool    Error() const = 0;



    bool IsStreamOpened() const;


    void SetFlagBit(int flagBit);

    void ResetFlagBit(int flagBit);

    void SwitchFlagBit(int flagBit);

    bool IsSetFlagBit(int flagBit);


    enum TStreamFlagBit
    {
        STREAM_FLAG_SELF_CLOSE  = 0x01,
        STREAM_FLAG_SELF_DROP   = 0x02,
        STREAM_FLAG_BUFFER_FREE = 0x04,
    };

    enum TStreamOpenMode
    {
        STREAM_OPEN_MODE_NONE   = 0x00,
        STREAM_OPEN_MODE_READ   = 0x01,
        STREAM_OPEN_MODE_WRITE  = 0x02,
        STREAM_OPEN_MODE_TEXT   = 0x04,
        STREAM_OPEN_MODE_BIN    = 0x08,
    };

protected:

    enum TStreamStatus
    {
        STREAM_INVALID     = 0,
        STREAM_OPENED      = 1,
        STREAM_EOF         = 2,
        STREAM_ERROR       = 3,
    };

    int             m_status;

    int             m_openMode;

    int             m_flagSet;
};


#endif // STREAMBASE_H_1ERWRIERDIFS_2374734JHFD_


