/*
The main function for the one-dimensional HHH program
- Ran Ben Basat, June 2017.
Based on an earlier version:
- Thomas Steinke (tsteinke@seas.harvard.edu) 2010-11
*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>
#include<sys/time.h>
#ifndef NRRUNS
#define NRRUNS 1
#endif

#ifndef VMULT
#define VMULT 1
#endif

#if VMULT>1
#define PROB
#endif




#ifndef CLK_PER_SEC
#ifdef CLOCKS_PER_SEC
#define CLK_PER_SEC CLOCKS_PER_SEC
#endif
#endif


#ifdef RHHH
#include "randhhh1D.h"
#define INCLUDED_SOMETHING
#endif


#ifndef INCLUDED_SOMETHING
#error Algorithm undefined
#endif


#if NUM_MASKS==5
LCLitem_t masks[NUM_MASKS] = {
	0xFFFFFFFFu,
	0xFFFFFF00u,
	0xFFFF0000u,
	0xFF000000u,
	0x00000000u
};
int leveleps[NUM_MASKS] = {
	32,
	24,
	16,
	8,
	0
};
#endif
#if NUM_MASKS==33
LCLitem_t masks[NUM_MASKS] = {
	0xFFFFFFFFu << 0,
	0xFFFFFFFFu << 1,
	0xFFFFFFFFu << 2,
	0xFFFFFFFFu << 3,
	0xFFFFFFFFu << 4,
	0xFFFFFFFFu << 5,
	0xFFFFFFFFu << 6,
	0xFFFFFFFFu << 7,
	0xFFFFFFFFu << 8,
	0xFFFFFFFFu << 9,
	0xFFFFFFFFu << 10,
	0xFFFFFFFFu << 11,
	0xFFFFFFFFu << 12,
	0xFFFFFFFFu << 13,
	0xFFFFFFFFu << 14,
	0xFFFFFFFFu << 15,
	0xFFFFFFFFu << 16,
	0xFFFFFFFFu << 17,
	0xFFFFFFFFu << 18,
	0xFFFFFFFFu << 19,
	0xFFFFFFFFu << 20,
	0xFFFFFFFFu << 21,
	0xFFFFFFFFu << 22,
	0xFFFFFFFFu << 23,
	0xFFFFFFFFu << 24,
	0xFFFFFFFFu << 25,
	0xFFFFFFFFu << 26,
	0xFFFFFFFFu << 27,
	0xFFFFFFFFu << 28,
	0xFFFFFFFFu << 29,
	0xFFFFFFFFu << 30,
	0xFFFFFFFFu << 31,
	0x00000000u
};
int leveleps[NUM_MASKS] = {
	                     32,31,30,
	29,28,27,26,25,24,23,22,21,20,
	19,18,17,16,15,14,13,12,11,10,
	 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
};
#endif
double now_us ()
{
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return (tv.tv_sec * (uint64__t) 1000000 + (double)tv.tv_nsec/1000);
}

double dblmainmax(double a, double b) {return (a >= b ? a : b);}

int main(int argc, char * argv[]) {
		int m; //number of heavy hitters in output
		int counters = 100;
		int threshold = 1000;
		int n = 100000;
		double time, walltime;
		double epsil;
		HeavyHitter * ans;
		int i,j;
		unsigned int w, x, y, z;
		clock_t begint, endt;
		struct timeb begintb, endtb;
        FILE * readdata;
		unsigned int ip;
		unsigned int * data;
		double insertProb = 1. / VMULT;	
		double measurements[NRRUNS] = {0};
		double t_4 = 2.572; //Change if NRRUNS!=5
		double avg = 0, stdev = 0, upCI = 0, dnCI= 0, sos = 0;
		FILE * fp = NULL; //the file for the output
		FILE * fsummary = NULL;
		if (getenv("SUMMARYFILE") != NULL) fsummary = fopen(getenv("SUMMARYFILE"), "a");

		if (argc > 1) n = atoi(argv[1]);
		if (argc > 2) counters = atoi(argv[2]);
		if (argc > 3) threshold = atoi(argv[3]);
		if (argc > 4) fp = fopen(argv[4], "w");
        if (argc > 5) readdata = fopen(argv[5], "r");
		if(n/counters >= threshold) {
			printf("Unacceptable parameters: eps*n >= theshold\n");
			return 0;
		}
        
		data = (unsigned int *) malloc(sizeof(unsigned int) * n);

		for (i = 0; i < n;i++) {
            unsigned int item=0;
			for (int j = 0; j < 4; j++) {
			fscanf(readdata, "%u", &x);
			item = ((item << 8) | x);
            }
			data[i] = item;
		}

		for (j=0; j < NRRUNS; ++j){
			init((double)1/(double)counters, insertProb);
			double start_time=now_us();
			for (i = 0; i < n; i++){
				update(data[i], 1);
			}
			double end_time=now_us();
            time=(end_time-start_time);
			double throughput=n/time;
            printf("throughput %f ", throughput);
		}
		


		free(data);

		ans = output(threshold, &m, n);

		printf("%d HHHs\n", m);
		
		deinit();
		if (fp != NULL) {
			for (i = 0; i < m; i++) {
                int level=ans[i].mask;
                uint64__t key=ans[i].item;
                key=(key<<32)+level;
				fprintf(fp, "%llu %d\n",
				key,
                ans[i].upper * ( (int) (NUM_MASKS / insertProb)));	
			}
			fclose(fp);
		}
		
		epsil = -1;
		for (i = 0; i < m; i++) {
			epsil = dblmainmax(epsil, ((double)(ans[i].upper-ans[i].lower))/n );
		}

		free(ans);

		if (fsummary != NULL) {
			fprintf(fsummary, "check=false algorithm=%-14s nitems=%-10d counters=%-5d threshold=%-9d  outputsize=%-3d time=%lf walltime=%lf epsilon=%lf",
			                   argv[0],        n,           counters,     threshold,      m,              time,  walltime, epsil);
			fprintf(fsummary, "\n");
			fclose(fsummary);       
		}

	return 0;
}


