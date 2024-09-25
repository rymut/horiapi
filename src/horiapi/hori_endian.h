#pragma once

#ifndef __BYTE_ORDER__
#define __ORDER_LITTLE_ENDIAN__ 1
#define __ORDER_BIG_ENDIAN__ 2
#define __ORDER_PDP_ENDIAN__ 3

#if 0x41424344UL == ('ABCD')
#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#elif 0x44434241UL == ('ABCD')
#define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
#elif 0x42414443UL == ('ABCD')
#define __BYTE_ORDER__ __ORDER_PDP_ENDIAN__
#else
#error "Unknown byte order"
#endif 

#endif
