#include <unistd.h>		// sleep
#include <mdsobjects.h>

#define	SERVER_ADDR	"127.0.0.1:8000"
#define	TREE_NAME	"image"
#define	TAG_NAME	"\\RBACH01:FOO"
#define	SHOT_NUM	9999

using namespace MDSplus;

void print_node_data (TreeNode *node)
{
	Data *readData = node->getData();

	cout << "Data : " << readData << endl;
	cout << "getNumSegments : " << node->getNumSegments() << endl;
}

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

		//Array *dataArray = new Float32Array(data, 10);
		Array *dataArray = new Float32Array(data, 10);

		node->beginSegment(start, end, dimension, dataArray);

		printf ("start ...\n");
		sleep (3);

		float stime, etime, rtime;
		stime = 10.0;
		etime = 10.0;
		rtime = 0.1;

		for(i = 0; i < 5; i++) {
			for(j = 0; j < 2; j++) {
				currData[j] = i+1;
			}

			stime += 2 * rtime;
			etime = stime + 1 * rtime;

			printf ("[%02d] putSegment : %f, %fm %f \n", i, stime, etime, rate);

			Data *start2 = new Float64(stime);
			Data *end2 = new Float64(etime);
			Data *dim2 = new Range(start2, end2, rate);
			Array *subArr = new Float32Array(currData, 2);

			printf ("111\n");
			node->putSegment(subArr, -1);
			printf ("311\n");
			node->updateSegment(start2, end2, dim2);
			printf ("211\n");

			e->setEvent ("SEGMENT");

			print_node_data (node);

			delete(subArr);
			sleep (3);
		}

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
