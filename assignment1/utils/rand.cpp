#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
	int x = (1<<17);
//	cout << x << enl;
	for (int i=0; i< x; ++i)
		cout << 10000 + ( std::rand() % ( 99999 - 10000 + 1 ) ) << endl;
	return 0;
}
