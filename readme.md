

# Tagsystem

## Using this things

    1. Install git, make, and gcc
    2. Clone recurively so the sub-modules are also pulled: `git clone --recursive https://useless.club/git/tavoe/tagsystem.git`
    2. Fix the make file so it works on your platform (or make a new one, or add some new targets)
    3. run `make`
        * The make file should build lmdb and openssl
        * It should delete and re-create the bin folder
        * Builds ts to the bin folder
        * Adds lmdb and openssl .so to the library path
        * Builds/Runs the ts test program in the bin folder
        * Test program creates the database in the bin folder (it will be cleaned up when make is re-run and bin is deleted)
        * *The make file has a bunch of extra targets in it for various auxilary tests I have had to build/run

## Naming
    * Almost everything is prfixed with ts_ (except the iter macro I believe)
    * Most files are named ts....c/h (eg: tsenv.c/h, tstag.c/h)
    * The function/structs in tsenv.c/h are mostly named ts_env_... and often take a first parameter of ts_env
    * The exception to this is ts.c/h, which is the public-facing interface to all the rest of this thing

## Control Flow (or, the code executed when make is called)
    Tagsystem is a library, and not a single program, so there is not one "walk" through the code.
    The most likely entry points are all in ts.c/h, which can be follow through other aspects of the program.

    The code is ts.c is meant to be "context heavy", so typically, you will:
    ```
        ts_cs("tag1 + tag2 + tag3"); // (tag system, change set to tag1, tag2, and tag3)
        ts_mk0(); // make a new document at the intersection of the given sets.
                  // Similar to mkdir, except that you set your directory, then make
        ts_doc_id id;
        ts_mk(id);  // creates another doc with the same tags, but also returns 
                    // the unique id of this document. ts_mk0 stills names the doc, but it discards that id.
                    // The doc be automatically tagged with it's id either way
    ```

    `test/test.c` is a reference app that depends on tagsystem.so. It pretty much just calls some ts.c functions in order.

        
## What stuff is what

* Makefile - build step for the libs, tagsystem, and the test programs. Bit of a mess
bin - directory. Deleted/recreated on every run. Most binary+sample DB end up here. lmdb and openssl don't actually build out to here, but they should probably eventually (?)
lib - git submodules end up here. Don't edit it, it definitally won't be preserved
notes - junk?
src (see the header for the exact functions)
    * ts - top level functions
    * tsenv - tag system environment. Has an lmdb env instance and stores the path to the index and documents.
    * tsdoc - create/delete a document. Also, the functions of working with document IDs and generating a new, unique id.
    * tstag - Tagging and untagging documents. Fairly knarly, but self contained.
    * tsnode - If a tag is a tree of documents, a node is a single item in that tree. Added this later, and probably more logic needs to be consolidated here.
    * tssearch - Looking up docs in the database. Given some tags, yields on document at a time that fulfills those tags.
    * tswalk - A search walk several tag trees. Each individual walk is encapsulated by a walk. So, a search is a collection of walks.
    * tsutil - A few misc utilities.
    * tsiter - Macro for iterating. Wraps a certain common series of function calls. More or less a for-loop with a destructor.

test - test.c is the important one. The other ones are one-offs I used for debugging.. stuff

-----------------------------------------------------------

git submodule init
git submodule update --recursive

-----------------------------------------------------------


    # Proj commands -
    #     proj
    #         - force to overwrite directory
    #         - don't map names to name: and type: tags
    #     sync
    #         - pull first or push first
    #         - all file deletion



# Tagsystem - a file system with tags instead of files!

# In some scenarios, folders don't make a lot of sense. 
# Does a song belong in the artist's folder or the album's folder?
# Does an enemy in a game belong in the level 3 folder or the flying enemies folder?
# Does a javascript file belong in the scripts foldeer or the login page folder?

# More empowering than any of those scenerios, working with tags is about working with sets.
# At a the intersection of a given set of tags will be one or more documents. 
# Finding related documents is the root of modern software development. The last 20 years of 
# web develpment have seen websites go from serving single documents to sets of relevents documents (a feed of tweets, posts, or search results)

# Tagsystem provides a simple, performant, cross platform tool to tag and search for documents.
# Tagsystem tries to play nice with your existing tool chain. 
# Working with tags in your shell, ide, or graphical file explorer is not be more complicated than working with typical files.


# Lets take a look at the tagsystem shell commands!


# Each user has a dedicated tag database. 
# Your database defaults to ~/.tsys/, but can be overwitten by setting the environment variable TSYS_DB.

# When you're exploring the filesystem, you always have a working directory.
# You can see what your working directory is:

pwd # ==> /home/username

#  Similarly, tagsystem also has a present working set. We'll get into the path syntax in a moment.
tsys pws # ==> +username

# You can change your present working directory via 'change directory'
cd Project
pwd # ==> /home/username/Projects

# Similarly, you can change your pws via 'change set'
tsys cs Projects
tsys pws # ==> +username +Projects

# Going up a directory in bash looks like
cd ..

# Tags are unordered, so tsys cs .. makes no sense.
# Instead, we need to choose which tag to remove from our pws
tsys cs ~username
tsys pws # ==> +Projects

# Lets make a new tag and put some documents into it!
# Tagsystem assume we're adding a tag to the pws if ~ or + is not specified
tsys cs example_project

# When making a document, you can add more tags to the new document without changing your pws
# You will be able to uniquely identify this document by 'doc_1' until someone makes another document 'doc_1'
tsys mk +doc_1
tsys mk doc_2

# Documents don't need to have a unique tag. This document will be created with just the tags in the pws, +Projects +example_projects
tsys mk

# Output all the tags in our pws (+Projects +example_project)
# These documents don't have pretty names. There's ways to work around that that we can discuss later
tsys ls 
    # ==> /home/users/.tsys/docs/00/00000000
    # ==> /home/users/.tsys/docs/00/00000001
    # ==> /home/users/.tsys/docs/00/00000002

# Editing the documents in a set
tsys ls | vim

# Lets delete a document
tsys rm doc_1

# Lets add a tag to a document
tsys ls doc_2 | tsys tag +project_files

# Actually, lets remove some tags
tsys ls doc_2 | tsys tag ~project_files ~doc_2

# Lets get into the syntax of a tag set a bit more
# '+' means add a tag to a set.
# '~' means remove a tag from a set (I would use minus, but that gets counfusing when you have tsys rm -f, for example).
# '~~' is the equivalent of 'cd /'. It will remove all documents from the set.
ts cs +Project +username_2 ~username
# In a tag set description, whitespace will be ignored except within tag names. A tag can have spaces, but leading and traiting witespace will be trimmed.
# If no operation is provided '+' is assumed
ts cs Projects 


# That covers all the core tagsystem commands. Beyond the core commands, we have two commands that help to integrate the tagsystem with the filesystem. 
# They are 'project' and 'synchronize'.







# Questions raised!
#   how do I add a tag to a non-unique document? 
#       autotag with id
#       provide a tool for set differences (bleh)
#       honestly, tools for most set operations (unions, differnces, etc) would be really helpful pretty quickly
#   Should I provide an explicit properties construct? It's useful to me, and might empower queries. Shruggg.

-----------------------------------------------------------

valgrind --leak-check=full -v ./test/test

-----------------------------------------------------------

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

-----------------------------------------------------------