#! /usr/bin/env python
# encoding: utf-8

from waflib.Build import BuildContext

APPNAME = 'bourne'
VERSION = '9.0.0'


def build(bld):

    # Build static library if this is top-level otherwise just .o files
    features = ['cxx']
    if bld.is_toplevel():
        features += ['cxxstlib']

    bld(features=features,
        source=bld.path.ant_glob('src/**/*.cpp'),
        target='bourne',
        use=[],
        export_includes=['src'],
        install_path='${PREFIX}/lib')

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('test')
        bld.recurse('examples')

        source_path = bld.path.find_node('src')

        bld.install_files(
            dest="${PREFIX}/include",
            files=source_path.ant_glob('**/*.hpp'),
            cwd=source_path,
            relative_trick=True)

        bld.install_files(
            dest="${PREFIX}/",
            files=bld.path.ant_glob('NEWS.rst'))


class ReleaseContext(BuildContext):
    cmd = 'prepare_release'
    fun = 'prepare_release'


def prepare_release(ctx):
    """ Prepare a release. """

    # Rewrite versions
    with ctx.rewrite_file(filename="src/bourne/version.hpp") as f:

        pattern = r"#define STEINWURF_BOURNE_VERSION v\d+_\d+_\d+"
        replacement = "#define STEINWURF_BOURNE_VERSION v{}".format(
            VERSION.replace('.', '_'))

        f.regex_replace(pattern=pattern, replacement=replacement)

    with ctx.rewrite_file(filename="src/bourne/version.cpp") as f:
        pattern = r'return "\d+\.\d+\.\d+"'
        replacement = 'return "{}"'.format(VERSION)

        f.regex_replace(pattern=pattern, replacement=replacement)
