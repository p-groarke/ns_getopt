#define CATCH_CONFIG_MAIN // This tells Catch to provide a main()
#include <catch/catch.hpp>

#include <ns_getopt/ns_getopt.h>

void my_function(std::string_view s) {
	printf("%.*s\n", (int)s.size(), s.data());
}

TEST_CASE("Argument detection", "[parsing]") {

	auto raw_fun = [](std::string_view s) {
		printf("Raw args : %.*s\n", (int)s.size(), s.data());
	};

	auto multi_fun = [](opt::multi_array&& a, size_t len) {
		printf("Muli args : ");
		for (size_t i = 0; i < len; ++i) {
			printf("%.*s ", (int)a[i].size(), a[i].data());
		}
		printf("\n");
	};

	opt::argument args_c_array[] = { // clang-format
		{ "test", opt::type::no_arg, []() { printf("No arg : t\n"); },
				"This is a simple flag.", 't' },
		{ "M", opt::type::no_arg, []() { printf("No arg : m\n"); },
				"This is a simple flag.", 'M' },
		{ "A", opt::type::no_arg, []() { printf("No arg : a\n"); },
				"This is a simple flag.", 'a' },
		{ "requiredarg_with_long_name", opt::type::required_arg,
				[](std::string_view s) {
					printf("Required : %.*s\n", (int)s.size(), s.data());
				},
				"This argument requires a value.", 'r' },
		{ "optional", opt::type::optional_arg, my_function,
				"This parameter has an optional argument.", 'o' },
		{ "default", opt::type::default_arg, my_function,
				"An example of an argument with default value.", 'd',
				"my_default_val" },
		{ "multi", opt::type::multi_arg, multi_fun, 3,
				"This accepts 3 space seperated values.\n"
				"You can also have long descriptions that get\n"
				"automatically aligned simply by using \\n in\n"
				"your description.",
				'm' },
		{ "in_file", opt::type::raw_arg, raw_fun,
				"Description for file 1.\nIt can be multiple\nlines too." },
		{ "out_file", opt::type::raw_arg, raw_fun,
				"Description for out_file. Raw arguments are "
				"optional.\n" }
	};
	constexpr size_t args_size = sizeof(args_c_array) / sizeof(opt::argument);

	std::array<opt::argument, args_size> args_array = { { // clang-format
			{ "test", opt::type::no_arg, []() { printf("No arg : t\n"); },
					"This is a simple flag.", 't' },
			{ "M", opt::type::no_arg, []() { printf("No arg : m\n"); },
					"This is a simple flag.", 'M' },
			{ "A", opt::type::no_arg, []() { printf("No arg : a\n"); },
					"This is a simple flag.", 'a' },
			{ "requiredarg_with_long_name", opt::type::required_arg,
					[](std::string_view s) {
						printf("Required : %.*s\n", (int)s.size(), s.data());
					},
					"This argument requires a value.", 'r' },
			{ "optional", opt::type::optional_arg, my_function,
					"This parameter has an optional argument.", 'o' },
			{ "default", opt::type::default_arg, my_function,
					"An example of an argument with default value.", 'd',
					"my_default_val" },
			{ "multi", opt::type::multi_arg, multi_fun, 3,
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

	opt::argument* args_heap_array = new opt::argument[args_size]{
		{ "test", opt::type::no_arg, []() { printf("No arg : t\n"); },
				"This is a simple flag.", 't' },
		{ "M", opt::type::no_arg, []() { printf("No arg : m\n"); },
				"This is a simple flag.", 'M' },
		{ "A", opt::type::no_arg, []() { printf("No arg : a\n"); },
				"This is a simple flag.", 'a' },
		{ "requiredarg_with_long_name", opt::type::required_arg,
				[](std::string_view s) {
					printf("Required : %.*s\n", (int)s.size(), s.data());
				},
				"This argument requires a value.", 'r' },
		{ "optional", opt::type::optional_arg, my_function,
				"This parameter has an optional argument.", 'o' },
		{ "default", opt::type::default_arg, my_function,
				"An example of an argument with default value.", 'd',
				"my_default_val" },
		{ "multi", opt::type::multi_arg, multi_fun, 3,
				"This accepts 3 space seperated values.\n"
				"You can also have long descriptions that get\n"
				"automatically aligned simply by using \\n in\n"
				"your description.",
				'm' },
		{ "in_file", opt::type::raw_arg, raw_fun,
				"Description for file 1.\nIt can be multiple\nlines too." },
		{ "out_file", opt::type::raw_arg, raw_fun,
				"Description for out_file. Raw arguments are "
				"optional.\n" }
	};
	struct on_exit {
		on_exit(opt::argument* heap_array)
				: arr(heap_array) {
		}
		~on_exit() {
			delete[] arr;
		}
		opt::argument* arr = nullptr;
	} do_exit(args_heap_array);

	opt::options o = { "intro", "outro", opt::none, [](std::string_view s) {
						  printf("\n%.*s\n", (int)s.size(), s.data());
					  } };

	SECTION("no input should fail") {
		const char* argv[] = { "./exec" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == false);
	}

	SECTION("no_arg long option") {
		const char* argv[] = { "./exec", "--test" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("no_arg short option") {
		const char* argv[] = { "./exec", "-t" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("non-existent short option") {
		const char* argv[] = { "./exec", "-Z" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == false);
	}

	SECTION("concatenated short option") {
		const char* argv[] = { "./exec", "-Mta" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("required arg unsupported in concatenated short args") {
		const char* argv[] = { "./exec", "-Mtar", "blee" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == false);
	}

	SECTION("optional arg in concatenated options") {
		const char* argv[] = { "./exec", "-Mtao" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("optional arg provided") {
		const char* argv[] = { "./exec", "-o", "o" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("optional arg not provided") {
		const char* argv[] = { "./exec", "--optional" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("default arg in concatenated options") {
		const char* argv[] = { "./exec", "-Mtad" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("default arg") {
		const char* argv[] = { "./exec", "--default" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("default arg provided") {
		const char* argv[] = { "./exec", "--default", "d" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("required arg provided") {
		const char* argv[] = { "./exec", "--requiredarg_with_long_name", "r" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("required arg not provided") {
		const char* argv[] = { "./exec", "-r" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == false);
	}

	SECTION("multi arg empty") {
		const char* argv[] = { "./exec", "-m" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("multi arg max") {
		const char* argv[] = { "./exec", "--multi", "a1", "a2", "a3" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("multi arg more than max") {
		const char* argv[] = { "./exec", "--multi", "a1", "a2", "a3", "a4" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == false);
	}

	SECTION("one raw arg") {
		const char* argv[] = { "./exec", "a1" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("two raw arg") {
		const char* argv[] = { "./exec", "a1", "a2" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("raw args more than max") {
		const char* argv[] = { "./exec", "a1", "a2", "a3", "a4" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == false);
	}

	SECTION("raw args different positions 1") {
		const char* argv[] = { "./exec", "a1", "--test", "a2" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("raw args different positions 2") {
		const char* argv[] = { "./exec", "-M", "a1", "--test", "a2" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("raw args different positions 3") {
		const char* argv[]
				= { "./exec", "--multi", "m1", "m2", "--test", "r1", "r2" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == true);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == true);
	}

	SECTION("raw args different positions 4") {
		const char* argv[] = { "./exec", "--multi", "m1", "m2", "--test", "r1",
			"r2", "r3" };
		const size_t argc = sizeof(argv) / sizeof(char*);
		bool succeeded = opt::parse_arguments(argc, argv, args_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments(argc, argv, args_c_array, o);
		REQUIRE(succeeded == false);
		succeeded = opt::parse_arguments<args_size>(
				argc, argv, args_heap_array, o);
		REQUIRE(succeeded == false);
	}
}

TEST_CASE("Argument forwarding", "[parsing]") {
}
