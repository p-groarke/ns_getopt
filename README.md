# ns_getopt
C++ argument parsing that doesn't suck.

## Philosophy
ns_getopt aims to provide flexible argument parsing, with as little as possible work required from you. It supports long args, short args, raw args (ex. files), concatenated short args and multi args. Help is automatically generated and pretty. You wont have to update a `print_help()` method every time you change an argument. The code tries to minimize string copies where possible, but performance is not currently a top priority.

## Build and Install
Include the .h file directly, or install through cmake (todo).

## Example
ns_getopt minimizes the quantity of work you have to do when parsing arguments. The goal is to have a simple argument parsing solution up-and-runnning as fast as possible. It calls provided `std::function`s when an argument is detected. If there is an error, it will output a specific message to the user (can be disabled), print help and return false. Some behaviors can be modified if you provide a `opt::options` struct.

### Step 1
Declare your arguments and their appropriate functions.

```c++
std::vector<opt::argument> args = {
	{"test", opt::type::no_arg, [](){}, 't', "This is a simple flag."}
	, {"requiredarg", opt::type::required_arg, [](std::string&& s){}, '\0',
			"This argument requires a value."}
	, {"optional", opt::type::optional_arg, [](std::string&& s){}, 'o',
			"This parameter has an optional argument."}
	, {"default", opt::type::default_arg, [](std::string&& s){}, 'd',
			"An example of an argument with default value.", "my_default_val"}
	, {"multi", opt::type::multi_arg, [](std::vector<std::string>&& v){}, 'm',
			"This accepts multiple, space seperated, values.\n"
			"You can also have long descriptions that get\n"
			"automatically aligned simply by using \\n in\n"
			"your description."}
	, {"in_file", opt::type::raw_arg, [](std::string&& s){},
			"Description for file 1.\nIt can be multiple\nlines too."}
	, {"out_file", opt::type::raw_arg, [](std::string&& s){},
			"Description for out_file. Raw arguments are optional."}
};
```

### Step 2 (optional)
You can define a few options, like an intro text or outro text, whether the app is killed when there is an error parsing arguments or an `opt::argument` for the first argument (always your executable name).

```c++
opt::options o = {"A wonderful example.\nTalented Author\n"
		, "More info on github.\n"};
```

### Step 3
Call `bool parse_arguments(int argc, char** argv, std::vector<argument>& args, const options& option = {})`

```c++
bool succeeded = opt::parse_arguments(argc, argv, args, o);
```
TODO: Show created help.

That's pretty much it for a simple application. Enjoy!


# Documentation

## opt::type
This `enum class` lists supported argument types. Choices are:

```c++
opt::type::no_arg
opt::type::required_arg
opt::type::optional_arg
opt::type::default_arg
opt::type::multi_arg
opt::type::raw_arg
```

## User callbacks
3 callback types need to be provided by the user, here are their respective types.

#### opt::type::no_arg
A void function/lambda/method that takes no arguments.
```c++
std::function<void()> // void fun() {}
```

#### opt::type::required_arg, opt::type::optional_arg, opt::type::default_arg, opt::type::raw_arg
A void function/lambda/method that accepts a moved string.
```c++
std::function<void(std::string&&)> // void fun(std::string&& s) {}
```

#### opt::type::multi_arg
A void function/lambda/method that accepts a moved vector of strings.
```c++
std::function<void(std::vector<std::string>&&)> // void fun(std::vector<std::string>&& v) {}
```

## opt::argument
There are many ways to create your argument, depending on the type it is.

#### opt::type::no_arg
```c++
argument(std::string&& long_arg
		, type arg_type = type::no_arg
		, const std::function<void()>& no_arg_func = [](){}
		, char&& short_arg = '\0'
		, std::string&& description = "")
```

#### opt::type::required_arg, opt::type::optional_arg, opt::type::default_arg
```c++
argument(std::string&& long_arg
		, type arg_type = type::optional_arg
		, const std::function<void(std::string&&)>& one_arg_func
				= [](std::string&& s){}
		, char&& short_arg = '\0'
		, std::string&& description = ""
		, std::string&& default_arg = "")
```

#### opt::type::multi_arg
```c++
argument(std::string&& long_arg
		, type arg_type = type::multi_arg
		, const std::function<void(std::vector<std::string>&&)>&
				multi_arg_func = [](std::vector<std::string>&& v){}
		, char&& short_arg = '\0'
		, std::string&& description = "")
```

#### opt::type::raw_arg
```c++
argument(std::string&& name
		, type arg_type = type::raw_arg
		, const std::function<void(std::string&&)>& one_arg_func
				= [](std::string&& s){}
		, std::string&& description = "")
```

## opt::options
You can provide options to the parse function, here are the available options.

TODO: exit_on_error should be false by default.

```c++
options(std::string&& help_intro = ""
		, std::string&& help_outro = ""
		, argument&& first_argument = {"", type::no_arg, [](){}}
		, bool exit_on_error = true
		, int exit_code = -1)
```

## print_help
You can print the help whenever you wish, just call void `print_help(std::vector<argument>& args, const options& option, char* arg0)` with your argument vector, your options and the first argument (required for the "Usage: ..." output).

TODO: Default options!

```c++
opt::print_help(args, o, argv[0]);
```

Because lets be honest, getopt is fucking wank.

