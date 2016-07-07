
/* This is an independent implementation of the Serpent encryption  */
/* algorithm designed by Ross Anderson, Eli Biham and Lars Knudsen  */
/* and offered as a candidate algorithm for the US NIST Advanced    */
/* Encryption Standard (AES) effort. The algorithm is subject to    */
/* Patent action by its designers but royalty free use will be      */
/* offered to all conforming implementations.                       */
/*                                                                  */
/* Copyright in this implementation is held by Dr B R Gladman but   */
/* I hereby give permission for its free direct or derivative use   */
/* subject to acknowledgment of its origin.                         */
/*                                                                  */
/* Dr Brian Gladman (gladman@seven77.demon.co.uk) 25th June 1998    */
/*                                                                  */
/* Timing data:

Algorithm: serpent (serpent.c)
128 bit key:
Key Setup:    2445 cycles
Encrypt:      1059 cycles =    24.2 mbits/sec
Decrypt:       947 cycles =    27.0 mbits/sec
Mean:         1003 cycles =    25.5 mbits/sec
192 bit key:
Key Setup:    2498 cycles
Encrypt:      1059 cycles =    24.2 mbits/sec
Decrypt:       953 cycles =    26.9 mbits/sec
Mean:         1006 cycles =    25.4 mbits/sec
256 bit key:
Key Setup:    2404 cycles
Encrypt:      1060 cycles =    24.2 mbits/sec
Decrypt:       941 cycles =    27.2 mbits/sec
Mean:         1000 cycles =    25.6 mbits/sec

*/

#include "../std_defs.h"

#define BLOCK_REVERSE

static char *alg_name[] = { "serpent", "serpent6.c" };

char **cipher_name()
{
    return alg_name;
}


/* Partially optimised Serpent S Box boolean functions derived  */
/* using a recursive descent analyser but without a full search */
/* of all subtrees. The average number of terms per S box is    */
/* just less than 17.                                           */
/*                                                              */
/* Copyright:   Dr B. R Gladman (gladman@seven77.demon.co.uk)   */
/*              30th June 1998                                  */
/*                                                              */
/* I hereby give permission for the information in this file to */
/* be used freely subject only to acknowledgement of its origin */

/* 17 terms */

#define sb0(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t6,t7,t8,t9,t11,t12,t13,t15,t16;        \
    t1 = b ^ d;         \
    t2 = ~t1;           \
    t3 = a | d;         \
    t4 = b ^ c;         \
    h = t3 ^ t4;        \
    t6 = a ^ b;         \
    t7 = a | t4;        \
    t8 = c & t6;        \
    t9 = t2 | t8;       \
    e = t7 ^ t9;        \
    t11 = a ^ h;        \
    t12 = t1 & t6;      \
    t13 = e ^ t11;      \
    f = t12 ^ t13;      \
    t15 = e | f;        \
    t16 = t3 & t15;     \
    g = b ^ t16;        \
}

/* 17 terms */

#define ib0(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t6,t7,t8,t9,t11,t12,t13,t15,t16;        \
    t1 = a ^ d;         \
    t2 = c ^ d;         \
    t3 = ~t2;           \
    t4 = a | b;         \
    g = t3 ^ t4;        \
    t6 = b ^ t1;        \
    t7 = c | t6;        \
    t8 = a ^ t7;        \
    t9 = t2 & t8;       \
    f = t6 ^ t9;        \
    t11 = ~t8;          \
    t12 = b & d;        \
    t13 = f | t12;      \
    h = t11 ^ t13;      \
    t15 = t2 ^ t12;     \
    t16 = f | h;        \
    e = t15 ^ t16;      \
}

/* 18 terms */

#define sb1(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t9,t10,t12,t13,t14,t16,t17;    \
    t1 = a ^ d;         \
    t2 = b ^ d;         \
    t3 = a & b;         \
    t4 = ~c;            \
    t5 = t2 ^ t3;       \
    g = t4 ^ t5;        \
    t7 = a ^ t2;        \
    t8 = b | t4;        \
    t9 = d | g;         \
    t10 = t7 & t9;      \
    f = t8 ^ t10;       \
    t12 = c ^ d;        \
    t13 = t1 | t2;      \
    t14 = f ^ t12;      \
    h = t13 ^ t14;      \
    t16 = t1 | g;       \
    t17 = t8 ^ t14;     \
    e = t16 ^ t17;      \
}

