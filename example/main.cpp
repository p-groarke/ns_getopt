#include <notshit_getopt.h>

void my_method(std::string&& arg) {
	std::cout << arg << std::endl;
}

int main(int argc, char* argv[]) {
	bool do_something = false;

	auto vec_fun = [](std::vector<std::string>&& v) {
		for (const auto& x : v) {
			std::cout << x << std::endl;
		}
	};

	std::vector<opt::argument> args = {
		{"test", opt::type::no_arg, [&do_something](){ do_something = true; }, 't',
				"This is a simple flag."}
		, {"requiredarg", opt::type::required_arg,
				[](std::string&& s){ std::cout << s << std::endl; }, '\0',
				"This argument requires a value."}
		, {"optional", opt::type::optional_arg,
				my_method, 'o',
				"This parameter has an optional argument."}
		, {"default", opt::type::default_arg,
				my_method, 'd',
				"An example of an argument with default value.", "my_default_val"}
		, {"multi", opt::type::multi_arg, vec_fun, 'm',
				"This accepts multiple, space seperated, values.\n"
				"You can also have long descriptions that get\n"
				"automatically aligned simply by using \\n in\n"
				"your description."}
	};

	opt::options o = {"A wonderful example.\nPhilippe Groarke\n\n"
			"Usage: notshit_getopt [options]\n"
			, "More info on github.\n"};
	bool succeeded = opt::parse_arguments(argc, argv, args, o);

	if (!succeeded)
		return -1;

	std::cout << "Parsing arguments succeeded!" << std::endl;
	return 0;
}
