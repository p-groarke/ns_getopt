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
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <cstring>

#define GETOPT_DEBUG_MESSAGES 1

namespace opt {
/* List of argument type. */
enum class type : std::uint8_t {
	no_arg
	, required_arg
	, optional_arg
	, default_arg
	, multi_arg
	, raw_arg
};

struct argument {
	int raw_arg_pos;
	const char short_arg;
	const type arg_type;
	bool parsed;

	const std::string long_arg;
	const std::string description;
	const std::string default_arg;

	const std::function<void()> no_arg_func;
	const std::function<void(std::string&&)> one_arg_func;
	const std::function<void(std::vector<std::string>&&)> multi_arg_func;

	/*
	 * @todo In my opinion, this variable should't exist.
	 *		 We could remove the const from 'const int raw_arg_pos' and then
	 *		 loop through all arguments in the 'parse_arguments' function and
	 *		 assign the raw_arg_pos value to each argument of type raw_arg.
	 *
	 *		 This would allow multiple usage of 'parse_arguments'. Another
	 *		 solution would be to add 'argument::raw_arg_count = 0;' at the
	 *		 end of the 'parse_arguments' function.
	 */
	// static int raw_arg_count;

	/*
	 * For all constructors, the first three arguments :
	 * 'std::string&& long_arg arg_type, type, const std::function<...>& func'
	 * are required to prevent from compile time error 'ambiguous call to
	 * constructor'.
	 *
	 * @todo Some kind of error checking should be done to prevent from
	 *		 creating an argument with a type associated with the wrong
	 *		 function callback.
	 */

	/* no_arg constructor. */
	inline argument(std::string&& long_arg
			, type arg_type /* = type::no_arg */
			, const std::function<void()>& no_arg_func /* = [](){} */
			, char&& short_arg = '\0'
			, std::string&& description = "");

	/* required_arg, optional_arg or default_arg constructor. */
	inline argument(std::string&& long_arg
			, type arg_type /* = type::optional_arg  */
			, const std::function<void(std::string&&)>& one_arg_func
				/* = [](std::string&&){} */
			, char&& short_arg = '\0'
			, std::string&& description = ""
			, std::string&& default_arg = "");

	/* multi_arg constructor. */
	inline argument(std::string&& long_arg
			, type arg_type /* = type::multi_arg */
			, const std::function<void(std::vector<std::string>&&)>&
				multi_arg_func /* = [](std::vector<std::string>&&){} */
			, char&& short_arg = '\0'
			, std::string&& description = "");

	/*
	 * raw_arg constructor.
	 *
	 * The description parameter is required to prevent from compile time error
	 * 'ambiguous call to constructor' with the optional_arg constructor.
	 *
	 * @todo This problem could be solve by reordering constructor parameters
	 *		 and combining required_arg, optional_arg, default_arg and raw_arg
	 *		 in one constructor.
	 *
	 *	This would solve the problem.
	 *
	 *	argument(std::string&& long_arg
	 * 		, type arg_type
	 *		, const std::function<void(std::string&&)>& func
	 *		, std::string&& description = ""
	 *		, char&& short_arg = '\0'
	 *		, std::string&& default_arg = "")
	 *	: raw_arg_pos(type == type::raw_arg ? raw_arg_count++ : -1)
	 *	, short_arg(short_arg)
	 *	, arg_type(arg_type)
	 *	, parsed(false)
	 *	, long_arg(long_arg)
	 *	, description(description)
	 *	, default_arg(default_arg)
	 *	, one_arg_func(func){...}
	 */
	inline argument(std::string&& name
			, type arg_type /* = type::raw_arg */
			, const std::function<void(std::string&&)>&
				one_arg_func /* = [](std::string&&){} */
			, std::string&& description);

	inline void asserts();
};

struct options {
	const std::string help_intro;
	const std::string help_outro;
	const argument first_argument; /* @todo What is this for ??? */
	const bool exit_on_error;
	const int exit_code;
	const bool has_program_name;

