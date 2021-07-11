# Haverford College 
## Computer Science 350: Compiler Design

### Overview:
Haverford's CS core curriculum is centered around their proprietary assembly language HERA. In this class, we are tasked with writing a compiler to go from Andrew Appel's Tiger language to the HERA language. The class follows many of the sections in (Appel's compiler design book)[https://www.cs.princeton.edu/~appel/modern/c/]. 

### Purpose:
During the semester I took the course, I only made it through 9 out of the 10 assignments to finish the compiler. Here, I intend to start from the beginning and finish the compiler, incorporating all of the information I have learned since then. 

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
- Milestone 1: Sub and Parenthesis done without much difficulty. Comments were handled in two ways: Regular expression for C-style comments (which can't be nested), and advanced Lex functionality for Tiger-style nested comments. I learned everything about Lex functionality from (this Stanford Lex description)[https://web.stanford.edu/class/archive/cs/cs143/cs143.1112/materials/other/manflex.html]. There it meantions things like Starting conditions and usage of automata with regular expressions. Of particular interest was the Start conditions section.  
