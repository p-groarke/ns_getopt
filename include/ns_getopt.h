/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2017, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/

#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <cstring>

namespace opt {

/* List of argument types. */
enum class type : std::uint8_t {
	no_arg
	, required_arg
	, optional_arg
	, default_arg
	, multi_arg
	, raw_arg
};

/* User argument. */
struct argument {
	const std::function<void()> no_arg_func;
	const std::function<void(std::string&&)> one_arg_func;
	const std::function<void(std::vector<std::string>&&)> multi_arg_func;
	const std::string long_arg;
	const std::string description;
	const std::string default_arg;
	int raw_arg_pos;
	const char short_arg;
	const type arg_type;
	bool parsed;

	inline argument(std::string&& long_arg
			, type arg_type
			, const std::function<void()>& no_arg_func
			, std::string&& description = ""
			, char&& short_arg = '\0');

	inline argument(std::string&& long_arg
			, type arg_type
			, const std::function<void(std::string&&)>& one_arg_func
			, std::string&& description = ""
			, char&& short_arg = '\0'
			, std::string&& default_arg = "");

	inline argument(std::string&& long_arg
			, type arg_type
			, const std::function<void(std::vector<std::string>&&)>&
			, std::string&& description = ""
			, char&& short_arg = '\0');

	inline void asserts();
};

enum flag : unsigned int {
	USER_ERROR_MESSAGE = 1,
	EXIT_ON_ERROR = 2,
	PRINT_HELP_WITHOUT_ARGS = 4,

	DEFAULT_FLAGS = USER_ERROR_MESSAGE | PRINT_HELP_WITHOUT_ARGS
};

/* Configuration options. */
struct options {
	const std::function<void(std::string&&)> first_argument_func;
	const std::string help_intro;
	const std::string help_outro;
	const int exit_code;
	const flag flags;

	inline options(std::string&& help_intro = ""
			, std::string&& help_outro = ""
			, flag flags = DEFAULT_FLAGS
			, const std::function<void(std::string&&)>& first_argument_func
					= [](std::string&&){}
			, int exit_code = -1);
};

inline void print_help(const std::vector<argument>& args, const char* arg0,
		const options& option);

inline bool parse_arguments(int argc, char const* const* argv,
		std::vector<argument>& args, const options& option = {});

namespace {
	void print_description(const std::string& s, size_t indentation);

	inline bool do_exit(std::vector<argument>& args, const options& option
			, const char* arg0);

	inline bool _compare_no_case(unsigned char lhs, unsigned char rhs);

	inline bool compare_no_case(const char* lhs , const std::string& rhs
			, const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0);

	inline bool compare_no_case(const std::string& lhs , const std::string& rhs
			, const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0);

	inline void maybe_print_msg(const options& option, const std::string& msg);

