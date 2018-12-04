News for bourne
===============

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
* Minor: Added const version of ``operator[]`` accessors.
* Major: Made ``parser::parse`` static, and added an error code.
* Major: Moved details into detail namespace.

5.0.0
-----
* Major: Upgrade to waf-tools 4
* Minor: Upgrade to gtest 4

4.1.0
-----
* Minor: Allow integers to be converted implicitly to floating point values.

4.0.0
-----
* Minor: Added templated ``to`` and ``is`` function for checking and getting
  fields of certain types.
* Major: Removed length function, use size instead.
* Major: Values are no longer silently converted to array values if append is
  used.
* Minor: Added more comments.

3.0.0
-----
* Major: Replaced ``to_[type](bool)`` functions with a combination of
  ``to_[type]`` and ``is_[type]``.
* Minor: Added ``is_bool``, ``is_int``, ``is_float``, ``is_string``, ``is_object``, and
  ``is_array``.
* Patch: Fixed memory leak on reassignment.

2.0.0
-----
* Major: Removed templated constructor. This has been done to prevent slient,
  infinite append-loop when accidentally providing a none bourne::json
  convertable object to constructor.

1.1.0
-----
* Minor: Added equality operator.

1.0.0
-----
* Major: Initial release.
