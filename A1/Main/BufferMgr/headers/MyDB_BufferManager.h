
#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"
#include "MyDB_Table.h"
#include <map>
#include <set>
#include <queue>

using namespace std;

class MyDB_BufferManager {

public:

	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whicTable, i) is that the page will be 
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);

	//evict the last unpin page from the buffer(assume we can always find one).
	pair<MyDB_TablePtr, long> evictPage();

    //create a new page
	pageSp createPage(pair<MyDB_TablePtr, long> pageid,void* data, bool anon,bool dirty,bool pin,int handle_nums,MyDB_BufferManager* buf);

    // creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize 
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile);
	
	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager ();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS
	//lru table<key:timecount,value: pageid>
	map<size_t , pair<MyDB_TablePtr, long>, less<size_t>> lrutable;

	//page table<key:pageid,value: pointer to the page>
	map<pair<MyDB_TablePtr, long>,pageSp> pagetable;

	//buffer we have
	void * buffer;

	//store all the pointers to the empty page
	queue<void*> emptypage;

	//this is the key of lru table(some like time stamp)
	size_t lru_key;

	//a counter about the spare page id in anonymous page
	long curpos;

    //a set to store all the spare page id for the anonymous page
    set<long> anonPage;

	size_t getPageSize() const;

	void setPageSize(size_t pageSize);

	size_t getNumPages() const;

	void setNumPages(size_t numPages);

	const string &getTempFile() const;

	void setTempFile(const string &tempFile);

private:
    //this is the page size
	size_t pageSize;
    //this is the number of the pages we can have in buffer
	size_t numPages;
    //this is the name of the temp file
	string tempFile;

};

#endif


