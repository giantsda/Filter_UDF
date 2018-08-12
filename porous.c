/*
 ============================================================================
 Name        : porous.c
 Author      : Chen Shen
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "udf.h"

/* Global data */
int max_state = 3000;
int number_filter_cells;
double* filter_cell_status;
double* current_add;
int filter_ID = 6;
double base_vr = 3.7822e+10, lost_percent= 0.99698189;
int allowed_hit_one_step = 5;
Injection * injection; /* I need to pass it to DEFINE_EXECUTE_AT_END */
double my_x0 = 0.07054139, my_y0 = 0.049, my_z0 = 0., r = 0.000985; /* defines the origin for random particle injection check r value for finer mesh y0 was defined somewhere else .. */

/* Function prototype */
int
Probablity (double p);

DEFINE_EXECUTE_ON_LOADING(Initialize, libname) 
{
	/* 1. calculate how many cells are in the filter each node. 
	2. Allocate memory for filter_cell_status and current_add. 
	3. initialize filter_cell_status. 
	4. set seed for random number generator */
  number_filter_cells = 0;
  Domain * domain = Get_Domain (1);  /* 1 is for single phase */
  Thread * thread = Lookup_Thread (domain, filter_ID); 
  cell_t c;
  begin_c_loop (c, thread)
    {
      number_filter_cells++;
    }
  end_c_loop (c, thread);
  printf ("\n number_filter_cells=%d on node:%d <<<<<<<<<<<<<<<<<<<< \n",
	  number_filter_cells, myid);
  filter_cell_status = (double*) malloc (sizeof(double) * number_filter_cells);
  current_add = (double*) malloc (sizeof(double) * number_filter_cells);
  int i;
  for (i = 0; i < number_filter_cells; i++) /* initialize filter_cell_status */
    {
      filter_cell_status[i] = 0.;
    }
  srand (time (0) + myid * 123);  /* set seed for random number */
  fflush (stdout);
}

DEFINE_DPM_SCALAR_UPDATE(set_filter_cell_status, c, t, initialize, p) 
{
	/* 1.set_filter_cell_status once a particle hits the filter*/
  Domain * domain = Get_Domain (1);  /* 1 is for single phase */
  Thread * filter_thread = Lookup_Thread (domain, filter_ID);
  if (t == filter_thread) /* Thread comparison. True if the particle is in the filter. */
    {
      /*printf("particle %d in cell: %d,position: %f,%f,%f, current time=%f on node %d\n",p->part_id, c,P_POS(p)[0],P_POS(p)[1],P_POS(p)[2],P_TIME(p),myid); */
      if (filter_cell_status[c] < max_state
	  && current_add[c] < allowed_hit_one_step && N_TIME % 1 == 0) /* The max allowed hit per cell, and max allowed hits per time */
	{
	  filter_cell_status[c]++;
	  current_add[c]++;
	}
      p->stream_index = -1;
      p->gvtp.n_aborted++; /* once a particle reaches the filter, it is marked as aborted. */
    }
  fflush (stdout);
}

DEFINE_PROFILE(change_vis_res, t, i) 
{
	/* 1. change the cell_vr based on how many particles have hit the filter */
  cell_t c;
  real cell_vr;
  begin_c_loop (c, t)
    {
      cell_vr = base_vr * (1 + filter_cell_status[c] / max_state * 10); /* if the cell got max hit, the vr is 11 times of the base_vr */
      F_PROFILE (c, t, i) = cell_vr;
    }
  end_c_loop (c, t)
  fflush (stdout);
}