/* 17 terms */

#define ib1(a,b,c,d,e,f,g,h)                                \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t9,t11,t12,t13,t15,t16;    \
    t1 = a ^ d;         \
    t2 = a & b;         \
    t3 = b ^ c;         \
    t4 = a ^ t3;        \
    t5 = b | d;         \
    h = t4 ^ t5;        \
    t7 = c | t1;        \
    t8 = b ^ t7;        \
    t9 = t4 & t8;       \
    f = t1 ^ t9;        \
    t11 = ~t2;          \
    t12 = h & f;        \
    t13 = t9 ^ t11;     \
    g = t12 ^ t13;      \
    t15 = a & d;        \
    t16 = c ^ t13;      \
    e = t15 ^ t16;      \
}

/* 16 terms */

#define sb2(a,b,c,d,e,f,g,h)                            \
{   u4byte  t1,t2,t3,t5,t6,t7,t9,t10,t11,t13,t14,t15;   \
    t1 = ~a;            \
    t2 = b ^ d;         \
    t3 = c & t1;        \
    e = t2 ^ t3;        \
    t5 = c ^ t1;        \
    t6 = c ^ e;         \
    t7 = b & t6;        \
    h = t5 ^ t7;        \
    t9 = d | t7;        \
    t10 = e | t5;       \
    t11 = t9 & t10;     \
    g = a ^ t11;        \
    t13 = d | t1;       \
    t14 = t2 ^ h;       \
    t15 = g ^ t13;      \
    f = t14 ^ t15;      \
}

/* 16 terms */

#define ib2(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t9,t11,t12,t14,t15;            \
    t1 = b ^ d;         \
    t2 = ~t1;           \
    t3 = a ^ c;         \
    t4 = c ^ t1;        \
    t5 = b & t4;        \
    e = t3 ^ t5;        \
    t7 = a | t2;        \
    t8 = d ^ t7;        \
    t9 = t3 | t8;       \
    h = t1 ^ t9;        \
    t11 = ~t4;          \
    t12 = e | h;        \
    f = t11 ^ t12;      \
    t14 = d & t11;      \
    t15 = t3 ^ t12;     \
    g = t14 ^ t15;      \
}

/* 18 terms */

#define sb3(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t5,t6,t8,t9,t10,t12,t13,t15,t16,t17;    \
    t1 = a ^ c;         \
    t2 = a | d;         \
    t3 = a & b;         \
    t4 = a & d;         \
    t5 = b | t4;        \
    t6 = t1 & t2;       \
    f = t5 ^ t6;        \
    t8 = b ^ d;         \
    t9 = c | t3;        \
    t10 = t6 ^ t8;      \
    h = t9 ^ t10;       \
    t12 = c ^ t3;       \
    t13 = t2 & h;       \
    g = t12 ^ t13;      \
    t15 = ~g;           \
    t16 = t2 ^ t3;      \
    t17 = f & t15;      \
    e = t16 ^ t17;      \
}

/* 17 terms */

#define ib3(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t9,t11,t12,t14,t15,t16;        \
    t1 = b ^ c;         \
    t2 = b | c;         \
    t3 = a ^ c;         \
    t4 = t2 ^ t3;       \
    t5 = d | t4;        \
    e = t1 ^ t5;        \
    t7 = a ^ d;         \
    t8 = t1 | t5;       \
    t9 = t2 ^ t7;       \
    g = t8 ^ t9;        \
    t11 = a & t4;       \
    t12 = e | t9;       \
    f = t11 ^ t12;      \
    t14 = a & g;        \
    t15 = t2 ^ t14;     \
    t16 = e & t15;      \
    h = t4 ^ t16;       \
}

/* 17 terms */

