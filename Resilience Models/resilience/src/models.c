/*
 * models.c
 *
 *  Created on: Mar 22, 2014
 *      Author: LI
 */

#include "config.h"
#include "models.h"

double alph = 0.00098;
extern double results[];

unsigned int get_num(char filename[], unsigned int cl)
{
	FILE *ifp;
	ifp = fopen(filename,"r");
	int pos = 0;
	int count = 0;
	int pre = 0;
	while (!feof(ifp))
	{
		fscanf(ifp, "%d\n",&pos);
		int block = pos  * 16 / cl;
		if(block!=pre)
		{
			count ++;
//			printf( "%d\n",block);
		}
		pre = block;
	}
	return count;
}

void parser(int dst[], char filename[], unsigned int cl)
{
	FILE *ifp;
	ifp = fopen(filename,"r");
	int pos = 0;
	int count = 0;
	int pre = 0;
	while (!feof(ifp))
	{
		fscanf(ifp, "%d\n",&pos);
		int block = pos  * 16 / cl;
		if(block!=pre)
		{
			dst[count] = block;
			count ++;
//			printf( "%d\n",block);
		}
		pre = block;
	}
	return;
}

double combine(int n,int m)
{
	int i;
	double a = 1;
	double b = 1;
	for(i=0; i<m; i++)
	{
		a *= n - i;
		b *= m - i;
	}
	return a / b;
}

double power(double x, int n)
{
    int i;
    double mul = 1;
    for(i=1; i<=n; i++)
        mul *= x;
    return mul;
}

double min(double x, double y)
{
	double min = x;
	if(y<=x)
	{
		min = y;
	}
	return min;
}

double stride_acc(struct cache my_cache, int data_size, int num_elem, int stride, double ratio)
{
//	int elm_size = data_size / num_elem;
	int elm_size = 4;
	double num_acc = 0;
	if(my_cache.line_length<elm_size)
	{
		num_acc = (data_size / stride + 1) * (elm_size / my_cache.line_length + 1);
	}
	else
	{
		if(my_cache.line_length<=stride)
		{
			num_acc = data_size / stride + 1;
		}
		else
		{
			num_acc = data_size / my_cache.line_length + 1;
		}
	}
//	printf( "acc=:%f \n",num_acc);
	return num_acc;
}

void display(double prob_list[], int num)
{
	int i;
	for(i=0; i<num; i++)
	{
		printf( "%f \n",prob_list[i]);
	}
}

void update(double prob_dst[], double prob_src[], int num)
{
	int i;
	for(i=0; i<num; i++)
	{
		prob_dst[i] = prob_src[i];
	}
}

int get_refer_num(char data_structure, char order_list[], int list_length)
{
	int i, refer_num = 0;
	for(i=0; i<list_length; i++)
	{
		if(order_list[i]==data_structure)
		{
			refer_num += 1;
		}
	}
	return refer_num;
}

int get_idx_indatalist(char data_structure, char data_list[], int data_num)
{
	int i, idx = -1;
	for(i=0; i<data_num; i++)
	{
		if(data_list[i]==data_structure)
		{
			idx = i;
			break;
		}
	}
	return idx;
}

int get_idx_inorderlist(char data_structure, int refer_idx, char order_list[], int list_length)
{
	int i, idx = -1;
	for(i=0; i<list_length && idx<refer_idx; i++)
	{
		if(order_list[i]==data_structure)
		{
			idx += 1;
			if(idx==refer_idx)
			{
				return i;
			}
		}
	}
	return i;
}

int get_last_refer(char data_structure, char order_list[], int current_idx)
{
	int idx = current_idx;
	do
	{
		idx -= 1;
	} while(order_list[idx]!=data_structure && idx!=0);
	return idx;
}

int get_next_idx_in(char order_list[], int current_idx)
{
	int idx = current_idx;
	do
	{
		idx += 1;
	} while(order_list[idx]!=')');
	do
	{
		idx += 1;
	} while(order_list[idx]=='(');
	return idx;
}

int get_next_idx_out(char order_list[], int current_idx)
{
	int idx = current_idx;
	if(order_list[idx+1]!='(')
	{
		idx += 1;
	}
	else
	{
		idx += 2;
	}
	return idx;
}

