#include <unistd.h>		// sleep
#include <mdsobjects.h>

#define	SERVER_ADDR	"127.0.0.1:8000"
#define	TREE_NAME	"image"
#define	TAG_NAME	"\\RBACH01:FOO"
#define	SHOT_NUM	9999

using namespace MDSplus;

int main(int argc, char *argv)
{
	int i, j;
	float data[10];
	float currData[2];

	Event *e = new Event();

	try {
		/* Open pulse file, shot # */
		Tree *t = new Tree(TREE_NAME, SHOT_NUM);

		/* Get node */
		TreeNode *node = t->getNode(TAG_NAME);

		printf ("deleteData ...\n");
		sleep (3);

		/* Make sure no data is contained */
		// Segmentation fault => 3.0 library & header is ok
		node->deleteData();

		Data *start = new Float64(10.);
		Data *end = new Float64(11.);
		Data *rate = new Float64(0.1);
		Data *dimension = new Range(start, end, rate);
		Data *readData = NULL;

#if 1
		memset (data, 0x00, sizeof(data));
		Array *dataArray = new Float32Array(data, 10);

		node->beginSegment(start, end, dimension, dataArray);

		printf ("start ...\n");
		sleep (3);

		for(i = 0; i < 5; i++) {
			for(j = 0; j < 2; j++) {
				currData[j] = i+1;
			}

			printf ("[%02d] putSegment ... \n", i);

			Array *subArr = new Float32Array(currData, 2);
			node->putSegment(subArr, -1);

			e->setEvent ("SEGMENT");

			readData = node->getData();
			cout << "Data : " << readData << endl;
			cout << "getNumSegments : " << node->getNumSegments() << endl;

			delete(subArr);
			sleep (3);
		}
#elif 0
		printf ("start ...\n");
		sleep (3);

		float stime, etime, rtime;

		for(i = 0; i < 5; i++) {
			for(j = 0; j < 2; j++) {
				currData[j] = i+1;
			}

			stime = 10 + i * 2;
			etime = stime + 1;

			printf ("[%02d] makeSegment : %f, %f, %f \n", i, stime, etime, rate);

			Data *start2 = new Float64(stime);
			Data *end2 = new Float64(etime);
			Data *rate2 = new Float64(1);
			Data *dimension2 = new Range(start2, end2, rate2);
			Array *subArr = new Float32Array(currData, 2);

			// preveous data is removed
			node->makeSegment(start2, end2, dimension, subArr);

			// time is invalid
			//Build_Signal([1.,1.,2.,2.,3.,3.,4.,4.,5.,5.], *, [10.,10.1,10.,10.1,10.,10.1,10.,10.1,10.,10.1])

			e->setEvent ("SEGMENT");

			readData = node->getData();
			cout << "Data : " << readData << endl;
			cout << "getNumSegments : " << node->getNumSegments() << endl;

			delete(subArr);
			sleep (3);
		}
#else
		_int64 currTime;
		for(i = 0; i < 5; i++) {
			for(j = 0; j < 2; j++) {
				currData[j] = i+1;
			}

			printf ("[%02d] putRow ... \n", i);
			//currTime = (i * 2) + 1;
			currTime = i + 1000;

			Array *subArr = new Float32Array(currData, 2);
			node->putRow(subArr, &currTime, 2);

			e->setEvent ("SEGMENT");

			readData = node->getData();
			cout << "Data : " << readData << endl;
			cout << "getNumSegments : " << node->getNumSegments() << endl;

			delete(subArr);
			sleep (3);
		}
#endif

		/* Free stuff */
		deleteData(start);
		deleteData(end);
		deleteData(rate);
		deleteData(dimension);
		delete(t);
	}
	catch(MdsException *exc) {
		cout << "Error appending segments: " << exc->what();
	}
}
