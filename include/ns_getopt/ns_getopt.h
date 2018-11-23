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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/

#pragma once
#include <array>
#include <cassert>
#include <cctype> // std::tolower
#include <cstdio>
#include <cstring>
#include <functional>
#include <string_view>

namespace opt {

/* Default multi argument array. */
using multi_array = std::array<std::string_view, 8>; // TODO: Size build option.
/* Used for stack strings (char s[N]). */
constexpr size_t stack_string_size = 128;

/* List of argument types. */
enum class type : std::uint8_t {
	no_arg,
	required_arg,
	optional_arg,
	default_arg,
	multi_arg,
	raw_arg
};

/* User argument. */
struct argument {
	const std::function<bool()> no_arg_func;
	const std::function<bool(std::string_view)> one_arg_func;
	const std::function<bool(multi_array&&, size_t)> multi_arg_func;
	const std::string_view long_arg;
	const std::string_view description;
	const std::string_view default_arg;
	const size_t multi_max_len;
	int raw_arg_pos;
	const char short_arg;
	const type arg_type;
	bool parsed;

	inline argument(std::string_view long_arg, type arg_type,
			const std::function<bool()>& no_arg_func,
			std::string_view description = "", char&& short_arg = '\0');

	inline argument(std::string_view long_arg, type arg_type,
			const std::function<bool(std::string_view)>& one_arg_func,
			std::string_view description = "", char&& short_arg = '\0',
			std::string_view default_arg = "");

	inline argument(std::string_view long_arg, type arg_type,
			const std::function<bool(multi_array&&, size_t)>&,
			size_t multi_max_subargs, std::string_view description = "",
			char&& short_arg = '\0');

	inline void asserts();
};

enum flag : unsigned int {
	none = 0,
	no_user_error_messages = 1,
	exit_on_error = 2,
	arguments_are_optional = 4,
	arg0_is_normal_argument = 8,
	dont_print_help = 16
};

inline flag operator|(flag lhs, flag rhs);
inline flag& operator|=(flag& lhs, flag rhs);

/* Configuration options. */
struct options {
	const std::function<bool(std::string_view)> first_argument_func;
	const std::string_view help_intro;
	const std::string_view help_outro;
	const int exit_code;
	const flag flags;

	inline options(
			std::string_view help_intro = "", std::string_view help_outro = "",
			flag flags = flag::none,
			const std::function<bool(std::string_view)>& first_argument_func
			= [](std::string_view) { return true; },
			int exit_code = -1);

	inline ~options(){}; // Fix clang < 4.0
};

template <size_t args_size>
inline void print_help(const std::array<argument, args_size>& args,
		const char* arg0, const options& option);

template <size_t args_size>
inline void print_help(const argument (&args)[args_size], const char* arg0,
		const options& option);

inline void print_help(const argument* args, size_t args_size, const char* arg0,
		const options& option);

template <size_t args_size>
inline bool parse_arguments(int argc, char const* const* argv,
		std::array<argument, args_size>& args, const options& option = {});

template <size_t args_size>
inline bool parse_arguments(int argc, char const* const* argv,
		argument (&args)[args_size], const options& option = {});

template <size_t args_size>
inline bool parse_arguments(int argc, char const* const* argv, argument* args,
		const options& option = {});

namespace detail {

template <size_t N = 128>
struct basic_stack_string {
	const char* c_str() const;
	size_t size() const;

