from conan.packager import ConanMultiPackager


if __name__ == "__main__":
    builder = ConanMultiPackager()
    builder.add_common_builds()
    for settings, options, env_vars, build_requires, reference in builder.items:
        options.update({"ns_getopt:build_testing": "True"})
    builder.run()
