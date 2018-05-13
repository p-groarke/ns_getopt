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
    # source_subfolder = "source_subfolder"
    # build_subfolder = "build_subfolder"
    settings = "os", "arch", "compiler", "build_type"
    requires = "catch2/2.2.2@bincrafters/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure()#build_folder=self.build_subfolder)
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.configure()#build_folder=self.build_subfolder)
        cmake.install()
        
    def package_info(self):
    	self.info.header_only()