int get_sum_rest(int current_idx, char order_list[], char data_list[], int size_list[], int data_num)
{
	int idx = current_idx;
	int sum_rest = 0;
	while(order_list[idx-1]!='(')
	{
		int idx_indatalist = get_idx_indatalist(order_list[idx-1],data_list,data_num);
		sum_rest += size_list[idx_indatalist];
		idx -= 1;
	}
	idx = current_idx;
	while(order_list[idx+1]!=')')
	{
		int idx_indatalist = get_idx_indatalist(order_list[idx+1],data_list,data_num);
		sum_rest += size_list[idx_indatalist];
		idx += 1;
	}
	return sum_rest;
}

double get_expected_left(double prob_list[], int set_size)
{
	int i;
	double expected_left = 0;
	for(i=0; i<=set_size; i++)
	{
//		printf( "Prob r=%d : %5.4f\n",i,prob_list[i]);
		expected_left += i * prob_list[i];
	}
	return expected_left;
}

int is_in_brac(char order_list[], int list_length, int current_idx)
{
	int i, is_in = 0;
	for(i=current_idx; i<list_length; i++)
	{
		if(order_list[i]=='(')
		{
			break;
		}
		if(order_list[i]==')')
		{
			is_in = 1;
			break;
		}
	}
	return is_in;
}

int is_in_same(char order_list[], int list_length, int current_idx, char data_structure)
{
	int is_in = 0;
	int idx = current_idx;
	while(order_list[idx]!='(')
	{
		idx -= 1;
		if(order_list[idx]==data_structure)
		{
			is_in = 1;
			break;
		}

	}
	idx = current_idx;
	while(order_list[idx]!=')')
	{
		idx += 1;
		if(order_list[idx]==data_structure)
		{
			is_in = 1;
			break;
		}
	}
	return is_in;
}

/* ***************************************************************************************************************************/

void prob_single(int set_size, int num_set, int data_length, double prob_list[])
{
	int i,j;
	for(i=0; i<set_size+1; i++)
	{
		prob_list[i] = 0.0;
		if(i<set_size)
		{
			if(i>data_length)
			{
				prob_list[i] = 0;
			}
			else
			{
				prob_list[i] = combine( data_length,i) * power(1/(double)num_set,i) * power(1-1/(double)num_set,data_length-i);
			}
//			printf( "Prob before x=%d: %9.8f\n",i,prob_list[i]);
		}
		else
		{
			for(j=set_size; j<data_length+1; j++)
			{
				double prob = combine(data_length,i) * power(1/(double)num_set,j) * power(1-1/(double)num_set,data_length-j);
				prob_list[i] += prob;
//				printf( "Prob before x=%d: %16.15f\n",data_length,prob);
			}
//			printf( "Prob before x=%d: %9.8f\n",i,prob_list[i]);
		}
	}
}

void prob_multiple(int set_size, int num_set, double prob_new[], double prob_x[], double prob_y[])
{
	int i,j,k,l;
	double total_combine[set_size+1][set_size+1];
	for(i=0; i<set_size+1; i++)
	{
		prob_new[i] = 0;
		for(j=0; j<set_size+1; j++)
		{
			total_combine[i][j] = 0;
			for(k=0; k<=i; k++)
			{
				for(l=0; l<=j; l++)
				{
					total_combine[i][j] += combine(i,k) * combine(j,l);
				}
			}
//		printf( "subtotal: %d %d %5.4f\n",i,j,total_combine[i][j]);
		}
	}
	for(i=0; i<set_size+1; i++)
	{
		for(j=0; j<set_size+1; j++)
		{
			for(k=0; k<=i; k++)
			{
				for(l=0; l<=j; l++)
				{
					double prob = combine(i,k) * combine(j,l) / total_combine[i][j] * prob_x[i] * prob_y[j];
					prob_new[k] += prob;
//					printf( "%d %d %d %d: %9.8f %9.8f\n",i,j,k,l,prob_x[k] * prob_y[l],prob);
				}
			}
		}
	}
	for(i=0; i<set_size+1; i++)
	{
//		printf( "Prob z=:%d %9.8f\n",i,prob_new[i]);
	}
}

void prob_after_ex(int set_size, int num_set, double prob_new[], double prob_x[], double prob_y[])
{
	int i,j,k;
	for(i=0; i<set_size+1; i++)
	{
		prob_new[i] = 0.0;
		for(j=0; j<set_size+1; j++)
		{
			for(k=0; k<set_size+1; k++)
			{
				if( j>=i && k>=(j-i) && set_size>=(i+k) )
				{
					prob_new[i] += combine(j,j-i) * combine(set_size-j,k-j+i) / combine(set_size,k) * prob_x[j] * prob_y[k];
				}
			}
		}
//		printf( "Prob %d %d %d: %5.4f\n",i,j,k,prob_new[i]);
	}
}

