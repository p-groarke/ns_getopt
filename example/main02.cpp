#include <notshit_getopt.h>

/* Using only no_arg. */

int main(int argc, char* argv[]) {
	std::vector<opt::argument> args = {
		{"test", opt::type::no_arg, {[]() { }}, 't', "This is a simple flag."},
		{"test2", opt::type::no_arg, {[]() { }}, 'd', "This is a simple flag."}
	};

	opt::options o = {"A wonderful example.\nTalented Author\n"
			, "More info on github.\n"};

	bool succeeded = opt::parse_arguments(argc, argv, args, o);

	if (!succeeded)
		return -1;

	std::cout << "Parsing arguments succeeded!" << std::endl;
	return 0;
}
