[![Build Status](https://liambeckman.com/jenkins/job/devilish/badge/icon)](https://liambeckman.com/jenkins/job/devilish/)

This is a minimalistic shell — It comes from the depths of hell, and can do lots of cool things!

---

# Installation

## Prerequisites

gcc :: for compiling the shell.

git :: for a quick git clone.

If you are running Windows, the above utilities will be packaged in any of the following: babun, cmder, or Linux Subsystem for Windows. Take your pick! : )

The above utilities should be installed (or readily available) if you are running a Unix derivative (such as Linux, macOS, or any of the BSD’s).

## Quickstart

```sh
# clone the git repo
git clone https://github.com/lbeckman314/devilish

# enter directory
cd devilish

# inspect the makefile and main script
cat makefile
cat devilish.sh | less

# compile
make

# run the script
./devilish
```

---

# Uninstallation

##  Delete the directory/folder.

```sh
rm -rfI devilish
```

---

# Documentation

You will be provided with a prompt (:), from which you can do all sorts of shell magic!

```sh
# First off, how to exit
: exit

# I can take comments and blank lines

# I can run commands in the background
sleep 100 &
background pid is 31415

# I know standard unix commands
ls
file 1
file 2
file 3

# Control-Z will run a foreground-only session
:^Z
Entering foreground-only mode (& is now ignored)
:^Z
Exiting foreground-only mode

# neato!
```
