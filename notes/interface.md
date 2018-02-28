




/var/tagsystem/
    index
    documents
        01/
            01010101
            01010101



Tagsystem will create a single, central database on install at /var/tagsystem. The central database will contain a folder called 'documents' and a folder called 'index'
Documents will contains hardlinks to all tracked files in a document-id[0:2]/document-id[2:40] format. That's just to keep folders from getting too big. Copied from git.
Index will contain an lmdb database used to track document tags and other metadata.


In userspace, there will be one or more 'projections' of the tagged files. These will be folders full of hard linked files to the tagsystem/documents folder.
There will also be a .tags folder in each of the projections containing a file matching each file in the projections. The .tags file contents will be the tags 
on the document. 




* Syncing from projections
* Adding/removing/renaming files without sync
* default tags (`name:filename`, `extension:fileextension`, `owner:fileowner`)
* Ignoring folders
* Tracking where projections are and knowing their origional state/not losing files/default tag to append to all files in projection
    Basically, you can either use the cli to directly manipulate documents+tags, or you can set up a projection manually and sync it with the database

    Open questions is
        - How does syncing work exactly (eg, if there's a tag but no file or file w/out tag. Does a )








/*
/var/tagsystem/
    tags/
        lmdb db
            forward index
            inverted index
            
    docs/
        ab/...
*/