#!/bin/bash

base=`readlink -mn $0`
basedir=`dirname $base`

cd "$basedir"

version=0.1.2
tagname="v$version"
packagename="msnake-$version"
tarball="$packagename.tar.gz"

if [ ! -d packaging ] ; then mkdir packaging ; fi
git archive "$tagname" --prefix="$packagename/" --output "packaging/$tarball"
cd packaging
tar xf "$tarball"

cd "$packagename"

dpkg-buildpackage
