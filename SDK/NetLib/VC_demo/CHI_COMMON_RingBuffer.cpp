#include "StdAfx.h"
#include "CHI_COMMON_RingBuffer.h"
#include <stdio.h>

#define ALIGN_LENGTH(v,a) ((((v) + ((a)-1)) & (~((a)-1))))

#define RB_DO_STAT(idx, v)	 RB_Do_Stat(idx, v)
#define RB_INIT_STAT()       RB_Init_Stat() 
//#define RB_DISP_STAT()       RB_Disp_Stat()

#define RB_DO_STAT_TotalReadError(v) RB_DO_STAT(E_RB_TotalReadError, v)
#define RB_DO_STAT_TotalWriteError(v) RB_DO_STAT(E_RB_TotalWriteError, v)

#define RB_DO_STAT_TotalWriteSize(v) RB_DO_STAT(E_RB_TotalWriteSize, v)
#define RB_DO_STAT_TotalReadSize(v) RB_DO_STAT(E_RB_TotalReadSize, v)

#define RB_DO_STAT_TotalReadTimes(v) RB_DO_STAT(E_RB_TotalReadTimes, v)
#define RB_DO_STAT_TotalWriteTimes(v) RB_DO_STAT(E_RB_TotalWriteTimes, v)

#define RB_LEN_BYTES 4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CHI_COMMON_RingBuffer::CHI_COMMON_RingBuffer()
{
    m_pRB = NULL;
    m_ulNodeNums = 0;
	m_u32RBsize = 0;
	m_bStartPlay = HI_FALSE;
	m_u32BufNumber = 0;
    RB_Init_Stat();

    m_s32Move4 = sizeof(HI_U32);
    m_s32Move8 = sizeof(HI_U32) + sizeof(HI_U32);
}

CHI_COMMON_RingBuffer::~CHI_COMMON_RingBuffer()
{
    RB_Destroy();
}

void CHI_COMMON_RingBuffer::RB_Init_Stat(void)
{
    memset(m_stat, 0, sizeof(m_stat));
}

void CHI_COMMON_RingBuffer::RB_Do_Stat(RB_STAT_E stattype, HI_U64 value)
{
    if ( stattype < E_RB_STAT_END)
    {
        if (m_stat[stattype] < 0xFFFFFFFFFFFFFFFF)
        {
            m_stat[stattype] += value;
        }
        else
        {
            m_stat[stattype] = 0;
        }
    }
    else
    {
        //	TRACE("stattype :%d out of bounds", stattype);
    }
}

void CHI_COMMON_RingBuffer::RB_Disp_Stat(void)
{
    //	TRACE("\nTotalReadError: %llu, TotalWriteError: %llu \n", m_stat[E_RB_TotalReadError], m_stat[E_RB_TotalWriteError]);
    //    TRACE("TotalWriteSize: %llu, TotalReadSize: %llu \n", m_stat[E_RB_TotalWriteSize], m_stat[E_RB_TotalReadSize]);
    //	TRACE("TotalReadTimes: %llu, TotalWriteTimes: %llu \n", m_stat[E_RB_TotalReadTimes], m_stat[E_RB_TotalWriteTimes]);
    //    fprintf(stdout, "\nTotalReadError: %llu, TotalWriteError: %llu \n",
    //                    m_stat[E_RB_TotalReadError],
    //                    m_stat[E_RB_TotalWriteError] );
    //
    //    fprintf(stdout, "TotalWriteSize: %llu, TotalReadSize: %llu \n",
    //                    m_stat[E_RB_TotalWriteSize],
    //                    m_stat[E_RB_TotalReadSize] );
    //
    //    fprintf(stdout, "TotalReadTimes: %llu, TotalWriteTimes: %llu \n",
    //                    m_stat[E_RB_TotalReadTimes],
    //                    m_stat[E_RB_TotalWriteTimes] );
}


