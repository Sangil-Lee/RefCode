//#include <mdsdata_Data.h>  
#include <mdsobjects.h>
//#include <mdstree_MdsTree.h>
//#include <mdslib.h>

using namespace MDSplus;

int main()
{
	//ex-1
	//Tree *ex1Tree = new Tree("pcs_kstar", 3448);

	Tree *ex1Tree = new Tree("pcs_kstar", 3579);
	TreeNode *ex1Node = ex1Tree->getNode("PCPF2U");
	Data *ex1Val = ex1Node->getData();
	cout << "Data read from NODE1: " << ex1Val << "\n";
	int ex1numElements;
	float *ex1retArray = ex1Val->getFloatArray(&ex1numElements);
	cout << ex1retArray << endl;

#if 1
	//ex-2
	double ex2dVal = 3.14;
	Float32 *ex2dData = new Float32(ex2dVal);
	Tree *ex2Tree = new Tree("my_tree", -1);
	TreeNode *ex2Node = ex2Tree ->getNode("NODE1");
	ex2Node->putData(ex2dData);

	//ex-3
	int *ex3iArr = new int[4];
	for(int i = 0; i < 4; i++) ex3iArr[i] = i;
	Int32Array *ex3iArrData = new Int32Array(ex3iArr, 4);
	Tree *ex3Tree = new Tree("my_tree", -1);
	TreeNode *ex3Node = ex3Tree ->getNode("NODE1");
	ex3Node->putData(ex3iArrData);

	//ex-4
	Tree *ex4Tree = new Tree("my_tree", -1);
	TreeNode *ex4Node = ex4Tree->getNode("NODE2");
	Data *ex4Expr = compile("2+NODE1");
	ex4Node->putData(ex4Expr);

	//ex-5
	short ex5rawArr[1000];
	float ex5dimArr[1000];
	//Fill rawArr and dimArr ...

	Tree *ex5Tree = new Tree("my_tree", -1);
	TreeNode *ex5Node = ex5Tree->getNode("NODE2");
	Int16Array *ex5rawData = new Int16Array(ex5rawArr, 1000);
	Float32Array *ex5dimData = new Float32Array(ex5dimArr, 1000);
	//Data *ex5Expr = compile("BUILD_SIGNAL(($VALUE + 5)*3, $1, $2)", 2, ex5rawData, ex5dimData);
	Data *ex5Expr = compileWithArgs("BUILD_SIGNAL(($VALUE + 5)*3, $1, $2)", 2, ex5rawData, ex5dimData);
	//$VALUE in signals means the raw data
	//In C++ the additional arguments must be preceded by their number
	ex5Node->putData(ex5Expr);

	//ex-6
	int ex6arr[6]={1,2,11,22,111,222};
	int ex6dims[2] = {3,2};
	Data *ex6arrD = new Int32Array(ex6arr, 2, ex6dims);
	cout << "Data: " << ex6arrD << "\n";

	//ex-7
	Tree *ex7Tree = new Tree("my_tree", -1);
	TreeNodeArray *ex7NodeArr = ex7Tree->getNodeWild("***");
	StringArray *ex7Names = ex7NodeArr->getPath();
	//method getPath returns a MDSplus StringArray type describing an array of strings 
	//(C++ has no native type for this) 
	for(int i = 0; i < ex7NodeArr->getNumNodes(); i++)
		  cout << ex7Names->getElementAt(i) << "\n";

	//ex-8
#if 1
	Tree *ex8Tree = new Tree("my_tree", -1, "NEW");
	ex8Tree->addNode(":NUM1", "NUMERIC");
	ex8Tree->addNode(":NUM2", "NUMERIC");
	ex8Tree->addNode(":NUM3", "NUMERIC");
	ex8Tree->addNode(":TXT", "TEXT");

	TreeNode *ex8SubNode = ex8Tree->addNode(".SUB1", "STRUCTURE");
	ex8Tree->setDefault(ex8SubNode);
	ex8Tree->addNode(":SUB_NODE1", "NUMERIC");
	ex8Tree->addNode(".SUB2", "STRUCTURE");
	TreeNode *lastNode = ex8Tree->addNode(".SUB2:SUB_NODE2", "NUMERIC");
	lastNode->addTag("TAG1");
	ex8Tree->write();
#endif

	//ex-9
	float ex9data[1000];
	Tree *ex9Tree = new Tree("my_tree", -1);
	TreeNode *ex9Node = ex9Tree->getNode("SIG_1");

	///Fill previous segments ...
	//acquire new set of 1000 samples in data ...

	Data *ex9Start = new Float64(10.);
	Data *ex9End = new Float64(11.);
	Data *ex9Dim = new Range(ex9Start, ex9End, new Float64(1E-3));
	//Dimension is represented by a MDSplus Range data object. Range describes a 
	//single speed clock by  specifying the init and 
	//end time and the clock period.
	Array *ex9DataArr = new Float32Array(ex9data, 1000);
	ex9Node->beginSegment(ex9Start, ex9End, ex9Dim, ex9DataArr);

	float ex9currData[100];
	for(int i = 0; i < 10; i++)
	{
		//Fill currData array with acquired data chunk
		Array *ex9subArr = new Float32Array(ex9currData, 100);
		ex9Node->putSegment(ex9subArr, -1);
	}

	//ex-10
	Tree *ex10Tree = new Tree("my_tree", -1);
	TreeNode *ex10Node = ex10Tree->getNode("SIG_1");
	for(int i = 0; i < 1000; i++)
	{
		float currVal = 0.;
		//fill currVal (here a scalar value) with the acquired sample //...
		Float32 *currData = new Float32(currVal);
#if 1
		_int64 currTime = i; //Time is here the index itself
		ex10Node->putRow(currData, &currTime);
#else
		Int64 currTime = Int64(i); //Time is here the index itself
		ex10Node->putRow(currData, (_int64*)currTime.getLong());
#endif
	}

	//ex-11
	Tree *ex11Tree = new Tree("my_tree", -1);
	TreeNode *ex11Node = ex11Tree->getNode("STRUCT");
	Data **ex11Fields = new Data*[2];
	ex11Fields[0] = new String("SOME TEXT HERE");
	ex11Fields[1] = new Int32(1);
	Data *ex11Apd = new Apd(2, ex11Fields);
	ex11Node->putData(ex11Apd);

	//ex-12
	Connection *ex12Conn = new Connection("172.17.100.200:8300");
	ex12Conn->openTree("pcs_kstar", -1);

	Tree *ex12Tree = new Tree("pcs_kstar", -1);

	Data *ex12Start = new Float64(10.);
	Data *ex12End = new Float64(11.);
	ex12Tree->setTimeContext(ex12Start, ex12End, new Float64(1E-2));

	TreeNode *ex12Node = ex12Tree->getNode("PCITFMSRD");

	//int data_length = ex12Node->getLength();
	int data_length;
	Data *ex12Data = ex12Node->getData();
	double *mdsValue = ex12Data->getDoubleArray(&data_length);

	for(int i = 0; i < data_length;i++)
	{
		printf("Re-Sampled Data[%d]:%f\n", i, mdsValue[i]);
	};

	Data **args = new Data *[2];
	args[0] = new Int32(2);
	args[1] = new Int32(10);
	Data *result = ex12Conn->get("(PCITFMSRD:FOO * $1)*$2", args, 2);

	ex12Conn->put("\\pcs_kstar", "(PCITFMSRD:FOO * $1)*$2", args, 2);
	printf("%p\n", result);

#endif
	return 0;
};
