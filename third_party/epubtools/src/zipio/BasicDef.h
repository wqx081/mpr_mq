

#ifndef BASICDEF_H_1ERWRIERDIFS_2374734JHFD_
#define BASICDEF_H_1ERWRIERDIFS_2374734JHFD_


#if defined(_MSC_VER) || defined(__BORLANDC__)

    typedef __int64 INT64;
    typedef unsigned __int64 UINT64;

#else

    typedef long long INT64;
    typedef unsigned long long UINT64;

#endif


#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif


#ifndef DECRYPT_BUFFER_SIZE
#define DECRYPT_BUFFER_SIZE 4096
#endif


//#define Z_OK              0
//#define Z_STREAM_END      1
//#define Z_NEED_DICT       2
//#define Z_ERRNO         (-1)
//#define Z_STREAM_ERROR  (-2)
//#define Z_DATA_ERROR    (-3)
//#define Z_MEM_ERROR     (-4)
//#define Z_BUF_ERROR     (-5)
//#define Z_VERSION_ERROR (-6)
#define   Z_EPUB_NOT_FOUND (-7)
#define   Z_ZIP_OPEN_ERROR (-8)
#define   Z_ZIP_ITEM_ERROR (-9)
#define   Z_ENCRYPT_NOT_SUPPORT (-10)

struct TDecryptInputInfo
{
    int algoType;               ///  enum EncryptAlgorithm
    unsigned char key[16];      ///  对称密钥
    unsigned char iv[16];       ///  仅CBC加密方式有效
    bool bPadding;              ///  是否采用padding结尾,此参数仅对ECB模式有效
    int  offsetOfLastChunk;     ///  最后一块的偏移量,此参数仅对ECB模式且havePadding为0时有效
};


//enum EncryptAlgorithm
//{
//    AES_ECB = 0x0001,       ///  AES ECB算法
//    AES_CBC = 0x0002,       ///  AES CBC算法
//    AES_CTR = 0x0004,       ///  AES CTR算法
//    SM4_ECB = 0x0100,       ///  国密4 ECB算法
//    SM4_CBC = 0x0200,       ///  国密4 CBC算法
//    SM4_CTR = 0x0400        ///  国密4 CTR算法
//};


#endif // BASICDEF_H_1ERWRIERDIFS_2374734JHFD_


