DOING:
- add --help option to all commands
    - env vars:
        - current db dir

TODO 0.0.1:
- test on windows
- publish 0.0.1
    - upload to github
    - upload 'release'
    - write readme
    - comment everything
    - delete old notes (can recover via git history)




TODO 0.0.2:
- clean up header references and compile headers into a single file to simplify import
- clean up CLI macro/provide C library that simplifies using the DB similar to the CLI api
- provide update db function and error if db is out of date
- implement proj/sync logic
    - proj
        - force to overwrite directory
        - don't map names to name: and type: tags
    - sync
        - pull first or push first
        - all file deletion
- a way for users to see what tags a document has (other than proj)

TODO 0.0.X:
- language bindings (gdscript, javascript probably)
- gui client (electron, probably)

DONE:
- implement tagging from stdin (check if each item is an id or a path and support both)
- clean up parsing and utils files
- debug core operations
- env var: current working set
    - because env variables are set in the child process and not the parent process (the spawning shell) setting the working dir as an env variable is useless.
    - instead, I am setting the env variable as a property in the meta db table. It ends up shared across all db users, but the db is single user, so it doesn't matter, really
- allow all lmdb operations to take a MDB_txn object so this shit is actually transactional
- fix issue with remove where tags aren't removed
- find and fix bug in cs/pws parsing that sometimes appends an 'a' to tag and generally sometimes borks