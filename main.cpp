/* 
 * main.cpp 
 * 
 *  Created on: 2013-5-27 
 *      Author: shiguang 
 */  
  
#include "CCycleBuffer.h"  
  
int main()  
{  
    CCycleBuffer buffer(100);  
    char* datachar = new char[40];  
    char* reschar = new char[100];  
  
    int res = 0;  
  
    for (int i = 0; i < 40; ++i)  
    {  
        datachar[i] = i + 1;  
    }  
    res = buffer.Write(datachar, 40);  
    printf("写入数据个数：%d\n", res);  
    res = buffer.Write(datachar, 40);  
    printf("写入数据个数：%d\n", res);  
    res = buffer.Write(datachar, 40);  
    printf("写入数据个数：%d\n", res);  
    res = buffer.Write(datachar, 40);  
    printf("写入数据个数：%d\n", res);  
  
    res = buffer.Read(reschar, 100);  
    printf("读取数据个数：%d\n", res);  
    for (int i = 0; i < 100; ++i)  
    {  
        if (i % 10 == 0)  
        {  
            printf("\n");  
        }  
        printf("%2d   ", reschar[i]);  
    }  
  
    return 0;  
}  