#define sb4(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t10,t11,t12,t14,t15,t16;       \
    t1 = ~a;            \
    t2 = a ^ d;         \
    t3 = a ^ b;         \
    t4 = c ^ t1;        \
    t5 = t2 | t3;       \
    e = t4 ^ t5;        \
    t7 = ~e;            \
    t8 = b | t7;        \
    h = t2 ^ t8;        \
    t10 = a & e;        \
    t11 = b ^ h;        \
    t12 = t8 & t11;     \
    g = t10 ^ t12;      \
    t14 = a | t7;       \
    t15 = t3 ^ t14;     \
    t16 = h & g;        \
    f = t15 ^ t16;      \
}

/* 17 terms */

#define ib4(a,b,c,d,e,f,g,h)                                    \
{   u4byte  t1,t2,t3,t4,t6,t7,t8,t10,t11,t12,t14,t15,t16;       \
    t1 = c ^ d;         \
    t2 = c | d;         \
    t3 = b ^ t2;        \
    t4 = a & t3;        \
    f = t1 ^ t4;        \
    t6 = a ^ d;         \
    t7 = b | d;         \
    t8 = t6 & t7;       \
    h = t3 ^ t8;        \
    t10 = ~a;           \
    t11 = c ^ h;        \
    t12 = t10 | t11;    \
    e = t3 ^ t12;       \
    t14 = c | t4;       \
    t15 = t7 ^ t14;     \
    t16 = h | t10;      \
    g = t15 ^ t16;      \
}

/* 17 terms */

#define sb5(a,b,c,d,e,f,g,h)                                \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t10,t11,t12,t14,t15,t16;   \
    t1 = ~a;            \
    t2 = a ^ b;         \
    t3 = a ^ d;         \
    t4 = c ^ t1;        \
    t5 = t2 | t3;       \
    e = t4 ^ t5;        \
    t7 = ~d;            \
    t8 = e & t7;        \
    f = t2 ^ t8;        \
    t10 = b | f;        \
    t11 = c | e;        \
    t12 = t7 ^ t10;     \
    h = t11 ^ t12;      \
    t14 = d | f;        \
    t15 = t1 ^ t14;     \
    t16 = e | h;        \
    g = t15 ^ t16;      \
}

/* 16 terms */

#define ib5(a,b,c,d,e,f,g,h)                                \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t10,t11,t13,t14,t15;       \
    t1 = ~c;            \
    t2 = b & t1;        \
    t3 = d ^ t2;        \
    t4 = a & t3;        \
    t5 = b ^ t1;        \
    h = t4 ^ t5;        \
    t7 = b | h;         \
    t8 = a & t7;        \
    f = t3 ^ t8;        \
    t10 = a | d;        \
    t11 = t1 ^ t7;      \
    e = t10 ^ t11;      \
    t13 = a ^ c;        \
    t14 = b & t10;      \
    t15 = t4 | t13;     \
    g = t14 ^ t15;      \
} 

/* 17 terms */

#define sb6(a,b,c,d,e,f,g,h)                                \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t9,t11,t12,t13,t15,t16;    \
    t1 = a ^ c;         \
    t2 = b | d;         \
    t3 = b ^ c;         \
    t4 = ~t3;           \
    t5 = a & d;         \
    f = t4 ^ t5;        \
    t7 = b | c;         \
    t8 = d ^ t1;        \
    t9 = t7 & t8;       \
    h = t2 ^ t9;        \
    t11 = t1 & t7;      \
    t12 = t4 ^ t8;      \
    t13 = h & t11;      \
    e = t12 ^ t13;      \
    t15 = t3 ^ t11;     \
    t16 = h | t15;      \
    g = t12 ^ t16;      \
}

/* 17 terms */

