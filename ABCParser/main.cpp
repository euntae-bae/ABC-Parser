#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cctype>
#include <cstring>

#include "abcparser.h"
#include "noteforavr.h"
using namespace std;

void cleanupFile(ifstream& ifs, ofstream& ofs, const char *outfile);
void initSheetData(SheetData *sheet);
void initNoteData(NoteData *note);
void initNoteDataAVR(NoteDataAVR *note);
void writeHeader(const SheetData& sheet, std::ofstream& ofs);
void writeDataStruct(std::ofstream& ofs);

// 출력 파일로 데이터를 출력한 뒤
// 콘솔창에 생성 완료를 알리고, 악보 정보(메타데이터)를 출력한다.


void cleanupFile(ifstream& ifs, ofstream& ofs, const char *outfile)
{
	ifs.close();
	ofs.close();
	remove(outfile);
}

void initSheetData(SheetData *sheet)
{
	sheet->refNumber = 0;
	sheet->tempo = 0;
	sheet->lineCnt = 0;
	sheet->noteCnt = 0;
	sheet->barCnt = 0;
	sheet->restCnt = 0;
	memset(sheet->title, 0, sizeof(sheet->title));
	memset(sheet->composer, 0, sizeof(sheet->composer));
	memset(sheet->meter, 0, sizeof(sheet->meter));
	memset(sheet->key, 0, sizeof(sheet->key));
}

void initNoteData(NoteData *note)
{
	note->scale = 0;
	note->octave = 0;
	note->length = 0.0f;
}

void initNoteDataAVR(NoteDataAVR *note)
{
	note->loopCnt = 0;
	note->remainCnt = 0;
	note->length = 0;
}

void writeHeader(const SheetData& sheet, std::ofstream& ofs)
{
	const char *prefix = "#define ";
	const char *delim = "\t\t";

	//cout << "writeHeader()";
	ofs << prefix << "REF_NUMBER" << delim << sheet.refNumber << endl;
	ofs << prefix << "TEMPO" << delim << sheet.tempo << endl;
	ofs << prefix << "TITLE" << delim << "\"" << sheet.title << "\"" << endl;
	ofs << prefix << "COMPOSER" << delim << "\"" << sheet.composer << "\"" << endl;
	ofs << prefix << "METER" << delim << "\"" << sheet.meter << "\"" << endl;
	ofs << prefix << "KEY" << delim << "\"" << sheet.key << "\"" << endl;
	ofs << prefix << "LINE_COUNT" << delim << sheet.lineCnt << endl;
	ofs << prefix << "NOTE_COUNT" << delim << sheet.noteCnt << endl;
	ofs << prefix << "REST_COUNT" << delim << sheet.restCnt << endl;
	ofs << prefix << "BAR_COUNT" << delim << sheet.barCnt << endl;
	ofs << endl;
}

void writeDataStruct(std::ofstream& ofs)
{
	int i;

	const char *noteDataDS[] = {
		"typedef struct tagNoteData {",
		"int scale;",
		"int octave;",
		"float length;",
		"} NoteData;",
	};

	ofs << noteDataDS[0] << endl;
	for (i = 1; i < (sizeof(noteDataDS) / sizeof(char*)) - 1; i++)
		ofs << '\t' << noteDataDS[i] << endl;
	ofs << noteDataDS[i] << endl << endl;
}

void printProgramInfo(void) {
	cout << "ABCParser version 1.02" << endl;
	cout << "(C)2018-2019 Euntae Bae" << endl;
}

char *argv0 = NULL;

void printUsage(void) {
	fprintf(stderr, "usage: %s [-p <#prescaler>] [-P] <src_file> <dest_file>\n", argv0);
	fprintf(stderr, "-p: prescaler, 분주비 설정 (기본값 32)\n");
	fprintf(stderr, "-P: program memory, 프로그램 메모리 사용\n");
}

void usage(void) {
	printUsage();
	exit(-1);
}

bool isNumber(const char *str) {
	const char *cwstr = str;
	while (*cwstr) {
		if (!isdigit(*cwstr))
			break;
		cwstr++;
	}
	if (*cwstr != '\0') {
		return false;
	}
	return true;
}

bool isOption(const char* arg) {
	return arg[0] == '-';
}

