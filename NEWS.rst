News for bourne
===============

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
* tbd

8.0.0
-----
* Major: Change cmake build to be object library based.

7.4.0
-----
* Minor: Added install step to CMake.

7.3.2
-----
* Patch: Use `std::size_t` instead of `uint32_t`.

7.3.1
-----
* Patch: Fix warning on windows.

7.3.0
-----
* Patch: Fixed multiple unhandled corner cases for parser.
* Minor: Added custom error codes for parser.
* Minor: Lowered CMake version requirement.

7.2.0
-----
* Minor: Improve cmake build file.

7.1.0
-----
* Minor: Added cmake build file.

7.0.2
-----
* Patch: Remove print when comparing objects of different types.

7.0.1
-----
* Patch: Change stol -> stoll. On Window stol returns a 32 bit, we need 64 bit.
  See more here: https://stackoverflow.com/a/30741963/1717320

7.0.0
-----
* Major: Added inline namespace for versioning.
* Major: Added prepare_release function to wscript.
* Major: Expose objects rather than static lib when used as a dependency.

6.0.0
-----
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
