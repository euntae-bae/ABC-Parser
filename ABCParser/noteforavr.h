#ifndef __NOTE_FOR_AVR_H__
#define __NOTE_FOR_AVR_H__

#include "DataAVR.h"
#define F_CPU_MHZ		16  // CPU Ŭ�� �ӵ�: 16MHz 
#define TIMER_OVF		256 // 8��Ʈ Ÿ�̸�/ī�����̹Ƿ� 256

float getFreq(int scale, int octave); // ����� ��Ÿ�긦 �Է��ϸ� �ش� ���� �����ϴ� ���ļ��� ��ȯ�Ѵ�.
float freqToPeriodNS(float freq); // ���ļ��� �Է��ϸ� �ֱ⸦ ������ ������ ��ȯ�ؼ� ��ȯ�Ѵ�.
int freqToPeriodNSI(float freq);  // freqToPeriodNS�� ����� ���������� int�� �����ͷ� �߶� ��ȯ�Ѵ�.
int getTimerCount(int prescaler, int period); // Ÿ�̸��� ���ֺ�� ���� �ֱ⸦ �Է��ϸ� ���� ����ϱ� ���� Ÿ�̸Ӱ� ī��Ʈ �ؾ� �ϴ� ���� ��ȯ�Ѵ�.
int getLoopCount(int timerCnt); // ���� ����ϱ� ���� �ʿ��� Ÿ�̸� �����÷ο� Ƚ���� ��ȯ�Ѵ�
int getRemainCount(int timerCnt); // �����÷ο찡 �ݺ��� ���Ŀ� �߰������� ī��Ʈ �ϱ� ���� TCNT�� �־�� �ϴ� ���� ��ȯ�Ѵ�.

float getUnitLength(int bpm);

#endif