#!/bin/bash

LOG_FILE='download.log'
GIT_DOWNLOADER="git clone"

SPDLOG_RELEASE=v1.14.1

# HTTPS
HTTPS_SOURCES="--depth 1 -b ${SPDLOG_RELEASE} --single-branch https://github.com/gabime/spdlog.git spdlog"

DEFAULT_SOURCES=${HTTPS_SOURCES}
DEFAULT_DOWNLOADER=${GIT_DOWNLOADER}

IFS=$'\n'

echo `date` > ${LOG_FILE}

for src in ${DEFAULT_SOURCES} ; do
    eval "${DEFAULT_DOWNLOADER} $src" >> ${LOG_FILE} 2>&1

    if [ $? -eq 0 ] ; then
        echo "Cloning $src: SUCCESS"
    else
        echo "Cloning $src: FAILURE"
    fi
done