/************************************************
* Function Name	     : CHI_PLAY_RingBuffer::RB_MaxWriteSize
* Description	     : 最大的可以写的缓冲区长度
* Return Type        : HI_U32 
* Parameters         : void
* Last Modified      : 2006-5-21 15:53:53
************************************************/
HI_U32 CHI_COMMON_RingBuffer::RB_MaxWriteSize(void)
{
    HI_U32 ulMaxWriteSize = 0;

    if (NULL == m_pRB)
    {
        return HI_ERR_NULL_POINTER;
    }

    /*READ == WRITE 认为是空缓冲*/
    if (m_ulReadPos == m_ulWritePos)
    {
        ulMaxWriteSize = m_ulSize ;
    }

    if (m_ulReadPos > m_ulWritePos)
    {
        ulMaxWriteSize = m_ulReadPos - m_ulWritePos;
    }
    else
    {
        ulMaxWriteSize = m_ulSize - m_ulWritePos + m_ulReadPos;
    }

    return ulMaxWriteSize;
}

/************************************************
* Function Name	     : CHI_PLAY_RingBuffer::RB_MaxReadSize
* Description	     : 最大的可以读出的缓冲区长度
* Return Type        : HI_U32 
* Parameters         : void
* Last Modified      : 2006-5-21 15:53:39
************************************************/
HI_U32 CHI_COMMON_RingBuffer::RB_MaxReadSize(void)
{
    HI_U32 ulMaxReadSize = 0;

    if ( NULL == m_pRB )
    {
        return HI_ERR_NULL_POINTER;
    }

    /*认为缓冲是空的, 所以, 可以读出的数据为0*/
    if ( m_ulReadPos == m_ulWritePos)
    {
        ulMaxReadSize = 0;
    }
    else if (m_ulReadPos < m_ulWritePos)
    {
        ulMaxReadSize = m_ulWritePos - m_ulReadPos;
    }
    else if (m_ulReadPos > m_ulWritePos)
    {
        ulMaxReadSize = m_ulSize - m_ulReadPos + m_ulWritePos;
    }
    return ulMaxReadSize;
}

HI_U32 CHI_COMMON_RingBuffer::RB_MAXSize(void)
{
    return m_ulSize;
}

/**********==============================================================================*/

HI_S32 CHI_COMMON_RingBuffer::RB_Init(IN HI_U32 ulSize)
{
    if (ulSize <= 0)
    {
        return HI_ERR_INVALID_ARGUMENTS;
    }

    RB_Destroy();
    //	m_pRB = new HI_U8[ ulSize ];
    m_pRB = (HI_U8*)malloc(ulSize);
    if (NULL == m_pRB)
    {
        return HI_ERR_NULL_POINTER;
    }

    m_ulSize = ulSize;
    m_ulReadPos = 0;
    m_ulWritePos = 0;
    m_ulNodeNums = 0;

    memset(m_pRB, 0, ulSize);

    RB_INIT_STAT();

    return HI_SUCCESS;
}

void CHI_COMMON_RingBuffer::RB_Clear(void)
{
    m_ulReadPos = m_ulWritePos ;
    m_ulNodeNums = 0;
}

void CHI_COMMON_RingBuffer::RB_Destroy(void)
{
    if (NULL != m_pRB)
    {
        //		SAFE_DELETE(m_pRB);
        free(m_pRB);
        m_pRB = NULL;
    }
}

HI_VOID CHI_COMMON_RingBuffer::DirectCopyData(HI_U8 *pData,
        HI_U32 ulWriteSize,
        HI_U32 ulDataLen,
        HI_U32 u32TimeStamp,
        HI_U32 u32FrameTag)
{
    memcpy( (m_pRB + m_ulWritePos ), (&ulDataLen), RB_LEN_BYTES);
    //store time
    memcpy((m_pRB + m_ulWritePos + RB_LEN_BYTES), &u32TimeStamp, m_s32Move4);
    //store frame flag
    memcpy((m_pRB + m_ulWritePos + RB_LEN_BYTES) + m_s32Move4, &u32FrameTag, m_s32Move4);
    //store data
    memcpy( (m_pRB + m_ulWritePos + RB_LEN_BYTES) + m_s32Move8, pData, ulDataLen - m_s32Move8);
    m_ulWritePos += ulWriteSize;
}

