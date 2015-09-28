/*
 * test.c
 *
 *  Created on: Mar 26, 2014
 *      Author: LI
 */

#include "config.h"
#include "models.h"

//#define A 16
//#define N 4096
//#define L 64      // large cache

//#define A 4
//#define N 64
//#define L 32      // small cache

double data_results[6];
double results[6];

int main() {

	int i,j;

//	int A[] = {16,16,16,16,8,16};
//	int N[] = {1024,2048,4096,8192,1024,1024};
//	int L[] = {64,64,64,64,16,16};

	int A[] = {2,4,8,16};
	int N[] = {1024,2048,4096,8192};
	int L[] = {8,16,32,64};

//	double exec_times[] = {0.02,0.11,1,0.05,0.79,0.25}; // nb: 175->1
//	int    data_sizes[] = {1000000,800,50000,20247800,135152,768604};
//	double exec_times[] = {0.02,1.1,9,0.5,7.9,2.5};

	double exec_times[] = {0.025,2.17,0.47,0.69,1.51,1.54};

	for(i=0; i<1; i++)
	{
		/* Define Cache */
		struct cache my_cache = { A[i], N[i], L[i], A[i]*N[i], A[i]*N[i]*L[i] };

		for(j=0;j<6;j++)
		{
			results[j] = 0;
		}

		/* Define FIT */
		double fit = (double)5000 / 1000000000 / 3600 / 122;

		/* MM */
		int data_sizes[] = {3200000,1600000,400000};
		int strides[] = {32,16,4};
		int data_num_mm = sizeof(data_sizes)/sizeof(data_sizes[0]);
		int num_elements[] = {100000,100000,100000};
		double runtime_mm = exec_times[0] * num_elements[0] / 10000;

		mm(my_cache,data_sizes,strides,data_num_mm,num_elements,fit,runtime_mm);


		/* CG
		char order_list[] = "(Ap)pxr(pr)r";
		char data_list[] = "Axpr";
		double size_a = 800 * 800 * 8 / L[i];
		double size_x = 800 * 8 / L[i];
		int size_list[] = {size_a,size_x,size_x,size_x};
		int list_length = sizeof(order_list)/sizeof(order_list[0])-1;
		int data_num_cg = sizeof(data_list)/sizeof(data_list[0])-1;
		double runtime_cg = exec_times[1] * 5000 / 10000;
		printf( "CG Runtime = %f",runtime_cg);

		cg(my_cache.set_size,my_cache.num_set,my_cache.line_length,order_list,list_length,data_list,size_list,data_num_cg,fit,runtime_cg); */

		/* Nbody
		double theta_nb = 1;
		struct rand_in node     = { 97, 4950*5*theta_nb, 80, 1000, 97*4950*5*theta_nb };
		struct rand_in particle = { 80, 1200*5*theta_nb, 4, 1000, 80*1200*5*theta_nb };
		double runtime_nb = exec_times[2] * 3000 / 10000;
		nbody(my_cache,node,particle,fit,runtime_nb);
		printf( "NB Runtime = %f\n",runtime_nb); */

		/* MG
		int data_size_mg = 46480*8;
//		int data_size_mg = 2024780;
		double runtime_mg = exec_times[3] * 363 / 10000;

		mg(my_cache,data_size_mg,fit,runtime_mg);*/

		/* FFT

		char filename[] = "C:\\Users\\LI\\Desktop\\ft.txt";
		int data_size_ft = 64 * 33 * 16;
//		int data_size_ft = 135152;
		double runtime_ft = exec_times[4] * 33 / 10000;
		printf( "FT Runtime = %f",runtime_ft);

		fft(my_cache,data_size_ft,filename,fit,runtime_ft); */

		/* MC
		double theta_mc = 1;
		struct rand_in grid   = { 48, 768604*theta_mc, 1, 100000, 36892992*theta_mc };
		struct rand_in energy = { 16, 768604*theta_mc, 1, 100000, 12297664*theta_mc };
		double runtime_mc = exec_times[5] * 36028 / 10000;
		mc(my_cache,grid,energy,fit,runtime_mc);
		printf( "MC Runtime = %f\n",runtime_mc);*/

		printf( "\n");
		for(j=0;j<5;j++)
		{
//			printf( "%9.8f ",results[j]);
		}
//		printf( "%9.8f\n",results[5]);

	}

	return 0;
}


