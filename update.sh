#!/bin/bash

CWD=`pwd`
SPDLOG_RELEASE=v1.10.0

if [ -d .git ] ; then

    git pull \
        && git submodule update --init \
        && git submodule update --remote \
        && cd 3rdparty/portable-target && git checkout master && git pull \
        && cd $CWD \
        && cd 3rdparty/pfs/common && git checkout master && git pull \
        && cd $CWD \
        && cd 3rdparty/pfs/debby && git checkout master && git pull \
            && git submodule update --init -- 3rdparty/rocksdb \
        && cd $CWD \
        && cd 3rdparty/spdlog && git checkout $SPDLOG_RELEASE \

fi

