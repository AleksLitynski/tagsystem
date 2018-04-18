DOING:
- debug core operations

TODO 0.0.1:
- provide update db function and error if db is out of date
- add --help option to all commands
    - env vars:
        - current db dir
        - current working set
- test on windows
- publish 0.0.1
    - upload to github
    - upload 'release'
    - write readme
    - comment everything


TODO 0.0.2:
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
- allow all lmdb operations to take a MDB_txn object so this shit is actually transactional


DONE:
- implement tagging from stdin (check if each item is an id or a path and support both)
- clean up parsing and utils files