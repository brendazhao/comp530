cmake_minimum_required(VERSION 3.6)
project(Main)

set(CMAKE_CXX_STANDARD 11)
set(INCLUDE_DIR
        BufferMgr/headers/
        Catalog/headers/
        Qunit/headers/)

include_directories (${INCLUDE_DIR})

set(SOURCE_FILES
        BufferMgr/headers/MyDB_BufferManager.h
        BufferMgr/headers/MyDB_PageHandle.h
        BufferMgr/source/MyDB_BufferManager.cc
        BufferMgr/source/MyDB_PageHandle.cc
        BufferTest/source/BufferQUnit.cc
        Catalog/headers/MyDB_Table.h
        Catalog/source/MyDB_Table.cc
        Qunit/headers/QUnit.h BufferMgr/headers/MyDB_Page.h BufferMgr/source/MyDB_Page.cc)


add_executable(Main ${SOURCE_FILES})