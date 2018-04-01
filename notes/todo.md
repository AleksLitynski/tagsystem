DOING:
- debug core operations

TODO:
- provide update db function and error if db is out of date
- implement proj/sync logic
    - proj
        - force to overwrite directory
        - don't map names to name: and type: tags
    - sync
        - pull first or push first
        - all file deletion
- add --help option to all commands
    - env vars:
        - current db dir
        - current working set
- a way for users to see what tags a document has (other than project)
- allow all lmdb operations to take a MDB_txn object so this shit is actually transactional
- clean up parsing and utils files 


WON'T DO:
- parameter handling
    - check how arguments are packed from the cli
        - Check how bash reserved chars are handled
        - Check how spaces are handled 
        - Check how quotes are handled
    - pack quoted strings
    - allow for string args with spaces




DONE:
- implement tagging from stdin (check if each item is an id or a path and support both)