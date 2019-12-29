#ifndef __ABC_PARSER_H__
#define __ABC_PARSER_H__

#include "Data.h"

int getTokenType(const char *tok);								// 토큰의 유형을 반환한다.
int readHeader(char htype, const char *line, SheetData *sheet);	// 헤더 정보가 담긴 토큰을 읽고 sheet의 멤버에 저장한다.
int getScale(const char *note);									// 음표에서 1차적인 음계값을 추출한다.
int getOctave(const char *note);								// 음표에서 옥타브 값을 추출한다.
int getAccdntl(const char *note);								// 음표에서 임시표 정보를 추출한다.
int calcScale(int *scale, int *octave, int *acc);				// 임시표를 반영한 음계값을 계산한다.
float getNoteLength(const char *note);							// 음표에서 음 길이를 추출한다.

// 테스트용 함수들
const char *getTokenTypeStr(int toktype);
const char *getScaleStr(int scale);
const char *getAccdntlStr(int acc);


#endif