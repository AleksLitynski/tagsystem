

# Tagsystem

## What is tagsystem?
Tagsystem lets you organize files with tags instead of with directories. 
Instead of creating files in directories, you create files and apply tags to them. 
When you want to retrieve your files later, you request all files 
in a tag set; an intersection of tags that defines a collection of files.

Tagsystem exists because sometimes folders don't make a lot of sense.
Does a song belong in the artist's folder or the album's folder?
Does an enemy in a game belong in the level 3 folder or the flying enemies folder?
Does a javascript file belong in the scripts folder or the login page folder?

More empowering than those scenarios, working with tags is about working with sets 
of documents. End users no longer go to web pages, they browse feeds. Facebook,
twitter, even google search results all present a user a collection of related
documents instead of a singular file.

Tagsystem provides a simple, performant, cross platform tool to tag and search for documents.
Tagsystem tries to play nicely with your existing tool chain. Eventually, you will be able to work 
with tags in your shell, ide, graphical file explorer, or via bindings to your programming language
of choice.

Currently, tagsystem exposes only two interfaces, a C library and a command line interface.

## Installing tagsystem
You can download a compiled version of the command line application from [the release page](...).

On unix systems, you just need to place the executable on your path and set the executable bit.

On windows, you still need to add the executable to you path, but you may also need to install 
msys2 before using tagsystem.

You can do this from powershell by running:

```
iwr chocolatey.org/install.ps1 | iex
choco install msys2
```

## Using the CLI
The tagsystem CLI application is called tsys. Tsys can create a database of tagged documents
and perform operations on those documents.

Tsys will create a database automatically if no database is detected. In order, tsys will attempt
to create a database in these locations:

1. At the location specified by the `TSDBPATH` environment variable 
2. At `$XDG_CONFIG_HOME/.tsysdb`
3. At `/<user>/home/.tsysdb`

On windows, the database will be created at `%APPDATA%/tsys/.tsysdb`.

The database has two parts, an lmdb database containing all tag metadata, 
and a directory containing all documents.

The database directory is structured like this:

```
.
├── docs
|   └──00
|       └── 00000001
└── index
    ├── data.mdb
    └── lock.mdb
```

The index folder contains an lmdb environment with three databases:

* index - stores each document name with a list of tags attached to the document.
* iindex - an inverted index, storing tags with a list of documents attached to them.
* meta - contains metadata about the database. Currently, just the working set.

Tsys provides a number of subcommands for working with the database

* `tsys help` Gives a complete overview of all available subcommands.
* `tsys pws` Similar to `pwd`, tagsystem tracks a present working set of tags. pws lets you see your present working set of tags.
* `tsys cs` Add or remove tags from your pws. Equivalent to `cd`.
* `tsys ls` List the documents in your present working set.
* `tsys mk` Create a new document in the present working set.
* `tsys rm` Delete all documents in the present working set.
* `tsys tag` Adds or removes tags from a document.

Each subcommand has a `--help` flag that provides details on use of that command.

## CLI Example

```
# Change to the tag set 'poems'
> tsys cs ~~poems
+poems

# make a new file named 'fluffy'
> tsys mk +fluffy
+poems+fluffy

# open file in vi for editing
> vi `tsys ls`

# add a tag 'cat' to our file
> ts ls | ts tag +cat

# check what our pws is
> ts pws
+poems+fluffy+cat

# delete all files tagged 'fluffy cat poems' without output 
> ts rm --silent
```

## Cloning tagsystem
Dependencies are included as git submodules. To clone with submodules, run `git clone --recursive https://github.com/tavoe/tagsystem.git`.

If you cloned the repo without submodules, you can clone them later with:

```
git submodule init
git submodule update --recursive
```

## Building tagsystem
1. Install make and gcc.
2. `make all` to build
3. `make runtest` to run unit tests 

