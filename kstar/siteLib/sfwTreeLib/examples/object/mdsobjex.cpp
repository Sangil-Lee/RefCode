//#include <mdslib.h>
#include <mdsobjects.h>

#if 0
#define	SERVER_ADDR	"172.17.102.120:8000"
#define	TREE_NAME	"image"
#define	SHOT_NUM	9999
#define	TAG_NAME	"\\RBACH01:FOO"
#define	EVENT_NAME	"SEGMENT"
#else
#define	SERVER_ADDR	"127.0.0.1:8000"
#define	TREE_NAME	"r_spectro"
#define	SHOT_NUM	1
//#define	TAG_NAME	"\\TOR_VB01:FOO"
#define	TAG_NAME	"\\TOR_VB02:CAL"
#define	EVENT_NAME	"VBS"
#endif

using namespace MDSplus;

int main(int argc, char *argv)
{
	int i, j;
	float data[10];
	float currData[2];

#if 0
	int socket;

	/* Connect to MDSplus */
	socket = MdsConnect (SERVER_ADDR);
	if ( socket == -1 ) {
		fprintf(stderr,"Error connecting to Atlas.\n");
		return EXIT_FAILURE;
	}
	printf ("after MdsConnect\n");
#endif

	try {
#if 0
		Connection *conn = new Connection (SERVER_ADDR);
		if (!conn) {
			printf ("Connection failed \n");
		}

		printf ("after Connection\n");
#endif

		/* Open pulse file, shot # */
		Tree *t = new Tree(TREE_NAME, SHOT_NUM);
		printf ("after Tree\n");

		/* Get node */
		TreeNode *node = t->getNode(TAG_NAME);
		printf ("after getNode\n");

		/* Make sure no data is contained */
		// Segmentation fault => 3.0 library & header is ok
		node->deleteData();

		//float stime = -5;
		float stime = -5;

		Data *start = new Float64(10.);
		Data *end = new Float64(11.);
		Data *rate = new Float64(0.1);
		Data *dimension = new Range(start, end, rate);

		memset (data, 0x00, sizeof(data));
		Array *dataArray = new Float32Array(data, 2);

		for (i = 0; i < 5; i++) {
			start = new Float64(stime + i);
			end = new Float64(stime + 0.1 + i);
			rate = new Float64(0.1);
			dimension = new Range(start, end, rate);

			node->beginSegment (start, end, dimension, dataArray);

			for(j = 0; j < 2; j++) {
				currData[j] = i+1;
			}

			printf ("[%02d] putSegment ... \n", i);

			Array *subArr = new Float32Array(currData, 2);

			node->putSegment (subArr, -1);

			Event::setEvent (EVENT_NAME);

			cout << "Data (" << node->getNumSegments() << ") : " << node->getData() << endl;

			/* Free stuff */
			//deleteData(start);
			//deleteData(end);
			//deleteData(rate);
			deleteData(dimension);
			delete(subArr);

			sleep (2);
		}
		delete(t);
	}
	catch(MdsException *exc) {
		cout << "Error appending segments: " << exc->what();
	}
}
