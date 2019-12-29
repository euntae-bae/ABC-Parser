#ifndef __DATA_H__
#define __DATA_H__

#define BUFSIZE 256

/*
const char * const SYM_HEADERS = "XTCMLQK:";
const char * const SYM_NOTES = ",'^_-cdefgabCDEFGAB";
*/

#define SYM_HEADERS "XTCMLQK:"
#define SYM_NOTES ",'^_-=cdefgabCDEFGAB"

// 악보에 대한 정보
typedef struct tagSheetData {
	int refNumber;			// X
	int tempo;				// Q
	char title[BUFSIZE];	// T
	char composer[BUFSIZE];	// C
	char meter[20];			// M
	char key[20];			// K
	int lineCnt;			// (입력 파일의) 행 개수
	int noteCnt;			// 음표 개수
	int restCnt;			// 쉼표 개수
	int barCnt;				// 마디 개수
} SheetData;

// 각 음표와 쉼표의 정보를 저장하기 위한 자료구조
typedef struct tagNoteData {
	int scale;			// 음계
	int octave;			// 옥타브
	float length;		// 음 길이(상대길이)
} NoteData;

// 임시표
enum Accidental {
	ACC_NONE = 0,
	ACC_SC1,		// #
	ACC_SC2,		// ##
	ACC_FL1,		// b
	ACC_FL2,		// bb
	ACC_NATL,		// natural
	ACC_UNKNOWN = -1
};

enum Note {
	NOTE_C = 0,
	NOTE_CS,		// C#
	NOTE_D,
	NOTE_DS,
	NOTE_E,
	NOTE_F,
	NOTE_FS,
	NOTE_G,
	NOTE_GS,
	NOTE_A,
	NOTE_AS,
	NOTE_B,
	NOTE_REST
};

#define SCALE_MIN	NOTE_C
#define SCALE_MAX	NOTE_B
#define NSCALES		SCALE_MAX + 1

enum TokenType {
	TOK_NULL,		// 널 문자
	TOK_BLANK,		// 공백문자
	TOK_NLINE,		// 개행문자
	TOK_COMMENT,	// 주석
	TOK_HEADER,		// 헤더
	TOK_NOTE,		// 음표
	TOK_REST,		// 쉼표
	TOK_BAR,		// 마디
	TOK_UNKNOWN = -1
};

#endif