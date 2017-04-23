#include <notshit_getopt.h>
#include <cstdio>

int main(int argc, char* argv[]) {
	bool do_something = false;

	auto vec_fun = [](std::vector<std::string>&& v) {
		for (const auto& x : v) {
			printf("%s ", x.c_str());
		}
		printf("\n");
	};

	std::vector<opt::argument> args = {
		{"test", opt::type::no_arg, [&do_something](){ do_something = true; }, 't'}
		, {"req", opt::type::required_arg,
				[](std::string&& s){ printf("%s\n",s.c_str()); }}
		, {"opt", opt::type::optional_arg,
				[](std::string&& s){ printf("%s\n",s.c_str()); }, 'o',
				"Test desc                      asdf          asdf     alksj"
				"asdffff asdf adsf dsaf  jkdsas dsfdssdfskadfskjf kjsdfk sa"}
		, {"multi", opt::type::multi_arg, vec_fun, 'm'}
	};

	bool succeeded = opt::parse_arguments(argc, argv, args);

	return 0;
}
