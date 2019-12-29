#include <cmath>
#include "Data.h"
#include "DataAVR.h"
#include "noteforavr.h"

float getFreq(int scale, int octave)
{
	if (scale == NOTE_REST)
		return -1;
	return freqData[scale] * powf(2, (float)octave - 1);
}

float freqToPeriodNS(float freq)
{
	return (1 / freq) * 1000000;
}

int freqToPeriodNSI(float freq)
{
	if (freq <= 0)
		return -1;
	return (int)freqToPeriodNS(freq);
}

int getTimerCount(int prescaler, int period)
{
	int timerPeriod = prescaler / F_CPU_MHZ;
	return period / timerPeriod;
}

int getLoopCount(int timerCnt)
{
	return timerCnt / TIMER_OVF;
}

int getRemainCount(int timerCnt)
{
	return 256 - (timerCnt % 256);
}

float getUnitLength(int bpm)
{
	// bpm�� 4����ǥ ����, abc�� 8����ǥ �����̹Ƿ� 1/2�� �ؾ� �Ѵ�.
	return (60 / (float)bpm) * 0.5f;
}