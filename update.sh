#!/bin/bash

CWD=`pwd`
SPDLOG_RELEASE=v1.10.0
ROCKSDB_RELEASE=v6.29.5

if [ -d .git ] ; then

    git checkout master && git pull origin master \
        && git submodule update --init --recursive \
        && git submodule update --init --remote -- 3rdparty/portable-target \
        && git submodule update --init --remote -- 3rdparty/pfs/common \
        && git submodule update --init --remote -- 3rdparty/pfs/debby \
        && cd 3rdparty/pfs/debby/3rdparty/rocksdb && git checkout $ROCKSDB_RELEASE \
        && cd $CWD \
        && cd 3rdparty/spdlog && git checkout $SPDLOG_RELEASE \
        && cd $CWD

fi

