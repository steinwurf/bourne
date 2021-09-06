Abacus Documentation
====================

Abacus is a C++ metrics library. We wanted to create a library that allows us to
create counters in our other libraries that:

* Are fast to increment/update
* Are fast to copy/transfer between dependencies
* Can separate hot and cold memory
* Can keep everything in one contiguous memory block

The Abacus repository: https://github.com/steinwurf/abacus

The ``examples`` folder contains some programs that demonstrate the usage
of the available API.

If you have any questions or suggestions about this library, please contact
us at our developer mailing list (hosted at Google Groups):

* http://groups.google.com/group/steinwurf-dev


Table of Contents
-----------------

.. toctree::
  :maxdepth: 2

  user_api/user_api
  examples/examples
  license


.. toctree::
  :maxdepth: 1

  news
