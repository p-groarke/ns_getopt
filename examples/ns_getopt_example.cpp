/* TEMP */
#include <cstdio>
#include <cstdlib>
static size_t allocations = 0;
void* operator new(std::size_t count) {
	allocations += count;
	printf("\nalloc : %zu\n", count);
	return malloc(count);
}

#include <ns_getopt/ns_getopt.h>

bool my_function(std::string_view s) {
	printf("%.*s\n", (int)s.size(), s.data());
	return true;
}

int main(int argc, char* argv[]) {
	struct on_exit {
		~on_exit() {
			printf("\n\nAllocated bytes : %zu\n", allocations);
		}
	} do_exit;
	//	bool do_something = false;

	//	int test[42] = {};
	auto raw_fun = [/*test*/](std::string_view s) {
		printf("Raw args : %.*s\n", (int)s.size(), s.data());
		return true;
		//		printf("%d\n", test[5]);
	};

	// multi_array is std::array<std::string_view, 8>.
	// len will always be <= multi_max_subargs
	auto arr_fun = [](opt::multi_array&& a, size_t len) {
		for (size_t i = 0; i < len; ++i) {
			printf("%.*s\n", (int)a[i].size(), a[i].data());
		}
		return true;
	};


	std::array<opt::argument, 9> args = { { // clang-format
			{ "test", opt::type::no_arg,
					[]() {
						printf("t\n");
						return true;
					},
					"This is a simple flag.", 't' },
			{ "M", opt::type::no_arg,
					[]() {
						printf("m\n");
						return true;
					},
					"This is a simple flag.", 'M' },
			{ "A", opt::type::no_arg,
					[]() {
						printf("a\n");
						return true;
					},
					"This is a simple flag.", 'a' },
			{ "requiredarg_with_long_name", opt::type::required_arg,
					[](std::string_view s) {
						printf("%.*s\n", (int)s.size(), s.data());
						return true;
					},
					"This argument requires a value.", 'r' },
			{ "optional", opt::type::optional_arg, my_function,
					"This parameter has an optional argument.", 'o' },
			{ "default", opt::type::default_arg, my_function,
					"An example of an argument with default value.", 'd',
					"my_default_val" },
			{ "multi", opt::type::multi_arg, arr_fun, 3,
					"This accepts 3 space seperated values.\n"
					"You can also have long descriptions that get\n"
					"automatically aligned simply by using \\n in\n"
					"your description.",
					'm' },
			{ "in_file", opt::type::raw_arg, raw_fun,
					"Description for file 1.\nIt can be multiple\nlines too." },
			{ "out_file", opt::type::raw_arg, raw_fun,
					"Description for out_file. Raw arguments are "
					"optional.\n" } } };

	opt::options o = { "A wonderful example.\nTalented Author",
		"More info on github.", opt::none, [](std::string_view s) {
			printf("%.*s\n", (int)s.size(), s.data());
			return true;
		} };

	/* You can print the help whenever you desire. */
	//	opt::print_help(args, o, argv[0]);

	bool succeeded = opt::parse_arguments(argc, argv, args, o);

	if (!succeeded)
		return -1;

	printf("Parsing arguments succeeded!\n");
	return 0;
}
