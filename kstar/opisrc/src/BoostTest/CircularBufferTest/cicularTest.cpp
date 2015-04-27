#include <cstdio>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <boost/circular_buffer.hpp>

using namespace boost;

int main(int /*argc*/, char* /*argv*/[]) {

	//circular_buffer<float> cb(5);
	circular_buffer<float> cb;
#if 0
	cb.set_capacity(10);
	srand(time(0));

	for(size_t i = 0; i < 10; i++)
	{
		int irand = rand()%100;
		float frand = (float)irand/93;
		float value = irand+frand; 
		cb.push_back(value);
	};

	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Test1-Data[%d]:(%.3f)\n",i,cb[i]);
	};

	printf("\n\n\n");
	for(size_t i = 0; i < 5; i++)
	{
		int irand = rand()%100;
		float frand = (float)irand/93;
		float value = irand+frand; 
		cb.push_back(value);
	};

	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Test2-Data[%d]:(%.3f)\n",i,cb[i]);
	};


#else
	// Insert some elements into the buffer.
	cb.set_capacity(5);
	cb.push_back(1.212);
	cb.push_back(2.212);
	cb.push_back(3.212);

	printf("*****Test-1:CircularBuffer*****\n");
	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Data[%d]:(%.3f),",i,cb[i]);
	}
	printf("\n*********Test-1:End************\n\n");

	cb.push_back(4.435);
	cb.push_back(5.545);

	printf("*****Test-2:CircularBuffer*****\n");
	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Data[%d]:(%.3f),",i,cb[i]);
	}
	printf("\n*********Test-2:End************\n\n");

	cb.push_back(11.313);
	cb.push_back(12.534);
	cb.push_back(13.432);
	cb.push_back(14.843);
	cb.push_back(15.343);
	printf("*****Test-3:CircularBuffer*****\n");
	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Data[%d]:(%.3f),",i,cb[i]);
	}
	printf("\n*********Test-3:End************\n\n");

	cb.push_back(20.438);
	cb.push_back(21.843);
	printf("*****Test-4:CircularBuffer*****\n");
	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Data[%d]:(%.3f),",i,cb[i]);
	}
	printf("\n*********Test-4:End************\n\n");

	// Elements can be popped from either the front or the back.
	cb.pop_back();
	printf("*****Test-5:CircularBuffer*****\n");
	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Data[%d]:(%.3f),",i,cb[i]);
	};
	printf("\n*********Test-5:End************\n\n");

	cb.pop_front(); // 3 is removed.
	printf("*****Test-6:CircularBuffer*****\n");
	for(size_t i = 0; i < cb.size(); i++)
	{
		printf("Data[%d]:(%.3f),",i,cb[i]);
	};
	printf("\n*********Test-6:End************\n\n");
#endif

	return 0;
}

