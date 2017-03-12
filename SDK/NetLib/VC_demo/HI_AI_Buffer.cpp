
#include "StdAfx.h"
#include "HI_AI_Buffer.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HI_AI_Buffer::HI_AI_Buffer(HI_U32 dwSize, HI_BOOL bAutoDelete)
{
    m_bAutoDelete = bAutoDelete;
    ptr.b = new HI_U8[dwSize];
    m_u32ByteLen = dwSize;
}

HI_AI_Buffer::HI_AI_Buffer()
{
    m_bAutoDelete = HI_FALSE;
    ptr.b = NULL;
    m_u32ByteLen = 0;
}

HI_AI_Buffer::HI_AI_Buffer(HI_VOID *pBuffer, HI_U32 dwLength)
{
    m_bAutoDelete = HI_FALSE;
    ptr.b = NULL;
    m_u32ByteLen = 0;

    GetBuffer(pBuffer, dwLength);
}

HI_AI_Buffer::~HI_AI_Buffer()
{
    //清除缓冲区
    if ((m_bAutoDelete) && (ptr.b != NULL))
    {
        delete[](ptr.b);
        ptr.b = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////

HI_VOID HI_AI_Buffer::Erase(HI_VOID)
{
    if (ptr.b)
    {
        memset(ptr.b, 0, m_u32ByteLen);
    }
}

//////////////////////////////////////////////////////////////////////////
/************************************************
* 函数名	     : HI_AI_GetUsedLength
* 描  述  	     : 获取分配的缓冲区大小
* 返回类型       : HI_S16 
* 参数           : HI_VOID
* 设计者         : lizhaozao 
* 最后修订时间   : 2008-1-18 17:05:50
************************************************/

HI_S16 HI_AI_Buffer::HI_AI_GetUsedLength(HI_VOID)
{
    //获取当前缓冲区使用数
    return ptr.s[1] + 2;
}

//////////////////////////////////////////////////////////////////////////

HI_VOID HI_AI_Buffer::GetBuffer(HI_VOID *pBuffer, HI_U32 dwLength)
{
    m_bAutoDelete = HI_FALSE;
    if (pBuffer)
    {
        ptr.v = pBuffer;
        m_u32ByteLen = dwLength;
    }
    else
    {
        ptr.b = NULL;
        m_u32ByteLen = 0;
    }
}

//////////////////////////////////////////////////////////////////////////
