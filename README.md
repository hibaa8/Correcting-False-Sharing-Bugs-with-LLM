## False Sharing Memory Bug Detection with LLM Static Code Analysis
Training LLM to detect and correct false sharing memory bugs in C code benchmarks

##### Navigation
- prompts/: contains the iterations of the prompt. Each prompt is stored in a separate file.
- src/: contains the prompt engineering notebook where working prompts are tested and the ts_fs_benchmarks notebook where all 12 benchmarks are with the current prompt.
- test_suites/: contains the true and false sharing benchmarks currently being used to access the model's performance.
    - shell_scripts/: scripts written to automate the runtime and memory overhead evaluation of  LLM-corrected code and baseline benchmark code.

##### Technical Details:
- All benchmarks are written in C or C++. 
- Groq is used to access the open source Meta LLama 3.1 Instruct 70B model for testing.

##### Resources:
False sharing: https://en.wikipedia.org/wiki/False_sharing

Meta Llama Cookbook:  https://github.com/meta-llama/llama-recipes/blob/main/recipes

Guiding paper: https://dl.acm.org/doi/pdf/10.1145/3643651.3659892

Benchmarks source: https://github.com/efeslab/huron/tree/master/test_suites
