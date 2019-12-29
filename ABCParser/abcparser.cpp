#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include "abcparser.h"
using namespace std;

int getTokenType(const char *tok)
{
	const char *cwstr = tok; // 작업용 포인터(current working string)
	char ch = tok[0];

	// 토큰의 앞글자를 통해 식별 가능한 경우들
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

	// 헤더인 경우를 먼저 검사한다.
	// 헤더의 경우, 헤더이름:과 같은 형태로 구성돼 있다.
	if (strchr(SYM_HEADERS, *cwstr)) {
		cwstr++;
		while (*cwstr) {
			// 헤더로 간주되는 문자 다음에 공백이나 콜론 외의 문자가 들어오면 헤더가 아니므로
			// 식별 작업을 다음으로 넘긴다.
			if (!isblank(*cwstr) && (*cwstr != ':'))
				break;
			if (*cwstr == ':')
				return TOK_HEADER;
			cwstr++;
		}
	}

	// 토큰이 음표인지 검사한다.
	cwstr = tok;
	while (*cwstr) {
		if (!strchr(SYM_NOTES, ch))
			break;
		cwstr++;
	}
	if (*cwstr == '\0')
		return TOK_NOTE;
	// 식별하지 못한 토큰은 무조건 TOK_UNKNOWN으로 처리한다.
	return TOK_UNKNOWN;
}

// 헤더 정보를 읽고 sheet에 정보를 저장한다.
// 오류가 발생할 경우 -1을 반환한다. 그 외의 경우에는 0을 반환한다.
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

// 음계값을 반환한다.
// 이 함수는 임시표를 반영하지 않는다.
// 임시표를 반영한 음계값을 별도로 계산해야 한다.
// 실패 시 -1 반환
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
				// 토큰 분류에서 미리 걸러지지만 예외 처리를 위해 쉼표의 경우에도 처리한다.
			case 'z':
				return NOTE_REST;
			}
		}
		cwstr++;
	}
	return -1;
}

// 음표의 옥타브 값을 반환한다.
// 성공 시 옥타브를, 실패 시 -1 반환
// ': 6옥타브 이상부터 하나씩 붙는다.
// 예) c': 6옥타브 도
// ,: 3옥타브 이하부터 하나식 붙는다.
// 예) C,: 3옥타브 도
int getOctave(const char *note)
{
	const char *cwstr = note;
	int octave = 0;

	// 대문자는 4옥타브 이하, 소문자는 5옥타브 이상이다.
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

	// 둘 중 하나는 0이어야 한다.
	// 둘 다 0이 아닌 경우 오류
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

// scale: 1차로 구한 음계값
// octave: 1차로 구한 옥타브 값
// acc: 현재 임시표 정보가 저장된 배열
// 임시표는 특정 음계가 제자리표를 만나거나 임시표가 적용된 마디가 끝날 때까지 적용되기 때문에 배열로 임시표 정보를 관리해야 한다.
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
	// natural이 나오는 경우 적용된 임시표를 제거한다.
	else if (accdntl == ACC_NATL) {
		acc[oldsc] = ACC_NONE;
		return 0;
	}

	return -1;
}

// 음길이의 상대값을 부동소수형 데이터로 반환한다.
// 쉼표에도 적용 가능
// 오류 발생 시 음수(-1.0f)를 반환한다.
float getNoteLength(const char *note)
{
	const char *cwstr = note; // 현재 작업용 포인터
	int slcnt = 0; // 슬래시 기호의 개수
	char numbuf[10] = { 0, };
	int idx = 0;
	int dividend = 0, divisor = 0;
	float length = 0.0f;

	//printf("note: %s\n", note);

	// 숫자나 / 기호가 나올 때까지 cwstr의 주소값을 증가시킨다.
	while (*cwstr) {
		if (isdigit(*cwstr) || *cwstr == '/')
			break;
		cwstr++;
	}

	// 음 길이를 따로 표기하지 않는 경우(*cwstr이 널문자에 도달한 경우) 기본 박자(1) 반환
	if (!(*cwstr)) {
		length = 1.0f;
	}
	// /만 표기된 경우
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
	// 숫자가 들어가는 경우
	else if (isdigit(*cwstr)) {
		idx = 0;
		while ((*cwstr != '/') && (*cwstr != '\0')) {
			numbuf[idx++] = *cwstr++;
		}
		numbuf[idx] = '\0';
		dividend = atoi(numbuf);
		//printf("dividend: %d\n", dividend);

		// 음 길이가 단위 길이의 정수배인 경우:
		// /이 없고 숫자만 표기된 경우이므로 *cwstr은 널문자에 도달했을 것이다.
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