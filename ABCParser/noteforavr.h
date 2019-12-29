#ifndef __NOTE_FOR_AVR_H__
#define __NOTE_FOR_AVR_H__

#include "DataAVR.h"
#define F_CPU_MHZ		16  // CPU 클럭 속도: 16MHz 
#define TIMER_OVF		256 // 8비트 타이머/카운터이므로 256

float getFreq(int scale, int octave); // 음계와 옥타브를 입력하면 해당 음에 대응하는 주파수를 반환한다.
float freqToPeriodNS(float freq); // 주파수를 입력하면 주기를 나노초 단위로 변환해서 반환한다.
int freqToPeriodNSI(float freq);  // freqToPeriodNS와 기능은 동일하지만 int형 데이터로 잘라서 반환한다.
int getTimerCount(int prescaler, int period); // 타이머의 분주비와 음의 주기를 입력하면 음을 재생하기 위해 타이머가 카운트 해야 하는 값을 반환한다.
int getLoopCount(int timerCnt); // 음을 재생하기 위해 필요한 타이머 오버플로우 횟수를 반환한다
int getRemainCount(int timerCnt); // 오버플로우가 반복된 이후에 추가적으로 카운트 하기 위해 TCNT에 넣어야 하는 값을 반환한다.

float getUnitLength(int bpm);

#endif