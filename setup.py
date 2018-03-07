# coding: utf-8

#
# This file is part of the IKPdb Debugger
# Copyright (c) 2016, 2017 by Cyril MORISSE, Audaxis
# Licence: MIT. See LICENCE at repository root
#
import sys
from setuptools import setup, find_packages, Extension

name = 'ikp3db'
version = '1.1.2'


if sys.version_info[:2] != (3,6):
    sys.exit('Sorry, IKPdb only supports Python 3.6.x for now.')


long_description = (
    open('README.rst').read()
)

iksettrace3_module = Extension('iksettrace3', sources=['iksettrace3.c'])

setup(
    name = name,
    version = version,
    #packages = find_packages('src'),  # We use py_modules below
    py_modules = ['ikp3db'],
    #package_dir={'': 'src'},
    license='MIT',
    author='Cyril MORISSE, Audaxis',
    author_email='cmorisse@boxes3.net',
    description="A hackable CPython 3.6+ remote debugger designed for the Web and online IDE integration. Fork of IKPdb.",
    long_description = long_description,
    keywords = "debugger debug remote tcp",
    include_package_data=True,
    url = 'https://github.com/cmorisse/ikp3db',
    classifiers=[
        #'Framework :: Buildout',
        'Development Status :: 5 - Production/Stable',
        'Environment :: Web Environment',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Debuggers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python',
        'Natural Language :: English',
     ],
     ext_modules=[iksettrace3_module]
)
