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

// ��� ���Ϸ� �����͸� ����� ��
// �ܼ�â�� ���� �ϷḦ �˸���, �Ǻ� ����(��Ÿ������)�� ����Ѵ�.


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
	fprintf(stderr, "-p: prescaler, ���ֺ� ���� (�⺻�� 32)\n");
	fprintf(stderr, "-P: program memory, ���α׷� �޸� ���\n");
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

	// �׽�Ʈ�� ����
	//const char *infile = "in.abc";
	//const char *outfile = "out.h";

	ifstream ifs; // �Է� ����
	ofstream ofs; // ��� ����

	vector<NoteData> noteList;
	SheetData sheetData;

	int prescaler = 32; // ���ֺ�� �⺻������ 32
	vector<NoteDataAVR> noteListAVR;
	NoteDataAVR curNoteAVR;

	char buffer[BUFSIZE];
	char *tok;
	char ch;
	int tokType;
	char headerType;

	// ���� �ӽ�ǥ�� ����ϱ� ���� �迭
	// �ϳ��� ���� �ȿ����� ��ȿ�ϱ� ������ ���� �ٲ� ������ 0���� �ʱ�ȭ�ؾ� �Ѵ�.
	NoteData curNote;
	int accData[NSCALES] = { 0, };

	// ���α׷� �޸� ��� ����(-p �ɼ����� ����)
	bool useProgmem = false;

	printProgramInfo();
	argv0 = argv[0];

	if (argc < 3 || argc > 6)
		usage();

	//char *infile = argv[1];
	//char *outfile = argv[2];
	char *infile = NULL;
	char *outfile = NULL;

	// ���� �̸��� ���� �̸��� ���� �����ؾ� �Ѵ�.
	// -p�� �����ϴ� �ɼǰ� ���ֺ�� ���� �����ؾ� �Ѵ�.
	// �ɼ��� ���� �̸� ��, �� ��� �� �� �ִ�.

	for (int i = 1; i < argc; i++) {
		if (isOption(argv[i])) { // �ɼ��� ���
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

		// �ּ�(%) ��ȣ�� �� ���ڷ� �����ϸ� �����Ѵ�.
		if (ch == '%' || ch == '\0')
			continue;

		// ��ū�� ������ ���� ó�� ��� �б�:
		// ��ū ������ ó���ϴ� ���� ���� ������ ó���ϴ� ���� ���������� ������ ������ ��.
		// ���� ������ ó���ϴ� ���: ���� ������ ����, �ּ�, ���
		// ��ū ������ ó���ؾ� �ϴ� ���: ��ǥ, ��ǥ
		// TOK_UNKNOWN�� �߰ߵǴ� ��� �ܼ� ȭ������ ���� ��ȣ�� �Բ� ����Ѵ�.
		tok = strtok(buffer, delim);
		while (tok) {
			tokType = getTokenType(tok);
			if (tokType == TOK_NULL || tokType == TOK_BLANK || tokType == TOK_NLINE || tokType == TOK_COMMENT) {
				break;
			}
			else if (tokType == TOK_HEADER) {
				headerType = tok[0];
				tok = strtok(NULL, "\n"); // ����� ������
				if (readHeader(headerType, tok, &sheetData) == -1) {
					fprintf(stderr, "line %d: illegal header. abort program.\n", sheetData.lineCnt);
					cleanupFile(ifs, ofs, outfile);
					return -1;
				}
				break; // ����� ���� ������ ó���ϹǷ� �۾��� �Ϸ��ϸ� �ٷ� ���� �������� �Ѿ�� �Ѵ�.
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

	float playingTime = 0.0f; // ������ �� ����ð��� ����Ѵ�.

	for (vector<NoteData>::const_iterator iter = noteList.begin(); iter != noteList.end(); iter++) {
		// ��ǥ�� ���
		if (iter->scale == NOTE_REST) {
			curNoteAVR.loopCnt = -1; // ��ǥ���� ������ ���� loopCnt�� -1�� ����ִ´�.
			curNoteAVR.remainCnt = 0;
		}
		// ��ǥ�� ���
		else {
			curFreq = getFreq(iter->scale, iter->octave);
			curPeriod = freqToPeriodNSI(curFreq);
			curTimerCnt = getTimerCount(32, curPeriod / 2); // �� ���� ������ ON/OFF�� �ɰ����Ƿ� �ֱ��� 1/2�� �־���� �Ѵ�.
			curNoteAVR.loopCnt = getLoopCount(curTimerCnt);
			curNoteAVR.remainCnt = getRemainCount(curTimerCnt);
		}
		curNoteLengthSec = unitNoteLength * iter->length;
		curNoteAVR.length = (int)(curNoteLengthSec * 1000); // ms ������ ��ȯ�ϱ� ���� 1000�� �����ش�.
		noteListAVR.push_back(curNoteAVR);

		playingTime += curNoteLengthSec;
	}

	// ���Ͽ� �����͸� ����
	vector<NoteDataAVR>::const_iterator iter;
	ofs << "// music playing time - " << (int)(playingTime / 60) << ":" << (int)playingTime % 60 << endl;
	ofs << "const NoteDataAVR " << sheetData.title << "[" << noteListAVR.size() << "] ";
	if (useProgmem) { // -P �ɼ��� ������ ��� ���α׷� �޸𸮿� �����͸� ������ PROGMEM ������ �߰��Ѵ�.
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

// ���Ͽ� ���� ������: ��� ����(��ũ�� �Ǵ� �������� Ȱ��), ������ ���ǹ�, NoteData �迭