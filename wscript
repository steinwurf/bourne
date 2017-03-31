#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'bourne'
VERSION = '4.1.0'


def build(bld):

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_BOURNE_VERSION="{}"'.format(
            VERSION))

    bld.recurse('src/bourne')

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('test')
        bld.recurse('examples')
