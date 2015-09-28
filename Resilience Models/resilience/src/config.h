/*
 * config.h
 *
 *  Created on: Mar 22, 2014
 *      Author: LI
 */

#ifndef CONFIG_H_
#define CONFIG_H_

struct cache {
    unsigned int set_size;
    unsigned int num_set;
    unsigned int line_length;
    unsigned int num_block;
    unsigned int cache_capacity;
};

struct rand_in {
	unsigned int elm_size;
	unsigned int elm_numb;
	unsigned int num_refs;
	unsigned int num_iter;
	unsigned int footprint;
};

#endif
