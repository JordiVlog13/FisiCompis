 g++ -fopenmp estado_inicial.cpp -o estado_inicial.exe
 ./estado_inicial.exe
 g++ -fopenmp evolucion.cpp -o evolucion.exe -lm 
 ./evolucion.exe 2.2 1000
 python animacion_ising.py