#include <iostream>
#include "noteforavr.h"

using namespace std;

int main(void)
{
	int tempo;

	cout << "tempo(bpm): ";
	cin >> tempo;
	cout << "unit length: " << getUnitLength(tempo) << endl;
	return 0;
}