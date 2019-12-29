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
	// bpm은 4분음표 기준, abc는 8분음표 기준이므로 1/2배 해야 한다.
	return (60 / (float)bpm) * 0.5f;
}