DEFINE_DPM_INJECTION_INIT(random_injection, I) 
{
	/* 1. generate random position for injection. 2.Pass I for further use.*/
	
  /* printf("Executed at time step %d from node %d \n",N_TIME,myid); */
  Particle *p;
  injection = I;
  loop (p, I->p_init)
  /* Standard Fluent Looping Macro to get particle. >> p_init << for transient mode,p for SS */
    {
      double x = 2000., y = 2000., z = 2000.; /* make sure it is out of the domain initially. */
      if (Probablity (lost_percent))  /* 99 percent particle is going to be deleted. */
	p->stream_index = -1;  /*  delete particle. */
      else 			/* if not deleted, its position is regenerated. */
	{
	  while (sqrt (pow (x - my_x0, 2) + pow (z - my_z0, 2)) >= r) /* while it is out of the domain */
	    {
	      x = my_x0
		  + (double) rand () / (double) ((unsigned) RAND_MAX + 1) * 2
		      * r - r;  /* from -0.001 to 0.001 */
	      y = my_y0
		  + (double) rand () / (double) ((unsigned) RAND_MAX + 1) * 2
		      * r * 0.2 - 0.2 * r; /* vertical locations are allowed to move by 0.2r */
	      z = my_z0
		  + (double) rand () / (double) ((unsigned) RAND_MAX + 1) * 2
		      * r - r;
	    }
	  P_POS (p)[0] = x;
	  P_POS (p)[1] = y;
	  P_POS (p)[2] = z;
	}
    }
  fflush (stdout);
}

DEFINE_EXECUTE_AT_END(set_UDS) 
{
	/*1.set UDS for postprocessing. UDS_1 for filter_cell_status and 
	UDS_2 denotes it is in the filter or not. 
	2. restore deleted particles. 
	3. reinitialize current_add */
  Domain * domain = Get_Domain (1);  /* 1 is for single phase */
  Thread * thread = Lookup_Thread (domain, filter_ID); 
  cell_t c;
  begin_c_loop (c, thread)
    {
      C_UDSI (c, thread, 0) = filter_cell_status[c];
      C_UDSI (c, thread, 1) = 1.;
    }
  end_c_loop (c, Thread);
 
 int i;
  for (i = 0; i < number_filter_cells; i++)
    {
      current_add[i] = 0;
    }
 
  if (!RP_HOST) /* have to add it because host node does not have data on it! */
    {
      Particle *p;
      loop (p, injection->p_init)
	{
	  p->stream_index = p->part_id; /* recover deleted particles.I think it is OK for any position int. */
	}
    }
  fflush (stdout);
}

DEFINE_ON_DEMAND(fprint_cell_status) /* print cell status to files */
{
  if (!RP_HOST)
    {
      Domain * domain = Get_Domain (1);  /* 1 is for single phase */
      Thread * thread = Lookup_Thread (domain, filter_ID); 
      char filename[256];
      strcpy (filename, "filter_cell_status_node_");
      char snum[64];
      sprintf (snum, "%d", myid);
      strcat (filename, snum);
      strcat (filename, ".txt");
      FILE* fp = fopen (filename, "w");
      if (fp == NULL)
	printf ("ERROR: Unable to open %s\n", filename);
      else
	printf ("writing local data to %s on node %d\n", filename, myid);
      /* Loop over cells on filter */
      real x[ND_ND];
      cell_t c;
      begin_c_loop (c, thread)
	{
	  C_CENTROID (x, c, thread);
	  fprintf (fp, "%f,%f,%f,%f\n", x[0], x[1], x[2],
		   filter_cell_status[c]);
	}
      end_c_loop (c, thread);
      fclose (fp);
      fflush (stdout);
    }
}

DEFINE_ON_DEMAND(random_number_generator) /* for debugging */
{
  /*	
  printf("current is %f,  time step=%d,   on node %d \n", CURRENT_TIME,N_TIME,myid);
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
      printf ("injection is >>> %p \n", injection);
      Particle *p;

      loop (p, injection->p)
	{
	  printf ("particle %d stream index=%d\n", p->part_id, (int) p->stream_index);
	  p->stream_index = p->part_id;
	}
    }

  fflush (stdout);
}

int
Probablity(double p)
{
  double random_number = (double) rand () / (double) ((unsigned) RAND_MAX + 1);
  if (random_number < p)
    return 1;
  else
    return 0;
}


