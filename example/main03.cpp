#include <notshit_getopt.h>

/* Using custom argument buffer. */

int main() {
	std::vector<opt::argument> args = {
		{"test", opt::type::no_arg, {[]() { }}, 't', "This is a simple flag."},
		{"test2", opt::type::no_arg, {[]() { }}, 'd', "This is a simple flag."}
	};

	const char* my_args[] = {"-t"};
	int my_args_size = (int)sizeof(my_args) / sizeof(char*);

	opt::options o = {"A wonderful example.\nTalented Author\n"
		, "More info on github.\n"
		, opt::argument{"john", opt::type::no_arg, {[]() { }}, '\0', "John Doe."}
		, false /* Exit on error. */
		, 123 /* Error code. */
		, false /* Has program name. */
	};

	bool succeeded = opt::parse_arguments(my_args_size, my_args, args, o);

	if (!succeeded)
		return -1;

	std::cout << "Parsing arguments succeeded!" << std::endl;
	return 0;
}
