#!/bin/sh

version="1.24"
name="libresourceqt-$version"

echo "packaging $name"

rm -rf $name $name.tar.gz
mkdir -v $name && \
cp -va COPYING libresourceqt.* common.pri libdbus-qeventloop libresourceqt resourceqt-client tests demo $name && \
tar cvzf $name.tar.gz $name && \
rm -rf $name

