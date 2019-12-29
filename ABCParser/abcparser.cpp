#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include "abcparser.h"
using namespace std;

int getTokenType(const char *tok)
{
	const char *cwstr = tok; // �۾��� ������(current working string)
	char ch = tok[0];

	// ��ū�� �ձ��ڸ� ���� �ĺ� ������ ����
	if (isblank(ch))
		return TOK_BLANK;

	switch (ch) {
	case '\0':
		return TOK_NULL;
	case '\n':
		return TOK_NLINE;
	case '%':
		return TOK_COMMENT;
	case 'z':
	case 'Z':
		return TOK_REST;
	case '|':
		return TOK_BAR;
	}

	// ����� ��츦 ���� �˻��Ѵ�.
	// ����� ���, ����̸�:�� ���� ���·� ������ �ִ�.
	if (strchr(SYM_HEADERS, *cwstr)) {
		cwstr++;
		while (*cwstr) {
			// ����� ���ֵǴ� ���� ������ �����̳� �ݷ� ���� ���ڰ� ������ ����� �ƴϹǷ�
			// �ĺ� �۾��� �������� �ѱ��.
			if (!isblank(*cwstr) && (*cwstr != ':'))
				break;
			if (*cwstr == ':')
				return TOK_HEADER;
			cwstr++;
		}
	}

	// ��ū�� ��ǥ���� �˻��Ѵ�.
	cwstr = tok;
	while (*cwstr) {
		if (!strchr(SYM_NOTES, ch))
			break;
		cwstr++;
	}
	if (*cwstr == '\0')
		return TOK_NOTE;
	// �ĺ����� ���� ��ū�� ������ TOK_UNKNOWN���� ó���Ѵ�.
	return TOK_UNKNOWN;
}

// ��� ������ �а� sheet�� ������ �����Ѵ�.
// ������ �߻��� ��� -1�� ��ȯ�Ѵ�. �� ���� ��쿡�� 0�� ��ȯ�Ѵ�.
int readHeader(char htype, const char *line, SheetData *sheet)
{
//	printf("readHeader()\n");
//	printf("type: %c\n", htype);
//	printf("header: %s\n", line);

	if (htype == '\0') {
		fprintf(stderr, "readHeader: null header type\n");
		return -1;
	}
	else if (!strchr(SYM_HEADERS, htype)) {
		fprintf(stderr, "readHeader: illegal header type '%c'(dec:%d)\n", htype, htype);
		return -1;
	}

	switch (htype) {
	case 'X':
		sheet->refNumber = atoi(line);
		break;
	case 'Q':
		sheet->tempo = atoi(line);
		break;
	case 'T':
		strcpy(sheet->title, line);
		break;
	case 'C':
		strcpy(sheet->composer, line);
		break;
	case 'M':
		strcpy(sheet->meter, line);
		break;
	case 'K':
		strcpy(sheet->key, line);
		break;
	default:
		fprintf(stderr, "readHeader: Not defined header.\n");
	}
	return 0;
}

// ���谪�� ��ȯ�Ѵ�.
// �� �Լ��� �ӽ�ǥ�� �ݿ����� �ʴ´�.
// �ӽ�ǥ�� �ݿ��� ���谪�� ������ ����ؾ� �Ѵ�.
// ���� �� -1 ��ȯ
int getScale(const char *note)
{
	const char *cwstr = note;

	while (*cwstr) {
		if (isalpha(*cwstr)) {
			switch (tolower(*cwstr)) {
			case 'c':
				return NOTE_C;
			case 'd':
				return NOTE_D;
			case 'e':
				return NOTE_E;
			case 'f':
				return NOTE_F;
			case 'g':
				return NOTE_G;
			case 'a':
				return NOTE_A;
			case 'b':
				return NOTE_B;
				// ��ū �з����� �̸� �ɷ������� ���� ó���� ���� ��ǥ�� ��쿡�� ó���Ѵ�.
			case 'z':
				return NOTE_REST;
			}
		}
		cwstr++;
	}
	return -1;
}

