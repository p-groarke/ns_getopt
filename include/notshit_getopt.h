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
#include <vector>
#include <cassert>
#include <cstring>

#define GETOPT_DEBUG_MESSAGES 1
namespace opt {
	namespace {

inline bool _compare_no_case(unsigned char lhs, unsigned char rhs) {
	return std::tolower(lhs) == std::tolower(rhs);
}

inline bool compare_no_case(const char* lhs , const std::string& rhs
		, const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0) {
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
		, const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0) {
	return compare_no_case(lhs.c_str(), rhs, lhs_start_pos,
			rhs_start_pos);
}

//inline bool compare_no_case(const std::string& lhs, char rhs
//		, const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0) {
//	return compare_no_case(lhs, std::string(1, rhs), lhs_start_pos, rhs_start_pos);
//}

inline void maybe_print_msg(const std::string& msg) {
#ifdef GETOPT_DEBUG_MESSAGES
	std::cout << msg << std::endl;
#endif
}

} // namespace {}

enum class type {no_arg, required_arg, optional_arg, multi_arg};

struct argument {
	const std::string long_arg;
	const type arg_type;
	const std::function<void()> no_arg_func;
	const std::function<void(std::string&&)> one_arg_func;
	const std::function<void(std::vector<std::string>&&)> multi_arg_func;
	const char short_arg;
	const std::string description;
	bool parsed;

	argument(std::string&& long_arg
			, type arg_type = type::no_arg
			, std::function<void()>&& no_arg_func = [](){}
			, char&& short_arg = '\0'
			, std::string&& description = "")
		: long_arg(long_arg)
		, arg_type(arg_type)
		, no_arg_func(no_arg_func)
		, short_arg(short_arg)
		, description(description)
		, parsed(false)
	{
		assert(arg_type == type::no_arg &&
				"opt::type::no_arg requires a void function with "
				"no arguments.");
	}

	argument(std::string&& long_arg
			, type arg_type = type::optional_arg
			, std::function<void(std::string&&)>&& one_arg_func
					= [](std::string&& s){}
			, char&& short_arg = '\0'
			, std::string&& description = "")
		: long_arg(long_arg)
		, arg_type(arg_type)
		, one_arg_func(one_arg_func)
		, short_arg(short_arg)
		, description(description)
		, parsed(false)
	{
		assert(arg_type == type::required_arg
				|| arg_type == type::optional_arg
				&& "opt::type::required_arg or opt::type::optional_arg "
				"requires a void function that accepts a const string&.");
	}

	argument(std::string&& long_arg
			, type arg_type = type::multi_arg
			, std::function<void(std::vector<std::string>&&)>&&
					multi_arg_func = [](std::vector<std::string>&& v){}
			, char&& short_arg = '\0'
			, std::string&& description = "")
		: long_arg(long_arg)
		, arg_type(arg_type)
		, multi_arg_func(multi_arg_func)
		, short_arg(short_arg)
		, description(description)
		, parsed(false)
	{
		assert(arg_type == type::multi_arg &&
				"opt::type::multi_arg requires a void function that "
				"accepts a const vector<string>&.");
	}
};

struct options {
	const bool exit_on_error;
	const std::string help_intro;
	const std::string help_outro;
	const argument first_argument;

	options(bool exit_on_error = true
			, std::string&& help_intro = ""
			, std::string&& help_outro = ""
			, argument&& first_argument = {"", type::no_arg, [](){}})
		: exit_on_error(exit_on_error)
		, help_intro(help_intro)
		, help_outro(help_outro)
		, first_argument(first_argument)
	{}
};

inline bool print_help(std::vector<argument>& args, options&& option) {
	std::cout << option.help_intro;
	std::cout << std::endl;
	for (const auto& x : args) {
		std::cout << std::setw(2) << "";

		if (x.short_arg != '\0') {
			std::cout << std::setw(5) << std::left
				<< std::string("-" + std::string(1, x.short_arg) + ",");
		} else {
			std::cout << std::setw(5) << "";
		}

		std::cout << std::setw(20) << std::left << "--" + x.long_arg;
		std::cout << x.description;

		std::cout << std::endl;
	}

	std::cout << std::endl;
	std::cout << option.help_outro;

	if (option.exit_on_error)
		exit(1);

	return false;
}

/* TODO: case insensitive. Default args. Equal sign. Nested? Unique args.
 * raw args. concatenate short args (ex. ls -la) */
inline bool parse_arguments(int argc, char** argv,
		std::vector<argument>& args, options&& option = {}) {

	for (size_t i = 1; i < argc; ++i) {

		/* Check single short arg and long args. */
		if ((strncmp(argv[i], "-", 1) == 0 && strlen(argv[i]) == 2)
				|| strncmp(argv[i], "--", 2) == 0) {
			size_t found = -1;
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
				return print_help(args, std::move(option));
			}

			if (args[found].parsed) {
				maybe_print_msg("'" + std::string(argv[i]) + "' already parsed.");
				return print_help(args, std::move(option));
			}

			argument& found_arg = args[found];
			found_arg.parsed = true;

			switch(found_arg.arg_type) {
				case type::no_arg: {
					found_arg.no_arg_func();
				} break;

				case type::required_arg: {
					if (i + 1 >= argc) {
						maybe_print_msg("'" + std::string(argv[i])
								+ "' requires 1 argument.");
						return print_help(args, std::move(option));
					}

					if (strncmp(argv[i + 1], "-", 1) == 0) {
						maybe_print_msg("'" + std::string(argv[i])
								+ "' requires 1 argument.");
						return print_help(args, std::move(option));
					}
					found_arg.one_arg_func(argv[++i]);
				} break;

				case type::optional_arg: {
					if (i + 1 >= argc) {
						found_arg.one_arg_func("");
						break;
					}

					if (strncmp(argv[i + 1], "-", 1) == 0) {
						found_arg.one_arg_func("");
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
				return print_help(args, std::move(option));
			}

			if (not_found.size() != 0) {
				maybe_print_msg("'-" + not_found + "' not found.");
				return print_help(args, std::move(option));
			}

			/* Accept duplicate flags because who cares. */
			std::sort(found_v.begin(), found_v.end());
			auto last = std::unique(found_v.begin(), found_v.end());
			found_v.erase(last, found_v.end());

			for (const auto& x : found_v) {
				if (args[x].parsed) {
					maybe_print_msg("'" + std::string(1, args[x].short_arg)
							+ "' already parsed.");
					return print_help(args, std::move(option));
				}

				if (args[x].arg_type != type::no_arg) {
					maybe_print_msg("'" + std::string(1, args[x].short_arg)
							+ "' requires argument.");
					return print_help(args, std::move(option));
				}
			}

			for (const auto& x : found_v) {
				args[x].parsed = true;
				args[x].no_arg_func();
			}
		}

	}

	return true;
}

} // namespace opt
