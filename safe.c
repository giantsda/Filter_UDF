#include "udf.h"

int aa=1,n;
double eps=1e-8;
double node_x[2],node_z[2]; // node_x stores the smallest and largest value of all node x component.
int ii;
int num=0;

 /*            DEFINE_PROFILE(vis_res222,t,i)
{
	
	
	Domain * domain=Get_Domain(1);
	Thread *thread=Lookup_Thread(domain,6);
	printf("Got the thread %x  \n",thread);
	int zone_ID=	 THREAD_ID(thread);
	printf("porous code!!!!!!!!!!!!!!!!!     i=%d        !!!!!!!!!!!!!!  %d \n",i,zone_ID);

	 
		printf("i=%f ; i=%x \n",i,i);
		
	real x[ND_ND];
	real a;
	cell_t c;
	double xxx[2]={0.08,-0.27},nx,ny,nz;
	double node;
	face_t f;
	Node *v;
	int nc=1;
	begin_c_loop(c,t)
	{
	//	printf("%d  \n",nc);
		v=C_NODE(c,t,0);
		nx=NODE_X(v);
		nz=NODE_Z(v);
		node_x[0]=nx;
		node_x[1]=nx;
		node_z[0]=nz;
		node_z[1]=nz;
		c_node_loop(c,t,n)
		{
			v=C_NODE(c,t,n);
			nx=NODE_X(v);
			ny=NODE_Y(v);
			nz=NODE_Z(v);
			if (nx<=node_x[0])
			node_x[0]=nx;
			if (nx>=node_x[1])
			node_x[1]=nx;
			if (nz<=node_z[0])
			node_z[0]=nz;
			if (nz>=node_z[1])
			node_z[1]=nz;		
		 	if (nc<=20)
			{
			printf("%d: %3.20f %3.20f %f node %d \n",nc,nx,ny,nz,n);
			}
		}
		
		printf("in this cell, the node_x= %f %f  the node_z= %f  %f  \n",
		node_x[0],node_x[1],node_z[0],node_z[1]);

		if (xxx[0]>=node_x[0] && xxx[0]<=node_x[1] && xxx[1]>=node_z[0] && xxx[1]<=node_z[1])
			printf("THE PARTICLE IS HERERERERERER!!!!!!!!!!!!!!!\n");
 
 
		nc++;
	}
	end_c_loop(c,t);
	

	DEFINE_PROFILE(vis_res222,t,i)

 
	begin_c_loop(c,t)
	{
		C_CENTROID(x,c,t);
		if (x[0]<0)
			a=1e9;
		else
			a=1;
		F_PROFILE(c,t,i)=a;
		
		int haha=C_NNODES(c,t);
		if (aa==1)
			printf(">>> %d %f %f %f %d \n",c,x[0],x[1],x[2],haha);
	}
	end_c_loop(c,t)
	//for (int ii=0;ii<=5;ii++)
 		//	printf("%f\n",t[ii]);  
 
	
	fflush(stdout); 
	aa++;
}    
 
 
	DEFINE_DPM_SCALAR_UPDATE(filter_DPM,c,t,initialize,p)
	{
	 
	printf("Now particle %d in cell : %d  %d   %d\n",p->part_id,c,aa,P_CELL(p));
	aa++;
	}            */       

 

DEFINE_PROFILE(vis_res222,t,i)
{
	ii=i;
	num++;
	
	 
	printf("num=%d  <<<<<<<<<<<<<<<<<<<  \n",num);
	fflush(stdout); 
}


  DEFINE_EXECUTE_AT_END(end_of_time_step)
{	
	 num=0;
	 
	 fflush(stdout); 
}   

 