/* ***************************************************************************************************************************/

int exclusice_acc_expected(int set_size, int num_set, int data_length)
{
	if(data_length>set_size*num_set)
	{
		return set_size*num_set;
	}
	int i;
	double probs[set_size+1];
	double prob = 0;
	double expt = 0;
	for(i=0; i<set_size+1; i++)
	{
		probs[i] = 0.0;
		if(i<set_size)
		{
			if(i>data_length)
			{
				probs[i] = 0;
			}
			else
			{
				probs[i] = combine( data_length,i) * power(1/(double)num_set,i) * power(1-1/(double)num_set,data_length-i);
			}
			prob += probs[i];
			expt += probs[i] * i;
//			printf( "Prob x=%d: %9.8f\n",i,probs[i]);
		}
		else
		{
			probs[i] = 1 - prob;
			expt += probs[i] * i;
//			printf( "Prob x=%d: %9.8f\n",i,probs[i]);
		}
	}
	return (int)ceil(expt);
}

int inclusice_acc_expected(int set_size, int num_set, int x, int y)
{
	int expt;
	int sum = x + y;
	double rx = (double)x / sum;
	if(sum>set_size)
	{
		expt = (int)ceil(set_size*rx);
	}
	else
	{
		expt = x;
	}
	return expt;
}

int exclusice_after(int set_size, int x, int y)
{
	int expt = 0;
	int sum = x + y;
	if(sum>set_size)
	{
		expt = set_size - y;
	}
	else
	{
		expt = x;
	}
//	printf( "expt=%d\n",expt);
	return expt;
}

int inclusice_after(int set_size, int x, int y, int e)
{
	int i, maxi = 0;
	double expt = 0;
	int avlc = set_size - e;

	if(x>set_size)
	{
		maxi = set_size + 1;
	}
	else
	{
		maxi = x + 1;
	}
	double probs[maxi];

	if(y<=avlc)
	{
		expt = x;
	}
	else
	{
		for(i=0; i<maxi; i++)
		{
			probs[i] = 0.0;
			if( x>=i && (y-avlc)>=(x-i) && e>=(y-avlc+i) )
			{
				probs[i] = combine(x,x-i) * combine(e-x,y-avlc-x+i) / combine(e,y-avlc);
				expt += probs[i] * i;
//				printf( "Prob x=%d: %f\n",i,probs[i]);
			}
		}
	}
	return (int)ceil(expt);
}

int get_acc_inithrefer(int set_size, int num_set, char data_structure, int refer_idx, char order_list[], int list_length, char data_list[], int size_list[], int data_num)
{
	int num_acc = 0;
	int x = 0;
	int y = 0;
	int e = 0;
	int own_size = size_list[get_idx_indatalist(data_structure,data_list,data_num)];
	int current_idx = get_idx_inorderlist(data_structure,refer_idx,order_list,list_length);

	int last_refer = get_last_refer(data_structure,order_list,current_idx);
	int idx = last_refer;

	if(refer_idx==0)
	{
		return own_size;
	}

	if(is_in_brac(order_list,list_length,last_refer)==0)
	{
		x = exclusice_acc_expected(set_size,num_set,own_size);
		idx = get_next_idx_out(order_list,last_refer);

//		printf( "x=%d\n",x);
	}
	else
	{
		x = exclusice_acc_expected(set_size,num_set,own_size);
		int y_size = get_sum_rest(last_refer,order_list,data_list,size_list,data_num);
		y = exclusice_acc_expected(set_size,num_set,y_size);
		e = exclusice_acc_expected(set_size,num_set,own_size+y_size);
		x = inclusice_acc_expected(set_size,num_set,x,y);
		idx = get_next_idx_in(order_list,last_refer);

//		printf( "x=%d\n",x);
	}

	int sum_size = 0;
	while(order_list[idx]!=data_structure)
	{
		sum_size += size_list[get_idx_indatalist(order_list[idx],data_list,data_num)];
		if(is_in_brac(order_list,list_length,idx)==0)
		{
			idx = get_next_idx_out(order_list,idx);
		}
		else
		{
			idx = get_next_idx_in(order_list,idx);
		}
	}

	if(is_in_brac(order_list,list_length,last_refer)==0)
	{
		y = exclusice_acc_expected(set_size,num_set,sum_size);
		x = exclusice_after(set_size,x,y);
//		printf( "x=%d\n",x);
	}
	else
	{
		y = exclusice_acc_expected(set_size,num_set,sum_size);
		x = inclusice_after(set_size,x,y,e);
//		printf( "x=%d\n",x);
	}

	num_acc = own_size - x * num_set;

//	printf( "x=%d\n",x);

	if(num_acc<0)
	{
		num_acc = 0;
	}

	return num_acc;
}



