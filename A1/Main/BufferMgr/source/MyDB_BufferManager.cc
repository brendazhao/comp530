
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <map>
#include <iterator>
#include <iostream>
using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
    //this is the handle we gaonna to return
    MyDB_PageHandle res=make_shared<MyDB_PageHandleBase>();
    //look for this page in page table, to determine whether to allocate space
    bool flagPageTable=false;
    pair<MyDB_TablePtr, long> cur(whichTable,i);
    map<pair<MyDB_TablePtr, long>,pageSp>::iterator it_page = pagetable.find(cur);
    if(it_page!=pagetable.end()) {
        flagPageTable=true;
    }

    //we have this page in the buffer
    if(flagPageTable){
        res->thispage = it_page->second;
    }
    //we have this page in the disk and we gonna to put it into the buffer
    else {
        //buffer has no space, so we need to evict a page
        if (emptypage.empty()) {
            evictPage();
        }
        //buffer now have space，get an empty page;
        void *unusedpage = emptypage.front();
        //this space in the buffer has been used
        emptypage.pop();
        //new a pageSp
        pageSp newpage = createPage(cur, unusedpage, false, false, false, 0,this);
        //put this item in the pagetable
        pagetable.insert(map<pair<MyDB_TablePtr, long>, pageSp>::value_type(cur, newpage));
        res->thispage = newpage;
    }
    //we have already new a handle to this page
    res->thispage->handle_nums=res->thispage->handle_nums+1;
    res->db_bufferManager=this;
    return res;
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    //this is the handle we gaonna to return
    MyDB_PageHandle res=make_shared<MyDB_PageHandleBase>();
    //buffer has no space, so we need to evict a page
	if(emptypage.empty()){
		evictPage();
	}
    //buffer now have space，get an empty page;
    void *unusedpage = emptypage.front();
    //this space in the buffer has been used
	emptypage.pop();
    //new a new tableptr
	MyDB_TablePtr tableptr=make_shared<MyDB_Table>("zqltable",tempFile);
    //find a empty pos for anon page
    long unusepos;
    if(!anonPage.empty()){
        set<long>:: iterator i=anonPage.begin();
        unusepos=*i;
        anonPage.erase(i);
    }
    else{
        unusepos=curpos;
        curpos++;
    }
    //new a new page id
	pair<MyDB_TablePtr, long> tmpid(tableptr,unusepos);
    //new this page
	pageSp newpage=createPage(tmpid,unusedpage,true,false,false,0,this);
	pagetable.insert(map<pair<MyDB_TablePtr, long>, pageSp>::value_type(tmpid, newpage));
    //wrapped this handle
	res->thispage=newpage;
	res->thispage->handle_nums=res->thispage->handle_nums+1;
	res->db_bufferManager=this;

	return res;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    ///this is the handle we gaonna to return
    MyDB_PageHandle res=make_shared<MyDB_PageHandleBase>();

    //look for this page in page table, to determine whether to allocate space
	bool flagPageTable=false;
	pair<MyDB_TablePtr, long> cur(whichTable,i);
	map<pair<MyDB_TablePtr, long>,pageSp>::iterator it = pagetable.find(cur);
	if(it!=pagetable.end()) {
		flagPageTable=true;
	}

    //we have this page in the buffer
	if(flagPageTable){
        map<pair<MyDB_TablePtr, long>, pageSp>::iterator it_page = pagetable.find(cur);
        res->thispage = it_page->second;
	}
    //we have this page in the disk and we gonna to put it into the buffer
    else {
        //buffer has no space, so we need to evict a page
        if (emptypage.empty()) {
            evictPage();
        }
        //buffer now have space，get an empty page;
        void *unusedpage = emptypage.front();
        //this space in the buffer has been used
        emptypage.pop();
        //new a pageSp
        pageSp newpage = createPage(cur, unusedpage, false, false, true, 0,this);
        //put this item in the pagetable
        pagetable.insert(map<pair<MyDB_TablePtr, long>, pageSp>::value_type(cur, newpage));
        res->thispage = newpage;
	}
    //we have already new a handle to this page
	res->thispage->handle_nums=res->thispage->handle_nums+1;
	res->db_bufferManager=this;
	return res;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    //this is the handle we gaonna to return
    MyDB_PageHandle res=make_shared<MyDB_PageHandleBase>();
    //buffer has no space, so we need to evict a page
    if(emptypage.empty()){
        evictPage();
    }
    //buffer now have space，get an empty page;
    void *unusedpage = emptypage.front();
    //this space in the buffer has been used
    emptypage.pop();
    //new a new tableptr
    MyDB_TablePtr tableptr=make_shared<MyDB_Table>("zqltable",tempFile);
    //find a empty pos for anon page
    long unusepos;
    if(!anonPage.empty()){
        set<long>:: iterator i=anonPage.begin();
        unusepos=*i;
        anonPage.erase(i);
    }
    else{
        unusepos=curpos;
        curpos++;
    }
    //new a new page id
    pair<MyDB_TablePtr, long> tmpid(tableptr,unusepos);
    //new this page
    pageSp newpage=createPage(tmpid,unusedpage,true,false,true,0,this);
    pagetable.insert(map<pair<MyDB_TablePtr, long>, pageSp>::value_type(tmpid, newpage));
    //wrapped this handle
    res->thispage=newpage;
    res->thispage->handle_nums=res->thispage->handle_nums+1;
    res->db_bufferManager=this;

    return res;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
	unpinMe->thispage->pin=false;
}