// ��ǥ�� ��Ÿ�� ���� ��ȯ�Ѵ�.
// ���� �� ��Ÿ�긦, ���� �� -1 ��ȯ
// ': 6��Ÿ�� �̻���� �ϳ��� �ٴ´�.
// ��) c': 6��Ÿ�� ��
// ,: 3��Ÿ�� ���Ϻ��� �ϳ��� �ٴ´�.
// ��) C,: 3��Ÿ�� ��
int getOctave(const char *note)
{
	const char *cwstr = note;
	int octave = 0;

	// �빮�ڴ� 4��Ÿ�� ����, �ҹ��ڴ� 5��Ÿ�� �̻��̴�.
	while (*cwstr) {
		if (isalpha(*cwstr)) {
			if (isupper(*cwstr)) {
				octave = 4;
				break;
			}
			else {
				octave = 5;
				break;
			}
		}
		cwstr++;
	}

	cwstr = note;
	while (*cwstr) {
		if (*cwstr == '\'')
			octave++;
		if (*cwstr == ',')
			octave--;
		cwstr++;
	}

	return octave;
}

int getAccdntl(const char *note)
{
	const char *cwstr = note;
	int accdntl = ACC_NONE;
	int sccnt = 0, flatcnt = 0;

	while (*cwstr) {
		if (*cwstr == '^')
			sccnt++;
		if (*cwstr == '_')
			flatcnt++;
		if (*cwstr == '=')
			return ACC_NATL;
		cwstr++;
	}

	// �� �� �ϳ��� 0�̾�� �Ѵ�.
	// �� �� 0�� �ƴ� ��� ����
	if (sccnt && flatcnt)
		return ACC_UNKNOWN;

	if (sccnt == 1)
		accdntl = ACC_SC1;
	else if (sccnt == 2)
		accdntl = ACC_SC2;
	else if (sccnt > 2)
		return ACC_UNKNOWN;

	if (flatcnt == 1)
		accdntl = ACC_FL1;
	else if (flatcnt == 2)
		accdntl = ACC_FL2;
	else if (flatcnt > 2)
		return ACC_UNKNOWN;

	return accdntl;
}

// scale: 1���� ���� ���谪
// octave: 1���� ���� ��Ÿ�� ��
// acc: ���� �ӽ�ǥ ������ ����� �迭
// �ӽ�ǥ�� Ư�� ���谡 ���ڸ�ǥ�� �����ų� �ӽ�ǥ�� ����� ���� ���� ������ ����Ǳ� ������ �迭�� �ӽ�ǥ ������ �����ؾ� �Ѵ�.
int calcScale(int *scale, int *octave, int *acc)
{
	int oldsc = *scale;
	int newsc = oldsc;
	int accdntl = acc[oldsc];

	if (accdntl == ACC_NONE)
		return 0;
	else if (accdntl == ACC_SC1 || accdntl == ACC_SC2) {
		if (accdntl == ACC_SC1)
			newsc += 1;
		else
			newsc += 2;

		if (newsc > SCALE_MAX) {
			newsc = newsc % (SCALE_MAX + 1);
			(*octave)++;
		}
		*scale = newsc;
		return 0;
	}
	else if (accdntl == ACC_FL1 || accdntl == ACC_FL2) {
		if (accdntl == ACC_FL1)
			newsc -= 1;
		else
			newsc -= 2;

		if (newsc < SCALE_MIN) {
			newsc = SCALE_MAX + newsc;
			(*octave)--;
			if (*octave <= 0)
				return -1;
		}
		*scale = newsc;
	}
	// natural�� ������ ��� ����� �ӽ�ǥ�� �����Ѵ�.
	else if (accdntl == ACC_NATL) {
		acc[oldsc] = ACC_NONE;
		return 0;
	}

	return -1;
}