/* ***************************************************************************************************************************/

double template_access(struct cache my_cache, int *template, int length)
{
	int i;
	double num_acc = 0;
	for(i=0; i<length; i++)
	{
		int block = template[i];
		int p_idx = i-1;
		while(p_idx>0 && template[p_idx]!=block)
		{
			p_idx -= 1;
		}
		if(p_idx==0)
		{
			if(template[p_idx]!=block)
			{
				num_acc += 1;
			}
			else
			{
				if(i>my_cache.num_block)
				{
					num_acc += 1;
				}
			}
		}
		else
		{
			if(i-p_idx>my_cache.num_block)
			{
				num_acc += 1;
			}
		}
	}
	return num_acc;
}

double random_access(struct cache my_cache, struct rand_in my_input, double ratio)
{
	double expt_elm_notin = 0;
	double expt_block_notin = 0;
	double expt_reload = 0;
	double complusary = my_input.elm_size * my_input.elm_numb / my_cache.line_length;
	double CBs = my_cache.num_block * ratio;
	double CC  = my_cache.cache_capacity * ratio;
	double block_notin = my_input.elm_size * my_input.elm_numb / my_cache.line_length - CBs;
	double p_in = CC / (my_input.elm_size * my_input.elm_numb);
	if(p_in<1)
	{
		expt_elm_notin = my_input.num_refs * ( 1 - p_in );
		if(my_input.elm_size>my_cache.line_length)
		{
			expt_block_notin = my_input.elm_size * expt_elm_notin / my_cache.line_length;
		}
		else
		{
			expt_block_notin = expt_elm_notin;
		}
		expt_reload = min(block_notin,expt_block_notin);
	}
	return complusary + expt_reload * my_input.num_iter;
//	return expt_reload * my_input.num_iter;
}

void mm(struct cache my_cache, int data_sizes[], int strides[], int data_num, int num_elements[], double fit, double runtime)
{
//	printf( "Modeling MM Algorithm ...\n\n");
	int i;
	double sum_size = 0;
	double sum_dvf = 0;
	for(i=0; i<data_num; i++)
	{
		sum_size += data_sizes[i];
	}
	for(i=0; i<data_num; i++)
	{
		double num_acc = stride_acc(my_cache,data_sizes[i],num_elements[i],strides[i],data_sizes[i]/sum_size);
		double num_fail_i = data_sizes[i] * fit * runtime;
		double DVF_i = num_acc * num_fail_i;
		sum_dvf += DVF_i;
		double dvf_norm = DVF_i/data_sizes[i]/alph/runtime;
		printf( "Number of Access to %d: %f\n",i,num_acc);
		printf( "Number of Failures on %d: %f\n",i,num_fail_i);
//		printf( "DVF for %d (/MB): %17.16f (%f)\n",i,DVF_i,DVF_i/(data_sizes[i]*my_cache.line_length)/alph);
//		printf( "%f ",DVF_i);
//		printf( "%f ",dvf_norm);
	}
//	printf( "%f\n",sum_dvf);
//	printf( "%f ",sum_dvf/sum_size/alph/runtime);
//	printf( "DVF for MM: %17.16f\n\n", sum_dvf);
//	printf( "Normalized DVF for MM: %17.16f\n\n", sum_dvf/(sum_size*my_cache.line_length)/alph);
	results[0] = sum_dvf/sum_size/alph/runtime;
}