#define ib6(a,b,c,d,e,f,g,h)                                \
{   u4byte  t1,t2,t3,t4,t6,t7,t8,t9,t11,t12,t13,t15,t16;    \
    t1 = ~c;            \
    t2 = a ^ c;         \
    t3 = b ^ d;         \
    t4 = a | t1;        \
    f = t3 ^ t4;        \
    t6 = a | b;         \
    t7 = b & t2;        \
    t8 = f ^ t6;        \
    t9 = t7 | t8;       \
    e = c ^ t9;         \
    t11 = ~f;           \
    t12 = d | t2;       \
    t13 = t9 ^ t11;     \
    h = t12 ^ t13;      \
    t15 = b ^ t11;      \
    t16 = e & h;        \
    g = t15 ^ t16;      \
}

/* 17 terms */

#define sb7(a,b,c,d,e,f,g,h)                                \
{   u4byte  t1,t2,t3,t4,t5,t7,t8,t9,t11,t12,t13,t15,t16;    \
    t1 = ~c;            \
    t2 = b ^ c;         \
    t3 = b | t1;        \
    t4 = d ^ t3;        \
    t5 = a & t4;        \
    h = t2 ^ t5;        \
    t7 = a ^ d;         \
    t8 = b ^ t5;        \
    t9 = t2 | t8;       \
    f = t7 ^ t9;        \
    t11 = d & t3;       \
    t12 = t5 ^ f;       \
    t13 = h & t12;      \
    g = t11 ^ t13;      \
    t15 = t1 | t4;      \
    t16 = t12 ^ g;      \
    e = t15 ^ t16;      \
}

/* 17 terms */

#define ib7(a,b,c,d,e,f,g,h)                                \
{   u4byte  t1,t2,t3,t4,t6,t7,t8,t9,t11,t12,t14,t15,t16;    \
    t1 = a & b;         \
    t2 = a | b;         \
    t3 = c | t1;        \
    t4 = d & t2;        \
    h = t3 ^ t4;        \
    t6 = ~d;            \
    t7 = b ^ t4;        \
    t8 = h ^ t6;        \
    t9 = t7 | t8;       \
    f = a ^ t9;         \
    t11 = c ^ t7;       \
    t12 = d | f;        \
    e = t11 ^ t12;      \
    t14 = a & h;        \
    t15 = t3 ^ f;       \
    t16 = e ^ t14;      \
    g = t15 ^ t16;      \
}

#define k_xor(r,a,b,c,d)    \
{   a ^= l_key[4 * r +  8]; \
    b ^= l_key[4 * r +  9]; \
    c ^= l_key[4 * r + 10]; \
    d ^= l_key[4 * r + 11]; \
}

#define k_set(r,a,b,c,d)    \
{   a = l_key[4 * r +  8];  \
    b = l_key[4 * r +  9];  \
    c = l_key[4 * r + 10];  \
    d = l_key[4 * r + 11];  \
}

#define k_get(r,a,b,c,d)    \
{   l_key[4 * r +  8] = a;  \
    l_key[4 * r +  9] = b;  \
    l_key[4 * r + 10] = c;  \
    l_key[4 * r + 11] = d;  \
}

/* the linear transformation and its inverse    */

#define rot(a,b,c,d)    \
{   a = rotl(a, 13);    \
    c = rotl(c, 3);     \
    d ^= c ^ (a << 3);  \
    b ^= a ^ c;         \
    d = rotl(d, 7);     \
    b = rotl(b, 1);     \
    a ^= b ^ d;         \
    c ^= d ^ (b << 7);  \
    a = rotl(a, 5);     \
    c = rotl(c, 22);    \
}

#define irot(a,b,c,d)   \
{   c = rotr(c, 22);    \
    a = rotr(a, 5);     \
    c ^= d ^ (b << 7);  \
    a ^= b ^ d;         \
    d = rotr(d, 7);     \
    b = rotr(b, 1);     \
    d ^= c ^ (a << 3);  \
    b ^= a ^ c;         \
    c = rotr(c, 3);     \
    a = rotr(a, 13);    \
}

u4byte  l_key[140]; /* storage for the key schedule         */

/* initialise the key schedule from the user supplied key   */

