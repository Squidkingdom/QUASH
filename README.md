# Quash

This is a unix shell written in C++ for my Operating Systems (EECS 678) class in my 5th semester of classes.
This project demonstrates a comphrehensive understanding of low level computing within the context of a Unix shell and their implementations within the operating systems.

## Built in commands:
* jobs
* kill
* cd
* export
* pwd
* echo

## Features and Functions
* Echo w/ multiple arguments, environment variables
* Execution with environment variables
* Pipes between multiple commands
  - Also supports file redirects (including append), environment variables, and built in commands
* Background process with job completion notifications

Extracting the release(https://github.com/Squidkingdom/QUASH/releases) and running `make` should create the binary `quash` which can be run without arguments.
