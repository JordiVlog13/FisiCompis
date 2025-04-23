# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <omp.h>
# define N 10
# define COLUMNAS N // Número de columnas 
# define FILAS N // Número de filas

void paso_Markov(int estado[][COLUMNAS], double temperatura);

int main(){
    srand(time(NULL)); // Inicializamos la semilla 

    const int pasos_Montecarlo = 1000; // Número de pasos Montecarlo: iteraciones globales
    const int pasos_Markov = FILAS * COLUMNAS; // Número de pasos Markov: iteraciones sobre cada partícula
    double temp = 1.5; // Temperatura del sistema
    int estado[FILAS][COLUMNAS]; // Definimos el estado de la red

    // Abrimos los ficheros de lecrura y escritura y comprobamos que no hay error al leerlos
    FILE *ifile = fopen("estado_inicial.txt", "r");
    FILE *ofile = fopen("estados.txt", "w");

    if (ifile == NULL)
    {
        perror("No se pudo abrir el archivo de lectura\n");
        return 1;
    }

    if (ofile == NULL)
    {
        perror("No se pudo abrir el archivo de escritura\n");
        return 1;
    }

    // Leemos los datos iniciales del fichero "estado_inicial.txt"
    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            if(fscanf(ifile, "%d", &estado[i][j]) != 1)
            {
                printf("Error al leer el valor [%d][%d].\n", i, j);
                fclose(ifile);
                return 1;
            }
        }
        
    }

    // LLevamos a cabo N pasos Montecarlo
    double inicio = omp_get_wtime();
    for (int n = 0; n < pasos_Montecarlo; n++)
    {
        for (int p = 0; p < pasos_Markov; p++) // Repetimos los pasos Markov una vez para cada elemento de la red (de forma aleatoria)
        {
            paso_Markov(estado, temp);
        }

        // Después de cada paso Montecarlo escribimos el estado en el fichero de salida
        for (int i = 0; i < FILAS; i++)
        {
            for (int j = 0; j < COLUMNAS-1; j++)
            {
                if(fprintf(ofile, "%d, ", estado[i][j]) < 0)
                {
                    printf("Error al escribir el valor [%d][%d].\n", i, j);
                    fclose(ifile);
                    return 1;
                }
            }
            fprintf(ofile, "%d\n", estado[i][COLUMNAS-1]);
            
        }
        fprintf(ofile, "\n");
    }
    double final = omp_get_wtime();
    printf("El tiempo de ejecucion ha sido de %f s.", final - inicio);
    
    
    fclose(ifile);
    fclose(ofile);
    
    return 0;
}

void paso_Markov(int estado[][COLUMNAS], double temperatura){
    int n = rand() % FILAS;
    int m = rand() % COLUMNAS;
    double eta = (double) rand() / RAND_MAX;
    int energia = 0;

    for (int i = n-1; i <= n+1; i += 2)
    {
        int fila = (i + FILAS) % FILAS;
        energia += estado[fila][m];
            
    }

    for (int j = m-1; j <= m+1; j += 2)
    {    
        int columna = (j + COLUMNAS) % COLUMNAS;
        energia += estado[n][columna];
        
    }

    energia *= 2 * estado[n][m];

    if (exp(-energia/temperatura) < 1)
    {
        if (eta < exp(-energia/temperatura))
        {
            estado[n][m] = -estado[n][m];
        }
    }
    else
    {
        estado[n][m] = -estado[n][m];
    }

    
    return;
}
