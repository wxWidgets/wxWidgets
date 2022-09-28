This is a mini-quick guide about using Git to share patches for wxWidgets.
Better and extensive information can be found at
https://git-scm.com/book/en/v2  and  https://git-scm.com/docs

What is Git?
------------
Git is a distributed version control system.
'Version control' is a system that records changes to a file or set of files
over time so that you can recall specific versions later.
'Distributed' means that every developer makes a full copy of the files in the
repository of the project, including their full history of changes.

What is GitHub?
---------------
It's a host of Git repositories. Its site https://github.com is a web interface
for repositories. wxWidgets is just one of many projects that uses a repository
that you can browse or download or collaborate with.

1. Install git
--------------
Download from https://git-scm.com/downloads the version you like. It can be a
terminal-based or a GUI one.
In this guide I use the terminal, just to show the gist of the usage of git.

Git needs to identify you by user/mail because your uploads will carry this info.
   git config --global user.name "My name"
   git config --global user.email my@domain.com
Using '--global' will ask you just this time, no matter what project you work on.

2. Download wxWidgets code... perhaps not yet
---------------------------------------------
If you just want the last snap of wx's code, don't plan to modify it, then you
don't need to create a "divergence" from the "main-stream" of wx. You may
simply download the repository, compile wx and continue with your business.
But if you want to contribute, you need a "fork" and a "branch".

3. Create a fork
----------------
This is where you tell GitHub you create a divergence to upload code to.
First, sign in at https://github.com You may need to first create an account.
I advise to use the same user/email as you did with Git configuration.

A user & password is not enough to upload code. You need other authentication mode.
You may use SSH keys. You may use PAT.
See the Git-book and
https://docs.github.com/en/get-started/getting-started-with-git/about-remote-repositories#cloning-with-https-urls


Now you've signed in, create a fork.
  Browse to https://github.com/wxWidgets/wxWidgets
  About the top right corner find and use the "Fork" button. Select "Create a new fork"
The new fork will be created as github.com/<your_github_name>/wxWidgets.git

This fork is your fork. You don't need another fork to submit changes for
another contribution to wxWidgets.

4. Download wxWidgets code, now yes
-----------------------------------
Create the directory 'wxFork' (or the name you like).

Option a)
  git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git wxFork
Option b)
  git clone --recurse-submodules https://github.com/<your_github_name>/wxWidgets.git wxFork

This will populate the 'wxFork' directory in your local PC with all last
("master branch") wx sources.

'clone' stores the remote (github.com/xxxxx) and labels it as "origin".
The active branch (see '5.') in that remote is labeled as "master".

The difference between a) and b) is met when you want to "push" your changes
to wxWidgets repository. See point '7.' some lines below.

If you need to update your local copy with the current "remote" wxWidgets
master, you can use:
  git pull --ff-only

The files downloaded with this command will be stored for your local active "branch"

5. Branches. Work your changes locally
--------------------------------------
Same as you need a 'fork' in GitHub you want a 'branch' (read: divergence) in you local PC.
  git checkout -b mybranch origin/master

This will create (the '-b' flag) a branch named "mybranch", which reflects all
changes since the starting point 'origin/master' (what you did at step 4).
'checkout' will also "jump" (read: set as active) to that branch.

You may have several branches (say you work for different patches).
You can jump to an existing branch named 'br2' by
  git checkout br2
To list all available branches use
  git branch
To delete the branch "badbranch" use
  git branch -d badbranch

An important feature of Git is that if you open a file with your editor you will see
the version that exists in the active branch. If you checkout other branch, you'll
see other version (if it's different, obviously).
In other words, Git changes your working directory. So be sure to switch to the
proper branch before any work.

Make your changes, compile and test them in that branch you jumped to. Only one
branch per contribution, please.

6. Prepare for sharing
-----------------------
Not all changed files will be part of your contribution. No compiled files,
just code files.

  git status -v
Inspect the output of this command. It will show the files that you changed
and/or added.

Tell Git the staged files:
  git add --all
or tell one by one:
  git add file1 file2 file3


Git needs not only changes, but snapshots. To do this, commit your changes:
  git commit -m "My beautiful commit message"

  git diff
is useful to see only the changes. Review it before committing.
  git diff --no-prefix master... > my_custom_work.patch
prepares a .patch file, which you may use instead of a "pull request" (see '8.')

7. Sending your work to GitHub
-------------------------------
You can not write to https://github.com/wxWidgets because is not yours.
Where you can write to is to your fork, https://github.com/<your_github_name>

The basic git command 'push' looks like
  git push <remote> <branch>

Recall point '4.' when 'clone' stored the remote and labeled it as 'origin'.
If you used <your_github_name> you simply call
  git push origin mybranch

But if you used the wxWidgets remote then you don't have write access and so
you need to create a new remote to your fork.
  git remote add my-github git@github.com/<your_github_name>/wxWidgets.git
where 'my-github' is the name you want.

Now you can push your branch to that remote:
   git push my-github mybranch
 
Likely you may 'push' several times your same branch to the same remote. If
you use
   git push --set-upstream my-github mybranch
or its equivalent
   git push -u my-github mybranch
then the next time you call 'push' won't need any arguments:
   git push

8. What is a Pull Request?
--------------------------
You sent your code, but nobody is aware of it yet.
Time to sent a notification. of a special type that allows not only to keep all
contributions organized, but also allow others to comment your changes.

Browse to GitHub, sign in, and use the fork button to enter your fork.
You should see your pushed branch there. And a button 'Compare and pull request'.

Other paths to create a PR are:
Search the 'Contribute' combo box and select 'Open pull request'
Or, on the left, click the button for your branches and create the PR from
there ('New pull request').
The general 'Pull requests' tab may also be useful.

Now people, specially administrators, will see your notification and will be
able to make comments.
Finally, when your code is accepted, some administrator will merge it to the
main master stream of wxWidgets.

9. Testing other's PR code
--------------------------
Let's say you are looking at PR #12345 and you want to download and test the
commits.
In your browser you see the URL https://github.com/wxWidgets/wxWidgets/pull/12345
Change it to
   https://patch-diff.githubusercontent.com/raw/wxWidgets/wxWidgets/pull/12345.patch
and save that link as a file in your local wxFork dir (let's say you name this file
as 'PR12345.patch'.

Create a branch for it
   git checkout -b testPR

and apply the patch
   git am PR12345.patch

Now you can compile and test this contribution in a safe environment (i.e. a branch).


