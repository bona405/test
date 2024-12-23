/*
 * sha.h
 *
 *  Created on: 2022. 11. 23.
 *      Author: HP
 */

#ifndef SRC_SECRUE_SHA_H_
#define SRC_SECRUE_SHA_H_

#include <stdlib.h>
#include <memory.h>

#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  SHAWORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
	BYTE data[64];
	SHAWORD datalen;
	unsigned long long bitlen;
	SHAWORD state[8];
} SHA256_CTX;

void sha256_init(SHA256_CTX* ctx);
void sha256_update(SHA256_CTX* ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX* ctx, BYTE hash[]);


#endif /* SRC_SECRUE_SHA_H_ */
