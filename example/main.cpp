#include <ns_getopt.h>

void my_method(std::string&& arg) {
	std::cout << arg << std::endl;
}

int main(int argc, char* argv[]) {
//	bool do_something = false;

//	auto raw_fun = [](std::string&& s) {
//		std::cout << "Raw arg : " << s << std::endl;
//	};

	auto vec_fun = [](std::vector<std::string>&& v) {
		for (const auto& x : v) {
			std::cout << x << std::endl;
		}
	};

	std::vector<opt::argument> args = {
		{"test", opt::type::no_arg,
				[](){ std::cout << "t" << std::endl; },
				"This is a simple flag.", 't'}
		, {"M", opt::type::no_arg,
				[](){ std::cout << "m" << std::endl; },
				"This is a simple flag.", 'M'}
		, {"A", opt::type::no_arg,
				[](){ std::cout << "a" << std::endl; },
				"This is a simple flag.", 'a'}
		, {"requiredarg", opt::type::required_arg,
				[](std::string&& s){ std::cout << s << std::endl; },
				"This argument requires a value.", 'r'}
		, {"optional", opt::type::optional_arg,
				my_method,
				"This parameter has an optional argument.", 'o'}
		, {"default", opt::type::default_arg,
				my_method,
				"An example of an argument with default value.", 'd',
				"my_default_val"}
		, {"multi", opt::type::multi_arg, vec_fun,
				"This accepts multiple, space seperated, values.\n"
				"You can also have long descriptions that get\n"
				"automatically aligned simply by using \\n in\n"
				"your description.", 'm'}
//		, {"in_file", opt::type::raw_arg, raw_fun,
//				"Description for file 1.\nIt can be multiple\nlines too."}
//		, {"out_file", opt::type::raw_arg, raw_fun,
//				"Description for out_file. Raw arguments are optional."}
	};

	opt::options o = {"A wonderful example.\nTalented Author\n"
			, "More info on github.\n"
			, opt::none
			, [](std::string&& s) { std::cout << s << std::endl; }
	};

	bool succeeded = opt::parse_arguments(argc, argv, args, o);

	/* You can print the help whenever you desire. */
	//opt::print_help(args, o, argv[0]);

	if (!succeeded)
		return -1;

	std::cout << "Parsing arguments succeeded!" << std::endl;
	return 0;
}
