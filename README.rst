A hackable CPython remote debugger designed for integration with the latest generation of Javascript editor / IDE (eg. Cloud9, Atom, VS Code)
=============================================================================================================================================


IKP3db is a Python 3 debugger. For **Python 2** see the IKPdb project on github and pypi. https://github.com/audaxis/ikpdb


Features
--------

* Debugging of multithreaded programs
* Conditional breakpoints
* Variables hot modifications
* "Turbo mode"
* easy integration in javascripts frameworks / editors

Installation from pypi
----------------------

When it will be published, you will be able the latest stable version from pypi using:

.. code-block:: bash

   $ pip3 install ikp3db

Installation from sources
_________________________

Install using one of these:

.. code-block:: bash

   $ pip3 install git+https://github.com/cmorisse/ikp3db.git@1.x#egg=ikp3db  # 1.x is the branch name
   $ pip3 install git+https://github.com/cmorisse/ikp3db.git#egg=ikp3db  # from master

IKP3db sources will be installed in ./ikp3db_src/ikp3db

Installation from sources to contribute
_______________________________________

Git clone and install using one of these:

.. code-block:: bash

   $ pip3 install --src ./ikp3db_src -e git+https://github.com/cmorisse/ikp3db.git@1.x#egg=ikp3db  # 1.x is the branch name
   $ pip3 install --src ./ikp3db_src -e git+https://github.com/cmorisse/ikp3db.git#egg=ikp3db  # from master

IKP3db sources will be installed in ./ikp3db_src/ikp3db


.. _getting-started:

Getting started
---------------

Cloud9 is IKP3db debugger client reference implementation so head 
to `Cloud9 <https://c9.io/>`_, create an account there then refer to the Getting
Started section of `IKPdb documentation <https://ikpdb.readthedocs.io/>`_.

Documentation
-------------

IKP3db is the Python 3 version of IKPdb ; for now there is only one documentation.

https://ikpdb.readthedocs.io/


Requirements
------------

CPython 3.6.x

A C compiler (eg. python-dev Debian package, xcode tools on macOS).

License
-------

``IKP3db`` is licensed under the MIT License.
See the License paragraph in the documentation.

Source code
------------

Source code is available on github:

https://github.com/cmorisse/ikp3db


Issues
------

Issues are managed using Github's Issues Tracker.

