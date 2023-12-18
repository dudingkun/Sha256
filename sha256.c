/*
 * sha256.c
 */

#include "sha256.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#define U32             (uint32_t)

#define ROR(x, y)       ((U32(x) >> U32(y)) | U32((U32(x) << (32 - U32(y))) & 0xffffffff))
#define SHR(x, y)       (U32(x) >> U32(y))

#define CH(x, y, z)     ((U32(x) & U32(y)) ^ ((~U32(x)) & U32(z)))
#define MAJ(x, y, z)    ((U32(x) & U32(y)) ^ (U32(x) & U32(z)) ^ (U32(y) & U32(z)))
#define S0(x)           (ROR(U32(x), 2) ^ ROR(U32(x), 13) ^ ROR(U32(x), 22))
#define S1(x)           (ROR(U32(x), 6) ^ ROR(U32(x), 11) ^ ROR(U32(x), 25))
#define G0(x)           (ROR(U32(x), 7) ^ ROR(U32(x), 18) ^ SHR(U32(x), 3))
#define G1(x)           (ROR(U32(x), 17) ^ ROR(U32(x), 19) ^ SHR(U32(x), 10))

enum CharacterVariable
{
    a = 0, b, c, d, e, f, g, h
};


static const uint32_t g_auiK[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static const uint32_t g_auiH[8] = {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19,
};

static void U64ToU8BufBe(const uint64_t uiU64, uint8_t aucBeBuf[8])
{
    uint64_t uiU64Temp = uiU64;

    for (uint8_t i = 0; i < 8; i++)
    {
        aucBeBuf[7 - i] = (uint8_t)(uiU64Temp % 256U);

        uiU64Temp /= 256U;
    }
}

static void U32ToU8BufBe(const uint32_t uiU32, uint8_t aucBeBuf[4])
{
    uint32_t uiU32Temp = uiU32;

    for (uint8_t i = 0; i < 4; i++)
    {
        aucBeBuf[3 - i] = (uint8_t)(uiU32Temp % 256U);

        uiU32Temp /= 256U;
    }
}

static uint32_t U8BufToU32Be(const uint8_t aucBuf[4])
{
    uint32_t uiU32 = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        uiU32 *= 256;
        uiU32 += (uint32_t)aucBuf[i];
    }

    return uiU32;
}

static void PreDealAddBit(uint8_t aucMsgBlock[64], const uint64_t uiTotalLen, uint8_t *pucIsRemainder)
{
    const uint32_t uiBlockLen = uiTotalLen % 64;

    if (uiBlockLen < 64)
    {
        aucMsgBlock[uiBlockLen] = 0x80U;

        if (uiBlockLen < 56)
        {
            for (uint8_t i = uiBlockLen + 1; i < 56; i++)
            {
                aucMsgBlock[i] = 0U;
            }

            U64ToU8BufBe(uiTotalLen * 8, &aucMsgBlock[56]);

            *pucIsRemainder = 0;
        }
        else
        {
            for (uint8_t i = uiBlockLen + 1; i < 64; i++)
            {
                aucMsgBlock[i] = 0U;
            }

            *pucIsRemainder = 1;
        }
    }
    else
    {
        *pucIsRemainder = 2;
    }
}

static void AddOneMsgBlock(uint8_t aucMsgBlock[64], const uint64_t uiTotalLen, uint8_t ucIsRemainder)
{
    if (1 == ucIsRemainder)
    {
        memset(&aucMsgBlock[0], 0U, 56);

        U64ToU8BufBe(uiTotalLen, &aucMsgBlock[56]);
    }
    else if (2 == ucIsRemainder)
    {
        aucMsgBlock[0] = 0x80U;

        memset(&aucMsgBlock[1], 0U, 56 - 1);

        U64ToU8BufBe(uiTotalLen, &aucMsgBlock[56]);
    }
    else
    {

    }
}

static void CalaParamW(const uint32_t auiM[16], uint32_t auiW[64])
{
    uint32_t i = 0;
    for (i = 0; i < 16; i++)
    {
        auiW[i] = auiM[i];
    }

    for (; i < 64; i++)
    {
        auiW[i] = G1(auiW[i - 2]) + auiW[i - 7] + G0(auiW[i - 15]) + auiW[i - 16];
    }
}

static void Compression(uint32_t auiAToH[8], const uint32_t auiW[64])
{
    uint32_t auiA2HTemp[8] = {0};
    memcpy(auiA2HTemp, auiAToH, sizeof(auiA2HTemp));

    for (uint32_t i = 0; i < 64; i++)
    {
        const uint32_t uiTemp1 = auiA2HTemp[h] + S1(auiA2HTemp[e]) + \
                                    CH(auiA2HTemp[e], auiA2HTemp[f], auiA2HTemp[g]) + g_auiK[i] + auiW[i];
        const uint32_t uiTemp2 = S0(auiA2HTemp[a]) + MAJ(auiA2HTemp[a], auiA2HTemp[b], auiA2HTemp[c]);

        auiA2HTemp[h] = auiA2HTemp[g];
        auiA2HTemp[g] = auiA2HTemp[f];
        auiA2HTemp[f] = auiA2HTemp[e];
        auiA2HTemp[e] = auiA2HTemp[d] + uiTemp1;
        auiA2HTemp[d] = auiA2HTemp[c];
        auiA2HTemp[c] = auiA2HTemp[b];
        auiA2HTemp[b] = auiA2HTemp[a];
        auiA2HTemp[a] = uiTemp1 + uiTemp2;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        auiAToH[i] += auiA2HTemp[i];
    }
}

static void GetM(const uint8_t aucData[64], uint32_t auiM[16])
{
    for (uint8_t i = 0; i < 16; i++)
    {
        auiM[i] = U8BufToU32Be(&aucData[i * 4]);
    }
}

int32_t Sha256(const void *pvData, uint64_t uiDateLen, uint8_t aucResult[32])
{
    uint32_t auiH[8] = {0};
    uint8_t *pucData = (uint8_t *)pvData;
    uint32_t auiM[16] = {0};
    uint32_t auiW[64] = {0};
    uint8_t ucIsRemainder = 0U;
    uint8_t aucRemainderMsgBuf[64] = {0};

    memcpy(auiH, g_auiH, sizeof(g_auiH));

    if (uiDateLen < 0x100000000)
    {
        const uint32_t uiBlockNum = uiDateLen / 64;
        uint32_t i = 0;

        for (i = 0; i < uiBlockNum; i++)
        {
            GetM(&pucData[64 * i], auiM);

            CalaParamW(auiM, auiW);

            Compression(auiH, auiW);
        }

        if (0 != (uiDateLen % 64))
        {
            memcpy(aucRemainderMsgBuf, &pucData[64 * i], uiDateLen % 64);
            PreDealAddBit(aucRemainderMsgBuf, uiDateLen, &ucIsRemainder);
            GetM(aucRemainderMsgBuf, auiM);
            CalaParamW(auiM, auiW);
            Compression(auiH, auiW);
        }
        else
        {
            ucIsRemainder = 2;
        }

        if (0 != ucIsRemainder)
        {
            AddOneMsgBlock(aucRemainderMsgBuf, uiDateLen, ucIsRemainder);
            GetM(aucRemainderMsgBuf, auiM);
            CalaParamW(auiM, auiW);
            Compression(auiH, auiW);
        }
    }
    else
    {
        // 
    }
    for (uint8_t i = 0; i < 8; i++)
    {
        U32ToU8BufBe(auiH[i], &aucResult[i * 4]);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif      // __cplusplus