// �������� ��밪�� �ε��Ҽ��� �����ͷ� ��ȯ�Ѵ�.
// ��ǥ���� ���� ����
// ���� �߻� �� ����(-1.0f)�� ��ȯ�Ѵ�.
float getNoteLength(const char *note)
{
	const char *cwstr = note; // ���� �۾��� ������
	int slcnt = 0; // ������ ��ȣ�� ����
	char numbuf[10] = { 0, };
	int idx = 0;
	int dividend = 0, divisor = 0;
	float length = 0.0f;

	//printf("note: %s\n", note);

	// ���ڳ� / ��ȣ�� ���� ������ cwstr�� �ּҰ��� ������Ų��.
	while (*cwstr) {
		if (isdigit(*cwstr) || *cwstr == '/')
			break;
		cwstr++;
	}

	// �� ���̸� ���� ǥ������ �ʴ� ���(*cwstr�� �ι��ڿ� ������ ���) �⺻ ����(1) ��ȯ
	if (!(*cwstr)) {
		length = 1.0f;
	}
	// /�� ǥ��� ���
	// /: 1/2
	// //: 1/4
	else if (*cwstr == '/') {
		while (*cwstr) {
			if (*cwstr == '/')
				slcnt++;
			cwstr++;
		}
		length = powf(0.5f, (float)slcnt);
	}
	// ���ڰ� ���� ���
	else if (isdigit(*cwstr)) {
		idx = 0;
		while ((*cwstr != '/') && (*cwstr != '\0')) {
			numbuf[idx++] = *cwstr++;
		}
		numbuf[idx] = '\0';
		dividend = atoi(numbuf);
		//printf("dividend: %d\n", dividend);

		// �� ���̰� ���� ������ �������� ���:
		// /�� ���� ���ڸ� ǥ��� ����̹Ƿ� *cwstr�� �ι��ڿ� �������� ���̴�.
		if (*cwstr == '\0')
			return (float)dividend;

		idx = 0;
		while (*cwstr) {
			if (isdigit(*cwstr))
				numbuf[idx++] = *cwstr;
			else if (*cwstr == '/')
				slcnt++;
			else
				printf("unknown character: %c\n", *cwstr);
			cwstr++;
		}
		numbuf[idx] = '\0';
		divisor = atoi(numbuf);
		//printf("divisor: %d\n", divisor);
		length = (float)dividend / divisor;
	}
	else
		length = -1.0f;

	//printf("slcnt: %d\n", slcnt);
	//printf("noteLength: %f\n", length);
	return length;
}

const char *getTokenTypeStr(int toktype)
{
	switch (toktype)
	{
	case TOK_NULL:
		return "Null";
	case TOK_BLANK:
		return "Blank";
	case TOK_NLINE:
		return "New line";
	case TOK_COMMENT:
		return "Comment";
	case TOK_HEADER:
		return "Header";
	case TOK_NOTE:
		return "Note";
	case TOK_REST:
		return "Rest";
	case TOK_BAR:
		return "Bar";
	}
	return "Unknown";
}

const char *getScaleStr(int scale)
{
	switch (scale) {
	case NOTE_C:
		return "C";
	case NOTE_CS:
		return "C#";
	case NOTE_D:
		return "D";
	case NOTE_DS:
		return "D#";
	case NOTE_E:
		return "E";
	case NOTE_F:
		return "F";
	case NOTE_FS:
		return "F#";
	case NOTE_G:
		return "G";
	case NOTE_GS:
		return "G#";
	case NOTE_A:
		return "A";
	case NOTE_B:
		return "B";
	case NOTE_REST:
		return "Rest";
	}
	return "Unknown";
}

const char *getAccdntlStr(int acc)
{
	switch (acc) {
	case ACC_NONE:
		return "None";
	case ACC_SC1:
		return "#";
	case ACC_SC2:
		return "##";
	case ACC_FL1:
		return "b";
	case ACC_FL2:
		return "bb";
	case ACC_NATL:
		return "Natural";
	}
	return "Unknown";
}