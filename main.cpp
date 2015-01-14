// main.cpp : Defines the entry point for the console application.
//
#include "consistenthash/ConsistentHash.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
using namespace std;
int main(int argc, char* argv[])
{
	const char* aServerNames[] = { "Server0", "Server1", "Server2", "Server3", "Server4", "Server5", "Server6", "Server7", "Server8", "Server9" };
	ConsistentHash aHash[5];
	int match[5] = { 0, 0, 0, 0, 0 };

	for(int j=0;j<5;++j)
	{
		for(int i=j;i<10;++i)
			aHash[j].addServer(aServerNames[i]);
		//aHash[j].print();
	}

	srand( (unsigned)time( NULL ) );

	printf("Starting...\n");
	const int kITER = 1000000;
	for(int i=0;i<kITER;++i)
	{
		//create a random string
		double r = rand();
		r /= RAND_MAX;
		char buf[100];
		sprintf(buf,"%.15f",r);
		string aKey(buf+2,10);

		// hash with view 0,1,2
		// and compare hashes
		const string& s0 = aHash[0][aKey];
		for(int j=1;j<5;++j)
		{
			const string& s = aHash[j][aKey];
			if(s == s0)
				++(match[j]);
		}
	}

	for(int j=1;j<5;++j)
	{
		printf("match[%d]=%2.2f%%\n",j,match[j]*(100.0/kITER));
	}
	return 0;
}