	template <size_t N2>
	basic_stack_string<N>& operator+=(const basic_stack_string<N2> rhs);
	basic_stack_string<N>& operator+=(std::string_view rhs);
	basic_stack_string<N>& operator+=(char rhs);
	basic_stack_string<N>& operator+=(const char* rhs);

private:
	char _data[N] = {};
	const size_t _max_size = N - 1;
	size_t _head = 0;
};
using stack_string = basic_stack_string<>;

template <size_t N>
std::ostream& operator<<(std::ostream& os, const basic_stack_string<N>& obj) {
	assert(obj.c_str()[N - 1] == 0);
	return os << obj.c_str();
}

template <class... Args>
stack_string make_stack_string(Args&&... args) {
	stack_string ret;
	return ((ret += args), ...);
}

inline void print_description(std::string_view s, size_t indentation);

inline bool do_exit(const argument* args, size_t args_size,
		const options& option, const char* arg0);

inline bool char_compare_no_case(unsigned char lhs, unsigned char rhs);

inline bool compare_no_case(const char* lhs, std::string_view rhs,
		const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0);

inline bool compare_no_case(std::string_view lhs, std::string_view rhs,
		const size_t lhs_start_pos = 0, const size_t rhs_start_pos = 0);

inline void maybe_print_msg(const options& option, stack_string msg);
inline void maybe_print_msg(const options& option, std::string_view msg);

inline bool has_flag(const flag flags, flag flag_to_check);

} // namespace detail


/* Implementation. */
inline argument::argument(std::string_view long_arg, type arg_type,
		const std::function<bool()>& no_arg_func, std::string_view description,
		char&& short_arg)
		: no_arg_func(no_arg_func)
		, long_arg(long_arg)
		, description(description)
		, multi_max_len(0)
		, raw_arg_pos(-1)
		, short_arg(short_arg)
		, arg_type(arg_type)
		, parsed(false) {
	assert(arg_type == type::no_arg);
	asserts();
}

inline argument::argument(std::string_view long_arg, type arg_type,
		const std::function<bool(std::string_view)>& one_arg_func,
		std::string_view description, char&& short_arg,
		std::string_view default_arg)
		: one_arg_func(one_arg_func)
		, long_arg(long_arg)
		, description(description)
		, default_arg(default_arg)
		, multi_max_len(0)
		, raw_arg_pos(-1)
		, short_arg(short_arg)
		, arg_type(arg_type)
		, parsed(false) {
	assert(arg_type == type::required_arg || arg_type == type::optional_arg
			|| arg_type == type::default_arg || arg_type == type::raw_arg);
	asserts();
}

inline argument::argument(std::string_view long_arg, type arg_type,
		const std::function<bool(multi_array&&, size_t)>& multi_arg_func,
		size_t multi_max_subargs, std::string_view description,
		char&& short_arg)
		: multi_arg_func(multi_arg_func)
		, long_arg(long_arg)
		, description(description)
		, multi_max_len(multi_max_subargs)
		, raw_arg_pos(-1)
		, short_arg(short_arg)
		, arg_type(arg_type)
		, parsed(false) {
	assert(arg_type == type::multi_arg);
	asserts();
}

inline void argument::asserts() {
	assert(long_arg.find(" ") == std::string_view::npos
			&& "One does not simply use spaces in his arguments.");
}

inline options::options(std::string_view help_intro,
		std::string_view help_outro, flag flags,
		const std::function<bool(std::string_view)>& first_argument_func,
		int exit_code)
		: first_argument_func(first_argument_func)
		, help_intro(help_intro)
		, help_outro(help_outro)
		, exit_code(exit_code)
		, flags(flags) {
}

template <size_t args_size>
inline void print_help(const std::array<argument, args_size>& args,
		const char* arg0, const options& option) {
	print_help(args.data(), args.size(), arg0, option);
}

template <size_t args_size>
inline void print_help(const argument (&args)[args_size], const char* arg0,
		const options& option) {
	print_help(args, args_size, arg0, option);
}

inline void print_help(const argument* args, size_t args_size, const char* arg0,
		const options& option) {
	using namespace detail;

	const size_t first_space = 1;
	const size_t sa_width = 4;
	const size_t sa_total_width = first_space + sa_width;
	const size_t la_space = 2;
	const size_t la_width_max = 30;
	const size_t ra_space = 4;
	const std::string_view opt_str = " <optional>";
	const std::string_view req_str = " <value>";
	const std::string_view multi_str = " <multiple>";
	const std::string_view default_beg = " <=";
	const std::string_view default_end = ">";

	printf("%.*s\n", (int)option.help_intro.size(), option.help_intro.data());

	{ /* Usage. */
		bool args_optional
				= has_flag(option.flags, flag::arguments_are_optional);
		stack_string raw_args;
		bool first = args_optional;
		for (const argument* x = args; x < args + args_size; x++) {
			if (x->arg_type == type::raw_arg) {
				raw_args += (first ? " [" : " ");
				raw_args += x->long_arg;
				first = false;
			}
		}
		if (args_optional && raw_args.size() > 0) {
			raw_args += "]";
		}

		printf("\nUsage: %s%s [options]\n\n", arg0, raw_args.c_str());
	}

	{ /* Raw args. */
		bool has_raw_args = false;
		size_t name_width = 0;
		for (const argument* x = args; x < args + args_size; x++) {
			if (x->arg_type != type::raw_arg)
				continue;

			has_raw_args = true;
			size_t s = x->long_arg.size() + ra_space;
			if (s > name_width)
				name_width = s;
		}

		if (has_raw_args)
			printf("Arguments:\n");

		for (const argument* x = args; x < args + args_size; x++) {
			if (x->arg_type != type::raw_arg)
				continue;
			printf("%*s", (int)first_space, "");
			printf("%-*.*s", (int)name_width, (int)x->long_arg.size(),
					x->long_arg.data());
			print_description(x->description, first_space + name_width);
		}
		if (has_raw_args)
			printf("\n");
	}

	{ /* Other args.*/
		printf("Options:\n");
		size_t la_width = 0;
		for (const argument* x = args; x < args + args_size; x++) {
			if (x->arg_type == type::raw_arg)
				continue;

			size_t s = 2 + x->long_arg.size() + la_space;
			if (x->arg_type == type::optional_arg) {
				s += opt_str.size();
			} else if (x->arg_type == type::required_arg) {
				s += req_str.size();
			} else if (x->arg_type == type::default_arg) {
				s += default_beg.size() + x->default_arg.size()
						+ default_end.size();
			} else if (x->arg_type == type::multi_arg) {
				s += multi_str.size();
			}

			if (s > la_width)
				la_width = s;
		}

		if (la_width > la_width_max) {
			la_width = la_width_max;
		}

		for (const argument* x = args; x < args + args_size; x++) {
			if (x->arg_type == type::raw_arg)
				continue;

			printf("%*s", (int)first_space, "");

			if (x->short_arg != '\0') {
				stack_string s_arg;
				s_arg += "-";
				s_arg += x->short_arg;
				s_arg += ",";
				printf("%-*s", (int)sa_width, s_arg.c_str());
			} else {
				printf("%*s", (int)sa_width, "");
			}

			stack_string la_str;
			la_str += "--";
			la_str += x->long_arg;
			if (x->arg_type == type::optional_arg) {
				la_str += opt_str;
			} else if (x->arg_type == type::required_arg) {
				la_str += req_str;
			} else if (x->arg_type == type::default_arg) {
				la_str += default_beg;
				la_str += x->default_arg;
				la_str += default_end;
			} else if (x->arg_type == type::multi_arg) {
				la_str += multi_str;
			}

			printf("%-*s", (int)la_width, la_str.c_str());

			// TODO: Verify comparison is still ok (size works as exapected).
			if (la_str.size() >= la_width) {
				printf("\n");
				printf("%*s", (int)(la_width + sa_total_width), "");
			}

			print_description(x->description, la_width + sa_total_width);
		}

		if (la_width == 0) // No options, width is --help only.
			la_width = 2 + 4 + la_space;

		printf("%*s%-*s%-*s%s\n", (int)first_space, "", (int)sa_width, "-h,",
				(int)la_width, "--help", "Print this help\n");

		printf("\n%.*s\n", (int)option.help_outro.size(),
				option.help_outro.data());
	}
}

template <size_t args_size>
inline bool parse_arguments(int argc, char const* const* argv,
		std::array<argument, args_size>& args, const options& option) {
	return parse_arguments<args_size>(argc, argv, args.data(), option);
}

template <size_t args_size>
inline bool parse_arguments(int argc, char const* const* argv,
		argument (&args)[args_size], const options& option) {
	return parse_arguments<args_size>(argc, argv, (argument*)args, option);
}

/**
 * TODO: Equal sign. Unique args (asserts)? Required raw_args?
 * Remove non-const stuff in argument?
 * User lambdas return bool (success).
 **/
template <size_t args_size>
inline bool parse_arguments(int argc, char const* const* argv, argument* args,
		const options& option) {
	using namespace detail;

	/* Prepare raw_args, they are parsed in declared order. */
	int parsed_raw_args = 0;
	int raw_args_count = 0;
	for (argument* x = args; x < args + args_size; x++) {
		if (x->arg_type == type::raw_arg)
			x->raw_arg_pos = raw_args_count++;
	}

	for (int i = 0; i < argc; ++i) {
		/* First argument is a special snowflake. */
		if (i == 0 && !has_flag(option.flags, flag::arg0_is_normal_argument)) {
			if (argc == 1
					&& !has_flag(option.flags, flag::arguments_are_optional)) {
				return do_exit(args, args_size, option, argv[0]);
			} else {
				option.first_argument_func(argv[i]);
			}
		}

		/* Help. */
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0
				|| strcmp(argv[i], "/?") == 0) {
			return do_exit(args, args_size, option, argv[0]);
		}

		/* Check single short arg and long args. */
		else if ((strncmp(argv[i], "-", 1) == 0 && strlen(argv[i]) == 2)
				|| strncmp(argv[i], "--", 2) == 0) {
			int found = -1;
			for (int j = 0; j < (int)args_size; ++j) {
				if (compare_no_case(argv[i], args[j].long_arg, 2)) {
					found = j;
					break;
				}
			}

			if (found == -1) {
				for (int j = 0; j < (int)args_size; ++j) {
					if (argv[i][1] == args[j].short_arg) {
						found = j;
						break;
					}
				}
			}

			if (found == -1) {
				maybe_print_msg(option,
						make_stack_string("'", argv[i], "' not found."));
				return do_exit(args, args_size, option, argv[0]);
			}

			if (args[found].parsed) {
				maybe_print_msg(option,
						make_stack_string("'", argv[i], "' already parsed."));
				return do_exit(args, args_size, option, argv[0]);
			}

			argument& found_arg = args[found];
			found_arg.parsed = true;
			std::string_view default_arg = found_arg.default_arg;

			switch (found_arg.arg_type) {
			case type::no_arg: {
				if (!found_arg.no_arg_func()) {
					maybe_print_msg(option,
							make_stack_string("problem parsing option."));
					return do_exit(args, args_size, option, argv[0]);
				}
			} break;

			case type::required_arg: {
				if (i + 1 >= argc) {
					maybe_print_msg(option,
							make_stack_string(
									"'", argv[i], "' requires 1 argument."));
					return do_exit(args, args_size, option, argv[0]);
				}

				if (strncmp(argv[i + 1], "-", 1) == 0) {
					maybe_print_msg(option,
							make_stack_string(
									"'", argv[i], "' requires 1 argument."));
					return do_exit(args, args_size, option, argv[0]);
				}
				if (!found_arg.one_arg_func(argv[++i])) {
					maybe_print_msg(option,
							make_stack_string("'", argv[i - 1],
									"' problem parsing argument."));
					return do_exit(args, args_size, option, argv[0]);
				}
			} break;

			case type::optional_arg: {
				default_arg = "";
				[[fallthrough]];
			}
			case type::default_arg: {
				if (i + 1 >= argc) {
					if (!found_arg.one_arg_func(std::move(default_arg))) {
						maybe_print_msg(option,
								make_stack_string("problem parsing option."));
						return do_exit(args, args_size, option, argv[0]);
					}
					break;
				}

				if (strncmp(argv[i + 1], "-", 1) == 0) {
					if (!found_arg.one_arg_func(std::move(default_arg))) {
						maybe_print_msg(option,
								make_stack_string("problem parsing option."));
						return do_exit(args, args_size, option, argv[0]);
					}
					break;
				}

				if (!found_arg.one_arg_func(argv[++i])) {
					maybe_print_msg(option,
							make_stack_string("problem parsing option."));
					return do_exit(args, args_size, option, argv[0]);
				}
			} break;

			case type::multi_arg: {
				multi_array a;
				size_t current_multi_arg = 0;

				while (i + 1 < argc) {
					if (strncmp(argv[i + 1], "-", 1) == 0) {
						break;
					}
					a[current_multi_arg] = argv[++i];
					++current_multi_arg;

					if (current_multi_arg - 1 >= found_arg.multi_max_len) {
						char buf[3] = {};
						snprintf(buf, sizeof(buf), "%zu",
								found_arg.multi_max_len);
						maybe_print_msg(option,
								make_stack_string("'", found_arg.long_arg,
										"' only supports ", buf,
										" arguments."));
						return do_exit(args, args_size, option, argv[0]);
					}
				}
				if (!found_arg.multi_arg_func(
							std::move(a), current_multi_arg)) {
					maybe_print_msg(option,
							make_stack_string(
									"problem parsing multi-arguments."));
					return do_exit(args, args_size, option, argv[0]);
				}
			} break;

			default: {
				// assert(false && "Something went horribly wrong.");
				maybe_print_msg(
						option, make_stack_string("problem parsing options."));
				return do_exit(args, args_size, option, argv[0]);
			};
			}
		}

		/* Concatenated short args. */
		else if (strncmp(argv[i], "-", 1) == 0 && strlen(argv[i]) > 2) {
			std::array<int, args_size> found_v;
			found_v.fill(args_size + 1);
			size_t found_size = 0;
			stack_string not_found;
			for (size_t j = 1; j < strlen(argv[i]); ++j) {
				bool found = false;
				for (int k = 0; k < (int)args_size; ++k) {
					if (argv[i][j] == args[k].short_arg) {
						found_v[found_size] = k;
						++found_size;
						found = true;
						break;
					}
				}
				if (!found) {
					not_found += argv[i][j];
				}
			}

			if (found_size == 0) {
				maybe_print_msg(option,
						make_stack_string("'", argv[i], "' not found."));
				return do_exit(args, args_size, option, argv[0]);
			}

			if (not_found.size() != 0) {
				maybe_print_msg(option,
						make_stack_string(
								"'", not_found.c_str(), "' not found."));
				return do_exit(args, args_size, option, argv[0]);
			}

			/* Accept duplicate flags because who cares. */
			std::sort(found_v.begin(), found_v.end());
			auto last = std::unique(found_v.begin(), found_v.end());
			found_size = last - found_v.begin() - 1;

			for (size_t j = 0; j < found_size; ++j) {
				const auto& x = found_v[j];
				if (args[x].parsed) {
					maybe_print_msg(option,
							make_stack_string("'", args[x].short_arg,
									"' already parsed."));
					return do_exit(args, args_size, option, argv[0]);
				}

				if (!(args[x].arg_type == type::no_arg
							|| args[x].arg_type == type::optional_arg
							|| args[x].arg_type == type::default_arg)) {

					maybe_print_msg(option,
							make_stack_string("'", args[x].short_arg,
									"' unsupported in concatenated short "
									"arguments."));
					return do_exit(args, args_size, option, argv[0]);
				}
			}

			for (size_t j = 0; j < found_size; ++j) {
				const auto& x = found_v[j];
				if (args[x].arg_type == type::no_arg) {
					args[x].parsed = true;
					if (!args[x].no_arg_func()) {
						maybe_print_msg(option,
								make_stack_string("'", args[x].short_arg,
										"' problem parsing option."));
						return do_exit(args, args_size, option, argv[0]);
					}
				} else if (args[x].arg_type == type::optional_arg) {
					args[x].parsed = true;
					if (!args[x].one_arg_func("")) {
						maybe_print_msg(option,
								make_stack_string("'", args[x].short_arg,
										"' problem parsing argument."));
						return do_exit(args, args_size, option, argv[0]);
					}
				} else if (args[x].arg_type == type::default_arg) {
					args[x].parsed = true;
					if (!args[x].one_arg_func(args[x].default_arg)) {
						maybe_print_msg(option,
								make_stack_string("'", args[x].short_arg,
										"' problem parsing argument."));
						return do_exit(args, args_size, option, argv[0]);
					}
				} else {
					maybe_print_msg(option,
							make_stack_string("'", args[x].short_arg,
									"' problem parsing argument."));
					return do_exit(args, args_size, option, argv[0]);
				}
			}
		}

		/* Check raw args. */
		else if (parsed_raw_args < raw_args_count) {
			int found = -1;
			for (int j = 0; j < (int)args_size; ++j) {
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
			if (!found_arg.one_arg_func(argv[i])) {
				maybe_print_msg(option,
						make_stack_string(
								"'", argv[i], "' problem parsing argument."));
				return do_exit(args, args_size, option, argv[0]);
			}
		}

		/* Everything failed. */
		else {
			maybe_print_msg(
					option, make_stack_string("'", argv[i], "' unrecognized."));
			return do_exit(args, args_size, option, argv[0]);
		}
	}

	return true;
}

inline flag operator|(flag lhs, flag rhs) {
	return static_cast<flag>(
			static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline flag& operator|=(flag& lhs, flag rhs) {
	lhs = static_cast<flag>(
			static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
	return lhs;
}

/* Internal functions. */
namespace detail {

template <size_t N>
inline const char* basic_stack_string<N>::c_str() const {
	assert(_data[N - 1] == 0);
	return _data;
}

template <size_t N>
inline size_t basic_stack_string<N>::size() const {
	assert(_head < N);
	return _head;
}

template <size_t N>
template <size_t N2>
inline basic_stack_string<N>& basic_stack_string<N>::operator+=(
		const basic_stack_string<N2> rhs) {
	assert(rhs.c_str()[N2 - 1] == 0);
	return this->operator+=(rhs.c_str());
}

template <size_t N>
inline basic_stack_string<N>& basic_stack_string<N>::operator+=(
		std::string_view rhs) {
	if (rhs[rhs.size() - 1] != 0) {
		char safe_rhs[N] = {};
		size_t len = std::min(N - 1, rhs.size());
		for (size_t i = 0; i < len; ++i) {
			safe_rhs[i] = rhs[i];
		}
		return this->operator+=(safe_rhs);
	}

	return this->operator+=(rhs.data());
}

template <size_t N>
inline basic_stack_string<N>& basic_stack_string<N>::operator+=(char rhs) {
	char buf[2] = {};
	buf[0] = rhs;
	return this->operator+=(buf);
}

template <size_t N>
inline basic_stack_string<N>& basic_stack_string<N>::operator+=(
		const char* rhs) {
	memcpy(_data + _head, rhs, _max_size - _head);
	assert(_data[N - 1] == 0);

	_head = (_head + strlen(rhs)) % _max_size;
	assert(_head <= N - 1);
	return *this;
}


inline void print_description(std::string_view s, size_t indentation) {
	if (s.size() == 0)
		return;

	// There is no \n.
	if (s.find('\n') == std::string_view::npos) {
		printf("%.*s\n", (int)s.size(), s.data());
		return;
	}

	size_t pos = 0;
	for (size_t found_pos;
			(found_pos = s.find('\n', pos)) != std::string_view::npos;) {
		std::string_view out = s.substr(pos, found_pos - pos);
		printf("%.*s\n", (int)out.size(), out.data());
		pos = found_pos + 1;

		// Look ahead.
		if (s.find('\n', pos) != std::string_view::npos) {
			printf("%*s", (int)indentation, "");
		}
	}

	// Still text left to print.
	if (pos < s.size()) {
		printf("%*s", (int)indentation, "");
		std::string_view out = s.substr(pos, s.size() - pos);
		printf("%.*s\n", (int)out.size(), out.data());
	}
}

inline bool do_exit(const argument* args, size_t args_size,
		const options& option, const char* arg0) {
	if (!has_flag(option.flags, flag::dont_print_help)) {
		print_help(args, args_size, arg0, option);
	}

	if (has_flag(option.flags, flag::exit_on_error))
		exit(option.exit_code);
	return false;
}

inline bool char_compare_no_case(unsigned char lhs, unsigned char rhs) {
	return std::tolower(lhs) == std::tolower(rhs);
}

inline bool compare_no_case(const char* lhs, std::string_view rhs,
		const size_t lhs_start_pos, const size_t rhs_start_pos) {
	if (lhs_start_pos >= strlen(lhs))
		return false;

	if (rhs_start_pos >= rhs.size())
		return false;

	if (strlen(lhs) - lhs_start_pos == rhs.size() - rhs_start_pos) {
		return std::equal(lhs + lhs_start_pos, lhs + strlen(lhs),
				rhs.begin() + rhs_start_pos, char_compare_no_case);
	}
	return false;
}

inline bool compare_no_case(std::string_view lhs, std::string_view rhs,
		const size_t lhs_start_pos, const size_t rhs_start_pos) {
	return compare_no_case(lhs.data(), rhs, lhs_start_pos, rhs_start_pos);
}

inline void maybe_print_msg(const options& option, stack_string msg) {
	maybe_print_msg(option, msg.c_str());
}

inline void maybe_print_msg(const options& option, std::string_view msg) {
	if (has_flag(option.flags, flag::no_user_error_messages))
		return;
	printf("%.*s\n", (int)msg.size(), msg.data());
}

inline bool has_flag(const flag flags, flag flag_to_check) {
	return (flags & (flag_to_check)) != 0;
}

} // namespace detail
} // namespace opt
