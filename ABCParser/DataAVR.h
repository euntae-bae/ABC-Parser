#ifndef __DATA_AVR_H__
#define __DATA_AVR_H__

// �ڼ��� ���� os_cpu.h�� ����� ������ ���� ����Ǳ� ������ �Ʒ��� Ÿ�� ���ǹ��� ��쿡 ���� ����ǰų� ������ �� ����
typedef unsigned char INT8U;
typedef signed char INT8S;
typedef unsigned int INT16U;

typedef struct tagNoteDataAVR {
	INT8S loopCnt; // �� ����� ���� Ÿ�̸� �����÷ο� Ƚ��
	INT8U remainCnt;  // ���� ���Ŀ� �߰������� Ÿ�̸ӿ� ���� �� 
	INT16U length; // ���� ����(ms����)
} NoteDataAVR;

// 1��Ÿ�� ���� ������
// n ��Ÿ���� ���� ���ϱ� ���ؼ��� �� ���� 2^(n-1)�� ���ϸ� �ȴ�.
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