pair<MyDB_TablePtr, long> MyDB_BufferManager :: evictPage(){
    //look up in lru table in order
	map<size_t , pair<MyDB_TablePtr, long>, less<size_t>> ::iterator it_lru = lrutable.begin();
	pair<MyDB_TablePtr, long> res;
	while(it_lru != lrutable.end())
	{
		pair<MyDB_TablePtr, long> iterPageID=it_lru->second;
		map<pair<MyDB_TablePtr, long>,pageSp>::iterator it_page = pagetable.find(iterPageID);
		//never will happen??
		if(it_page==pagetable.end()) {
			break;
		}
		else if(it_page->second->pin){
			continue;
		}
		else{
            //needs to write back?
			if(it_page->second->dirty){
				//update disk......
                it_page->second->writeBack(it_page->second->data);
				it_page->second->dirty=false;
			}
			res=it_page->first;
			//we have one more space in buffer
			emptypage.push(it_page->second->data);
		    //delete this item in lru table
			lrutable.erase(it_lru);
            //delete this item in page table
			pagetable.erase(it_page);
			break;
		}
	}
	return res;
}
pageSp MyDB_BufferManager :: createPage(pair<MyDB_TablePtr, long> pageid,void* data, bool anon,bool dirty,bool pin,int handle_nums,MyDB_BufferManager* buf){
	//new this page
	pageSp newpage=make_shared<page>();
    newpage->pageid=pageid;
	newpage->data=data;
	newpage->anon=anon;
	newpage->dirty=dirty;
	newpage->pin=pin;
	newpage->handle_nums=handle_nums;
    newpage->db_bufferManager=buf;
    return newpage;
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    //init private field
        this->pageSize=pageSize;
	this->numPages=numPages;
	this->tempFile=tempFile;
    //init public field
	curpos=0;
	lru_key=0;
	buffer =(void*)malloc(pageSize*numPages);
	//init the queue
	void *cur=(void*)buffer;
	int i=1;
	while(i<=numPages){
		emptypage.push(cur);
		cur=(void*)((char*)cur+pageSize);
		i++;
	}
}

MyDB_BufferManager :: ~MyDB_BufferManager () {

}

size_t MyDB_BufferManager::getPageSize() const {
	return pageSize;
}

void MyDB_BufferManager::setPageSize(size_t pageSize) {
	MyDB_BufferManager::pageSize = pageSize;
}

size_t MyDB_BufferManager::getNumPages() const {
	return numPages;
}

void MyDB_BufferManager::setNumPages(size_t numPages) {
	MyDB_BufferManager::numPages = numPages;
}

const string &MyDB_BufferManager::getTempFile() const {
	return tempFile;
}

void MyDB_BufferManager::setTempFile(const string &tempFile) {
	MyDB_BufferManager::tempFile = tempFile;
}

#endif
