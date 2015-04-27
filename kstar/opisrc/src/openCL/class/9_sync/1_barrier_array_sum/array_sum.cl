__kernel void array_sum(__global int* A)
{
	int id = get_global_id(0);
	int work_items = get_global_size(0);

	int num_float = ARR_SIZE / work_items;

	int i;


	for (i = 1; i < num_float; ++i)
	{
		A[id] += A[id + i*work_items];
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	if(id==0)
	{
		for (i = 1; i < work_items; ++i)
	   {
		   A[0] += A[i];
		}
	}
}