	inline options(std::string&& help_intro = ""
			, std::string&& help_outro = ""
			, argument&& first_argument = {"", type::no_arg, std::function<void()>{[]() {}}}
			, bool exit_on_error = true
			, int exit_code = -1
			, bool has_program_name = true);
};

inline void print_help(std::vector<argument>& args, const options& option , const char* arg0);

inline bool parse_arguments(int argc, char const* const* argv, std::vector<argument>& args,
		const options& option = {});

} //namespace opt

/* Implementation section. */
namespace opt {
/* no_arg constructor. */
argument::argument(std::string&& long_arg
		, type arg_type
		, const std::function<void()>& no_arg_func
		, char&& short_arg
		, std::string&& description)
	: raw_arg_pos(-1)
	, short_arg(short_arg)
	, arg_type(arg_type)
	, parsed(false)
	, long_arg(long_arg)
	, description(description)
	, no_arg_func(no_arg_func)
{
	assert(arg_type == type::no_arg &&
			"opt::type::no_arg requires a void function with "
			"no arguments.");
	asserts();
}

/* required_arg, optional_arg or default_arg constructor. */
argument::argument(std::string&& long_arg
		, type arg_type
		, const std::function<void(std::string&&)>& one_arg_func
		, char&& short_arg
		, std::string&& description
		, std::string&& default_arg)
	: raw_arg_pos(-1)
	, short_arg(short_arg)
	, arg_type(arg_type)
	, parsed(false)
	, long_arg(long_arg)
	, description(description)
	, default_arg(default_arg)
	, one_arg_func(one_arg_func)
{
	assert(((arg_type == type::required_arg)
			|| (arg_type == type::optional_arg)
			|| (arg_type == type::default_arg))
			&& "opt::type::required_arg or opt::type::optional_arg "
			"requires a void function that accepts a string&&.");
	asserts();
}

/* multi_arg constructor. */
argument::argument(std::string&& long_arg
		, type arg_type
		, const std::function<void(std::vector<std::string>&&)>& multi_arg_func
		, char&& short_arg
		, std::string&& description)
	:  raw_arg_pos(-1)
	, short_arg(short_arg)
	, arg_type(arg_type)
	, parsed(false)
	, long_arg(long_arg)
	, description(description)
	, multi_arg_func(multi_arg_func)
{
	assert(arg_type == type::multi_arg &&
			"opt::type::multi_arg requires a void function that "
			"accepts a const vector<string>&.");
	asserts();
}

/* raw_arg constructor. */
argument::argument(std::string&& name
		, type arg_type
		, const std::function<void(std::string&&)>& one_arg_func
		, std::string&& description)
//	: raw_arg_pos(raw_arg_count++)
	: raw_arg_pos(0)
	, short_arg('\0')
	, arg_type(arg_type)
	, parsed(false)
	, long_arg(name)
	, description(description)
	, one_arg_func(one_arg_func)
{
	assert(arg_type == type::raw_arg);
	asserts();
}

void argument::asserts() {
	assert(long_arg.find(" ") == std::string::npos
			&& "One does not simply use spaces in his arguments.");
}

options::options(std::string&& help_intro
		, std::string&& help_outro
		, argument&& first_argument
		, bool exit_on_error
		, int exit_code
		, bool has_program_name)
	: help_intro(help_intro)
	, help_outro(help_outro)
	, first_argument(first_argument)
	, exit_on_error(exit_on_error)
	, exit_code(exit_code)
	, has_program_name(has_program_name)
{}

namespace {
inline bool _compare_no_case(unsigned char lhs, unsigned char rhs);
inline bool compare_no_case(const char* lhs , const std::string& rhs
		, const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0);
inline bool compare_no_case(const std::string& lhs , const std::string& rhs
		, const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0);
inline void maybe_print_msg(const std::string& msg);
inline void print_description(const std::string& s, size_t indentation);
inline bool do_exit(std::vector<argument>& args, const options& option, const char* arg0);
} // namespace {}

void print_help(std::vector<argument>& args, const options& option
		, const char* arg0) {
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
	std::string raw_args = "";
	for (const auto& x : args) {
		if (x.arg_type == type::raw_arg)
			raw_args += " " + x.long_arg;
	}

	if(option.has_program_name) {
		std::cout << "Usage: " << arg0 << raw_args << " [options]"
				<< std::endl << std::endl;
	} else {
		std::cout << "Usage:" << raw_args << " [options]"
				<< std::endl << std::endl;
	}

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

	std::cout << std::endl;
	std::cout << option.help_outro;
	std::cout << std::endl;
}

/* TODO: Equal sign. Unique args (asserts)? */
bool parse_arguments(int argc, char const* const* argv
		, std::vector<argument>& args, const options& option) {

	/* No parameter given. */
	if(option.has_program_name && argc == 1) {
		/*
		 * @todo This doesn't work when using only no_arg arguments.
		 *		 See example/main02.cpp
		 */
		return do_exit(args, option, argv[0]);
	}

	int first_index = option.has_program_name ? 1 : 0;
	int biggest_raw_arg_index = 0;

	{
		int raw_arg_counter = 0;
		for(auto& arg : args) {
			if(arg.arg_type == type::raw_arg) {
				arg.raw_arg_pos = raw_arg_counter++;
			}
		}

		biggest_raw_arg_index = raw_arg_counter;
	}

	int parsed_raw_args = 0;

	for (int i = first_index; i < argc; ++i) {
		/* Help. */
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0
				|| strcmp(argv[i], "/?") == 0) {
			return do_exit(args, option, argv[0]);
		}

		/* Check single short arg and long args. */
		if ((strncmp(argv[i], "-", 1) == 0 && strlen(argv[i]) == 2)
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
				maybe_print_msg("'" + std::string(argv[i]) + "' not found.");
				return do_exit(args, option, argv[0]);
			}

			if (args[found].parsed) {
				maybe_print_msg("'" + std::string(argv[i]) + "' already parsed.");
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
						maybe_print_msg("'" + std::string(argv[i])
								+ "' requires 1 argument.");
						return do_exit(args, option, argv[0]);
					}

					if (strncmp(argv[i + 1], "-", 1) == 0) {
						maybe_print_msg("'" + std::string(argv[i])
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
				maybe_print_msg("'" + std::string(argv[i]) + "' not found.");
				return do_exit(args, option, argv[0]);
			}

			if (not_found.size() != 0) {
				maybe_print_msg("'-" + not_found + "' not found.");
				return do_exit(args, option, argv[0]);
			}

			/* Accept duplicate flags because who cares. */
			std::sort(found_v.begin(), found_v.end());
			auto last = std::unique(found_v.begin(), found_v.end());
			found_v.erase(last, found_v.end());

			for (const auto& x : found_v) {
				if (args[x].parsed) {
					maybe_print_msg("'" + std::string(1, args[x].short_arg)
							+ "' already parsed.");
					return do_exit(args, option, argv[0]);
				}

				if (args[x].arg_type != type::no_arg) {
					maybe_print_msg("'" + std::string(1, args[x].short_arg)
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
		else if (parsed_raw_args < biggest_raw_arg_index) {
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
			maybe_print_msg("'" + std::string(argv[i]) + "' unrecognized.");
			return do_exit(args, option, argv[0]);
		}
	}

	return true;
}

namespace {
bool _compare_no_case(unsigned char lhs, unsigned char rhs) {
	return ::tolower(lhs) == ::tolower(rhs);
}

bool compare_no_case(const char* lhs , const std::string& rhs
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

bool compare_no_case(const std::string& lhs , const std::string& rhs
		, const size_t lhs_start_pos, const size_t rhs_start_pos) {
	return compare_no_case(lhs.c_str(), rhs, lhs_start_pos,
			rhs_start_pos);
}

void maybe_print_msg(const std::string& msg) {
#if GETOPT_DEBUG_MESSAGES
	std::cout << "Parsing error : " << msg << std::endl;
#endif // GETOPT_DEBUG_MESSAGES
}

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

bool do_exit(std::vector<argument>& args, const options& option, const char* arg0) {
	print_help(args, option, arg0);
	if (option.exit_on_error)
		exit(option.exit_code);
	return false;
}

} // namespace {}
} // namespace opt
