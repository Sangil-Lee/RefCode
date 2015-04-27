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
		/*Open TEST pulse file, shot 1 */
		Tree *t = new Tree(TREE_NAME, SHOT_NUM);

		/* Get node \SEGMENTED */
		TreeNode *node = t->getNode(TAG_NAME);

		/* Make sure no data is contained */
		// Segmentation fault => 3.0 library & header is ok
		node->deleteData();

		Data *start = new Float64(10.);
		Data *end = new Float64(11.);
		Data *rate = new Float64(0.1);
		Data *dimension = new Range(start, end, rate);
		Data *readData = NULL;

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

			delete(subArr);
			sleep (3);
		}

		/* Free stuff */
		//deleteData(start);
		//deleteData(end);
		//deleteData(rate);
		deleteData(dimension);
		delete(t);
	}
	catch(MdsException *exc) {
		cout << "Error appending segments: " << exc->what();
	}
}
