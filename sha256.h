/*
 * sha256.h
 */

#ifndef SHA256_H_
#define SHA256_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

int32_t Sha256(const void *pvData, uint32_t uiDateLen, uint8_t aucResult[32]);

#ifdef __cplusplus
}
#endif      // __cplusplus

#endif      // SHA256_H_