HI_VOID CHI_COMMON_RingBuffer::WrapCopyData(HI_U8 *pData,
        HI_U32 ulWriteSize,
        HI_U32 ulDataLen,
        HI_U32 u32TimeStamp,
        HI_U32 u32FrameTag)
{
    /*写需要分段*/
    HI_U32 ulFirstChunkSize = 0;
    HI_U32 ulSecondChunkSize = 0;
    ulFirstChunkSize = m_ulSize - m_ulWritePos;
    ulSecondChunkSize = ulWriteSize - ulFirstChunkSize;

    memcpy( (m_pRB + m_ulWritePos), ( &ulDataLen ), RB_LEN_BYTES);
    if (ulFirstChunkSize == RB_LEN_BYTES)
    {
        //store time
        memcpy(m_pRB, &u32TimeStamp, m_s32Move4);
        //store frame flag
        memcpy(m_pRB + m_s32Move4, &u32FrameTag, m_s32Move4);
        //store data
        memcpy(m_pRB + m_s32Move8, pData, ulSecondChunkSize - m_s32Move8);
        //            memcpy( m_pRB, pData, ulSecondChunkSize );
    }
    else if (ulFirstChunkSize == (RB_LEN_BYTES + m_s32Move4))
    {
        //时间戳写后，帧标志与数据写前
        //store time
        memcpy((m_pRB + m_ulWritePos + RB_LEN_BYTES), &u32TimeStamp, m_s32Move4);
        //store frame flag
        memcpy(m_pRB, &u32FrameTag, m_s32Move4);
        //store data
        memcpy(m_pRB + m_s32Move4, pData, ulSecondChunkSize - m_s32Move4);
    }
    else if (ulFirstChunkSize == (RB_LEN_BYTES + m_s32Move8))
    {
        //时间戳帧标志写后，数据写前
        //store time
        memcpy((m_pRB + m_ulWritePos + RB_LEN_BYTES), &u32TimeStamp, m_s32Move4);
        //store frame flag
        memcpy((m_pRB + m_ulWritePos + RB_LEN_BYTES + m_s32Move4), &u32FrameTag, m_s32Move4);
        //store data
        memcpy(m_pRB, pData, ulSecondChunkSize);
    }
    else
    {
        //时间戳帧标志写后，数据一部分写后一部分写前
        //store time
        memcpy((m_pRB + m_ulWritePos + RB_LEN_BYTES), &u32TimeStamp, m_s32Move4);
        //store frame flag
        memcpy((m_pRB + m_ulWritePos + RB_LEN_BYTES + m_s32Move4), &u32FrameTag, m_s32Move4);
        //store data
        memcpy( (m_pRB + m_ulWritePos + RB_LEN_BYTES + m_s32Move8),
                pData, ulFirstChunkSize - RB_LEN_BYTES - m_s32Move8);
        memcpy( m_pRB, (pData + ulFirstChunkSize - RB_LEN_BYTES - m_s32Move8), ulSecondChunkSize);

        //            memcpy( (m_pRB + m_ulWritePos + RB_LEN_BYTES), pData, ulFirstChunkSize - RB_LEN_BYTES);
        //            memcpy( m_pRB, (pData + ulFirstChunkSize - RB_LEN_BYTES), ulSecondChunkSize);
    }

    m_ulWritePos = ulSecondChunkSize;
}

