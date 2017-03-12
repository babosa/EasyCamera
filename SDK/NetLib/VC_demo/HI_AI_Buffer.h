

#if !defined(AFX_HI_AI_BUFFER_H__C6BD3348_C318_4313_A596_DA58127B1798__INCLUDED_)
#define AFX_HI_AI_BUFFER_H__C6BD3348_C318_4313_A596_DA58127B1798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "hi_pcdefs.h"

class HI_AI_Buffer
{
public:
    HI_AI_Buffer(HI_U32 dwSize, HI_BOOL bAutoDelete = HI_TRUE);
    HI_AI_Buffer(HI_VOID *pBuffer, HI_U32 dwLength);
    HI_AI_Buffer();

    virtual ~HI_AI_Buffer(HI_VOID);

    HI_S16 HI_AI_GetUsedLength(HI_VOID);
    HI_VOID Erase(HI_VOID);
    HI_VOID GetBuffer(HI_VOID *pBuffer, HI_U32 dwLength);

    union PointerUnion{
        HI_U8	*b;
        HI_U8	*u8;
        HI_CHAR	*c;
        HI_U16	*w;
        HI_S16 *s;
        HI_U32	*d;
        HI_S32	*i;
        HI_FLOAT	*f;
        HI_DOUBLE	*r;	       // real
        HI_VOID	*v;
    }ptr;

    HI_U32	m_u32ByteLen;	   /*数据长度*/

private:
    HI_BOOL	m_bAutoDelete;

    HI_AI_Buffer & operator= (const HI_AI_Buffer & Obj)
    {
        if (this == &Obj)
        {
            return * this;
        }
    };
    HI_AI_Buffer (const HI_AI_Buffer & Obj)
    {}
    ;
};

#endif // !defined(AFX_HI_AI_BUFFER_H__C6BD3348_C318_4313_A596_DA58127B1798__INCLUDED_)
