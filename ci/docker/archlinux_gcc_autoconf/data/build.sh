#!/bin/sh
set -e

if [ $# -ge 1 ] ; then
  REPO="$1"
else
  REPO="git://git.code.sf.net/p/libosmscout/code"
fi

if [ $# -ge 2 ] ; then
  BRANCH="$2"
else
  BRANCH="master"
fi

git clone -b "$BRANCH" "$REPO" libosmscout

export LANG=en_US.utf8

cd libosmscout
. ./setupAutoconf.sh
env
make -j `nproc` full
cd Tests && make check
