======
bourne
======

.. image:: https://travis-ci.org/steinwurf/bourne.svg?branch=master
    :target: https://travis-ci.org/steinwurf/bourne
    
.. image:: https://raw.githubusercontent.com/steinwurf/bourne/master/bourne.gif

A simple c++11 json reader and writer.

.. contents:: Table of Contents:
   :local:

Usage
=====

Usage examples are located in the examples folder.

Build
=====

Fetch the sources:

::

   git clone https://github.com/steinwurf/bourne
   cd bourne

We use the ``waf`` build system to build the bourne static library. We
have some additional waf tools which can be found at `waf
<https://github.com/steinwurf/waf>`_.

To configure and build bourne, run the following commands:

::

   python waf configure
   python waf build
   python waf install --destdir=/tmp

The final ``install`` step will create a folder containing all the
nessecary files needed to use the library (e.g. static library,
headers etc.). You can change the output folder by passing a different
path to ``--destdir``:

Use as Dependency in CMake
==========================

To depend on this project when using the CMake build system, add the following
in your CMake build script:

::

   add_subdirectory("/path/to/bourne" bourne)
   target_link_libraries(<my_target> steinwurf::bourne)

Where ``<my_target>`` is replaced by your target.

Credit
======

The code for this library have been inspired by the
`SimpleJSON <https://github.com/nbsdx/SimpleJSON/tree/f909b2433ada7e5ccaf4753ac74d82703a50547f>`_
library.
