#include <math.h>
#include <mdsobjects.h>

#define WIDTH		640
#define HEIGHT		480
#define NUM_FRAMES	100
#define PI			3.14159

#define	SERVER_ADDR	"127.0.0.1:8000"
#define	TREE_NAME	"image"
#define	TAG_NAME1	"\\RBACH01:FOO"
#define	TAG_NAME2	"\\RBACH02:FOO"
#define	SHOT_NUM	9999

using namespace MDSplus;

int main(int argc, char *argv)
{
	try {
		/*Open TEST pulse file, shot 1 */
		Tree *t = new Tree(TREE_NAME, SHOT_NUM);

		/* Get node \SEGMENTED */
		TreeNode *node = t->getNode(TAG_NAME1);
		/* Make sure no data is contained */
		node->deleteData();

		/* Allocate space for a 16 bit frame */
		short *currFrame = new short[WIDTH*HEIGHT];

		/* Build and store the frame sequence */
		for(int frameIdx = 0; frameIdx < NUM_FRAMES; frameIdx++)
		{
			/* get the angular coefficient of the current line */
			double m = tan((2*PI*frameIdx)/NUM_FRAMES);

			/* Prepare the current frame (black with a white line)  */
			memset(currFrame, 0, WIDTH * HEIGHT * sizeof(short));
			for(int i = 0; i < WIDTH; i++)
			{
				int j = (int)round((i-WIDTH/2)*m);
				if(j >= -HEIGHT/2 && j < HEIGHT/2)
					currFrame[(j+HEIGHT/2)*WIDTH +i] = 255;
			}

			/* Time is the frame index */
			float currTime = frameIdx;
			/* Start time and end time for the current segment are the same (1 frame is contained) */
			Data *startTime = new Float32(currTime);
			Data *endTime = new Float32(currTime);
			/* Segment dimension is a 1D array with one element (the segment has one row) */ 
			int oneDim = 1;
			/* Data dimension is a 3D array where the last dimension is 1 */
			Data *dims = new Float32Array(&currTime, 1, &oneDim);
			int segmentDims[3];
			/* unlike MDSplus C uses row-first ordering, so the last MDSplus dimension becomes the first one in C */
			segmentDims[0] = 1;
			segmentDims[1] = HEIGHT;
			segmentDims[2] = WIDTH;

			Data *segment = new Int16Array(currFrame, 3, segmentDims);

			/* Write the entire segment */
			node->makeSegment(startTime, endTime, dims, (Array *)segment);

			/* Free stuff */
			deleteData(segment);
			deleteData(startTime);
			deleteData(endTime);
			deleteData(dims);
		}
	}catch(MdsException *exc)
	{
		cout << "Error appending segments: " << exc->what();
	}
}
