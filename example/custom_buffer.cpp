#include <ns_getopt.h>

int main() {
	const char* args[] = {"-t", "-k"};
	int args_size = sizeof(args) / sizeof(char*);

	std::vector<opt::argument> opt_args = {
		{"tommy", opt::type::no_arg, [](){}, "Tommy Lee.", 't'},
		{"kirk", opt::type::no_arg, [](){}, "James T. Kirk.", 'k'}
	};

	if(!opt::parse_arguments(args_size, args, opt_args)) {
		return -1;
	}

	std::cout << "Parsing arguments succeeded!" << std::endl;
	return 0;
}
