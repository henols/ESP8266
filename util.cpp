/*
 * util.cpp
 *
 *  Created on: 28 okt 2014
 *      Author: Hernik
 */


#include "util.h"
#include <stdlib.h>
#include <string.h>

int indexOf(const char* buf,unsigned int len, const char* val) {
	return indexOf(buf, len, val, 0);
}

int indexOf(const char* base, unsigned int len, const char* str, unsigned int startIndex) {
	unsigned int result;
	unsigned int baselen = len;
	// str should not longer than base
	if (strlen(str) > baselen || startIndex > baselen) {
		result = -1;
	} else {
		if (startIndex < 0) {
			startIndex = 0;
		}
		char* pos = strstr(base + startIndex, str);
		if (pos == NULL) {
			result = -1;
		} else {
			result = pos - base;
		}
	}
	return result;

//	int valLen = strlen(val);
//	for (int i = pos; i < len - valLen; i++) {
//		int j;
//		for (j = 0; j < valLen; j++) {
//			if (buf[i + j] != val[j]) {
//				j = -1;
//				break;
//			}
//		}
//		if (j == valLen) {
//			return i;
//		}
//	}
//
//	return -1;
}

bool endsWith(const char* res, int pos, const char ** texts, int textsSize) {
	for (int i = 0; i < textsSize; i++) {
		if (endsWith(res, pos, texts[i])) {
			return true;
		}
	}
	return false;
}

bool endsWith(const char* res, int pos, const char* str) {
	int blen = pos;
	int slen = strlen(str);
	return (blen >= slen) && (0 == strcmp(res + blen - slen, str));
}

void ip2Char(uint32_t ip, char * ipStr) {
	char * c = itoa((uint8_t)(ip),ipStr,10);
	ipStr[++c - ipStr] ='.';
	c = itoa((uint8_t)(ip >> 8),++c,10);
	ipStr[++c - ipStr] ='.';
	c = itoa((uint8_t)(ip >> 16),++c,10);
	ipStr[++c - ipStr] ='.';
	c = itoa((uint8_t)(ip >> 24),++c,10);
}


