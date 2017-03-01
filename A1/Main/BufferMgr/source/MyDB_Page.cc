//
// Created by zhao on 2017/1/30.
//
#include <memory>
#include <MyDB_BufferManager.h>
#include <MyDB_Page.h>
#include <iostream>
using namespace std;

void* page:: readFromDisk(MyDB_TablePtr whichTable,long i){
    char *res= (char*)malloc(db_bufferManager->getPageSize());
    int fd=open(whichTable->getStorageLoc().c_str(),O_CREAT|O_RDWR|O_SYNC,0666);
    lseek(fd,i*db_bufferManager->getPageSize(),SEEK_SET);
    read(fd,res, db_bufferManager->getPageSize());
    close(fd);
    return (void*)res;
}
void page::writeBack(void *res){
    int fd=open(pageid.first->getStorageLoc().c_str(),O_CREAT|O_RDWR|O_SYNC,0666);
    lseek(fd,db_bufferManager->getPageSize()*pageid.second,SEEK_SET);
    write(fd,(char*)res,db_bufferManager->getPageSize());
    close(fd);  
    this->dirty=false;
}