u4byte *set_key(const u4byte in_key[], const u4byte key_len)
{    u4byte  i,lk,a,b,c,d,e,f,g,h;

    if(key_len < 0 || key_len > 256)

        return (u4byte*)0;

    i = 0; lk = (key_len + 31) / 32;
    
    while(i < lk)
    {
#ifdef  BLOCK_REVERSE
        l_key[i] = bswap(in_key[lk - i - 1]);
#else
        l_key[i] = in_key[i];
#endif  
        i++;
    }

    if(key_len < 256)
    {
        while(i < 8)

            l_key[i++] = 0;

        i = key_len / 32; lk = 1 << key_len % 32; 

        l_key[i] = l_key[i] & (lk - 1) | lk;
    }

    for(i = 0; i < 132; ++i)
    {
        lk = l_key[i] ^ l_key[i + 3] ^ l_key[i + 5] 
                                ^ l_key[i + 7] ^ 0x9e3779b9 ^ i;

        l_key[i + 8] = (lk << 11) | (lk >> 21); 
    }

    k_set( 0,a,b,c,d);sb3(a,b,c,d,e,f,g,h);k_get( 0,e,f,g,h);
    k_set( 1,a,b,c,d);sb2(a,b,c,d,e,f,g,h);k_get( 1,e,f,g,h);
    k_set( 2,a,b,c,d);sb1(a,b,c,d,e,f,g,h);k_get( 2,e,f,g,h);
    k_set( 3,a,b,c,d);sb0(a,b,c,d,e,f,g,h);k_get( 3,e,f,g,h);
    k_set( 4,a,b,c,d);sb7(a,b,c,d,e,f,g,h);k_get( 4,e,f,g,h);
    k_set( 5,a,b,c,d);sb6(a,b,c,d,e,f,g,h);k_get( 5,e,f,g,h);
    k_set( 6,a,b,c,d);sb5(a,b,c,d,e,f,g,h);k_get( 6,e,f,g,h);
    k_set( 7,a,b,c,d);sb4(a,b,c,d,e,f,g,h);k_get( 7,e,f,g,h);
    k_set( 8,a,b,c,d);sb3(a,b,c,d,e,f,g,h);k_get( 8,e,f,g,h);
    k_set( 9,a,b,c,d);sb2(a,b,c,d,e,f,g,h);k_get( 9,e,f,g,h);
    k_set(10,a,b,c,d);sb1(a,b,c,d,e,f,g,h);k_get(10,e,f,g,h);
    k_set(11,a,b,c,d);sb0(a,b,c,d,e,f,g,h);k_get(11,e,f,g,h);
    k_set(12,a,b,c,d);sb7(a,b,c,d,e,f,g,h);k_get(12,e,f,g,h);
    k_set(13,a,b,c,d);sb6(a,b,c,d,e,f,g,h);k_get(13,e,f,g,h);
    k_set(14,a,b,c,d);sb5(a,b,c,d,e,f,g,h);k_get(14,e,f,g,h);
    k_set(15,a,b,c,d);sb4(a,b,c,d,e,f,g,h);k_get(15,e,f,g,h);
    k_set(16,a,b,c,d);sb3(a,b,c,d,e,f,g,h);k_get(16,e,f,g,h);
    k_set(17,a,b,c,d);sb2(a,b,c,d,e,f,g,h);k_get(17,e,f,g,h);
    k_set(18,a,b,c,d);sb1(a,b,c,d,e,f,g,h);k_get(18,e,f,g,h);
    k_set(19,a,b,c,d);sb0(a,b,c,d,e,f,g,h);k_get(19,e,f,g,h);
    k_set(20,a,b,c,d);sb7(a,b,c,d,e,f,g,h);k_get(20,e,f,g,h);
    k_set(21,a,b,c,d);sb6(a,b,c,d,e,f,g,h);k_get(21,e,f,g,h);
    k_set(22,a,b,c,d);sb5(a,b,c,d,e,f,g,h);k_get(22,e,f,g,h);
    k_set(23,a,b,c,d);sb4(a,b,c,d,e,f,g,h);k_get(23,e,f,g,h);
    k_set(24,a,b,c,d);sb3(a,b,c,d,e,f,g,h);k_get(24,e,f,g,h);
    k_set(25,a,b,c,d);sb2(a,b,c,d,e,f,g,h);k_get(25,e,f,g,h);
    k_set(26,a,b,c,d);sb1(a,b,c,d,e,f,g,h);k_get(26,e,f,g,h);
    k_set(27,a,b,c,d);sb0(a,b,c,d,e,f,g,h);k_get(27,e,f,g,h);
    k_set(28,a,b,c,d);sb7(a,b,c,d,e,f,g,h);k_get(28,e,f,g,h);
    k_set(29,a,b,c,d);sb6(a,b,c,d,e,f,g,h);k_get(29,e,f,g,h);
    k_set(30,a,b,c,d);sb5(a,b,c,d,e,f,g,h);k_get(30,e,f,g,h);
    k_set(31,a,b,c,d);sb4(a,b,c,d,e,f,g,h);k_get(31,e,f,g,h);
    k_set(32,a,b,c,d);sb3(a,b,c,d,e,f,g,h);k_get(32,e,f,g,h);

    return l_key;
};

