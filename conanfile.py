#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools
import os


class NsGetoptConan(ConanFile):
	name = "ns_getopt"
	version = "1.0.0"
	description = "Modern C++ argument parsing."
	url = "https://github.com/p-groarke/ns_getopt"
	license = "BSD3"
	exports = ["LICENSE"]
	exports_sources = ["CMakeLists.txt", "include/*", "tests/*", "cmake/*"]
	generators = "cmake"
	source_subfolder = "source_subfolder"
	build_subfolder = "build_subfolder"
	settings = "os", "arch", "compiler", "build_type"
	requires = "catch2/2.2.2@bincrafters/stable"
	options = {"build_testing": [True, False]}
	default_options = ("build_testing=False")

	def build(self):
		cmake = CMake(self)
		cmake.definitions["BUILD_TESTING"] = self.options.build_testing
		cmake.configure()
		cmake.build()
		if self.options.build_testing:
			cmake.test()

	def package(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.install()

	def package_info(self):
		self.info.header_only()
