Lockfile
========

A lockfile is a file created in the filesystem to establish that a given file is locked for reading and writing by other processes. If `.git/index` needs to be written, for example, a `.git/index.lock` file will be created. Any modifications needed to the index will be written to the lockfile, then the changes are committed by renaming `index.lock` to `index`. While the lockfile exists, no other process may read or write to the locked file.