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

int32_t sha256(const void *pvdata, uint32_t uidateLen, uint8_t aucresult[32]);

#ifdef __cplusplus
}
#endif      // __cplusplus

#endif      // SHA256_H_