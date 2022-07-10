#!/bin/bash

CWD=`pwd`
SPDLOG_RELEASE=v1.10.0

if [ -d .git ] ; then

    git checkout master && git pull origin master \
        && git submodule update --init \
        && git submodule update --remote \
        && cd 3rdparty/portable-target && git checkout master && git pull \
        && cd $CWD \
        && cd 3rdparty/pfs/common && ./update.sh \
        && cd $CWD \
        && cd 3rdparty/pfs/debby && ./update.sh \
        && cd $CWD \
        && cd 3rdparty/spdlog && git checkout $SPDLOG_RELEASE \
        && cd $CWD
        
fi

