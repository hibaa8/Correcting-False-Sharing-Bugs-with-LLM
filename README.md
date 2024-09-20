Training LLM (Meta Llama 3.1) to detect and correct false sharing memory bugs in C code benchmarks

Methods used:
1) Add padding between the memory allocations 
2) Use align_alloc()
3) Restructure code storage arrangement

Training methods: prompt tuning + fine tuning (to be implemented)

Resources
False sharing: https://en.wikipedia.org/wiki/False_sharing
Starter benchmarks: https://github.com/efeslab/huron/tree/master/test_suites
Meta Llama Cookbook:  https://github.com/meta-llama/llama-recipes/blob/main/recipes
Guiding paper: https://dl.acm.org/doi/pdf/10.1145/3643651.3659892
