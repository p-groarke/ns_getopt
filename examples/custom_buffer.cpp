#include <iostream>
#include <ns_getopt/ns_getopt.h>

int main() {
	struct tk {
		bool t = false;
		bool k = false;
	} tk_args;

	const char* args[] = { "-t", "-k" };
	int args_size = sizeof(args) / sizeof(char*);

	std::array<opt::argument, 2> opt_args = { { // clang-format
			{ "tommy", opt::type::no_arg, [&tk_args]() { tk_args.t = true; },
					"Tommy Lee.", 't' },
			{ "kirk", opt::type::no_arg, [&tk_args]() { tk_args.k = true; },
					"James T. Kirk.", 'k' } } };

	opt::options o = { "A wonderful example.\nTalented Author\n",
		"More info on github.\n", opt::arg0_is_normal_argument,
		[](std::string_view s) { std::cout << s << std::endl; } };

	if (!opt::parse_arguments(args_size, args, opt_args)) {
		return -1;
	}

	std::cout << "t = " << tk_args.t << ", k = " << tk_args.k << std::endl;
	return 0;
}
