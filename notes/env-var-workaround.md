Two options:
1. Wrap tsys in a bash/csh/zsh/fish/batch/powershell function that sets the env variable. Shell functions don't spawn in a seperate process.
2. Get the parent uuid (process id + start time) and store a row in the database for it. Garbage collect the db when it's too big by checking what processes are stopped. Store the pws for that parent in the database. This may lead to un-expected consequences, but IDK what they're going to be yet :). Get Parent process id from $PPID. Get start time from 
3. Just store a single variable for the current path in the db. Let it be shared across processes

Just do 3 and get on with the project
// For portability sake, I'm going with v2 for now, even though it's hackier in a sense

https://dpaste.de/6evg



prince_jammys has disconnected (Client Quit)
 9∶21 AM
traxex2	nokdoot: simple command line utilities like wc, cp, cat
 9∶27 AM
ajl	I'm trying to write a command line utility with an API analogous to the cli filesystem API (cd, pwd, ls, etc). The utility I'm trying to write operates on sets of tags instead of folders. Instead of being in folder x/y/z, you're at the intersection of sets x+y+z. I was going to store the equivalent of the working directory in an environment variable so each shell could have it's own working set of tags, but have run into a snag. When a child process is spawned to change the working set, it gets it's own copy of all environment variables, then, when it exits, the parent processes environment variables are left untouched. Can anyone think of another way to approach this problem?
thinkdoge joined
hkhoi joined
k0mpa joined
banisterfiend has disconnected (Quit: My MacBook has gone to sleep. ZZZzzz…)
 9∶36 AM
jp	ajl: the child can't affect the parent environment by design
you need some sort of ipc like a pipe
(or use threads)
koollman	ajl: I'm not sure I understand the problem you're trying to solve
ajl	I'm sorry. It's a hobby project I've been tinkering with for months, so I'm probably pretty far off on a limb. 
koollman	ajl: let's imagine this with the fs api. chdir in a child does not affect the parent. But you can still run recursive algorithms by forking and the child process does not affect the parent.
ajl: so why would that information need to go 'upward' in your problem ?
othias	koollman: cd command
ajl	https://pastebin.com/D59SGrRY
jp	cd doesn't need to fork
 ajl: so is this and abstraction over chdir, or an implementation?
an*
ajl	So, I'm kind of realizing that that cd is built into the shell and as you say, doesn't actually fork. I guess I'm wondering if there's a way to do something similar while still in user space
jp	cd calls chdir() and the file descriptor which says where you are is changed
koollman	othias: cd does not send information upward, it calls chdir in the current program (the shell) :)
ajl	It's not really related to chdir except that I'd like it to follow similar semantics. I have a database of tagged files, and am providing a command line tool to select a subset of the tagged files, based on their tags. So, the database is technically a folder full of files with random number names. As a user, you could run `ts ls ajl+Projects+js` and get a list of all files tagged with 'ajl', 'Projects', and 'js'. 
Random832	ajl, but the point is, chdir doesn't affect the parent process, so why would *your* thing need to affect the parent process?
koollman	ajl: the shell is absolutely in userspace :)
Random832	oh wait i get it
koollman	ajl: but I guess I understand your problem slightly better
othias	koollman: yeah I was saying this because he said he wanted to do this in a child process by updating some environment variable
Random832	maybe make ts a shell function, and implement "ts cd" within the function, and have a "ts.real" binary that it calls for all the heavy stuff
koollman	(using a shell function or a shell alias would work)
jp	this seems like pthreads would be more useful thab fork
ajl	So, ts is an executable written in C, and depending on parameters will create files in the tag database, apply tags to the files, or print the path to a subset of the tagged files
jp	than*
ajl	Given that the executable will (as far as I know) always be spawned in a child process, I don't think setting an environment variable is an appropriate way to track the users working set, although initially I thought it would work. 
oh, I didn't what you were saying closely enough
Wouldn't a shell function still spawn in a child process?
VLetrmx	don't think so
jp	it will in badh
hvsr	Is this semantically correct when handling errors? - https://pastebin.com/raw/8rPcWKaY
jp	bash*
VLetrmx	jp, oh I didn't know that
Random832	jp, what
jp	shell functions in bash live in the environment
VLetrmx	hvsr, there's a few fundamental things you're missing there
Random832	ok i think you're confused
hvsr	VLetrmx: Like?
VLetrmx	you're not handling short writes
Random832	I think ajl is asking of shell functions *run* in a child process [when not otherwise creating a child process], not if they are inherited *by* child processes.
and they do not.
VLetrmx	if you're on Linux you're also not handling the possibility of EINTR
azarus	main() should come last
Random832	you can set variables or cd in a shell function and if you just run the function it will be reflected in the main bash process
azarus	spaces are weirdly placed
VLetrmx	you're not handling a return of NULL from malloc
koollman	jp, ajl : https://dpaste.de/6evg (now, imagine doing stuff with some sort of TSPWS, I guesS)
Random832	also, functions don't live in the environment, they're - optionally - exported to the environment. But they're inherited by [fork not exec] child shell processes regardless.
hvsr	azarus: I think spacing improves readablity.. and they are not randomly applied
azarus: idk
ajl	Oh, I see. That make sense. I'm going to read up on shell functions and see what I can do. I feel like this hurts portability, but is definitely a way to get what I want.
hvsr	VLetrmx: Does EINTR need to be handled separately?
koollman	ajl: one other 'obvious' way would be to write your own shell, but that's rather tedious :)
VLetrmx	hvsr, not sure what you mean
hvsr, in addition I don't think you're handling the case where cat is executed with no args, in which case you should read from stdin
ajl	It is a little tedious. The other thing I'm considering is abandoning the close analogy to the filesystem and tracking the working set in the database, so you end up with the working set being the same across all shells.
huaw	hello, I am trying to accomplish the following: void * ptr = malloc(sizeof(someStruct)+rest) (inserting a struct before a block of size "rest"), writing into the struct by someStruct *s = ptr; ptr-> ... it is there where valgrind complains of an error Invalid write of size 8. Do you happen to know why?
VLetrmx	hvsr, and then finally multiple files ofcourse
hvsr	VLetrmx: not trying to implement cat properly but to handle errors properly..
VLetrmx	o
othias	huaw: can you show more code please
huaw	othias, sure, a second please
koollman	ajl: that's another way. have an external reference that is not a environment variable. It does bring some other potential troubles (since the variable is now shared, running two programs will be tricky, or require careful handling)
ajl	koollman: I think I'm going to start with the shell function idea, but I really don't look forward to provide a wrapper for every shell under the sun.
huaw	silly me, I used the wrong struct in sizeof ... nonetheless, thank you for your help othias
othias	huaw: this cannot happen if you follow the idiom  struct foo *s = malloc(sizeof *s);
huaw	othias, it was even worse as I #defined sizeof() as something, hoping to save some time
i.e. #define ALLOC_OFFSET sizeof(someStruct) (wrapped w/ ifndef endif) and then just malloc(ALLOC_OFFSET+xxx)
othias	that does sound like a bad idea lol
huaw	yeah, as soon as I figured out what the error was, I did :%s/ALLOC_OFFSET/sizeof(...)/g just to be sure
koollman	ajl: you mostly need two. bourne-like and csh-like
10∶24 AM
gbobby	hvsr: the parameter type and return code of your if_error() method are not suitable for the return code of read(), which returns ssize_t, not int
5 users joined, 5 users left▶10∶37 AM
izabera	can a compiler reorder assignments to volatile variables?
5 users joined, 6 users left▶10∶55 AM
hvsr	gbobby: hmmmm :)