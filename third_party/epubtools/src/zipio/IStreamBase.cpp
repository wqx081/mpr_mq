
#include "IStreamBase.h"


IStreamBase::IStreamBase()
: m_flagSet(0)
, m_status(STREAM_INVALID)
{

}


IStreamBase::~IStreamBase()
{

}


int IStreamBase::Write(void* /*buffer*/, int /*bufSize*/)
{
    return -1;
}


bool IStreamBase::IsStreamOpened() const
{
    return m_status == STREAM_OPENED;
}


void IStreamBase::SetFlagBit(int flagBit)
{
    m_flagSet |= flagBit;
}


void IStreamBase::ResetFlagBit(int flagBit)
{
    m_flagSet &= ~flagBit;
}


void IStreamBase::SwitchFlagBit(int flagBit)
{
    m_flagSet ^= flagBit;
}


bool IStreamBase::IsSetFlagBit(int flagBit)
{
    return (m_flagSet & flagBit) != 0;
}