/*往RB写pData, 会在前面附加4Bytes的长度指示, 以4Byte为单位对齐*/
HI_S32 CHI_COMMON_RingBuffer::RB_Write_X(HI_U8 *pData,
        HI_U32 ulDataLen,
        HI_U32 u32TimeStamp,
        HI_U32 u32FrameTag,
        HI_U32 *pulWriteLen)
{
    HI_U32 ulMaxWriteSize = 0;

    //a data len include data,time,frame
    ulDataLen = ulDataLen + m_s32Move8;

    /*Data对齐后的长度*/
    HI_U32 lenAlign = ALIGN_LENGTH(ulDataLen, RB_LEN_BYTES);

    /*真实写入的长度*/
    HI_U32 ulWriteSize = lenAlign + RB_LEN_BYTES;

    if (ulDataLen == 0)
    {
        CString str;
        RB_DO_STAT_TotalWriteError((HI_U64)1);
        return HI_ERR_OVERFLOW_MEMORY;
    }

    RB_DO_STAT_TotalWriteTimes((HI_U64)1);

    ulMaxWriteSize = RB_MaxWriteSize();
    if (ulWriteSize >= ulMaxWriteSize )
    {
        RB_DO_STAT_TotalWriteError((HI_U64)1);
        return HI_ERR_ABORTED_OPERATION;
    }

    if (NULL == m_pRB)
    {
        return HI_ERR_NULL_POINTER;
    }
    // easy case, no wrapping
    if ( m_ulWritePos + ulWriteSize < m_ulSize )
    {
	//	TRACE("CHI_COMMON_RingBuffer:DirectCopyData!LEN=%d!size=%d!090518!\n",ulDataLen,ulWriteSize);
        DirectCopyData(pData, ulWriteSize, ulDataLen, u32TimeStamp, u32FrameTag);
    }
    else // harder case we need to wrap
    {
	//	TRACE("CHI_COMMON_RingBuffer:WrapCopyData!LEN=%d!size=%d!090518!\n",ulDataLen,ulWriteSize);
        WrapCopyData(pData, ulWriteSize, ulDataLen, u32TimeStamp, u32FrameTag);
    }

    if (pulWriteLen != NULL)
    {
        *pulWriteLen = ulWriteSize;
    }

    RB_DO_STAT_TotalWriteSize((HI_U64)ulWriteSize);

    SetNodeNumVal(HI_TRUE);

    return HI_SUCCESS;
}

HI_VOID CHI_COMMON_RingBuffer::SetNodeNumVal(HI_BOOL bSign)
{
    m_criticalSection.Lock();
    if (HI_TRUE == bSign)
    {
        m_ulNodeNums = m_ulNodeNums + 1;
    }
    else if (HI_FALSE == bSign)
    {
        m_ulNodeNums = m_ulNodeNums - 1;
    }

	if(m_u32BufNumber > 0){
		if(m_ulNodeNums >= m_u32BufNumber){
			m_bStartPlay = HI_TRUE;
		}else if(m_ulNodeNums <= 0){
			m_bStartPlay = HI_FALSE;
		}
	}else{
		if(m_bStartPlay == HI_FALSE)
			m_bStartPlay = HI_TRUE;
	}

    m_criticalSection.Unlock();
}

