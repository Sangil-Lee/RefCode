#include <unistd.h>		// sleep
//#include <math.h>
#include <mdsobjects.h>

#define	SERVER_ADDR	"127.0.0.1:8000"
#define	TREE_NAME	"image"
#define	TAG_NAME1	"\\RBACH01:FOO"
#define	TAG_NAME2	"\\RBACH02:FOO"
#define	SHOT_NUM	9999

const int SEGMENT_NUM	= 5;
const int SEGMENT_DATAS	= 10;
const int TOTAL_SAMPLES	= (SEGMENT_NUM * SEGMENT_DATAS);

const float START_TIME	= 10.0;
const float END_TIME	= 11.0;
const float SAMPLE_RATE	= ((END_TIME - START_TIME) / (float)SEGMENT_DATAS);

using namespace MDSplus;

void seg ()
{

}

int main(int argc, char *argv)
{
	int i, j;
	float data[TOTAL_SAMPLES];
	float currData[SEGMENT_DATAS];

	try {
		/*Open TEST pulse file, shot 1 */
		Tree *t = new Tree(TREE_NAME, SHOT_NUM);

		/* Get node \SEGMENTED */
		TreeNode *node = t->getNode(TAG_NAME1);

		/* Make sure no data is contained */
		//TODO : segmentation fault
		//node->deleteData();

		Data *start = new Float64(START_TIME);
		Data *end = new Float64(END_TIME);
		Data *rate = new Float64(SAMPLE_RATE);
		Data *dimension = new Range(start, end, rate);
		Array *dataArray = new Float32Array(data, TOTAL_SAMPLES);

		printf ("start(%f) end(%f) rate(%f)\n", START_TIME, END_TIME, SAMPLE_RATE);

		node->beginSegment(start, end, dimension, dataArray);

		for(i = 0; i < SEGMENT_NUM; i++) {
			// dummy data
			for(j = 0; j < SEGMENT_DATAS; j++) {
				currData[j] = i+1;
			}

			printf ("[%02d] putSegment ... \n", i);

			//Fill currData array with acquired data chunk
			Array *subArr = new Float32Array(currData, SEGMENT_DATAS);
			node->putSegment(subArr, -1);

			//delete(subArr);
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
