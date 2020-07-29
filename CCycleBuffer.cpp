/* 
 * CCycleBuffer.cpp 
 * 
 *  Created on: 2013-5-27 
 *      Author: shiguang 
 */  
  
#include "CCycleBuffer.h"  
// 定义  
CCycleBuffer::CCycleBuffer(int size)  
{  
    m_nBufSize = size;  
    m_nReadPos = 0;  
    m_nWritePos = 0;  
    m_pBuf = new char[m_nBufSize];  
    m_bEmpty = true;  
    m_bFull = false;  
}  
  
CCycleBuffer::~CCycleBuffer()  
{  
    delete[] m_pBuf;  
}  
  
/************************************************************************/  
/* 向缓冲区写入数据，返回实际写入的字节数                               */  
/************************************************************************/  
int CCycleBuffer::Write(char* buf, int count)  
{  
    if (count <= 0)  
        return 0;  
    m_bEmpty = false;  
    // 缓冲区已满，不能继续写入  
    if (m_bFull)  
    {  
        return 0;  
    }  
    else if (m_nReadPos == m_nWritePos) // 缓冲区为空时  
    {  
        /*                          == 内存模型 == 
         (empty)             m_nReadPos                (empty) 
         |----------------------------------|-----------------------------------------| 
         m_nWritePos        m_nBufSize 
         */  
        int leftcount = m_nBufSize - m_nWritePos;  
        if (leftcount > count)  
        {  
            memcpy(m_pBuf + m_nWritePos, buf, count);  
            m_nWritePos += count;  
            m_bFull = (m_nWritePos == m_nReadPos);  
            return count;  
        }  
        else  
        {  
            memcpy(m_pBuf + m_nWritePos, buf, leftcount);  
            m_nWritePos =  
                    (m_nReadPos > count - leftcount) ?  
                            count - leftcount : m_nReadPos; // 计算未写入数据大小  
            memcpy(m_pBuf, buf + leftcount, m_nWritePos);  
            m_bFull = (m_nWritePos == m_nReadPos);  
            return leftcount + m_nWritePos;  
        }  
    }  
    else if (m_nReadPos < m_nWritePos) // 有剩余空间可写入  
    {  
        /*                           == 内存模型 == 
         (empty)                 (data)                     (empty) 
         |-------------------|----------------------------|---------------------------| 
         m_nReadPos                m_nWritePos       (leftcount) 
         */  
        // 剩余缓冲区大小(从写入位置到缓冲区尾)  
        int leftcount = m_nBufSize - m_nWritePos;  
        if (leftcount > count)   // 有足够的剩余空间存放  
        {  
            memcpy(m_pBuf + m_nWritePos, buf, count);  
            m_nWritePos += count;  
            m_bFull = (m_nReadPos == m_nWritePos);  
            assert(m_nReadPos <= m_nBufSize);  
            assert(m_nWritePos <= m_nBufSize);  
            return count;  
        }  
        else       // 剩余空间不足  
        {  
            // 先填充满剩余空间，再回头找空间存放  
            memcpy(m_pBuf + m_nWritePos, buf, leftcount);  
            m_nWritePos =  
                    (m_nReadPos >= count - leftcount) ?  
                            count - leftcount : m_nReadPos;  
            memcpy(m_pBuf, buf + leftcount, m_nWritePos);  
            m_bFull = (m_nReadPos == m_nWritePos);  
            assert(m_nReadPos <= m_nBufSize);  
            assert(m_nWritePos <= m_nBufSize);  
            return leftcount + m_nWritePos;  
        }  
    }  
    else  
    {  
        /*                          == 内存模型 == 
         (unread)                 (read)                     (unread) 
         |-------------------|----------------------------|---------------------------| 
         m_nWritePos    (leftcount)    m_nReadPos 
         */  
        int leftcount = m_nReadPos - m_nWritePos;  
        if (leftcount > count)  
        {  
            // 有足够的剩余空间存放  
            memcpy(m_pBuf + m_nWritePos, buf, count);  
            m_nWritePos += count;  
            m_bFull = (m_nReadPos == m_nWritePos);  
            assert(m_nReadPos <= m_nBufSize);  
            assert(m_nWritePos <= m_nBufSize);  
            return count;  
        }  
        else  
        {  
            // 剩余空间不足时要丢弃后面的数据  
            memcpy(m_pBuf + m_nWritePos, buf, leftcount);  
            m_nWritePos += leftcount;  
            m_bFull = (m_nReadPos == m_nWritePos);  
            assert(m_bFull);  
            assert(m_nReadPos <= m_nBufSize);  
            assert(m_nWritePos <= m_nBufSize);  
            return leftcount;  
        }  
    }  
}  
  
