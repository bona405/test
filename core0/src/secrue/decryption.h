/*
 * decryption.h
 *
 *  Created on: 2022. 11. 21.
 *      Author: HP
 */

#ifndef SRC_SECRUE_DECRYPTION_H_
#define SRC_SECRUE_DECRYPTION_H_

#include "stdlib.h"
#include "stdio.h"

#include "string.h"

typedef unsigned char BYTE;
typedef unsigned char* LPBYTE;
typedef const unsigned char *LPCBYTE;

#define KEYLEN      32

#define Nr          (KEYLEN/8+3)*2      //The number of rounds in AES Cipher.
#define KEYEXPSIZE  (Nr+1)*16
#define BLOCKLEN    16

#define SWAP(T,A,B)         {T=A; A=B; B=T;}
#define ROTATE(T,A,B,C,D)   {T=A; A=B; B=C; C=D; D=T;}


extern void AES_CBC_Decrypt(LPBYTE Output, LPCBYTE Input, int Length, LPCBYTE Key, LPCBYTE Iv);
static void GetSBox(LPBYTE TA);
static void KeyExpansion(LPBYTE ExpKey, LPCBYTE Key);
static void InvCipher(BYTE State[4][4], LPBYTE ExpKey);

#endif /* SRC_SECRUE_DECRYPTION_H_ */