/* encrypt a block of text  */

void encrypt(const u4byte in_blk[4], u4byte out_blk[])
{    u4byte  a,b,c,d,e,f,g,h;
    
#ifdef  BLOCK_REVERSE
    a = bswap(in_blk[3]); b = bswap(in_blk[2]); 
    c = bswap(in_blk[1]); d = bswap(in_blk[0]);
#else
    a = in_blk[0]; b = in_blk[1]; c = in_blk[2]; d = in_blk[3];
#endif

    k_xor( 0,a,b,c,d); sb0(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor( 1,e,f,g,h); sb1(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor( 2,a,b,c,d); sb2(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor( 3,e,f,g,h); sb3(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor( 4,a,b,c,d); sb4(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor( 5,e,f,g,h); sb5(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor( 6,a,b,c,d); sb6(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor( 7,e,f,g,h); sb7(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor( 8,a,b,c,d); sb0(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor( 9,e,f,g,h); sb1(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(10,a,b,c,d); sb2(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(11,e,f,g,h); sb3(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(12,a,b,c,d); sb4(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(13,e,f,g,h); sb5(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(14,a,b,c,d); sb6(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(15,e,f,g,h); sb7(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(16,a,b,c,d); sb0(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(17,e,f,g,h); sb1(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(18,a,b,c,d); sb2(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(19,e,f,g,h); sb3(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(20,a,b,c,d); sb4(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(21,e,f,g,h); sb5(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(22,a,b,c,d); sb6(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(23,e,f,g,h); sb7(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(24,a,b,c,d); sb0(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(25,e,f,g,h); sb1(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(26,a,b,c,d); sb2(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(27,e,f,g,h); sb3(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(28,a,b,c,d); sb4(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(29,e,f,g,h); sb5(e,f,g,h,a,b,c,d); rot(a,b,c,d); 
    k_xor(30,a,b,c,d); sb6(a,b,c,d,e,f,g,h); rot(e,f,g,h); 
    k_xor(31,e,f,g,h); sb7(e,f,g,h,a,b,c,d); k_xor(32,a,b,c,d); 
    
#ifdef  BLOCK_REVERSE
    out_blk[3] = bswap(a); out_blk[2] = bswap(b); 
    out_blk[1] = bswap(c); out_blk[0] = bswap(d);
#else
    out_blk[0] = a; out_blk[1] = b; out_blk[2] = c; out_blk[3] = d;
#endif
};

/* decrypt a block of text  */

void decrypt(const u4byte in_blk[4], u4byte out_blk[4])
{   u4byte  a,b,c,d,e,f,g,h;
    
#ifdef  BLOCK_REVERSE
    a = bswap(in_blk[3]); b = bswap(in_blk[2]); 
    c = bswap(in_blk[1]); d = bswap(in_blk[0]);
#else
    a = in_blk[0]; b = in_blk[1]; c = in_blk[2]; d = in_blk[3];
#endif

    k_xor(32,a,b,c,d); ib7(a,b,c,d,e,f,g,h); k_xor(31,e,f,g,h);
    irot(e,f,g,h); ib6(e,f,g,h,a,b,c,d); k_xor(30,a,b,c,d);
    irot(a,b,c,d); ib5(a,b,c,d,e,f,g,h); k_xor(29,e,f,g,h);
    irot(e,f,g,h); ib4(e,f,g,h,a,b,c,d); k_xor(28,a,b,c,d);
    irot(a,b,c,d); ib3(a,b,c,d,e,f,g,h); k_xor(27,e,f,g,h);
    irot(e,f,g,h); ib2(e,f,g,h,a,b,c,d); k_xor(26,a,b,c,d);
    irot(a,b,c,d); ib1(a,b,c,d,e,f,g,h); k_xor(25,e,f,g,h);
    irot(e,f,g,h); ib0(e,f,g,h,a,b,c,d); k_xor(24,a,b,c,d);
    irot(a,b,c,d); ib7(a,b,c,d,e,f,g,h); k_xor(23,e,f,g,h);
    irot(e,f,g,h); ib6(e,f,g,h,a,b,c,d); k_xor(22,a,b,c,d);
    irot(a,b,c,d); ib5(a,b,c,d,e,f,g,h); k_xor(21,e,f,g,h);
    irot(e,f,g,h); ib4(e,f,g,h,a,b,c,d); k_xor(20,a,b,c,d);
    irot(a,b,c,d); ib3(a,b,c,d,e,f,g,h); k_xor(19,e,f,g,h);
    irot(e,f,g,h); ib2(e,f,g,h,a,b,c,d); k_xor(18,a,b,c,d);
    irot(a,b,c,d); ib1(a,b,c,d,e,f,g,h); k_xor(17,e,f,g,h);
    irot(e,f,g,h); ib0(e,f,g,h,a,b,c,d); k_xor(16,a,b,c,d);
    irot(a,b,c,d); ib7(a,b,c,d,e,f,g,h); k_xor(15,e,f,g,h);
    irot(e,f,g,h); ib6(e,f,g,h,a,b,c,d); k_xor(14,a,b,c,d);
    irot(a,b,c,d); ib5(a,b,c,d,e,f,g,h); k_xor(13,e,f,g,h);
    irot(e,f,g,h); ib4(e,f,g,h,a,b,c,d); k_xor(12,a,b,c,d);
    irot(a,b,c,d); ib3(a,b,c,d,e,f,g,h); k_xor(11,e,f,g,h);
    irot(e,f,g,h); ib2(e,f,g,h,a,b,c,d); k_xor(10,a,b,c,d);
    irot(a,b,c,d); ib1(a,b,c,d,e,f,g,h); k_xor( 9,e,f,g,h);
    irot(e,f,g,h); ib0(e,f,g,h,a,b,c,d); k_xor( 8,a,b,c,d);
    irot(a,b,c,d); ib7(a,b,c,d,e,f,g,h); k_xor( 7,e,f,g,h);
    irot(e,f,g,h); ib6(e,f,g,h,a,b,c,d); k_xor( 6,a,b,c,d);
    irot(a,b,c,d); ib5(a,b,c,d,e,f,g,h); k_xor( 5,e,f,g,h);
    irot(e,f,g,h); ib4(e,f,g,h,a,b,c,d); k_xor( 4,a,b,c,d);
    irot(a,b,c,d); ib3(a,b,c,d,e,f,g,h); k_xor( 3,e,f,g,h);
    irot(e,f,g,h); ib2(e,f,g,h,a,b,c,d); k_xor( 2,a,b,c,d);
    irot(a,b,c,d); ib1(a,b,c,d,e,f,g,h); k_xor( 1,e,f,g,h);
    irot(e,f,g,h); ib0(e,f,g,h,a,b,c,d); k_xor( 0,a,b,c,d);
    
#ifdef  BLOCK_REVERSE
    out_blk[3] = bswap(a); out_blk[2] = bswap(b); 
    out_blk[1] = bswap(c); out_blk[0] = bswap(d);
#else
    out_blk[0] = a; out_blk[1] = b; out_blk[2] = c; out_blk[3] = d;
#endif
};
