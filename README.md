# ns_getopt

ns_getopt has been superceeded by [fea_getopt](https://github.com/p-groarke/fea_getopt/).

My needs have evolved quite a lot since writing ns_getopt, and as such, this library wasn't good enough anymore. ns_getopt is still useable, and if you need stack-only argument parsing, this is a great starting point (all one needs to do is replace std::function with std::inplace_function from sg14).

fea_getopt is built from the ground up to support utf8, utf16 or utf32. It is also covered by some much better unit tests, including expansive fuzzing. Making ns_getopt "optimized" was the biggest mistake I made in terms of readability and debuggability. As such, fea_getopt doesn't care and uses all the fancy heap allocated things it feels like to improve redability, robustness and debuggability. fea_getopt is easier to use since the api has nicely named functions to add your callbacks. It will also beautifully wrap long descriptions that exceed your command prompt width. Finally, it will make you sandwich if you ask nice enough.

The philosophy and usage stays the same though. You provide callbacks that return whether or not the arguments were valid.