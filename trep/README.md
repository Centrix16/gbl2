# trep -- homoiconicity a programming language of general purpose
## syntax
```
parent (heir)
```

***parent is the name of the function, or just the parent leaf of the tree***
***heir is the arguments of a function, or a child leaf of a tree***

## Hello, world!
```
output "Hello, world!" (~n)
```
output -- display function  
(~n) -- move to a new line

## Calculating the degree of a number (n ^ deg)
```
let (n)(2) (deg)(2) (res)(1)

while (deg)(; 
	(let (res)(* (res)(n)))
	(let (deg)(- (deg)(1)))
)

output "= " (res)(~n)
```

## Installation
on all platforms, to compile the interpreter (ti), just enter:
```
make
```

After that, the `ti` executable file will appear in the directory

## Usage
Using the interpreter is very simple:
```
./ti %your_file%
```
In addition, there is an interactive interpreter written in trep itself, to call it type:
```
./ti trep_line.trep

or

./tl.sh
```
