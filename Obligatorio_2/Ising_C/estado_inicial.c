# include <stdlib.h>
# include <stdio.h>
# define COLUMNAS 100
# define FILAS 100

int main(){
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