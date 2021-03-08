all: myshell dinPhil matMul

myshell: ./MyShell/myshell.c
	gcc ./MyShell/myshell.c -o myshell

dinPhil: ./DiningPhilosopher/dinPhil.cpp
	gcc ./DiningPhilosopher/dinPhil.cpp -o dinPhil -lpthread

matMul: ./MatrixMultiplication/matMul.cpp
	g++ ./MatrixMultiplication/matMul.cpp -o matMul -lpthread

clean:
	rm myshell dinPhil matMul
