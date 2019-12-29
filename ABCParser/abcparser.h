#ifndef __ABC_PARSER_H__
#define __ABC_PARSER_H__

#include "Data.h"

int getTokenType(const char *tok);								// ��ū�� ������ ��ȯ�Ѵ�.
int readHeader(char htype, const char *line, SheetData *sheet);	// ��� ������ ��� ��ū�� �а� sheet�� ����� �����Ѵ�.
int getScale(const char *note);									// ��ǥ���� 1������ ���谪�� �����Ѵ�.
int getOctave(const char *note);								// ��ǥ���� ��Ÿ�� ���� �����Ѵ�.
int getAccdntl(const char *note);								// ��ǥ���� �ӽ�ǥ ������ �����Ѵ�.
int calcScale(int *scale, int *octave, int *acc);				// �ӽ�ǥ�� �ݿ��� ���谪�� ����Ѵ�.
float getNoteLength(const char *note);							// ��ǥ���� �� ���̸� �����Ѵ�.

// �׽�Ʈ�� �Լ���
const char *getTokenTypeStr(int toktype);
const char *getScaleStr(int scale);
const char *getAccdntlStr(int acc);


#endif