/*
 * nbody.h
 *
 *  Created on: Mar 22, 2014
 *      Author: LI
 */

#ifndef NBODY_H_
#define NBODY_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Functions */
double power(double,int);
double combine(int,int);
double min(double,double);
void display(double[],int);
void update(double[],double[],int);
int get_refer_num(char,char[],int);
int get_idx_indatalist(char,char[],int);
int get_idx_inorderlist(char,int,char[],int);
int get_last_refer(char,char[],int);
int get_next_idx_in(char[],int);
int get_next_idx_out(char[],int);
int is_in_brac(char[],int,int);
int is_in_same(char[],int,int,char);
int get_sum_rest(int,char[],char[],int[],int);
double get_expected_left(double[],int);
//double get_acc_inithrefer(int,int,char,int,char[],int,char[],int[],int);
int get_acc_inithrefer(int,int,char,int,char[],int,char[],int[],int);

int exclusice_acc_expected(int,int,int);
int inclusice_acc_expected(int,int,int,int);
int exclusice_after(int,int,int);
int inclusice_after(int,int,int,int);

/* Models */
double stride_acc(struct cache,int,int,int,double);
void mm(struct cache,int[],int[],int,int[],double,double);

//void cg(int,int,int,char[],int,char[],int[],int,double,double);
double cg(int,int,int,char[],int,char[],int[],int,double,double);

double template_access(struct cache,int*,int);
void mg(struct cache,int,double,double);
void fft(struct cache,int,char[],double,double);

double random_access(struct cache,struct rand_in,double);
void nbody(struct cache,struct rand_in,struct rand_in,double,double);
void mc(struct cache,struct rand_in,struct rand_in,double,double);

/* Additions */
unsigned int get_num(char[],unsigned int);
void parser(int[],char[],unsigned int);


#endif /* NBODY_H_ */
