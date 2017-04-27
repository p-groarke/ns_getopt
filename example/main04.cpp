#include <notshit_getopt.h>

/* Using second layer custom argument buffer. */

int main(int argc, char* argv[]) {
	std::vector<opt::argument> args = {
		{"in_file", opt::type::raw_arg, [](std::string&& s){ std::cout << "First raw arg = " << s << std::endl; },
			"Description for file 1.\nIt can be multiple\nlines too."}
	};

	opt::options o = {"A wonderful example.\nTalented Author\n"
		, "More info on github.\n"
	};

	bool succeeded = opt::parse_arguments(argc, argv, args, o);

	if (!succeeded)
		return -1;

	std::cout << "First parsing arguments succeeded!" << std::endl;

	opt::options my_options = {"Custom buffer args."
		, ""
		, opt::argument{"john", opt::type::no_arg, {[]() { }}, '\0', "John Doe."}
		, false /* Exit on error. */
		, 123 /* Error code. */
		, false /* Has program name. */
	};

	const char* my_args[] = {"123", "456"};
	int my_args_size = (int)sizeof(my_args) / sizeof(char*);
	std::vector<opt::argument> second_layer_args = {
		{"in_file2", opt::type::raw_arg, [](std::string&& s){ std::cout << "Second raw arg = " << s << std::endl;},
				"Description for file 1.\nIt can be multiple\nlines too."}
	};

	succeeded = opt::parse_arguments(my_args_size, my_args, second_layer_args, my_options);

	std::cout << "Second parsing arguments succeeded!" << std::endl;
	return 0;
}
