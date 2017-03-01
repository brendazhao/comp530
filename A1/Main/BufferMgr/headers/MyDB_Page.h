//
// Created by zhao on 2017/1/29.
//

#ifndef MAIN_MYDB_PAGE_H
#define MAIN_MYDB_PAGE_H

#include <memory>
#include "MyDB_Table.h"
using namespace std;

class MyDB_BufferManager;
class page;
typedef shared_ptr<page> pageSp;

class page{
public:
    //raw data
    void* data;
    //whether this page is been pinned
    bool pin;
    //whether this page is dirty
    bool dirty;
    //whether this page is anonymous
    bool anon;
    //this page is from which table(*pageid[0]) and which page(*pageid[1])
    pair<MyDB_TablePtr, long> pageid;
    //record this page has how many handles
    int handle_nums;
    //reference to buffer manager
    MyDB_BufferManager* db_bufferManager;
    //read from the disk
    void* readFromDisk(MyDB_TablePtr whichTable,long i);
    //write back to disk
    void writeBack(void* res);

};

#endif //MAIN_MYDB_PAGE_H
