#include "udf.h"

int Probablity(double p);
Injection * injection; // I need to pass it to DEFINE_EXECUTE_AT_END
double x0=0.07054139,y0=0.049,z0=0.,r=0.000985;



 DEFINE_EXECUTE_ON_LOADING(on_laoding, libname)
{
	printf("srand is done\n");
	srand(time(0)+myid*123);
	fflush(stdout); 
}
 
DEFINE_DPM_INJECTION_INIT(random_injection,I)
{
	// printf("Executed at time step %d from node %d \n",N_TIME,myid);
	Particle *p;
	injection=I;
	cell_t cell;
	Thread *cthread;
	//printf("injection is set:%p on node %d\n",injection,myid);
 loop(p,I->p_init)  /* Standard Fluent Looping Macro to get particle p_init for transient mode ,p for SS
                  streams in an Injection */
	{	
	double x=2000,y=2000,z=2000;
		if(Probablity(0.5))  //80 percent particle is going to be deleted.
		 	p->stream_index = -1;  // deleted
		else 					// if not deleted, its position is regenerated.
		{
			while(sqrt(pow(x-x0,2)+pow(z-z0,2))>=r)  // while it is out of the domain
			{
				x=x0+(double)rand() / (double)((unsigned)RAND_MAX + 1)*2*r-r;  // from -0.001 to 0.001
				y=y0+(double)rand() / (double)((unsigned)RAND_MAX + 1)*2*r*0.2-0.2*r;  // vertical locations are allowed to move by 0.2r
				z=z0+(double)rand() / (double)((unsigned)RAND_MAX + 1)*2*r-r;
			}
			P_POS(p)[0]=x;
			P_POS(p)[1]=y;
			P_POS(p)[2]=z;			
		}
	}
	fflush(stdout); 
}


DEFINE_EXECUTE_AT_END(restor_injection)
{
	if (!RP_HOST)
	{
		Particle *p;
		loop(p,injection->p_init)   
		{
			p->stream_index =p->part_id;
		}
	}
	fflush(stdout); 
}


 

DEFINE_ON_DEMAND(random_number_generator)
{
/*	printf("current is %f,  time step=%d,   on node %d \n", CURRENT_TIME,N_TIME,myid);
 	//if(myid==0)
	{
		for(int i = 0; i<100; i++)
		{
			if(Probablity(0.1))
				printf("1");
			else
				printf("0");
		}
			printf("\n");
	}
	*/
	
		if (!RP_HOST)
		{
			printf("injection is >>> %p \n",injection);
			Particle *p;
			
			loop(p,injection->p)   
			{
				printf("particle %d stream index=%d\n",p->part_id,p->stream_index);
				p->stream_index =p->part_id;
			}	
		}
	 
	fflush(stdout); 
}

int Probablity(double p)
{
	double random_number=(double)rand() / (double)((unsigned)RAND_MAX + 1);
	if(random_number<p)
		return 1;
	else
		return 0;
}