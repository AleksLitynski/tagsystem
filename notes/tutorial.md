
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