HI_S32 CHI_COMMON_RingBuffer::RB_Read_X(HI_U8 *pData, HI_U32 *pulData)
{
    HI_U32 ulMaxReadSize = 0;
    HI_U32 ulReadSize = 0; /*真实读出的数据*/

    /*读需要分段*/
    HI_U32 ulFirstChunkSize = 0;
    HI_U32 ulSecondChunkSize = 0;

    RB_DO_STAT_TotalReadTimes((HI_U64)1);

    ulMaxReadSize = RB_MaxReadSize();
    //增加保护代码
    if (ulMaxReadSize == 0)
    {
        return HI_ERR_OVERFLOW_MEMORY;
    }

    *pulData = RB_GetLen_X();
    ulReadSize = ALIGN_LENGTH(*pulData, RB_LEN_BYTES) + RB_LEN_BYTES;

    //增加保护代码
    //if (ulReadSize <= 0 || ulReadSize >= m_ulSize)
    if (ulReadSize > m_ulSize) //|| ulReadSize > ulMaxReadSize

    {
#if 0
        CString str;
        str.Format("RB RD_Error: ulMaxReadSize=%d, ulReadSize=%d, m_ulReadPos=%d, m_ulWritePos=%d, m_ulSize=%d >>\n",
                   ulMaxReadSize, ulReadSize, m_ulReadPos, m_ulWritePos, m_ulSize);
        InsertEventList(str);
#endif

        RB_DO_STAT_TotalReadError((HI_U64)1);

        return HI_ERR_ABORTED_OPERATION;
    }

    if (ulReadSize > ulMaxReadSize)
    {
        RB_DO_STAT_TotalReadError((HI_U64)1);
        return HI_ERR_ABORTED_OPERATION;
    }

    if (NULL == m_pRB)
    {
        return HI_ERR_NULL_POINTER;
    }
    // easy case, no wrapping
    if ( m_ulReadPos + ulReadSize < m_ulSize)
    {
        memcpy( pData, (m_pRB + m_ulReadPos + RB_LEN_BYTES), ulReadSize - RB_LEN_BYTES );
        m_ulReadPos += ulReadSize;
    }
    else // harder case, buffer wraps
    {
        ulFirstChunkSize = m_ulSize - m_ulReadPos;
        ulSecondChunkSize = ulReadSize - ulFirstChunkSize;
        if (ulFirstChunkSize == RB_LEN_BYTES)
        {
            memcpy( pData , m_pRB, ulSecondChunkSize );
        }
        else
        {
            memcpy( pData, (m_pRB + m_ulReadPos + RB_LEN_BYTES), ulFirstChunkSize - RB_LEN_BYTES);
            memcpy( pData + ulFirstChunkSize - RB_LEN_BYTES, m_pRB, ulSecondChunkSize);
        }

        m_ulReadPos = ulSecondChunkSize;
    }

    RB_DO_STAT_TotalReadSize((HI_U64)ulReadSize);

    SetNodeNumVal(HI_FALSE);

    return HI_SUCCESS;
}



HI_U32 CHI_COMMON_RingBuffer::RB_GetLen_X(void)
{
    HI_U32 len;
    if (NULL == m_pRB)
    {
        return HI_ERR_NULL_POINTER;
    }
    memcpy((VOID *)(&len), (VOID *)(m_pRB + m_ulReadPos), RB_LEN_BYTES);
    return len;
}


HI_U8 CHI_COMMON_RingBuffer::RB_GetFirstHI_U8(void)
{
    if (NULL == m_pRB)
    {
        return 0;
    }
    return (HI_U8)(*(HI_U8 *)(m_pRB + m_ulReadPos));
}

/*2Byte*/
HI_U16 CHI_COMMON_RingBuffer::RB_GetFirstHI_U16(void)
{
    if (NULL == m_pRB)
    {
        return 0;
    }
    return (HI_U16)(*(HI_U16 *)(m_pRB + m_ulReadPos));
}

HI_U32 CHI_COMMON_RingBuffer::RB_GetFirstHI_U32(void)
{
    if (NULL == m_pRB)
    {
        return HI_ERR_NULL_POINTER;
    }
    return (HI_U32)(*(HI_U32 *)(m_pRB + m_ulReadPos));
}

HI_U32 CHI_COMMON_RingBuffer::RB_GetMaxWritePercent(void)
{
    HI_U32 ulMaxWriteSize = RB_MaxWriteSize();
    return ulMaxWriteSize * 100 / m_ulSize * 100;
}

HI_U32 CHI_COMMON_RingBuffer::RB_GetMaxReadPercent(void)
{
    HI_U32 ulMaxReadSize = RB_MaxReadSize();
    return ulMaxReadSize * 100 / m_ulSize * 100;
}

HI_U32 CHI_COMMON_RingBuffer::RB_GetNodeNums(void)
{
    return m_ulNodeNums;
}

