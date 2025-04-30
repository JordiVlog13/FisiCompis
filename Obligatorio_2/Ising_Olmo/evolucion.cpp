# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <omp.h>
# include "C:\Users\olmov\Desktop\compu\FisiCompis\Obligatorio_2\Ising_Olmo\include\Random123\philox.h"
using namespace r123;
#define N 100
#define FILAS N
#define COLUMNAS N

// Función que realiza una actualización de la red siguiendo el método checkerboard
// 'color' debe ser 0 (blanco) o 1 (negro) para seleccionar el subconjunto de celdas
void paso_Markov_checkerboard(int estado[][COLUMNAS], double temperatura, int color, unsigned int& seed, Philox4x32& rng, Philox4x32::key_type key, unsigned long long& counter) {
    #pragma omp for collapse(2) schedule(static) // Paraleliza los bucles anidados i, j
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            // Solo actualizamos las celdas del color especificado
            if ((i + j) % 2 != color) continue;

            // Generación de un número aleatorio eta con Philox
            Philox4x32::ctr_type ctr = {{(uint32_t)(counter++), 0, 0, 0}};
            Philox4x32::ctr_type result = rng(ctr, key);
            double eta = (double) result[0] / (double) UINT32_MAX;

            // Cálculo del cambio de energía
            int energia = 0;
            for (int di = -1; di <= 1; di += 2) // Vecinos verticales
                energia += estado[(i + di + FILAS) % FILAS][j];
            for (int dj = -1; dj <= 1; dj += 2) // Vecinos horizontales
                energia += estado[i][(j + dj + COLUMNAS) % COLUMNAS];

            energia *= 2 * estado[i][j];

            // Aplicación del criterio de Metropolis
            if (exp(-energia / temperatura) < 1) {
                if (eta < exp(-energia / temperatura))
                    estado[i][j] = -estado[i][j];
            } else {
                estado[i][j] = -estado[i][j];
            }
        }
    }
}

double calcular_energia(int estado[][COLUMNAS]){
    double energia = 0;
    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            double e0 = 0;
            for (int di = -1; di < 2; di+=2)
            {
                e0 += estado[(i + di + FILAS) % FILAS][j];
            }

            for (int dj = -1; dj < 2; dj+=2)
            {
                e0 += estado[i][(j + dj + COLUMNAS) % COLUMNAS];
            }
            e0 *= estado[i][j];
            energia += e0;
        }
        
    }
    return energia * (-0.5);
}

double calcular_magnetizacion(int estado[][COLUMNAS]){
    double magnetizacion = 0;
    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            magnetizacion += estado[i][j];
        }
        
    }
    
    return abs(magnetizacion) / (FILAS * COLUMNAS);
}

int main(int argc, char* argv[]) {
    // Valores por defecto
    int pasos_Montecarlo = 1000; // Número de pasos de Monte Carlo
    double temp = 1.5;            // Temperatura del sistema

    // Leer parámetros desde la línea de comandos (opcional)
    if (argc > 1) temp = atof(argv[1]);
    if (argc > 2) pasos_Montecarlo = atoi(argv[2]);

    printf("Usando temperatura = %.2f, pasos Monte Carlo = %d\n", temp, pasos_Montecarlo);

    int estado[FILAS][COLUMNAS];       // Matriz de estado (red de Ising)

    FILE *ifile = fopen("estado_inicial.txt", "r");
    FILE *ofile = fopen("estados.txt", "w");
    FILE *energ_file = fopen("energias.txt", "w");
    FILE *magnet_file = fopen("magnetizaciones.txt", "w");

    if (ifile == NULL || ofile == NULL || energ_file == NULL || magnet_file == NULL) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    // Lectura del estado inicial desde archivo
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            fscanf(ifile, "%d", &estado[i][j]);
        }
    }

    double inicio = omp_get_wtime(); // Medición de tiempo de ejecución

    unsigned long long counters[omp_get_max_threads()] = {0};
    for (int n = 0; n < pasos_Montecarlo; n++) {
        // Cada paso de Monte Carlo se divide en dos fases (blancas y negras)
        #pragma omp parallel
        {
            // Inicialización del generador aleatorio por hilo
            unsigned int seed = time(NULL) ^ omp_get_thread_num();
            Philox4x32 rng;
            Philox4x32::key_type key = {{(uint32_t)omp_get_thread_num() + 123}};
            int tid = omp_get_thread_num();
            unsigned long long& counter = counters[tid];

            // Fase blanca: actualiza celdas (i+j)%2 == 0
            paso_Markov_checkerboard(estado, temp, 0, seed, rng, key, counter);
            // Fase negra: actualiza celdas (i+j)%2 == 1
            paso_Markov_checkerboard(estado, temp, 1, seed, rng, key, counter);
        }

        // Escribir el estado actual al archivo de salida
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS - 1; j++)
                fprintf(ofile, "%d,", estado[i][j]);
            fprintf(ofile, "%d\n", estado[i][COLUMNAS - 1]);
        }
        fprintf(ofile, "\n");
        
        // Escribir la energía del sistema
        fprintf(energ_file, "%f\n", calcular_energia(estado));

        // Escribir la magnetización del sistema
        fprintf(magnet_file, "%f\n", calcular_magnetizacion(estado));
    }

    double final = omp_get_wtime();
    printf("Tiempo de ejecucion: %f s\n", final - inicio);

    fclose(ifile);
    fclose(ofile);
    fclose(energ_file);
    fclose(magnet_file);

    return 0;
}
