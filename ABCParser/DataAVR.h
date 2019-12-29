#ifndef __DATA_AVR_H__
#define __DATA_AVR_H__

// 자세한 값은 os_cpu.h에 기술된 정보에 따라 변경되기 때문에 아래의 타입 정의문은 경우에 따라 변경되거나 삭제될 수 있음
typedef unsigned char INT8U;
typedef signed char INT8S;
typedef unsigned int INT16U;

typedef struct tagNoteDataAVR {
	INT8S loopCnt; // 음 출력을 위한 타이머 오버플로우 횟수
	INT8U remainCnt;  // 루프 이후에 추가적으로 타이머에 들어가는 값 
	INT16U length; // 음의 길이(ms단위)
} NoteDataAVR;

// 1옥타브 음계 데이터
// n 옥타브의 값을 구하기 위해서는 이 값에 2^(n-1)을 곱하면 된다.
const float freqData[12] = {
	32.7032f, // C
	34.6478f, // C#
	36.7081f, // D
	38.8909f, // D#
	41.2034f, // E
	43.6535f, // F
	46.2493f, // F#
	48.9994f, // G
	51.9130f, // G#
	55.0000f, // A
	58.2705f, // A#
	61.7354f  // B
};

#endif