HI_S32 CHI_COMMON_RingBuffer::ReadCurPTS(HI_U32 *pulPTS)
{
    if (RB_GetNodeNums() > 0)
    {
        *pulPTS = ReadPTS(m_ulReadPos);
        return HI_SUCCESS;
    }
    else
    {
        return HI_ERR_ABORTED_OPERATION;
    }
}

HI_S32 CHI_COMMON_RingBuffer::ReadNextPTS(HI_U32 *pulPTS)
{
    if (RB_GetNodeNums() > 1)
    {
        HI_U32 ulPos = 0;
        ulPos = GetNextReadPos();
        *pulPTS = ReadPTS(ulPos);
        return HI_SUCCESS;
    }
    else
    {
        return HI_ERR_ABORTED_OPERATION;
    }

}

HI_U32 CHI_COMMON_RingBuffer::ReadPTS(HI_U32 ulReadPos)
{
    HI_U32 ulPTS = 0;
    /*读需要分段*/
    HI_U32 ulFirstChunkSize = 0;
    HI_U32 ulSecondChunkSize = 0;

    if (NULL == m_pRB)
    {
        return HI_ERR_NULL_POINTER;
    }

    if (RB_GetNodeNums() > 0)
    {
        if (ulReadPos + RB_LEN_BYTES * 2 < m_ulSize)
        {
            //直接取出,时间戳在存储到rb数据中的第0-3个字节
            memcpy((VOID *)(&ulPTS), (VOID *)(m_pRB + ulReadPos + RB_LEN_BYTES), RB_LEN_BYTES);
        }
        else
        {
            if ((ulReadPos + RB_LEN_BYTES) > m_ulSize)
            {
                //在开始循环取
                if ((HI_S32)((HI_S32)m_ulSize - (HI_S32)ulReadPos) < 0)
                {
                    return 0;
                }
                memcpy((VOID *)(&ulPTS), (VOID *)(m_pRB + RB_LEN_BYTES - (m_ulSize - ulReadPos)), RB_LEN_BYTES);
            }
            else
            {
                //两边都有
                ulFirstChunkSize = m_ulSize - (ulReadPos + RB_LEN_BYTES);
                ulSecondChunkSize = RB_LEN_BYTES - ulFirstChunkSize;
                memcpy((VOID *)(&ulPTS), (VOID *)(m_pRB + ulReadPos + RB_LEN_BYTES), ulFirstChunkSize);
                memcpy((VOID *)(&ulPTS + ulFirstChunkSize), (VOID *)m_pRB, ulSecondChunkSize);
            }
        }
    }
    return ulPTS;
}

HI_U32 CHI_COMMON_RingBuffer::GetNextReadPos(void)
{
    HI_U32 ulPos = 0;
    HI_U32 ulDataLen = 0;
    HI_U32 ulReadSize = 0;
    ulDataLen = RB_GetLen_X();
    ulReadSize = ALIGN_LENGTH(ulDataLen, RB_LEN_BYTES) + RB_LEN_BYTES;

    if (m_ulReadPos + ulReadSize < m_ulSize)
    {
        ulPos = m_ulReadPos + ulReadSize;
    }
    else
    {
        ulPos = m_ulReadPos + ulReadSize - m_ulSize;
    }

    return ulPos;
}

HI_S32 CHI_COMMON_RingBuffer::SkipOneNodeReadPos(void)
{
    HI_U32 ulMaxReadSize = 0;
    ulMaxReadSize = RB_MaxReadSize();
    if (ulMaxReadSize > 0)
    {
        m_ulReadPos = GetNextReadPos();
        SetNodeNumVal(HI_FALSE);

        return HI_SUCCESS;
    }
    else
    {
        return HI_ERR_ABORTED_OPERATION;
    }
}


HI_BOOL CHI_COMMON_RingBuffer::RB_GetPlayState()
{
	return m_bStartPlay;
}

