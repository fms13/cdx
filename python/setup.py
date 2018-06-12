##
# \addtogroup python_implementation
# @{
#
# \file setup.py
#
# \date Oct 21, 2015
# \author Frank Schubert
#
# \brief File for the installation of the Python version of the CDX library.
#
# based on https://pythonhosted.org/an_example_pypi_project/setuptools.html

import os
from setuptools import setup

# Utility function to read the README file.
# Used for the long_description.  It's nice, because now 1) we have a top level
# README file and 2) it's easier to type in the README file than to put a raw
# string in below ...
def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

setup(
    name = "cdx",
    version = "1.0",
    author = "Frank M. Schubert",
    author_email = "fmschubert@ieee.org",
    description = ("Python version of the Channel Data Exchange (CDX) library."),
    license = "BSD2",
    keywords = "channel model, channel impulse response, storage",
    url = "http://snacs.sourceforge.net/cdx_format.html",
    packages=['cdx'],
    long_description=read('README'),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Topic :: Utilities",
        "License :: OSI Approved :: BSD2 License",
    ],
    scripts = [ 'scripts/cdx-display']
)

## @} #

