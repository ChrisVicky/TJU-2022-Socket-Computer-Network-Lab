/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：utils.c
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月15日
*   描    述：
*
*****************************************************************/

#define SUCCESS(format, ...) printf("\33[1;33m" format "\33[0m",## __VA_ARGS__)
#define ERROR(format, ...) printf("\33[1;31m ERROR: " format "\33[0m", ## __VA_ARGS__)
#define PRINT(format, ...) printf("\33[1;37m" format "\33[0m", ## __VA_ARGS__)
#define LOG(format, ...) printf("\33[1;36m LOG: " format "\33[0m", ## __VA_ARGS__)



