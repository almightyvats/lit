# lit
`lit` is a version control system (VCS) similar to Git, just not as powerfull.

### lit usage

All functionality of the VCS is bundled into one executable named `lit`.
Similar to `git`, it provides multiple *sub-commands* like `checkout` and `status`.
It is a CLI application.
The usage of the *sub-commands* are explained below.

### `lit help`

For listing the available commands and their usage, write `lit help`.
Each sub-command will be listed and described briefly.

### `lit init`

In order to initialize a directory as the root of your *lit repository(local)*, write `lit init`.
```
$ lit init
This directory is now the root of the repository.
```
If you see a message like this then your current repository is setup for lit and can use the sub-commands.

### `lit commit`

The syntax of using the commit command is `lit commit <commit message>`.
This command will commit all your changes in the current directory with a unique identifier, which you see after commiting.

```
$ lit commit 'Initial commit'
Commit: r0
Date: Mon Nov 18 23:27:53 CEST 2020
```
Note: There's no staging area like Git, also you can't choose particular files to commit, *all* changes will be committed.

### `lit show`

The syntax for using the show command is `lit show` or `lit show <commit no>`.
This command allows you to inspect a commit by specifying the unique commit number.
If no commit is specified, display the currently checked out one.

```
$ lit show r0
Commit: r0
Parent: none
Date: Mon Nov 16 23:41:04 2020

Intial commit


--- ~/file1~
+++ ~/file1~
@@ -0,0 +1,2 @@
+Greetings from Lit
+Hope you have a wonderful time in checking the project
```

### `lit status`

As you start working with your personal project and at anytime in your project you want to check if any file is added, removed, and/or modified since the last time you checkout or commit changes.
Use the `lit status` command, an A(green) for added files, an M (Yellow) for modified files, and a D(Red) for deleted files, along with the file names will be served.

```
$ lit status
M  "file1"
A  "file2"
```
Assuming in the above case that some modifications were done in *file1* and a new *file2* was added.

### `lit checkout`

The syntax for using this command is `lit checkout` or `lit checkout <commit no>`.
This resets the state of all files to the given commit's state.
All un-committed changes are dropped upon checkout.

If no commit is specified, reset the current state to the currently checked out commit.

Note: You can add *new branches* by first checking out a previous commit, and then creating a new commit.

### `lit merge`

The syntax for using this command is `lit merge <commit no>`.
This command initiates a merge with the currently checked out commit and the specified commit.

If no conflict between the two branches that you want to merge, a merge followed by an auto-commit will occur.
Consider the following two scenarios for understanding the working of merge:

You have three commits already (r0, r1, r2), in order to make a new branch,
1.  Checkout r0
2.  Add a *file3*
3.  Make a commit

```
$ lit checkout r0
$ lit status
A  "file3"
$ lit commit 'Adding file3'
Commit: r3
Date: Tue Nov 17 00:08:19 2020

```
Now that your branch is created, a merge can be done as follows,
1.  Checkout r2
2.  Merge into r3

```
$ lit checkout r2
$ lit merge r3

Commit: r4
Date: Tue Nov 17 00:18:24 2020

```
This would mean that your merge was successful without any conflict and new commit (r4) was made automatically.

Consider the next scenario,

You continue to work on your personal project and you make changes to the files and make another commit (r5).
Then you make a branch by checking out r2 and make changes to same file(s) *at the same line(s)*.

Now if you checkout r5 again and merge it into r6, you would probably run into a conflict, that looks something like the following

```
$ lit checkout r5
$ lit merge r6
Merge conflict(s) detected:
- file1
Remove conflict or drop changes to merge!

$ ls
file1	file1.r5	file1.r6	file3	 sub

$ cat file1
Greeting from Lit
Hope you have a wonderful time in checking the project
Excited for the next assignment
<<<<<<<<<< file1.r5
I have no exeperience in game development
========== 
But I have experience in Qt and vectors - the math ones
>>>>>>>>>> file1.r6

```
This means that you have a merge conflict and now you have the following two options:

1. To complete the merge after manually resolving the conflict, invoke the `commit` sub-command.
2. To abort the merge, use `lit checkout`.

Note: A merge is only initiated if there are no un-committed changes.

### `lit log`

The syntax for using this command is `lit log`.
It displays a graph of all commits, one line per commit and also highlights the currently checked out commit.

```
$ lit log
o─┐    ← r7 Merge r2 with r3
│ o      r6 Extending file another way
o │      r5 Extend file1 from master 
o─│      r4 Merge r3 into r2
│ o      r3 Adding file3
o │─ r6  r2 Extending file2
o │      r1 Adding file2
o─┘─ r3  r0 Intial commit
```
In the above graph the left most line is the *main branch* and the second line (second column) will represent all the *branches* ( `o` infront of r3 and r6 suggests that they are branches).
The third line will show the origin of the branch (the parent of the branch), in the above case, r2 is the parent of r6 and r0 is the parent of r3.