/************************************************************************/  
/* 从缓冲区读数据，返回实际读取的字节数                                 */  
/************************************************************************/  
int CCycleBuffer::Read(char* buf, int count)  
{  
    if (count <= 0)  
        return 0;  
    m_bFull = false;  
    if (m_bEmpty)       // 缓冲区空，不能继续读取数据  
    {  
        return 0;  
    }  
    else if (m_nReadPos == m_nWritePos)   // 缓冲区满时  
    {  
        /*                          == 内存模型 == 
         (data)          m_nReadPos                (data) 
         |--------------------------------|--------------------------------------------| 
         m_nWritePos         m_nBufSize 
         */  
        int leftcount = m_nBufSize - m_nReadPos;  
        if (leftcount > count)  
        {  
            memcpy(buf, m_pBuf + m_nReadPos, count);  
            m_nReadPos += count;  
            m_bEmpty = (m_nReadPos == m_nWritePos);  
            return count;  
        }  
        else  
        {  
            memcpy(buf, m_pBuf + m_nReadPos, leftcount);  
            m_nReadPos =  
                    (m_nWritePos > count - leftcount) ?  
                            count - leftcount : m_nWritePos;  
            memcpy(buf + leftcount, m_pBuf, m_nReadPos);  
            m_bEmpty = (m_nReadPos == m_nWritePos);  
            return leftcount + m_nReadPos;  
        }  
    }  
    else if (m_nReadPos < m_nWritePos)   // 写指针在前(未读数据是连接的)  
    {  
        /*                          == 内存模型 == 
         (read)                 (unread)                      (read) 
         |-------------------|----------------------------|---------------------------| 
         m_nReadPos                m_nWritePos                     m_nBufSize 
         */  
        int leftcount = m_nWritePos - m_nReadPos;  
        int c = (leftcount > count) ? count : leftcount;  
        memcpy(buf, m_pBuf + m_nReadPos, c);  
        m_nReadPos += c;  
        m_bEmpty = (m_nReadPos == m_nWritePos);  
        assert(m_nReadPos <= m_nBufSize);  
        assert(m_nWritePos <= m_nBufSize);  
        return c;  
    }  
    else          // 读指针在前(未读数据可能是不连接的)  
    {  
        /*                          == 内存模型 == 
         (unread)                (read)                      (unread) 
         |-------------------|----------------------------|---------------------------| 
         m_nWritePos                  m_nReadPos                  m_nBufSize 
 
         */  
        int leftcount = m_nBufSize - m_nReadPos;  
        if (leftcount > count)   // 未读缓冲区够大，直接读取数据  
        {  
            memcpy(buf, m_pBuf + m_nReadPos, count);  
            m_nReadPos += count;  
            m_bEmpty = (m_nReadPos == m_nWritePos);  
            assert(m_nReadPos <= m_nBufSize);  
            assert(m_nWritePos <= m_nBufSize);  
            return count;  
        }  
        else       // 未读缓冲区不足，需回到缓冲区头开始读  
        {  
            memcpy(buf, m_pBuf + m_nReadPos, leftcount);  
            m_nReadPos =  
                    (m_nWritePos >= count - leftcount) ?  
                            count - leftcount : m_nWritePos;  // 计算剩余未读数据大小  
            memcpy(buf + leftcount, m_pBuf, m_nReadPos); // 注意目的指针位置  
            m_bEmpty = (m_nReadPos == m_nWritePos);  
            assert(m_nReadPos <= m_nBufSize);  
            assert(m_nWritePos <= m_nBufSize);  
            return leftcount + m_nReadPos;  
        }  
    }  
}  
  
/************************************************************************/  
/* 获取缓冲区有效数据长度                                               */  
/************************************************************************/  
int CCycleBuffer::GetLength()  
{  
    if (m_bEmpty)  
    {  
        return 0;  
    }  
    else if (m_bFull)  
    {  
        return m_nBufSize;  
    }  
    else if (m_nReadPos < m_nWritePos)  
    {  
        return m_nWritePos - m_nReadPos;  
    }  
    else  
    {  
        return m_nBufSize - m_nReadPos + m_nWritePos;  
    }  
}  
  
void CCycleBuffer::Empty()  
{  
    m_nReadPos = 0;  
    m_nWritePos = 0;  
    m_bEmpty = true;  
    m_bFull = false;  
}  
  
bool CCycleBuffer::isEmpty()  
{  
    return m_bEmpty;  
}  
  
bool CCycleBuffer::isFull()  
{  
    return m_bFull;  
}  