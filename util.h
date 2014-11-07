/*
 * util.h
 *
 *  Created on: 28 okt 2014
 *      Author: Hernik
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

int indexOf(const char* buf,unsigned int len, const char* val);

int indexOf(const char* base,unsigned int len, const char* str,unsigned int startIndex) ;
bool endsWith(const char* res, int pos, const char ** texts, int textsSize);
bool endsWith(const char* res, int pos, const char* str);

void ip2Char(uint32_t ip, char * ipStr);


#endif /* UTIL_H_ */