	inline bool has_flag(const flag flags, flag flag_to_check);
}

/* Implementation details. */

inline argument::argument(std::string&& long_arg
		, type arg_type
		, const std::function<void()>& no_arg_func
		, std::string&& description
		, char&& short_arg)
	: no_arg_func(no_arg_func)
	, long_arg(long_arg)
	, description(description)
	, raw_arg_pos(-1)
	, short_arg(short_arg)
	, arg_type(arg_type)
	, parsed(false)

{
	assert(arg_type == type::no_arg);
	asserts();
}

inline argument::argument(std::string&& long_arg
		, type arg_type
		, const std::function<void(std::string&&)>& one_arg_func
		, std::string&& description
		, char&& short_arg
		, std::string&& default_arg)
	: one_arg_func(one_arg_func)
	, long_arg(long_arg)
	, description(description)
	, default_arg(default_arg)
	, raw_arg_pos(-1)
	, short_arg(short_arg)
	, arg_type(arg_type)
	, parsed(false)
{
	assert(arg_type == type::required_arg
			|| arg_type == type::optional_arg
			|| arg_type == type::default_arg
			|| arg_type == type::raw_arg);
	asserts();
}

inline argument::argument(std::string&& long_arg
		, type arg_type
		, const std::function<void(std::vector<std::string>&&)>& multi_arg_func
		, std::string&& description
		, char&& short_arg)
	: multi_arg_func(multi_arg_func)
	, long_arg(long_arg)
	, description(description)
	, raw_arg_pos(-1)
	, short_arg(short_arg)
	, arg_type(arg_type)
	, parsed(false)
{
	assert(arg_type == type::multi_arg);
	asserts();
}

inline void argument::asserts() {
	assert(long_arg.find(" ") == std::string::npos
			&& "One does not simply use spaces in his arguments.");
}

inline options::options(std::string&& help_intro
		, std::string&& help_outro
		, flag flags
		, const std::function<void(std::string&&)>& first_argument_func
		, int exit_code)
	: first_argument_func(first_argument_func)
	, help_intro(help_intro)
	, help_outro(help_outro)
	, exit_code(exit_code)
	, flags(flags)
{}

inline void print_help(const std::vector<argument>& args, const char* arg0
		, const options& option) {
	const size_t first_space = 1;
	const size_t sa_width = 4;
	const size_t sa_total_width = first_space + sa_width;
	const size_t la_space = 2;
	const size_t la_width_max = 30;
	const size_t ra_space = 4;
	const std::string opt_str = " <optional>";
	const std::string req_str = " <value>";
	const std::string multi_str = " <multiple>";
	const std::string default_beg = " < =";
	const std::string default_end = ">";

	std::cout << option.help_intro;
	std::cout << std::endl;

	/* Usage. */
	bool print_help_without_args = has_flag(option.flags, PRINT_HELP_WITHOUT_ARGS);
	std::string raw_args = "";
	bool first = !print_help_without_args;
	for (const auto& x : args) {
		if (x.arg_type == type::raw_arg) {
			raw_args += (first ? " [" : " ") + x.long_arg;
			first = false;
		}
	}
	if (!print_help_without_args) {
		raw_args += "]";
	}

	std::cout << "Usage: " << arg0 << raw_args << " [options]"
			<< std::endl << std::endl;

	/* Raw args. */
	std::cout << "Arguments:" << std::endl;
	size_t name_width = 0;
	for (const auto& x : args) {
		if (x.arg_type != type::raw_arg)
			continue;

		size_t s = x.long_arg.size() + ra_space;
		if (s > name_width)
			name_width = s;
	}

	for (const auto& x : args) {
		if (x.arg_type != type::raw_arg)
			continue;
		std::cout << std::setw(first_space) << "";
		std::cout << std::setw(name_width) << std::left << x.long_arg;
		print_description(x.description, first_space + name_width);
	}
	std::cout << std::endl;

	/* Other args.*/
	std::cout << "Options:" << std::endl;
	size_t la_width = 0;
	for (const auto& x : args) {
		if (x.arg_type == type::raw_arg)
			continue;

		size_t s = 2 + x.long_arg.size() + la_space;
		if (x.arg_type == type::optional_arg) {
			s += opt_str.size();
		} else if (x.arg_type == type::required_arg) {
			s += req_str.size();
		} else if (x.arg_type == type::default_arg) {
			s += default_beg.size() + x.default_arg.size() + default_end.size();
		} else if (x.arg_type == type::multi_arg) {
			s += multi_str.size();
		}

		if (s > la_width)
			la_width = s;
	}

	if (la_width > la_width_max) {
		la_width = la_width_max;
	}

	for (const auto& x : args) {
		if (x.arg_type == type::raw_arg)
			continue;

		std::cout << std::setw(first_space) << "";

		if (x.short_arg != '\0') {
			std::cout << std::setw(sa_width) << std::left
					<< std::string("-" + std::string(1, x.short_arg) + ",");
		} else {
			std::cout << std::setw(sa_width) << "";
		}

		std::string la_str = "--" + x.long_arg;
		if (x.arg_type == type::optional_arg) {
			la_str += opt_str;
		} else if (x.arg_type == type::required_arg) {
			la_str += req_str;
		} else if (x.arg_type == type::default_arg) {
			la_str += default_beg + x.default_arg + default_end;
		} else if (x.arg_type == type::multi_arg) {
			la_str += multi_str;
		}

		std::cout << std::setw(la_width) << std::left << la_str;
		if (la_str.size() >= la_width) {
			std::cout << std::endl;
			std::cout << std::setw(la_width + sa_total_width) << "";
		}

		print_description(x.description, la_width + sa_total_width);
	}

	std::cout << std::setw(first_space) << ""
			<< std::setw(sa_width) << std::left << "-h,"
			<< std::setw(la_width) << std::left << "--help"
			<< "Print this help." << std::endl;

	std::cout << std::endl;
	std::cout << option.help_outro;
	std::cout << std::endl;
}

/* TODO: Equal sign. Unique args (asserts)? Required raw_args? */
inline bool parse_arguments(int argc, char const* const* argv,
		std::vector<argument>& args, const options& option) {

	/* Prepare raw_args, they are parsed in declared order. */
	int parsed_raw_args = 0;
	int raw_args_count = 0;
	for (auto& x : args) {
		if (x.arg_type == type::raw_arg)
			x.raw_arg_pos = raw_args_count++;
	}

	for (int i = 0; i < argc; ++i) {
		/* First argument is a special snowflake. */
		if (i == 0) {
			if (argc == 1 && has_flag(option.flags, PRINT_HELP_WITHOUT_ARGS)) {
				return do_exit(args, option, argv[0]);
			} else {
				option.first_argument_func(argv[i]);
			}
		}

		/* Help. */
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0
				|| strcmp(argv[i], "/?") == 0) {
			return do_exit(args, option, argv[0]);
		}

		/* Check single short arg and long args. */
		else if ((strncmp(argv[i], "-", 1) == 0 && strlen(argv[i]) == 2)
				|| strncmp(argv[i], "--", 2) == 0) {
			int found = -1;
			for (size_t j = 0; j < args.size(); ++j) {
				if (compare_no_case(argv[i], args[j].long_arg, 2)) {
					found = j;
					break;
				}
			}

			if (found == -1) {
				for (size_t j = 0; j < args.size(); ++j) {
					if (argv[i][1] == args[j].short_arg) {
						found = j;
						break;
					}
				}
			}

			if (found == -1) {
				maybe_print_msg(option
						, "'" + std::string(argv[i]) + "' not found.");
				return do_exit(args, option, argv[0]);
			}

			if (args[found].parsed) {
				maybe_print_msg(option
						, "'" + std::string(argv[i]) + "' already parsed.");
				return do_exit(args, option, argv[0]);
			}

			argument& found_arg = args[found];
			found_arg.parsed = true;
			std::string default_arg = found_arg.default_arg;

			switch(found_arg.arg_type) {
				case type::no_arg: {
					found_arg.no_arg_func();
				} break;

				case type::required_arg: {
					if (i + 1 >= argc) {
						maybe_print_msg(option
								, "'" + std::string(argv[i])
								+ "' requires 1 argument.");
						return do_exit(args, option, argv[0]);
					}

					if (strncmp(argv[i + 1], "-", 1) == 0) {
						maybe_print_msg(option
								, "'" + std::string(argv[i])
								+ "' requires 1 argument.");
						return do_exit(args, option, argv[0]);
					}
					found_arg.one_arg_func(argv[++i]);
				} break;

				case type::optional_arg: {
					default_arg = "";
				}
				case type::default_arg: {
					if (i + 1 >= argc) {
						found_arg.one_arg_func(
								std::move(default_arg));
						break;
					}

					if (strncmp(argv[i + 1], "-", 1) == 0) {
						found_arg.one_arg_func(
								std::move(default_arg));
						break;
					}
					found_arg.one_arg_func(argv[++i]);
				} break;

				case type::multi_arg: {
					std::vector<std::string> v;
					while (i + 1 < argc) {
						if (strncmp(argv[i + 1], "-", 1) == 0) {
							break;
						}
						v.emplace_back(argv[++i]);
					}
					found_arg.multi_arg_func(std::move(v));
				} break;

				default: {
					assert(false && "Something went horribly wrong.");
				};
			}
		}

		/* Concatenated short args. */
		else if (strncmp(argv[i], "-", 1) == 0 && strlen(argv[i]) > 2) {
			std::vector<int> found_v;
			std::string not_found = "";
			for (size_t j = 1; j < strlen(argv[i]); ++j) {
				bool found = false;
				for (size_t k = 0; k < args.size(); ++k) {
					if (argv[i][j] == args[k].short_arg) {
						found_v.push_back(k);
						found = true;
						break;
					}
				}
				if (!found) {
					not_found.append(1, argv[i][j]);
				}
			}

			if (found_v.size() == 0) {
				maybe_print_msg(option
						, "'" + std::string(argv[i]) + "' not found.");
				return do_exit(args, option, argv[0]);
			}

			if (not_found.size() != 0) {
				maybe_print_msg(option
						, "'-" + not_found + "' not found.");
				return do_exit(args, option, argv[0]);
			}

			/* Accept duplicate flags because who cares. */
			std::sort(found_v.begin(), found_v.end());
			auto last = std::unique(found_v.begin(), found_v.end());
			found_v.erase(last, found_v.end());

			for (const auto& x : found_v) {
				if (args[x].parsed) {
					maybe_print_msg(option
							, "'" + std::string(1, args[x].short_arg)
							+ "' already parsed.");
					return do_exit(args, option, argv[0]);
				}

				if (args[x].arg_type != type::no_arg) {
					maybe_print_msg(option
							, "'" + std::string(1, args[x].short_arg)
							+ "' unsupported in concatenated short arguments.");
					return do_exit(args, option, argv[0]);
				}
			}

			for (const auto& x : found_v) {
				args[x].parsed = true;
				args[x].no_arg_func();
			}
		}

		/* Check raw args. */
		else if (parsed_raw_args < raw_args_count) {
			int found = -1;
			for (size_t j = 0; j < args.size(); ++j) {
				if (args[j].raw_arg_pos == parsed_raw_args) {
					found = j;
					break;
				}
			}
			if (found == -1)
				continue;

			argument& found_arg = args[found];
			found_arg.parsed = true;
			++parsed_raw_args;
			found_arg.one_arg_func(argv[i]);
		}

		/* Everything failed. */
		else {
			maybe_print_msg(option
					, "'" + std::string(argv[i]) + "' unrecognized.");
			return do_exit(args, option, argv[0]);
		}
	}