int main(int argc, char *argv[])
{
	const char *delim = " \t";

	// 테스트용 파일
	//const char *infile = "in.abc";
	//const char *outfile = "out.h";

	ifstream ifs; // 입력 파일
	ofstream ofs; // 출력 파일

	vector<NoteData> noteList;
	SheetData sheetData;

	int prescaler = 32; // 분주비는 기본적으로 32
	vector<NoteDataAVR> noteListAVR;
	NoteDataAVR curNoteAVR;

	char buffer[BUFSIZE];
	char *tok;
	char ch;
	int tokType;
	char headerType;

	// 음의 임시표를 기록하기 위한 배열
	// 하나의 마디 안에서만 유효하기 때문에 마디가 바뀔 때마다 0으로 초기화해야 한다.
	NoteData curNote;
	int accData[NSCALES] = { 0, };

	// 프로그램 메모리 사용 여부(-p 옵션으로 설정)
	bool useProgmem = false;

	printProgramInfo();
	argv0 = argv[0];

	if (argc < 3 || argc > 6)
		usage();

	//char *infile = argv[1];
	//char *outfile = argv[2];
	char *infile = NULL;
	char *outfile = NULL;

	// 파일 이름과 파일 이름은 서로 인접해야 한다.
	// -p를 포함하는 옵션과 분주비는 서로 인접해야 한다.
	// 옵션은 파일 이름 앞, 뒤 모두 올 수 있다.

	for (int i = 1; i < argc; i++) {
		if (isOption(argv[i])) { // 옵션인 경우
			//cout << argv[i] << endl;
			if (strchr(argv[i], 'P'))
				useProgmem = true;
			if (strchr(argv[i], 'p')) {
				if (i == argc - 1)
					usage();
				else if (!isNumber(argv[++i]))
					usage();
				prescaler = atoi(argv[i]);
			}
		}
		else {
			if (i == argc - 1)
				usage();
			//cout << argv[i] << endl;
			infile = argv[i++];
			//cout << argv[i] << endl;
			outfile = argv[i];
		}
	}

	ifs.open(infile);
	if (!ifs.is_open()) {
		perror(infile);
		return -1;
	}

	ofs.open(outfile);
	if (!ofs.is_open()) {
		perror(outfile);
		return -1;
	}

	initSheetData(&sheetData);
	initNoteData(&curNote);
	noteList.clear();

	while (!ifs.eof()) {
		ifs.getline(buffer, BUFSIZE, '\n');
		sheetData.lineCnt++;
		ch = buffer[0];

		// 주석(%) 기호나 널 문자로 시작하면 무시한다.
		if (ch == '%' || ch == '\0')
			continue;

		// 토큰의 유형에 따라 처리 방법 분기:
		// 토큰 단위로 처리하는 경우와 라인 단위로 처리하는 경우로 나뉘어지기 때문에 주의할 것.
		// 라인 단위로 처리하는 경우: 무시 가능한 문자, 주석, 헤더
		// 토큰 단위로 처리해야 하는 경우: 음표, 쉼표
		// TOK_UNKNOWN이 발견되는 경우 콘솔 화면으로 라인 번호와 함께 출력한다.
		tok = strtok(buffer, delim);
		while (tok) {
			tokType = getTokenType(tok);
			if (tokType == TOK_NULL || tokType == TOK_BLANK || tokType == TOK_NLINE || tokType == TOK_COMMENT) {
				break;
			}
			else if (tokType == TOK_HEADER) {
				headerType = tok[0];
				tok = strtok(NULL, "\n"); // 헤더의 데이터
				if (readHeader(headerType, tok, &sheetData) == -1) {
					fprintf(stderr, "line %d: illegal header. abort program.\n", sheetData.lineCnt);
					cleanupFile(ifs, ofs, outfile);
					return -1;
				}
				break; // 헤더는 라인 단위로 처리하므로 작업을 완료하면 바로 다음 라인으로 넘어가야 한다.
			}
			else if (tokType == TOK_NOTE) {
				initNoteData(&curNote);
				sheetData.noteCnt++;
				curNote.scale = getScale(tok);
				curNote.octave = getOctave(tok);
				curNote.length = getNoteLength(tok);

				accData[curNote.scale] = getAccdntl(tok);
				if (calcScale(&curNote.scale, &curNote.octave, accData) == -1) {
					fprintf(stderr, "line %d: Unknown Accdntl '%s'\n", sheetData.lineCnt, tok);
				}

				noteList.push_back(curNote);
			}
			else if (tokType == TOK_REST) {
				initNoteData(&curNote);
				sheetData.restCnt++;
				curNote.scale = NOTE_REST;
				curNote.octave = 0;
				curNote.length = getNoteLength(tok);
				noteList.push_back(curNote);
			}
			else if (tokType == TOK_BAR) {
				sheetData.barCnt++;
				memset(accData, 0, sizeof(accData));
			}
			else {
				cout << "line " << sheetData.lineCnt << ": " << "unknown token '" << tok << "'" << endl;
			}
			tok = strtok(NULL, delim);
		}
	}

	/*
	for (vector<NoteData>::const_iterator iter = noteList.begin(); iter != noteList.end(); iter++) {
		ofs << "scale: " << iter->scale << endl;
		ofs << "octave: " << iter->octave << endl;
		ofs << "length: " << iter->length << endl;
		ofs << endl;
	}
	*/

	//writeHeader(sheetData, ofs);
	//writeDataStruct(ofs);

	//ofs << "NoteData noteData[] = {" << endl;
	//for (vector<NoteData>::const_iterator iter = noteList.begin(); iter != noteList.end(); iter++) {
	//	ofs << "\t{ " << iter->scale << ", " << iter->octave << ", " << iter->length << " }," << endl;
	//}
	//ofs << "};" << endl;

	initNoteDataAVR(&curNoteAVR);
	noteListAVR.clear();

	float unitNoteLength = getUnitLength(sheetData.tempo);

	float curFreq = 0.0f;
	int curPeriod = 0;
	int curTimerCnt;
	float curNoteLengthSec;

	float playingTime = 0.0f; // 음악의 총 재생시간을 계산한다.

	for (vector<NoteData>::const_iterator iter = noteList.begin(); iter != noteList.end(); iter++) {
		// 쉼표인 경우
		if (iter->scale == NOTE_REST) {
			curNoteAVR.loopCnt = -1; // 음표와의 구분을 위해 loopCnt에 -1을 집어넣는다.
			curNoteAVR.remainCnt = 0;
		}
		// 음표인 경우
		else {
			curFreq = getFreq(iter->scale, iter->octave);
			curPeriod = freqToPeriodNSI(curFreq);
			curTimerCnt = getTimerCount(32, curPeriod / 2); // 한 번의 진동은 ON/OFF로 쪼개지므로 주기의 1/2를 넣어줘야 한다.
			curNoteAVR.loopCnt = getLoopCount(curTimerCnt);
			curNoteAVR.remainCnt = getRemainCount(curTimerCnt);
		}
		curNoteLengthSec = unitNoteLength * iter->length;
		curNoteAVR.length = (int)(curNoteLengthSec * 1000); // ms 단위로 변환하기 위해 1000을 곱해준다.
		noteListAVR.push_back(curNoteAVR);

		playingTime += curNoteLengthSec;
	}

	// 파일에 데이터를 쓴다
	vector<NoteDataAVR>::const_iterator iter;
	ofs << "// music playing time - " << (int)(playingTime / 60) << ":" << (int)playingTime % 60 << endl;
	ofs << "const NoteDataAVR " << sheetData.title << "[" << noteListAVR.size() << "] ";
	if (useProgmem) { // -P 옵션이 설정된 경우 프로그램 메모리에 데이터를 쓰도록 PROGMEM 선언을 추가한다.
		ofs << "PROGMEM";
	}
	ofs << " = {" << endl;
	for (iter = noteListAVR.begin(); (iter + 1) != noteListAVR.end(); iter++) {
		ofs << '\t' << "{ " << (int)(iter->loopCnt) << ", " << (int)(iter->remainCnt) << ", " << (int)(iter->length) << " }," << endl;
	}
	ofs << '\t' << "{ " << (int)(iter->loopCnt) << ", " << (int)(iter->remainCnt) << ", " << (int)(iter->length) << " }" << endl;
	ofs << "};" << endl;

	ifs.close();
	ofs.close();
	cout << "Creating " << outfile << " is succeed." << endl;
	return 0;
}

// 파일에 쓰는 데이터: 헤더 정보(매크로 또는 전역변수 활용), 데이터 정의문, NoteData 배열