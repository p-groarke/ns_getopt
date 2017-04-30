#include <ns_getopt.h>

int main() {
	struct tk {
		bool t = false;
		bool k = false;
	} tk_args;

	const char* args[] = {"program_name", "-t", "-k"};
	int args_size = sizeof(args) / sizeof(char*);

	std::vector<opt::argument> opt_args = {
		{"tommy", opt::type::no_arg, [&tk_args](){ tk_args.t = true; }, "Tommy Lee.", 't'},
		{"kirk", opt::type::no_arg, [&tk_args](){ tk_args.k = true; }, "James T. Kirk.", 'k'}
	};

	opt::options o = {"A wonderful example.\nTalented Author\n"
		, "More info on github.\n", opt::DEFAULT_FLAGS
		, [](std::string&& s) { std::cout << s << std::endl; }
	};

	if(!opt::parse_arguments(args_size, args, opt_args)) {
		return -1;
	}

	std::cout << "t = " << tk_args.t << ", k = " << tk_args.k << std::endl;
	return 0;
}
