#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "abcparser.h"

int main(void)
{
	const char *notes[] = {
		"g", "z3/2", "f", "z", "g/", "f/", "z", "e", "f",
		"z11/4", "a", "b", "c'", "b//", "z5/4", "g/",
		"^f//", "g//", "z11/4", "a", "b", "c'", "b//", "z5/4", "g/-",
		"E/", "^F/", "G/", "A/", "d2", "A2", "g/", "^f/", "d/", "A/",
		"C,,3", "d'''7", "F,//", "__A/", "_b'//", "^F11/4", "^^g'", "^C,1/2", "^^^A",
		"^C/", "C", "c'", "_c", "C3/4", "=C", "C/", "C/", "z2", "C4", "C2/3", "C,"
	};
	
	int len = sizeof(notes) / sizeof(char*);
	int i;

	int scale;
	int octave;
	int accdntl;
	float length;
	

	for (i = 0; i < len; i++) {
		scale = getScale(notes[i]);
		octave = getOctave(notes[i]);
		length = getNoteLength(notes[i]);
		accdntl = getAccdntl(notes[i]);

		printf("note: %s\n", notes[i]);
		printf("octave: %d\n", octave);
		printf("scale(#1): %s\n", getScaleStr(scale));
		printf("accdntl: %s\n", getAccdntlStr(accdntl));
		printf("length: %f\n", length);
		printf("\n");
	}
	return 0;
}