#!/bin/bash
## \file build-all-deb-packages.sh
#
# \date Nov 30, 2016
# \author Frank Schubert
#
# \brief This script generates all Debian packages for the CDX C++ and Python libraries and the CDX tools:
# libcdx, libcdx-dev, python-cdx, and cdx-tools

# exit on error:
set -e

# generate libcdx0 and libcdx-dev:

rm -rf ~/packaging/libcdx
mkdir -p ~/packaging/libcdx

# get version from changlog file:
cd ~/workspace/cdx/trunk/CPP
LIBCDX_VERSION=$(dpkg-parsechangelog --show-field Version | sed  's/-.*//')

echo libcdx-C++ version: $LIBCDX_VERSION

cp -r ~/workspace/cdx/trunk/CPP ~/packaging/libcdx/libcdx_$LIBCDX_VERSION
cd ~/packaging/libcdx

tar cfz libcdx_$LIBCDX_VERSION.orig.tar.gz libcdx_$LIBCDX_VERSION
cd libcdx_$LIBCDX_VERSION
DEB_BUILD_OPTIONS="parallel=4" nice debuild -us -uc

# Build the package for the Python CDX library, python-cdx_1.1-1_all.deb:

# get version:
cd ~/workspace/cdx/trunk/python
export PY_CDX_VERSION=$(dpkg-parsechangelog --show-field Version | sed  's/-.*//')

echo libcdx-Python version: $PY_CDX_VERSION

rm -rf ~/packaging/python-cdx
mkdir -p ~/packaging/python-cdx
cp -r ~/workspace/cdx/trunk/python ~/packaging/python-cdx/python-cdx_$PY_CDX_VERSION
cd ~/packaging/python-cdx
tar czf python-cdx_$PY_CDX_VERSION.orig.tar.gz python-cdx_$PY_CDX_VERSION
cd python-cdx_$PY_CDX_VERSION
debuild -us -uc

echo Install the new libcdx packages now, before the package for cdx-tools is build
read

# Build the Debian package for the cdx-tools:

rm -rf ~/packaging/cdx-tools
mkdir -p ~/packaging/cdx-tools
cd ~/packaging/cdx-tools

# get version:
cd ~/workspace/cdx/trunk/tools
CDX_TOOLS_VERSION=$(dpkg-parsechangelog --show-field Version | sed  's/-.*//')

echo CDX tools version: $CDX_TOOLS_VERSION

cp -r ~/workspace/cdx/trunk/tools ~/packaging/cdx-tools/cdx-tools_$CDX_TOOLS_VERSION
cd ~/packaging/cdx-tools
tar cfz cdx-tools_$CDX_TOOLS_VERSION.orig.tar.gz cdx-tools_$CDX_TOOLS_VERSION
cd cdx-tools_$CDX_TOOLS_VERSION/
debuild -us -uc
