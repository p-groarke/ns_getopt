#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4

from bincrafters import build_template_default

if __name__ == "__main__":
	builder = build_template_default.get_builder()
	for settings, options, env_vars, build_requires, reference in builder.items:
		options.update({"ns_getopt:build_testing": "True"})
	builder.run()
