#! /usr/bin/env python
# encoding: utf-8

import os
import hashlib
import shutil
import platform
import waflib
import pathlib
import subprocess

from waflib.Build import BuildContext

APPNAME = "bourne"
VERSION = "10.1.0"


def options(opt):
    opt.load("cmake")


def configure(conf):
    conf.load("cmake")

    if conf.is_toplevel():
        conf.cmake_configure()


def build(bld):
    bld.load("cmake")

    if bld.is_toplevel():
        bld.cmake_build()


class ReleaseContext(BuildContext):
    cmd = "prepare_release"
    fun = "prepare_release"


def prepare_release(ctx):
    """Prepare a release."""

    # Rewrite versions
    with ctx.rewrite_file(filename="src/bourne/version.hpp") as f:
        pattern = r"#define STEINWURF_BOURNE_VERSION v\d+_\d+_\d+"
        replacement = "#define STEINWURF_BOURNE_VERSION v{}".format(
            VERSION.replace(".", "_")
        )

        f.regex_replace(pattern=pattern, replacement=replacement)

    with ctx.rewrite_file(filename="src/bourne/version.cpp") as f:
        pattern = r'return "\d+\.\d+\.\d+"'
        replacement = 'return "{}"'.format(VERSION)

        f.regex_replace(pattern=pattern, replacement=replacement)


def docs(ctx):
    """Build the documentation in a virtualenv"""

    with ctx.create_virtualenv() as venv:
        # To update the requirements.txt just delete it - a fresh one
        # will be generated from test/requirements.in
        if not os.path.isfile("docs/requirements.txt"):
            venv.run("python -m pip install pip-tools")
            venv.run("pip-compile docs/requirements.in")

        venv.run("python -m pip install -r docs/requirements.txt")

        build_path = os.path.join(ctx.path.abspath(), "build", "site", "docs")

        venv.run(
            "giit clean . --build_path {}".format(build_path), cwd=ctx.path.abspath()
        )
        venv.run(
            "giit sphinx . --build_path {}".format(build_path), cwd=ctx.path.abspath()
        )
