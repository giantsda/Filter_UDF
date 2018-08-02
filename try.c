#include "udf.h"

int n=0;
int number_of_cells;
double eps=1e-8;
double node_x[2],node_z[2]; // node_x stores the smallest and largest value of all node x component.
int  *cell_status;

 DEFINE_EXECUTE_ON_LOADING(on_laoding, libname)
{
	number_of_cells=0;
	Domain * domain=Get_Domain(1);  // 1 is for single phase
	Thread * thread=Lookup_Thread(domain,8); // 
	int zone_ID=	 THREAD_ID(thread);
	real x[ND_ND];
	cell_t c;
	begin_c_loop(c,thread)
		{
			C_CENTROID(x,c,thread);
			//	printf("cell: %d on node:%d \n",c,myid);
			number_of_cells++;
		}
	end_c_loop(f,t);
	printf("number_of_cells=%d on node:%d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n",number_of_cells,myid);
	cell_status=(int*)malloc(sizeof(int)*number_of_cells);
	printf("memory is allocated from node %d\n",myid);
	for (int i=0;i<number_of_cells;i++)
	{
		cell_status[i]=0;
	}
	fflush(stdout); 
}
 
  DEFINE_PROFILE(change_vis_res,t,i)
{
	//printf("number_of_cells=%d \n",number_of_cells);
     cell_t c;
	 real x[ND_ND],a;
	begin_c_loop(c,t)
	{
		if (x[0]<0)
			a=1e9;
		else
			a=1;
		F_PROFILE(c,t,i)=a;
		int haha=C_NNODES(c,t);
		 	}
	end_c_loop(c,t)
	fflush(stdout); 
}    

DEFINE_DPM_SCALAR_UPDATE(dpm_udf,c,t,initialize,p)
{
	Domain * domain=Get_Domain(1);  // 1 is for single phase
	Thread * filter_thread=Lookup_Thread(domain,8); // 
	if(t==filter_thread)
	{
		printf("UDF 1: particle %d is currently in cell: %d,\
		position: %f,%f,%f, current particle time=%f on node %d\n",p->part_id, c,P_POS(p)[0],P_POS(p)[1],P_POS(p)[2],P_TIME(p),myid);
		cell_status[c]++;
		p->stream_index = -1; 
		p->gvtp.n_aborted++; // once a particle reaches the filter, it is marked as aborted.
	}
	fflush(stdout); 
}

DEFINE_ON_DEMAND(print_cell_status)
{
	for (int i=0;i<number_of_cells;i++)
		printf("cell_status[%d]=%d; \n",i,cell_status[i]);
}




