#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include "abcparser.h"

using namespace std;
#define BUFSIZE 256

char *getToken(char *line)
{
	for (int i = 0; i < strlen(line); i++) {
		cout << "line[" << i << "]: " << line[i] << endl;
	}
	return NULL;
}

void printTokens(char *line)
{
	const char *delim = " \t:-";
	char *tok;

	cout << "line: " << line << endl;
	cout << "tokens: ";
	if (!line[0]) {
		cout << "NULL" << endl << endl;
		return;
	}

	if (line[0] == '%') {
		cout << "COMMENT" << endl << endl;
		return;
	}

	tok = strtok(line, delim);
	while (tok) {
		cout << tok << ", ";
		tok = strtok(NULL, delim);
	}
	cout << endl << endl;
}

void sheetDataInit(SheetData *sheet) {
	sheet->refNumber = 0;
	sheet->tempo = 0;
	sheet->lineCnt = 0;
	sheet->noteCnt = 0;
	sheet->barCnt = 0;
	memset(sheet->title, 0, sizeof(sheet->title));
	memset(sheet->composer, 0, sizeof(sheet->composer));
	memset(sheet->meter, 0, sizeof(sheet->meter));
	memset(sheet->key, 0, sizeof(sheet->key));
}

void noteDataInit(NoteData *note) {
	note->scale = 0;
	note->octave = 0;
	note->length = 0.0f;
}

int main(int argc, char *argv[])
{
	const char *delim = " \t";
	const char *infile = "in.txt";
	const char *outfile = "out.txt";

	vector<NoteData> noteList;
	NoteData curNote;
	int accData[NSCALES] = { 0, };
	int curAcc = ACC_NONE;

	SheetData sheetData;
	ifstream ifs;
	ofstream ofs;
	char buffer[BUFSIZE];
	char *tok = NULL;
	int tokType;
	char headerType;
	//int tokcnt = 0;

	ifs.open(infile);

	if (!ifs.is_open()) {
		perror(argv[1]);
		return -1;
	}

	ofs.open(outfile);
	if (!ofs.is_open()) {
		perror(argv[2]);
		return -1;
	}

	sheetDataInit(&sheetData);
	noteDataInit(&curNote);
	noteList.clear();

	while (!ifs.eof()) {
		// 개행문자만 있는 행인 경우 buffer[0]는 널 문자가 된다.
		ifs.getline(buffer, BUFSIZE - 1, '\n');
		sheetData.lineCnt++;

		ofs << "line[" << sheetData.lineCnt << "]: " << buffer << endl;
		tok = strtok(buffer, delim);
		while (tok) {
			tokType = getTokenType(tok);

			if (tokType == TOK_NULL || tokType == TOK_BLANK || tokType == TOK_NLINE || tokType == TOK_COMMENT) {
				ofs << "token type: " << getTokenTypeStr(tokType) << endl;
				break;
			}

			else if (tokType == TOK_UNKNOWN) {
				cout << "line " << sheetData.lineCnt  << ": " <<"Unknown Token: " << tok << endl;
				tok = strtok(NULL, delim);
				continue;
			}

			else if (tokType == TOK_HEADER) {
				headerType = tok[0];
				tok = strtok(NULL, "\n");
				readHeader(headerType, tok, &sheetData);
				break;
			}

			else if (tokType == TOK_NOTE) {
				noteDataInit(&curNote);
				sheetData.noteCnt++;
				curNote.scale = getScale(tok);
				curNote.octave = getOctave(tok);
				curNote.length = getNoteLength(tok);

				curAcc = getAccdntl(tok);
				accData[curNote.scale] = curAcc;
				if (calcScale(&curNote.scale, &curNote.octave, accData) == -1) {
					fprintf(stderr, "line %d: Unknown Accdntl '%s'\n", sheetData.lineCnt, tok);
				}

				ofs << "scale: " << curNote.scale << endl;
				ofs << "octave: " << curNote.octave << endl;
				ofs << "length: " << curNote.length << endl;
				noteList.push_back(curNote);
			}
			else if (tokType == TOK_REST) {
				noteDataInit(&curNote);
				sheetData.restCnt++;
				curNote.scale = NOTE_REST;
				curNote.octave = 0;
				curNote.length = getNoteLength(tok);

				ofs << "scale: " << curNote.scale << endl;
				ofs << "octave: " << curNote.octave << endl;
				ofs << "length: " << curNote.length << endl;
				noteList.push_back(curNote);
			}
			else if (tokType == TOK_BAR) {
				sheetData.barCnt++;
				memset(accData, 0, sizeof(accData));
			}

			ofs << "token: " << tok << endl;
			ofs << "token type: " << getTokenTypeStr(tokType) << endl;
			ofs << "------------------------------------" << endl;
			tok = strtok(NULL, delim);
		}
		ofs << endl;
	}
	
	cout << endl << endl;
	for (vector<NoteData>::const_iterator iter = noteList.begin(); iter != noteList.end(); iter++) {
		cout << "scale: " << iter->scale << endl;
		cout << "octave: " << iter->octave << endl;
		cout << "length: " << iter->length << endl;
		cout << endl;
	}

	ifs.close();
	ofs.close();
	return 0;
}