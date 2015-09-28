/*
 * test.c
 *
 *  Created on: Mar 26, 2014
 *      Author: LI
 */
/*
#include "config.h"
#include "models.h"

#define A 16
#define N 4096
#define L 64      // large cache

//#define A 4
//#define N 64
//#define L 32      // small cache


double results[6];

int main() {

	int i,j;
	int idx = 0;
	double degrade[7] = {0.00,0.05,0.10,0.15,0.20,0.25,0.30};
//	double failure[7] = {1.00,0.26,0.26,0.26,0.26,0.26,0.26};
	double failure[7] = {1.00,0.000004,0.000004,0.000004,0.000004,0.000004,0.000004};
	for(i=idx; i<idx+1; i++)
	{
		for(j=0;j<6;j++)
		{
			results[j] = 0;
		}*/

		/* Defien FIT
		double fit = (double)5000 / 1000000000 / 3600 / 122 * failure[i];

		/* Defien Cache
		struct cache my_cache = { A, N, L, A*N, A*N*L };

		/* MM
		int data_sizes[] = {32000,16000,4000};
		int strides[] = {32,16,4};
		int data_num_mm = sizeof(data_sizes)/sizeof(data_sizes[0]);
		int num_elements[] = {1000,1000,1000};
		double runtime_mm = 15.3 * (1+degrade[i]);

		mm(my_cache,data_sizes,strides,data_num_mm,num_elements,fit,runtime_mm);


		/* CG
		char order_list[] = "(Ap)pxr(pr)r";
		char data_list[] = "Axpr";
		double size_a = 500 * 500 * 8 / L;
		double size_x = 500 * 8 / L;
		int size_list[] = {size_a,size_x,size_x,size_x};
		int list_length = sizeof(order_list)/sizeof(order_list[0])-1;
		int data_num_cg = sizeof(data_list)/sizeof(data_list[0])-1;
		double runtime_cg = 0.21 * (1+degrade[i]);

		cg(my_cache.set_size,my_cache.num_set,my_cache.line_length,order_list,list_length,data_list,size_list,data_num_cg,fit,runtime_cg);

		/* Nbody
		double theta_nb = 1;
		struct rand_in node     = { 97, 4950*theta_nb, 80, 1000, 97*4000*theta_nb };
		struct rand_in particle = { 80, 1200*theta_nb, 4, 1000, 80*1200*theta_nb };
		double runtime_nb = 2.27 * (1+degrade[i]);
		nbody(my_cache,node,particle,fit,runtime_nb);

		/* MG
		int data_size_mg = 46480*8;
		double runtime_mg = 1.10 * (1+degrade[i]);

		mg(my_cache,data_size_mg,fit,runtime_mg);

		/* FFT

		char filename[] = "C:\\Users\\LI\\Desktop\\ft.txt";
		int data_size_ft = 64 * 33 * 16;
		double runtime_ft = 0.31 * (1+degrade[i]);

		fft(my_cache,data_size_ft,filename,fit,runtime_ft);

		/* MC
		double theta_mc = 1;
		struct rand_in grid   = { 48, 768604*theta_mc, 1, 100, 36892992*theta_mc };
		struct rand_in energy = { 16, 768604*theta_mc, 1, 100, 12297664*theta_mc };
		double runtime_mc = 5.56 * (1+degrade[i]);
		mc(my_cache,grid,energy,fit,runtime_mc);

		for(j=0;j<5;j++)
		{
			printf( "%9.8f ",results[j]);
		}
		printf( "%9.8f\n",results[5]);

	}

	return 0;
}
*/
