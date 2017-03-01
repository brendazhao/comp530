
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_BufferManager.h"
#include <string.h>
using namespace std;

void *MyDB_PageHandleBase :: getBytes () {
    void* rawdata=NULL;
    pair<MyDB_TablePtr, long> cur=thispage->pageid;
    //look for this page in lru, to determine whether to read from buffer or disk
    bool flagBuf=false;
    map<size_t, pair<MyDB_TablePtr, long>, less<size_t>>::iterator it_lru = db_bufferManager->lrutable.begin();
    while (it_lru != db_bufferManager->lrutable.end()) {
        if (it_lru->second.first != cur.first || it_lru->second.second != cur.second) {
            it_lru++;
        }
        else {
            flagBuf=true;
            break;
        }
    }
    //read from buffer
    if(flagBuf) {
        rawdata = thispage->data;
        //lru_key++(worked as timestamp)
        db_bufferManager->lru_key = db_bufferManager->lru_key + 1;
        //delete the old one
        db_bufferManager->lrutable.erase(it_lru);
        //add the new one with new timestamp
        db_bufferManager->lrutable.insert(
                map<size_t, pair<MyDB_TablePtr, long>, less<size_t>>::value_type(db_bufferManager->lru_key,
                                                                                  cur));
    }
    //read from disk
    else {
        rawdata = thispage->readFromDisk(cur.first, cur.second);
        //lru_key++(worked as timestamp)
        db_bufferManager->lru_key=db_bufferManager->lru_key+1;
        //insert into lru table
        db_bufferManager->lrutable.insert(map<size_t , pair<MyDB_TablePtr, long>, less<size_t>>  :: value_type(db_bufferManager->lru_key, cur));
    }
    // put the "rawdata" into address:thispage->data
    //memcpy(thispage->data, rawdata, db_bufferManager->getPageSize());
    memcpy(thispage->data,rawdata, db_bufferManager->getPageSize());

    return thispage->data;
}

void MyDB_PageHandleBase :: wroteBytes () {
    //this page is dirty
    thispage->dirty=true;
}
MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
    pair<MyDB_TablePtr, long> cur=thispage->pageid;
    //gonna to delete a handle
    thispage->handle_nums=thispage->handle_nums-1;
    if(thispage->handle_nums<=0 && thispage->pin){
        thispage->pin = false;
    }
    if(thispage->handle_nums<=0 && thispage->anon){
        //recycle the id of this anon page
        db_bufferManager->anonPage.insert(db_bufferManager->curpos);
    }
   if(thispage->handle_nums<=0) {
        //find this item in lru table,delete it
        map<size_t, pair<MyDB_TablePtr, long>, less<size_t>>::iterator it_lru = db_bufferManager->lrutable.begin();
        while (it_lru != db_bufferManager->lrutable.end()) {
            if (it_lru->second.first != cur.first || it_lru->second.second != cur.second) {
                it_lru++;
            } else {
                //erase this one
                db_bufferManager->lrutable.erase(it_lru);
                break;
            }
        }
       //find this item in pagetable,delete it
       map<pair<MyDB_TablePtr, long>,pageSp>::iterator it_page = db_bufferManager->pagetable.find(cur);
       //need to write back
       if(it_page->second->dirty){
           //update disk......
           it_page->second->writeBack(it_page->second->data);
           it_page->second->dirty=false;
       }
       //recycle this page
       db_bufferManager->emptypage.push(it_page->second->data);
       //delete it in the page table
       db_bufferManager->pagetable.erase(it_page);
    }
}

#endif

