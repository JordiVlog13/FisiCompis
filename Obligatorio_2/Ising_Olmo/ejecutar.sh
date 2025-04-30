 g++ -fopenmp estado_inicial.cpp -o estado_inicial.exe 
 ./estado_inicial.exe
 g++ -fopenmp evolucion.cpp -o evolucion.exe -lm -O3
 ./evolucion.exe 2.269 500
 python animacion_ising.py