	return true;
}


/* Internal functions. */
namespace {
void print_description(const std::string& s, size_t indentation) {
	if (s.size() == 0)
		return;

	std::istringstream iss(s);
	std::string line;
	std::getline(iss, line);
	while (true) {
		std::cout << line << std::endl;
		if (std::getline(iss, line)) {
			std::cout << std::setw(indentation) << "";
		} else {
			break;
		}
	}
}

inline bool do_exit(std::vector<argument>& args, const options& option
		, const char* arg0) {
	print_help(args, arg0, option);
	if (has_flag(option.flags, EXIT_ON_ERROR))
		exit(option.exit_code);
	return false;
}

inline bool _compare_no_case(unsigned char lhs, unsigned char rhs) {
	return std::tolower(lhs) == std::tolower(rhs);
}

inline bool compare_no_case(const char* lhs , const std::string& rhs
		, const size_t lhs_start_pos, const size_t rhs_start_pos) {
	if (lhs_start_pos >= strlen(lhs))
		return false;

	if (rhs_start_pos >= rhs.size())
		return false;

	if (strlen(lhs) - lhs_start_pos == rhs.size() - rhs_start_pos) {
		return std::equal(lhs + lhs_start_pos, lhs + strlen(lhs),
			rhs.begin() + rhs_start_pos, _compare_no_case);
	}
	return false;
}

inline bool compare_no_case(const std::string& lhs , const std::string& rhs
		, const size_t lhs_start_pos, const size_t rhs_start_pos) {
	return compare_no_case(lhs.c_str(), rhs, lhs_start_pos,
			rhs_start_pos);
}

inline void maybe_print_msg(const options& option, const std::string& msg) {
	if (!has_flag(option.flags, USER_ERROR_MESSAGE))
		return;
	std::cout << msg << std::endl;
}

inline bool has_flag(const flag flags, flag flag_to_check) {
	return (flags & (flag_to_check)) != 0;
}

} // namespace {}
} // namespace opt

