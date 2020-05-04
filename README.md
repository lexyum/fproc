# fproc - read and manipulate FASTA files

## What is a FASTA file?
The FASTA format is a [text-based format for representing nucleotide sequences](https://en.wikipedia.org/wiki/FASTA_format). In this implementation a file is assumed to consist of alternating description lines and sequences. Description lines are delineated by a '>' symbol at the start of each line, and sequence lines are assumed to be contiguous (no line breaks).

## How do I use fproc?
To run fproc it is only necessary to build with GNU Make and run the resulting executable (`./fproc` by default). Entering the command `help` (or any other unrecognised command) causes an exhaustive list of commands to be printed. Two example files (testinput1.fasta and testinput2.fasta) are provided to run tests on, the former a skeleton example and the second resembling an actual collection of sequences.

## What actually *is* fproc?
The core of fproc is a binary tree implementation, using the Day-Stout-Warren algorithm to balance it according to a specified ordering. The command `read <file>` checks for the existence of the specified file, and if found initialises a binary tree and places each definition line and corresponding sequence in a node. 

