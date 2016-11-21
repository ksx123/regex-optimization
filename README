+=============================================================================+
|  RegEx Processor - by Michela Becchi, Washington University of Saint Louis  |
+=============================================================================+

The code includes:

    * NFA and DFA generation algorithms for sets of regular expressions
    * DFA minimization (J. Hopcroft, "An nlogn algorithm for minimizing states in a finite automaton," in Theory of Machines and Computation, J. Kohavi, Ed. New York: Academic, 1971, pp. 189-196)
    * Export/import of DFA to/from text format
    * Export of NFA and DFA into DOT format for pictorial representation (http://www.graphviz.org/)
    * NFA transformation algorithms
          o epsilon transition removal
          o state reduction 
    * Default transition compression algorithms for DFA:
          o Sailesh Kumar, Sarang Dharmapurikar, Fang Yu, Patrick Crowley and Jonathan Turner, "Algorithms to Accelerate Multiple Regular Expressions Matching for Deep Packet Inspection," Proceedings of ACM SIGCOMM'06, Pisa, Italy, September 12-15, 2006.
          o Michela Becchi and Patrick Crowley, "An Improved Algorithm to Accelerate Regular Expression Evaluation", Proceedings of the 2007 ACM/IEEE Symposium on Architectures for Networking and Communications Systems (ANCS), Orlando, FL, December, 2007. 
    * Hybrid-FA generation algorithm, as described in:
          Michela Becchi and Patrick Crowley, "A Hybrid Finite Automaton for Practical Deep Packet Inspection", In Proceedings of the International Conference on emerging Networking EXperiments and Technologies (CoNEXT), New York, NY, December, 2007. 
    * DFA, NFA and Hybrid-FA traversal routines
    * Regular expression workload consisting of:
          o a synthetic regular expression generator
          o a traffic trace generator
          o a memory layout generator (implementing different memory encoding schemes)
          o a memory/cache simulator 
    In particular, the workload is described in:
        Michela Becchi, Mark Franklin and Patrick Crowley, "A Workload for Evaluating Deep Packet Inspection Architectures", In Proceedings of the 2008 IEEE International Symposium on Worload Characterization (IISWC), Seattle , WA, September, 2008. 

Build Information
=================

The code can be compiled and built using the provided makefile. 
Targets:
- all: builds the whole code (default)
- clean: deletes all object files and executable
- main: builds only the main DFA/NFA/Hybrid-FA processing code
- main_regexgen: builds only the synthetic regular expression generator
- main_tracegen: builds only the traffic trace generator
- main_nfa: builds only the NFA related workload
- main_dfas: builds only the DFA related workload

The code has been compiled under cygwin using gcc 3.4.4, and under linux versions 2.6.9 and 2.6.18 using gcc 3.4.6 and 4.1.1 respectively.

Usage
=====

Call each program without target in order to see its usage:

- regex: DFA/NFA/HYBRID-FA generator
- regex_gen: synthetic regular expression generator (note: an example of token-file is provided in the config folder)
- regex_tracegen: synthetic trace generator
- regex_nfa/regex_dfas: NFA/DFA based workload

In particular:

> ./regex

Usage: regex [options]
             [--parse|-p <regex_file> [--m|--i] | --import|-i <in_file> ]
             [--export|-e  <out_file>][--graph|-g <dot_file>]
             [--trace|-t <trace_file>]
             [--hfa]

Options:
    --help,-h       print this message
    --version,-r    print version number
    --verbose,-v    basic verbosity level 
    --debug,  -d    enhanced verbosity level 

Other:
    --parse,-p <regex_file>  process regex file
    --m,--i  m modifier, ignore case
    --import,-i <in_file>    import DFA from file
    --export,-e <out_file>   export DFA to file
    --graph,-g <dot_file>    export DFA in DOT format into specified file
    --trace,-t <trace_file>  trace file to be processed
    --hfa                    builds a hybrid-FA


For convenience, some files containing sets of regular expressions have been provided in the /ruleset folder.
NOTE: main.c has a place-holder in the main() function where the user can add custom code (@see dfa.h, nfa.h, hybrid_fa.h) 


Observations and Suggestions
============================

(1) NFA reduction can be configured with the COMMON_TARGET_REDUCTION and HYBRID_FA_REDUCTION variables in stdinc.h

(2) For large DFAs, DFA generation may require considerable amount of time. It is suggested to export DFAs to be used frequently through the --export option

(3) As explained in 

Michela Becchi and Patrick Crowley, "A Hybrid Finite Automaton for Practical Deep Packet Inspection",  in Proceedings of the International Conference on emerging Networking EXperiments and Technologies (CoNEXT), New York, NY, December, 2007

some regular expression sub-patterns may lead to state explosion during DFA generation. Depending on the memory resource constraints your system has, this may lead to *out-of-memory* situations when creating the DFA. 

You may want to use the --debug option to monitor the size of the DFA you are building. In case of state blow-up, the code is configured to generate multiple DFAs. In particular, MAX_DFA_SIZE defined in stdinc.h defines the upper bound to the size of each created DFA (beyond this threshold, rule partitioning into multiple DFAs is performed).

(4) You may build a hibrid-FA by using the --hfa option. The hybrid-FA generation is guided by settings the global variables HYBRID_FA_REDUCTION, SPECIAL_MIN_DEPTH, MAX_TX, MAX_HEAD_SIZE, TAIL_DFAS defined in stdinc.h

(5) The DFA class provides the implementation of compression algorithms proposed in
 
+ Sailesh Kumar, Sarang Dharmapurikar, Fang Yu, Patrick Crowley and Jonathan Turner, "Algorithms to Accelerate Multiple Regular Expressions Matching for Deep Packet Inspection," Proceedings of ACM SIGCOMM'06, Pisa, Italy, September 12-15, 2006.

+ Michela Becchi and Patrick Crowley, "An Improved Algorithm to Accelerate Regular Expression Evaluation", Proceedings of the 2007 ACM/IEEE Symposium on Architectures for Networking and Communications Systems (ANCS), Orlando, FL, December, 2007.

Please, consider the space and time complexity analysis detailed in the latter paper (it has important practical implications).
As a matter of fact, compressing a DFA with the second algorithm is very fast and may take a few minutes on DFAs of the order of 30,000 nodes. The D2FA algorithm may take several hours. If the space reduction graph is dense, then you may need to run the algorithm incrementally through several DFA scans (see DECREMENT variable).

Code Organization 
=================
(Please, refers to the code comments for specific information)

Main classes
------------

+ nfa.h/nfa.c

Implementation of a non-deterministic finite automaton (NFA).
This class provides:
 - methods to build a NFA by adding transitions, concatenating and or-ing NFAs, copying NFAs, etc.
 - function to transform a NFA into the equivalent without epsilon transitions
 - function to reduce a NFA by collapsing equivalent sub-NFAs
 - function to transform a NFA into DFA (subset construction procedure)  
 - code to export the NFA into format suitable for dot program (http://www.graphviz.org/)

+ dfa.h/dfa.c

Implementation of a deterministic finite automaton (DFA).
The class provides:
- getter and setter methods to access important DFA information
- O(nlogn) minimization algorithm
- code to export and import DFA from textual representation
- code to export the DFA in format suitable for dot program (http://www.graphviz.org/)
- code to compress the DFA through default transitions. Algorithms described in the following papers are provided:

+ Sailesh Kumar, Sarang Dharmapurikar, Fang Yu, Patrick Crowley and Jonathan Turner, "Algorithms to Accelerate Multiple Regular Expressions Matching for Deep Packet Inspection," Proceedings of ACM SIGCOMM'06, Pisa, Italy, September 12-15, 2006.

+ Michela Becchi and Patrick Crowley, "An Improved Algorithm to Accelerate Regular Expression Evaluation", Proceedings of the 2007 ACM/IEEE Symposium on Architectures for Networking and Communications Systems (ANCS), Orlando, FL, December, 2007.

+ hybrid_fa.h/c

Implementation of hybrid-FA as described in the following paper:

Michela Becchi and Patrick Crowley, "A Hybrid Finite Automaton for Practical Deep Packet Inspection",  in Proceedings of the International Conference on emerging Networking EXperiments and Technologies (CoNEXT), New York, NY, December, 2007

+ trace.h/c

Code implementing the input text processing for a given automaton.

+ main.c

Program entry point.
Please modify the main() function to add custom code (e.g.: statistics, calls to compression routines, etc.)
The options allow to create a NFA/DFA from a list of regular expressions. Additionally, the DFA can be exported in proprietary format for later re-use, and be imported. Moreover, export to DOT format (http://www.graphviz.org/) is possible.
Finally, processing a trace file is an option.

+ main_regexgen.c

Synthetic regular expression generator entrypoint - please edit the configuration in the file if necessary

+ main_tracegen.c

Synthetic trace generator

+ main_nfa.c/main_dfas.c

Entrypoint for NFA-DFA based workload

Utilility classes
-----------------

+ stdinc.h/c

Defines broad scope include files, constant definitions and variables.

+ parser.h/c

Implements parser routines allowing compiling a set of regular expressions in a specified file into a NFA.

+ int_set.h/c, linked_set.h/c

Implement sets of integers.

+ partition.h/c, wgraph.h/c

Implement the partition data structures and the undirected graph needed for the D2FA compression code.

+ subset.h/c

Data structure needed during subset construction and NFA compression.

+ dheap.h/c

Implements the D-Heap data structure needed in several algorithm.

+ cache.h/c

Implements cache simulator

+ fa_memory.h/c, dfas_memory.h/c

Encode NFA/DFAs and produce corresponding memory layout

+ reg_ex.h

Data structure for regular expression generation
