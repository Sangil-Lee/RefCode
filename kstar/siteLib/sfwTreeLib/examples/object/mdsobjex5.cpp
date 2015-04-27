//#include <mdsdata_Data.h>  
#include <mdsobjects.h>
//#include <mdstree_MdsTree.h>
//#include <mdslib.h>

#if 1
#define	SERVER_ADDR	"172.17.102.120:8000"
#define	TREE_NAME	"image"
#define	SHOT_NUM	9999
#define	TAG_NAME	"\\RBACH01:FOO"
#else
#define	SERVER_ADDR	"172.17.102.58:8000"
#define	TREE_NAME	"r_spectro"
#define	SHOT_NUM	1
#define	TAG_NAME	"\\TOR_VB01:FOO"
#endif

using namespace MDSplus;

int main()
{
	Connection *ex12Conn = new Connection(SERVER_ADDR);
	if (!ex12Conn) {
		printf ("failed ...\n");
		return -1;
	}
	printf ("111\n");
	ex12Conn->openTree(TREE_NAME, SHOT_NUM);
	printf ("211\n");

	Tree *ex12Tree = new Tree(TREE_NAME, SHOT_NUM);
	if (!ex12Tree) {
		printf ("new Tree failed ...\n");
		return -1;
	}
	printf ("311\n");

	Data *ex12Start = new Float64(10.);
	Data *ex12End = new Float64(11.);
	ex12Tree->setTimeContext(ex12Start, ex12End, new Float64(1E-2));

	TreeNode *ex12Node = ex12Tree->getNode(TAG_NAME);
	if (!ex12Node) {
		printf ("getNode failed ...\n");
		return -1;
	}

	//int data_length = ex12Node->getLength();
	int data_length;
	Data *ex12Data = ex12Node->getData();
	double *mdsValue = ex12Data->getDoubleArray(&data_length);

	//for(int i = 0; i < data_length;i++)
	for(int i = 0; i < 10;i++)
	{
		printf("Re-Sampled Data[%d/%d]:%f\n", i, data_length, mdsValue[i]);
	};

	return 0;
};
