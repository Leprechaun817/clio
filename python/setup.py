#!/usr/bin/env python3
"""
Clio
====

Python implementation of the
`Clio argument-parsing library <https://github.com/dmulholland/clio>`_.

Clio supports long and short-form options and arbitrarily-nested commands.
It aims to provide a consistent interface across multiple programming languages,
implemented wherever possible as a simple drop-in file.

Install::

    $ pip install pyclio

Import::

    >>> import clio

See the project's `Github homepage <https://github.com/dmulholland/clio>`_
for further details.

"""

import os
import re
import io

from setuptools import setup


filepath = os.path.join(os.path.dirname(__file__), 'clio.py')
with io.open(filepath, encoding='utf-8') as metafile:
    regex = r'''^__([a-z]+)__ = ["'](.*)["']'''
    meta = dict(re.findall(regex, metafile.read(), flags=re.MULTILINE))


setup(
    name = 'pyclio',
    version = meta['version'],
    py_modules = ['clio'],
    author = 'Darren Mulholland',
    url = 'https://github.com/dmulholland/clio',
    license = 'Public Domain',
    description = (
        'A minimalist argument-parsing library for building elegant '
        'command-line interfaces.'
    ),
    long_description = __doc__,
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'Programming Language :: Python :: 3',
        'Operating System :: OS Independent',
        'License :: Public Domain',
    ],
)
