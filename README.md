# Haverford College 
## Computer Science 350: Compiler Design

## Overview:
Haverford's CS core curriculum is centered around their proprietary assembly language HERA. In this class, we are tasked with writing a compiler to go from Andrew Appel's Tiger language to the HERA language. The class follows many of the sections in [Appel's compiler design book](https://www.cs.princeton.edu/~appel/modern/c/). 

### Purpose:
During the semester I took the course, I only made it through 9 out of the 10 assignments to finish the compiler. Here, I intend to start from the beginning and finish the compiler, incorporating all of the information I have learned since then. 

## Installation
0. Install `bison`, `flex`, `csh`, `gcc` or compiler of choice
1. Clone Repo
2. Make a folder at `/home/courses/`
3. Unzip HaverfordCS.7z containing standard library and other helper files in that folder
4. Build and run Tiger

## Running instructions
- Run `make` to build a new executable
- Then run, `compiler-tests [0-9][a-z]` to run it on specific milestone(s) test cases
- Or run `compiler-test *.tig` to run it on a specific tiger file
- Alternatively Run `Debug/tiger *.tig` to do the same thing
- Or run `Debug/tiger` to run with standard input

### Contents:
- Milestone 1: Subtraction, Parenthesis, and Comments
- Milestone 2: Calls to print and printint; one simple string, type checking in Bison
- Milestone 3: Multiple strings, Sethi-Ullman register allocation, division, interesting escape sequences
- Milestone 4: Type-checking on the AST, unary negation, booleans and if/else, full tiger standard library
- Milestone 5: Sequences, if/then, void (a.k.a. "no value"), and (optionally) nil, comparisons, while, break
- Milestone 6: For Loops
- Milestone 7: Let Expressions
- Milestone 8: Assignment experessions, Function definitions
- Milestone 9: Variable accession within functions, type declarations, arrays, Record types
- Milestone 10: Debugging :)

### Notes for each Milestone:
- Milestone 1: Sub and Parenthesis done without much difficulty. Comments were handled in two ways: Regular expression for C-style comments (which can't be nested), and advanced Lex functionality for Tiger-style nested comments. I learned everything about Lex functionality from [this Stanford Lex description](https://web.stanford.edu/class/archive/cs/cs143/cs143.1112/materials/other/manflex.html). There it meantions things like Starting conditions and usage of automata with regular expressions. Of particular interest was the Start conditions section.  
- Milestone 3: (d) Implementing escape sequences. The control characters are ‘\a’, ‘\b’, ‘\f’, ‘\n’, ‘\r’, ‘\t’, ‘\v’. To handle \ddd, I convert just ddd from string to int, then convert to octal number and back to string, if it's less than 377. Referencing the [Tiger Language Reference Manual](https://www.lrde.epita.fr/~tiger/tiger.html#Lexical-Specifications). 
- Milestone 4: (a) Much like result\_reg in AST.h uses init\_result\_reg in result.cc, I have chosen to implement a similar structure to handle typechecking. I have typecheck() and init\_typecheck() where the former calls the latter if the type has not yet been calculated and stored. Otherwise, it just returns the stored type value. (b) For unary negation, I had help from this [documentation](https://www.gnu.org/software/bison/manual/html_node/Contextual-Precedence.html). (d) Using a Global Symbol Table (ST) for Function definitions in the Tiger Standard Library. 
- Milestone 5: (b) Comparison operators were created in conjunction with the opExp class. Should be able to handle Ints and Strings.
- Milestone 6: For If, While, Comparison operators, I have global variables to keep track of which they are as a private class property. This is used in Branching and Label generation to create proper control flow. Also partially keeping track of Stack pointer using a global variable. 
- Milestone 7:
- Milestone 8: Moved to using synthesized variable and function libraries since keeping track of a global library was too intense. 
- Milestone 9: 

### Remaining Tests and TODO:
#### Concepts:
 - certain escape characters, make sure I implemented them all in Milestone 3
 - Nil
 - allow LetExps to take Nil in the body

#### Tests
-  Checking each test result one by one is tedious. It would be nice to have a test environment where we can:
    - compare HERA output to expected
    - compare HERA-C final value to expected
    - check for errors

#### Genera TODO
 - Getting tons of WARNING: REFERENCE TO MEMORY AT/JUST-ABOVE SP in later Milestones
 - Clean up the repo by compartmentalizing some of the files
 - Add better CLI arg handling


#### Test cases:
 - 7_40_LATER_SU_test.tig
 - 8b_61_mergesort.tig
 - 8b_68_HelpMakeOptimalAnagram.tig

