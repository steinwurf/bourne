#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'bourne'
VERSION = '6.0.0'


def build(bld):

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_BOURNE_VERSION="{}"'.format(
            VERSION))

    # Build static library if this is top-level otherwise just .o files
    features = ['cxx']
    if bld.is_toplevel():
        features += ['cxxstlib']

    bld(features=features,
        source=bld.path.ant_glob('src/**/*.cpp'),
        target='bourne',
        use=[],
        export_includes=['src'])

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('test')
        bld.recurse('examples')
