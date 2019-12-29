#ifndef __DATA_H__
#define __DATA_H__

#define BUFSIZE 256

/*
const char * const SYM_HEADERS = "XTCMLQK:";
const char * const SYM_NOTES = ",'^_-cdefgabCDEFGAB";
*/

#define SYM_HEADERS "XTCMLQK:"
#define SYM_NOTES ",'^_-=cdefgabCDEFGAB"

// �Ǻ��� ���� ����
typedef struct tagSheetData {
	int refNumber;			// X
	int tempo;				// Q
	char title[BUFSIZE];	// T
	char composer[BUFSIZE];	// C
	char meter[20];			// M
	char key[20];			// K
	int lineCnt;			// (�Է� ������) �� ����
	int noteCnt;			// ��ǥ ����
	int restCnt;			// ��ǥ ����
	int barCnt;				// ���� ����
} SheetData;

// �� ��ǥ�� ��ǥ�� ������ �����ϱ� ���� �ڷᱸ��
typedef struct tagNoteData {
	int scale;			// ����
	int octave;			// ��Ÿ��
	float length;		// �� ����(������)
} NoteData;

// �ӽ�ǥ
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
	TOK_NULL,		// �� ����
	TOK_BLANK,		// ���鹮��
	TOK_NLINE,		// ���๮��
	TOK_COMMENT,	// �ּ�
	TOK_HEADER,		// ���
	TOK_NOTE,		// ��ǥ
	TOK_REST,		// ��ǥ
	TOK_BAR,		// ����
	TOK_UNKNOWN = -1
};

#endif