double cg(int set_size, int num_set, int line_length, char order_list[], int list_length, char data_list[], int size_list[], int data_num, double fit, double runtime)
{
//	printf( "Modeling CG Algorithm ...\n\n");
	int i,j;
	double sum_dvf = 0;
	double sum_size = 0;
	for(i=0; i<data_num; i++)
	{
		sum_size += size_list[i];
	}
	for(i=0; i<data_num; i++)
	{
		char data_structure = data_list[i];
		int refer_num = get_refer_num(data_structure,order_list,list_length);
		double num_acc = 0;
		for(j=0; j<refer_num; j++)
		{
			int ith_acc = get_acc_inithrefer(set_size,num_set,data_structure,j,order_list,list_length,data_list,size_list,data_num);
//			printf( "Number of Accesses in %dth Reference for %c: %d\n",j+1,data_structure,ith_acc);
			num_acc += ith_acc;
		}
		num_acc *= 1;
		double num_fail_i = size_list[i] * line_length * fit * runtime;
		double DVF_i = num_acc * num_fail_i;
		sum_dvf += DVF_i;
		double dvf_norm = DVF_i/(size_list[i]*line_length)/alph/runtime;
//		printf( "Number of Access to %c: %f\n",data_structure,num_acc);
//		printf( "Number of Failures on %c: %17.16f\n",data_structure,num_fail_i);
//		printf( "DVF for %c (/MB): %17.16f (%f)\n",data_structure,DVF_i,DVF_i/size_list[i]/alph);
//		printf( "%f ",DVF_i);
//		printf( "%f ",dvf_norm);
	}
//	printf( "%f\n",sum_dvf);
//	printf( "%f ",sum_dvf/(sum_size*line_length)/alph/runtime);
//	printf( "DVF for CG: %17.16f\n\n", sum_dvf);
//	printf( "Normalized DVF for CG: %17.16f\n\n", sum_dvf/sum_size/alph);
	results[1] = sum_dvf/(sum_size*line_length)/alph/runtime;
	return sum_dvf;
}

void nbody(struct cache my_cache, struct rand_in node, struct rand_in particle, double fit, double runtime)
{
//	printf( "Modeling Nbody Algorithm ...\n\n");
	double ratio = node.elm_size / (double)(node.elm_size+particle.elm_size);
	double sum_size = node.footprint + particle.footprint;

	double num_acc_node = random_access(my_cache,node,ratio);
	double num_fail_node = node.footprint * fit * runtime;
	double DVF_node = num_acc_node * num_fail_node;
	printf( "Number of Access to QuadTree: %f\n",num_acc_node);
	printf( "Number of Failures on QuadTree: %f\n",num_fail_node);
	printf( "DVF for QuadTree (/MB): %17.16f (%f)\n",DVF_node,DVF_node/node.footprint/alph);
//	printf( "%f ",DVF_node);
//	printf( "%f ",DVF_node/node.footprint/alph/runtime);

	double num_acc_part = random_access(my_cache,particle,1-ratio);
	double num_fail_part = particle.footprint * fit * runtime;
	double DVF_part = num_acc_part * num_fail_part;
	printf( "Number of Access to Particle: %f\n",num_acc_part);
	printf( "Number of Failures on Particle: %f\n",num_fail_part);
	printf( "DVF for Particle (/MB): %17.16f (%f)\n\n",DVF_part,DVF_part/particle.footprint/alph);
//	printf( "%f ",DVF_part);
//	printf( "%f ",DVF_part/particle.footprint/alph/runtime);

//	printf( "%f\n",DVF_node+DVF_part);
//	printf( "%f ",(DVF_node+DVF_part)/sum_size/alph/runtime);

	printf( "DVF for Nody: %17.16f\n\n", DVF_node+DVF_part);
	printf( "Normalized DVF for NBody: %17.16f\n\n", (DVF_node+DVF_part)/sum_size/alph);
	results[2] = (DVF_node+DVF_part)/sum_size/alph/runtime;
}

void mg(struct cache my_cache, int data_size, double fit, double runtime)
{
//	printf( "Modeling MG Algorithm ...\n\n");
	int i,j;
	int num_blk = data_size / my_cache.line_length;
	int init_a[3] = {4,0,64};
	int max_a = num_blk - init_a[2];
	int *temp_a = malloc(3*max_a*sizeof(int));

	for(i=0; i<max_a; i++)
	{
		for(j=0; j<3; j++)
		{
			temp_a[3*i+j] = init_a[j] + i;
//			printf( "%d\n",max_a);
		}
	}

	double it = 1.0;
	if(data_size>my_cache.cache_capacity)
	{
		it = 25;
	}

	double num_acc = it*template_access(my_cache,temp_a,3*max_a);
	double num_fail = data_size * fit * runtime;
	double DVF = num_acc * num_fail;
//	printf( "Number of Access to MG: %f\n",num_acc);
//	printf( "Number of Failures on MG: %f\n",num_fail);
//	printf( "%f ",DVF);
//	printf( "%f\n",DVF);
//	printf( "DVF for MG (/MB): %17.16f (%f)\n\n",DVF,DVF/data_size/alph);

	printf( "%f %f ",DVF/data_size/alph/runtime,DVF/data_size/alph/runtime);

//	printf( "Normalized DVF for MG: %17.16f\n\n", DVF/data_size/alph);
	results[3] = DVF/data_size/alph/runtime;
}

