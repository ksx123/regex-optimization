/*
 * Copyright (c) 2007 Michela Becchi and Washington University in St. Louis.
 * All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. The name of the author or Washington University may not be used
 *       to endorse or promote products derived from this source code
 *       without specific prior written permission.
 *    4. Conditions of any other entities that contributed to this are also
 *       met. If a copyright notice is present from another entity, it must
 *       be maintained in redistributions of the source code.
 *
 * THIS INTELLECTUAL PROPERTY (WHICH MAY INCLUDE BUT IS NOT LIMITED TO SOFTWARE,
 * FIRMWARE, VHDL, etc) IS PROVIDED BY  THE AUTHOR AND WASHINGTON UNIVERSITY
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR WASHINGTON UNIVERSITY
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS INTELLECTUAL PROPERTY, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * */

/*
 * File:   regex.h
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory 
 *
 * Description:
 *
 * 		Provides the data structure encapsulating the information needed for generating
 * 		synthetics regular expression sets.
 */

#ifndef REGEX_H_
#define REGEX_H_

#include "stdinc.h"
  
typedef struct _parameters{
	//RegEx length
	unsigned min_length;
	unsigned max_length;
	unsigned avg_length;
	//sub-pattern length
	unsigned min_sp_length;
	unsigned max_sp_length;
	unsigned avg_sp_length;
	//multiple characters
	float special_range[6]; //frequency of \s \d \w \S \D \W
	float range; 		    //frequency of character ranges
	float single_wildcard;  //frequency of wildcards
	//repetitions
	float simple_repetitions;   //frequency of character repetitions
	float special_range_rep[6]; //frequency of \s+ \d+ \w+ \S+ \D+ \W+
	float range_rep;            //frequency of character range repetitions
	float dot_star;             //frequency of .*
	float not_newline_star;     //frequency of [^\n\r]*
	float pattern_rep;          //frequency of sub-pattern repetitions
	//counting constraints
	float simple_counting;      //frequency of simple character counting constraints 
	float range_counting;       //frequency of counting constraints on character ranges
	float pattern_counting;     //frequency of character ranges on sub-patterns
	float counting;             //frequency of counting constraints of .{n}, [^\n\r]{}
	//disjunctions
	float or_expr;              //frequency of disjuctions
} pattern_parameter_t;  
 
class regex{
	
	//number of regular expressions
	unsigned num_regex;
	
	//list of long exact-match sub-patterns
	char **long_tokens;
	
	//list of short exact-match sub-patterns
	char **short_tokens;
	
	//number of long exact-match sub-patterns
	int num_long_tokens;
	
	//number of short exact-match sub-patterns
	int num_short_tokens;
	
	//parameters
	pattern_parameter_t parameters;
	
public:
	
	//instantiates the regex
	regex(unsigned n_regex, int seed);
	
	//destructor
	~regex();
	
	/* read the file containing the tokens (exact-match sub-patterns) */
	void read_tokens(FILE *file);

	// build the regular-expression set
	void build_re(FILE *output);

	// modify the number of regex to be created
	void set_num_regex(unsigned n_regex);
	
};

inline void regex::set_num_regex(unsigned n_regex){num_regex=n_regex;}

#endif /*REGEX_H_*/
