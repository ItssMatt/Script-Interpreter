# About
A Basic Script Interpreter written in C++. 
Every script has its own stack. For now you can only create INT variables.
You can make your own scripts using the following opcodes: 
- PUSH (push an element on the stack),
- POP (pop an element from the stack),
- SUM (calculate the SUM of the last 2 elements pushed on the stack),
- SUB (calculate the difference between a variable and a constant integer),
- MOV ( copies an integer and puts it on a variable ),
- PRINT ( prints a message in console, from strings to constant integers and variables )

# Syntax
The interpreter also checks the opcodes to be written syntactically correct. If an error comes up while executing the script, it is thrown and printed in the console.