void fft(struct cache my_cache, int data_size, char filename[], double fit, double runtime)
{
//	printf( "Modeling FFT Algorithm ...\n\n");

	unsigned int num_temp = get_num(filename,my_cache.line_length);
	int *temp = malloc(num_temp*sizeof(int));
	parser(temp,filename,my_cache.line_length);

	double it = 1.0;
	if(data_size>my_cache.cache_capacity)
	{
		it = 15;
	}

	double num_acc = it*template_access(my_cache,temp,num_temp);
	double num_fail = data_size * fit * runtime;
	double DVF = num_acc * num_fail;
//	printf( "Number of Access to FFT: %f\n",num_acc);
//	printf( "Number of Failures on FFT: %17.16f\n",num_fail);
//	printf( "%f ",DVF);
//	printf( "%f\n",DVF);
//	printf( "DVF for FFT (/MB): %17.16f (%17.16f)\n\n",DVF,DVF/data_size/alph);

//	printf( "%f %f ",DVF/data_size/alph/runtime,DVF/data_size/alph/runtime);

//	printf( "Normalized DVF for FFT: %17.16f\n\n", DVF/data_size/alph);
	results[4] = DVF/data_size/alph/runtime;
}

void mc(struct cache my_cache, struct rand_in grid, struct rand_in energy, double fit, double runtime)
{
//	printf( "Modeling MC Algorithm ...\n\n");
	double ratio = 1.0;
	double sum_size = grid.footprint + energy.footprint;

	double num_acc_grid = random_access(my_cache,grid,ratio);
	double num_fail_grid = grid.footprint * fit * runtime;
	double DVF_grid = num_acc_grid * num_fail_grid;
	printf( "Number of Access to Grid: %f\n",num_acc_grid);
	printf( "Number of Failures on Grid: %f\n",num_fail_grid);
	printf( "DVF for Grid (/MB): %17.16f (%f)\n",DVF_grid,DVF_grid/grid.footprint/alph);
//	printf( "%f ",DVF_grid);

//	printf( "%f ",DVF_grid/grid.footprint/alph/runtime);

	double num_acc_energy = random_access(my_cache,energy,ratio);
	double num_fail_energy = energy.footprint * fit * runtime;
	double DVF_energy = num_acc_energy * num_fail_energy;
	printf( "Number of Access to Energy: %f\n",num_acc_energy);
	printf( "Number of Failures on Energy: %f\n",num_fail_energy);
	printf( "DVF for Energy (/MB): %17.16f (%f)\n\n",DVF_energy,DVF_energy/energy.footprint/alph);
//	printf( "%f ",DVF_energy);
//	printf( "%f\n",DVF_grid+DVF_energy);

//	printf( "%f ",DVF_energy/energy.footprint/alph/runtime);

	printf( "DVF for MC: %17.16f\n\n", DVF_grid+DVF_energy);
	printf( "Normalized DVF for MC: %17.16f\n\n", (DVF_grid+DVF_energy)/sum_size/alph);

//	printf( "%f ",(DVF_grid+DVF_energy)/sum_size/alph/runtime);

	results[5] = (DVF_grid+DVF_energy)/sum_size/alph/runtime;
}