## Code Overview
```
* cli/
    * Makefile          Clean, build, and run targets.
    * main.c/h          Dispatches to sub-commands defined in tscli.c/h.
* lib/                  All submodules are included in this directory.
* src/
    * Makefile          Clean and build targets.
    * tsargs.c/h        Command line argument parsing logic.
    * tscli.c/h         CLI subcommands. High level entry point for tsys.
    * tsclihelp.c/h     All CLI help messages.
    * tscliutils.c/h    Misc. utility functions to support CLI logic.
    * tsdb.c/h          Manages the tagsystem db and wraps lmdb operations.
    * tsdbpath.c/h      Logic to select the default database path.
    * tsdoc.c/h         Creates, deletes, tags, and untags documents.
    * tserror.c/h       Error codes and error messages.
    * tsid.c/h          Generates random document ids and provides functions to read/write individual bits in ids.
    * tssearch.c/h      Simultaniously walks several tags to search for documents shared by all tags.
    * tssearchset.c/h   Creates a tssearch and allocates memory for each tag included in the search.
    * tsstr.c/h         String operations meant to supliment the sds library.
    * tstaglist.c/h     Parses a string into a linked list of tag add/remove operations.
    * tstags.c/h        Creates a tag tree and insert into, remove from, saves, and loads the tag tree.
    * tstagset.c/h      Creates or updates a hash set of tags from a tstaglist.
    * tswalk.c/h        Traverses a single tstags tree. Used by tssearch.
* test/
    * Makefile          Build, clean, and test targets.
    * cmocka.[unix/win].def.h   Header files needed to build cmocka without cmake.
    * test_*.c          Unit tests are broken into logically named files.
    * test.c            Test utility functions and entry point for the test executable.
    * test.h            All tests share a single header file.
    * test-cli.sh       Not a true test, but a useful script to run to confirm the tsys exe is working as expected.
* Makefile              Targets to build cli, src, and test and to run the unit tests.
```

To quickly compute which documents exist at the insersection of a set of tags, 
we store an inverted index. That is, in adition to storing a mapping of documents to tags, 
we store a mapping from tags to docuemnts.

Each docuemnt has a 20 byte (160 bit) randomly generated name. Each tag in the inverted
index is a binary tree containing some number of these document ids. To save space,
suffixes are compressed, so once a branch of the tree no longer contains any divergences,
it will contain the entire document id the branch was building up to.

To find all documents in a single tag tree, we make a depth first traversal of the tag tree.
To find all documents in a set of tag tree, we traverse several tree simultaniously. If a
branch is unavailable in any tag tree, we know the document does not exist in the tag set.

## Roadmap

### Near Term
* Clean up include statements.
* Compile headers into a single ts.h file.
* Provide a simple, high level C API to work with the tagsystem.
* Provide an 'update database' function and check that the database is up to date before operating on it.
* Implement Project and Sync commands
    * It should be possible to project a subset of the tagsystem database into a directory.
    * Files in the projected folder should be hardlinked to files in the database.
    * Files in the projected folder should be named after special tags in the form "`<name:file_name>.<type:file_type>`.
    * The directory should have a .tags file containing one file per projected file. The files in the .tags folder should contain the tags applied to the document, one line at a time.
    * If changes are made to the .tags files, or files are added or deleted, it should be possible to use a 'sync' command to push the tag updates into the database.
    * It should be possible to safely re-project a new set of files into the same directory, to change which files are being viewed.
    * Together, I expect proj/sync to make it far easier to use tagged files as part of day-to-day workflows.

### Long Term
* Provide binding to popular languages.
* Provide a GUI client, likely written in electron.
* Provide a REST wrapper around tagsystem. Currently, tagsystem is written to be expressly a single user application. There's no telling what happens if two processes interact with tsys simultaniously. A http server multiplexing requests would be the ideal way to provide parallel access to a tagsystem database.
* Fix how we track pws so it doesn't get stored globally in the db, but is instead tracked per process
    1. Wrap tsys in a bash/csh/zsh/fish/batch/powershell function that sets an env variable TSPWS. Shell functions don't spawn in a seperate process.
    2. Get the parent process uuid (process id + start time) and store a row in the database for it. Garbage collect the db when it's too big by checking what processes are stopped. Store the pws for that parent in the database. This may lead to un-expected consequences.
* Replace _get_dbi function with a token/switch based solution to avoid string comparisons.
* Provide a read only, no copy version of tssearch to speed up searching for documents in sets.
* Provide a set difference operation in addition to a set intersection operation.
* Provide a key/value store associated with files. This may be feature creep and redundant to extended file attributes.