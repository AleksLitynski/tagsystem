DOING:
- comment everything


TODO 0.0.1:
- test on windows
- publish 0.0.1
    - upload to github
    - upload 'release'
    - write readme
        - env vars:
            - current db dir
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



TODO 0.0.X:
- language bindings (gdscript, javascript probably)
- gui client (electron, probably)
- fix how we track pws so it doesn't get stored globally in the db, but is instead tracked per process
    1. Wrap tsys in a bash/csh/zsh/fish/batch/powershell function that sets the env variable. Shell functions don't spawn in a seperate process.
    2. Get the parent uuid (process id + start time) and store a row in the database for it. Garbage collect the db when it's too big by checking what processes are stopped. Store the pws for that parent in the database. This may lead to un-expected consequences, but IDK what they're going to be yet :). Get Parent process id from $PPID. Get start time from 
    3. Just store a single variable for the current path in the db. Let it be shared across processes
- replace _get_dbi with a token/switch based solution to avoid string compares


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
- add --help option to all commands
- a way for users to see what tags a document has (other than proj) (--tags flag added to list command)