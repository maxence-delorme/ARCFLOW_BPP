#ifndef TIME_H 
	
	#define TIME_H

	#ifdef WIN32
		#include <Windows.h>
	#else
		#include <sys/time.h>
		#include <ctime>
	#endif

	int GetTimeMs64();

#endif 
