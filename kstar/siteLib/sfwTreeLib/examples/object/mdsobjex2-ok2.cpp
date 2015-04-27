#include <unistd.h>		// sleep
#include <mdsobjects.h>

#define	SERVER_ADDR	"127.0.0.1:8000"
#define	TREE_NAME	"image"
#define	TAG_NAME1	"\\RBACH01:FOO"
#define	TAG_NAME2	"\\RBACH02:FOO"
#define	SHOT_NUM	9999

using namespace MDSplus;

int main(int argc, char *argv)
{
	int i, j;
	float data[1000];
	float currData[100];

	for(j = 0; j < 1000; j++) {
		data[j] = 0;
	}

	try {
		/*Open TEST pulse file, shot 1 */
		Tree *t = new Tree(TREE_NAME, SHOT_NUM);

		/* Get node \SEGMENTED */
		TreeNode *node = t->getNode(TAG_NAME1);
		/* Make sure no data is contained */
		//TODO: Segmentation fault
		//node->deleteData();

		Data *start = new Float64(10.);
		Data *end = new Float64(11.);
		Data *rate = new Float64(1E-3);
		Data *dimension = new Range(start, end, rate);

		Array *dataArray = new Float32Array(data, 1000);
		node->beginSegment(start, end, dimension, dataArray);

		for(i = 0; i < 10; i++) {
			for(j = 0; j < 100; j++) {
				currData[j] = i+1;
			}

			printf ("[%02d] putSegment ... \n", i);

			//Fill currData array with acquired data chunk
			Array *subArr = new Float32Array(currData, 100);
			node->putSegment(subArr, -1);

			delete(subArr);
			//sleep (3);
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
