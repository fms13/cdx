#!/bin/bash
## \file build-all-deb-packages.sh
#
# \date Nov 30, 2016
# \author Frank Schubert
#
# \brief This script generates all Debian packages for the CDX C++ and Python libraries and the CDX tools:
# libcdx, libcdx-dev, python-cdx, and cdx-tools

# generate libcdx0 and libcdx-dev:

rm -rf ~/packaging/libcdx && \
mkdir -p ~/packaging/libcdx && \
cp -r ~/workspace/cdx/trunk/CPP ~/packaging/libcdx/libcdx_1.0 && \
cd ~/packaging/libcdx && \
tar cfz libcdx_1.0.orig.tar.gz libcdx_1.0 && \
cd libcdx_1.0 && \
DEB_BUILD_OPTIONS="parallel=4" nice debuild -us -uc

# Build the package for the Python CDX library, python-cdx_1.1-1_all.deb:

cd ~/workspace/cdx/trunk/python && \
export PY_CDX_VERSION=$(dpkg-parsechangelog --show-field Version | sed  's/-.*//') && \
rm -rf ~/packaging/python-cdx && \
mkdir -p ~/packaging/python-cdx && \
cp -r ../python ~/packaging/python-cdx/python-cdx_$PY_CDX_VERSION && \
cd ~/packaging/python-cdx && \
tar czf python-cdx_$PY_CDX_VERSION.orig.tar.gz python-cdx_$PY_CDX_VERSION && \
cd python-cdx_$PY_CDX_VERSION && \
debuild -us -uc

echo Install the new libcdx packages now, before the package for cdx-tools is build
read

# Build the Debian package for the cdx-tools:

rm -rf ~/packaging/cdx-tools && \
mkdir -p ~/packaging/cdx-tools && \
cd ~/packaging/cdx-tools && \
cp -r ~/workspace//cdx/trunk/tools ~/packaging/cdx-tools/cdx-tools_1.0 && \
cd ~/packaging/cdx-tools && \
tar cfz cdx-tools_1.0.orig.tar.gz cdx-tools_1.0 && \
cd cdx-tools_1.0/ && \
debuild -us -uc
