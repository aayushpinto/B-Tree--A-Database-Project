///////////////////////////////////////////////////////////////////////////////
/////////////  The Header File for the Buffer Manager /////////////////////////
///////////////////////////////////////////////////////////////////////////////


#ifndef BUF_H
#define BUF_H

#include "db.h"
#include "page.h"
#include "new_error.h"

#define NUMBUF 20   

#define HTSIZE 7

class   BufMgr;

class FrameDesc {

  public:

    FrameDesc() {
        pageNo  = INVALID_PAGE;
        pinCount = 0;
        dirty = FALSE;
        hate=0;
    }

    int pageNo, pinCount, dirty, hate;
};

class HashTable{

    public:
        struct hashentry{
            int pageNo;
            int frameNo;
        };

        hashentry entries[200];

        HashTable(){
            for (int i = 0; i < 200; i++){
                entries[i].pageNo = INVALID_PAGE;
                entries[i].frameNo = -1;
            }
        };
};

enum bufErrCodes  {
    HASHMEMORY, 
    HASHDUPLICATEINSERT, 
    HASHREMOVEERROR, 
    HASHNOTFOUND, 
    QMEMORYERROR, 
    QEMPTY, 
    INTERNALERROR, 
	BUFFERFULL, 
    BUFMGRMEMORYERROR, 
    BUFFERPAGENOTFOUND, 
    BUFFERPAGENOTPINNED, 
    BUFFERPAGEPINNED
};

class Replacer;

class BufMgr {

    private: 
        int numBuffers;
        FrameDesc *frameDesc;
        HashTable *hashTable;
        int *hateList;

        int hashFunc(PageId PageId_in_a_DB){
            return (PageId_in_a_DB%HTSIZE);
        };

    public:
    
        Page* bufPool; // The actual buffer pool
        
        BufMgr (int numbuf, Replacer *replacer = 0); 
        // Initializes a buffer manager managing "numbuf" buffers.
        // Disregard the "replacer" parameter for now. In the full 
        // implementation of minibase, it is a pointer to an object
        // representing one of several buffer pool replacement schemes.

        ~BufMgr();           // Flush all valid dirty pages to disk

        Status pinPage(PageId PageId_in_a_DB, Page*& page, int emptyPage=0);
            // Check if this page is in buffer pool, otherwise
            // find a frame for this page, read in and pin it.
            // also write out the old page if it's dirty before reading
            // if emptyPage==TRUE, then actually no read is done to bring
            // the page

        Status unpinPage(PageId globalPageId_in_a_DB, int dirty=0, int hate=0);
            // hate should be TRUE if the page is hated and FALSE otherwise
            // if pincount>0, decrement it and if it becomes zero,
            // put it in a group of replacement candidates.
            // if pincount=0 before this call, return error.

        Status newPage(PageId& firstPageId, Page*& firstpage, int howmany=1); 
            // call DB object to allocate a run of new pages and 
            // find a frame in the buffer pool for the first page
            // and pin it. If buffer is full, ask DB to deallocate 
            // all these pages and return error

        Status freePage(PageId globalPageId); 
            // User should call this method if it needs to delete a page
            // this routine will call DB to deallocate the page 

        Status flushPage(PageId pageid);
            // Used to flush a particular page of the buffer pool to disk
            // Should call the write_page method of the DB class

        Status flushAllPages();
        // Flush all pages of the buffer pool to disk, as per flushPage.

        /*** Methods for compatibility with project 1 ***/
        Status pinPage(PageId PageId_in_a_DB, Page*& page, int emptyPage, const char *filename);
        // Should be equivalent to the above pinPage()
        // Necessary for backward compatibility with project 1

        Status unpinPage(PageId globalPageId_in_a_DB, int dirty, const char *filename);
        // Should be equivalent to the above unpinPage()
        // Necessary for backward compatibility with project 1
        
        unsigned int getNumBuffers() const { return numBuffers; }
        // Get number of buffers

        unsigned int getNumUnpinnedBuffers();
        // Get number of unpinned buffers
};

#endif