# include <stdlib.h>
# include <stdio.h>
# include <time.h>
# include <omp.h>
# define N 100
# define COLUMNAS N
# define FILAS N

int main(){
    srand(time(NULL));
    FILE *archivo = fopen("estado_inicial.txt", "w");
    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            fprintf(archivo, "%d\t", (rand() % 2) * 2 - 1);
        }
        fprintf(archivo, "\n");
        
    }

    fclose(archivo);
    return 0;
}