/*void cg(int set_size, int num_set, int line_length, char order_list[], int list_length, char data_list[], int size_list[], int data_num, double fit, double runtime)
{
	printf( "Modeling CG Algorithm ...\n\n");
	int i,j;
	double sum_dvf = 0;
	double sum_size = 0;
	for(i=0; i<data_num; i++)
	{
		sum_size += size_list[i];
	}
	for(i=0; i<data_num; i++)
	{
		char data_structure = data_list[i];
		int refer_num = get_refer_num(data_structure,order_list,list_length);
		double num_acc = 0;
		for(j=0; j<refer_num; j++)
		{
			int ith_acc = get_acc_inithrefer(set_size,num_set,data_structure,j,order_list,list_length,data_list,size_list,data_num);
			printf( "Number of Accesses in %dth Reference for %c: %d\n",j+1,data_structure,ith_acc);
			num_acc += ith_acc;
		}
		num_acc *= 10;
		double num_fail_i = size_list[i] * line_length * fit * runtime;
		double DVF_i = num_acc * num_fail_i;
		sum_dvf += DVF_i;
		printf( "Number of Access to %c: %f\n",data_structure,num_acc);
		printf( "Number of Failures on %c: %17.16f\n",data_structure,num_fail_i);
		printf( "DVF for %c (/MB): %17.16f (%f)\n",data_structure,DVF_i,DVF_i/sum_size/alph);
	}
	printf( "Normalized DVF for CG: %17.16f\n\n", sum_dvf/sum_size/alph);
	results[2] = sum_dvf/sum_size/alph;
}*/

/*double get_acc_inithrefer(int set_size, int num_set, char data_structure, int refer_idx, char order_list[], int list_length, char data_list[], int size_list[], int data_num)
{
	double num_acc = 0;
	double num_left = 0;
	double prob_x[set_size+1];
	double prob_y[set_size+1];
	double prob_n[set_size+1];
	int own_size = size_list[get_idx_indatalist(data_structure,data_list,data_num)];
	int current_idx = get_idx_inorderlist(data_structure,refer_idx,order_list,list_length);
	if(refer_idx==0)
	{
		num_acc = (double)own_size;
	}
	else
	{
		int last_refer = get_last_refer(data_structure,order_list,current_idx);
		int idx = last_refer;
		if(is_in_brac(order_list,list_length,last_refer)==0)
		{
//			printf( "Single Before ... \n");
			prob_single(set_size,num_set,own_size,prob_x);
			idx = get_next_idx_out(order_list,last_refer);
			update(prob_n,prob_x,set_size+1);
//			display(prob_n,set_size+1);
		}
		else
		{
//			printf( "Multiple Before ... \n");
			prob_single(set_size,num_set,own_size,prob_x);
			int y_size = get_sum_rest(last_refer,order_list,data_list,size_list,data_num);
			prob_single(set_size,num_set,y_size,prob_y);
			prob_multiple(set_size,num_set,prob_n,prob_x,prob_y);
			idx = get_next_idx_in(order_list,last_refer);
		}
		while(order_list[idx]!=data_structure)
		{
			if(is_in_brac(order_list,list_length,idx)==1)
			{
				if(is_in_same(order_list,list_length,idx,data_structure)==1)
				{
//				    printf( "Get the expected number and break ... \n");
					num_left = get_expected_left(prob_n,set_size);
					int sum_rest = get_sum_rest(idx,order_list,data_list,size_list,data_num);
					double ratio = own_size / (double)(own_size+sum_rest);
					num_acc += own_size - num_left * ratio * num_set;
//					display(prob_n,set_size+1);
					return num_acc;
				}
				else
				{
//					printf( "Get the sum and probability after ... \n");
					int sum_rest = get_sum_rest(idx,order_list,data_list,size_list,data_num);
					prob_single(set_size,num_set,sum_rest,prob_y);
					update(prob_x,prob_n,set_size+1);
					prob_after_ex(set_size,num_set,prob_n,prob_x,prob_y);
					idx = get_next_idx_in(order_list,idx);
				}
			}
			else
			{
//				printf( "Get the exclusive probability after ... \n");
				int y_size = size_list[get_idx_indatalist(order_list[idx],data_list,data_num)];
				prob_single(set_size,num_set,y_size,prob_y);
				update(prob_x,prob_n,set_size+1);
				prob_after_ex(set_size,num_set,prob_n,prob_x,prob_y);
//				printf( "idx %d \n",idx);
			    idx = get_next_idx_out(order_list,idx);
//			    printf( "idx %d \n",idx);
			}

		}

		num_left = get_expected_left(prob_n,set_size);

		if(is_in_brac(order_list,list_length,idx)==1)
		{
			int sum_rest = get_sum_rest(idx,order_list,data_list,size_list,data_num);
			double ratio = own_size / (double)(own_size+sum_rest);
			num_acc += own_size - num_left * ratio * num_set;
		}
		else
		{
			num_acc += own_size - num_left * num_set;
		}
	}
	return num_acc;
}*/
