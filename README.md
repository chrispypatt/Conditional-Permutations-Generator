# Circuits and Conditional Permutations
This program is meant to convert boolean functions (with a specific input format) into conditional permutation representation. 

The conditional permutations are as follows:
   
    A = CBC’B’ ; A’ = BCB’C’
	B = CDC’D’ ; B’ = DCD’C’
	C = DED’E’ ; C’ = EDE’D’
	D = EBE’B’ ; D’ = BEB’E’
	E = DAD’A’ ; E’ = ADA’D’

# Running this code:

	cd <directory location>
	make 
	./bool2cp <b>	: b is the boolean function to parse*

*In  order to parse the boolean function, it must be converted to input format specified below where each operator is a 2 input gate of variables x and y:
* AND(x,y)
* NAND(x,y)
* OR(x,y)
* NOR(x,y)
* XOR(x,y)

This program allows for nested gates. To represent inverted variables, use `'`. 

Examples of valid inputs would be:

	F(x, y, z) = (x&z)’&(x’&y’)’
		AND(NAND(x,z),NAND(x',y'))

	F(x,y) = x+y = (x'y')'
        OR(x,y) = NAND(x',y')

    F(x,y,z) = (x'y'z'),
        NAND(AND(x',y'),z')

	F(x,y,z) = (x&z)'&((y&z)&x')'
		AND(NAND(x,z),NAND(AND(y,z),x'))


