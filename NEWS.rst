News for bourne
===============

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
* Major: Replaced ``to_[type](bool)`` functions with a combination of
  ``to_[type]`` and ``is_[type]``.
* Added ``is_bool``, ``is_int``, ``is_float``, ``is_string``, ``is_object``, and
  ``is_array``.


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
