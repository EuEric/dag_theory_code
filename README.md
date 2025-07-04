I acknowledge the usage of Chat GPT 4o to write functionalities that read input `yaml` files or directories. These functions include `process_yaml_file` and `process_input_path` from `main.cpp`.

The functionality of this code is to generate **priority assignments** for Ray DAGs from input `yaml` DAG files.

# Usage

The whole program can be compiled by running `make` in the root folder of this project. Once this command is finished, this creates an executable in `bin/` called `main`. To run the compiled program run `./bin/main` (from root of project). To remove the compiled files, run the `make clean` command from the same root folder. In addition, the program takes the following input arguments:

- '-i' or '--input' (required): Specify input path (file or directory) for `yaml` files containing Ray DAGs.
- '-o' or '--output' (required): Specify a directory to same the generated priority `yaml` files.
- '--proc': Sepcify the number of processes that will be usd to run the Ray DAGs. If missing, this variable defaults to 2.

## Related code sources

- To generate Ray DAGs, use the code from https://github.com/EuEric/RD-Gen.
- To generate runnable Ray applications from Ray DAGs, use the code from https://github.com/EuEric/ray_code_generator.
- To compute RT bounds and extract them into `yaml` format, use the code from https://github.com/EuEric/DAG-scheduling.

