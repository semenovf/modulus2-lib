#!/bin/sh

# Mandatory. Common library
git submodule update --init --remote --merge -- 3rdparty/pfs/common

# Optional. If need RocksDB support for settings plugin
git submodule update --init -- 3rdparty/rocksdb

# Optional. If need spdlog support for logger
git submodule update --init -- 3rdparty/spdlog