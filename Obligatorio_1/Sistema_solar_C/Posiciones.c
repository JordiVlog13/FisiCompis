# include "funciones.h"
# include <stdlib.h>
# include <math.h>
# include <stdio.h>
# include <string.h>
# include <omp.h>

# define PI 3.14159265
# define G 6.6743E-11
# define Ms 1.989E30
# define UA 1.496E11
# define CUERPOS 7 // Cuantos cuerpos estamos tomando

typedef double Vector2D[2];
int main()
{
    double start_time = omp_get_wtime();
    double paso = 0.001;
    int iteraciones = 100000;
    int calcular_energias = 1;

    // Guardamos espacio en memoria para lOS DATOS DE LA SIMULACIÓN
    Vector2D *posiciones = (Vector2D *) malloc(CUERPOS * 2 * sizeof(double)); 
    Vector2D *velocidades = (Vector2D *) malloc(CUERPOS * 2 * sizeof(double));
    Vector2D *aceleraciones = (Vector2D *) malloc(CUERPOS * 2 * sizeof(double));
    double *masas = (double *) malloc(CUERPOS * sizeof(double));
    double excentricidades[CUERPOS];
    double energia_total = 0.0;

    /**********************************************************************************************/
    /**********************  LECTURA DE LAS CONDICIONES INICIALES *********************************/
    /**********************************************************************************************/

    // Leemos el fichero con los datos iniciales
    FILE *archivo = fopen("Datos_iniciales/iniciales.txt", "r");
    if (archivo == NULL)
    {
        perror("No se pudo leer el archivo");
        return 1;
    }

    char buffer[256];

    // Salta cabecera
    fgets(buffer, sizeof(buffer), archivo);

    // Leer línea por línea los datos
    for (int i = 0; i < CUERPOS; i++) {
        if (fgets(buffer, sizeof(buffer), archivo)) {
            sscanf(buffer, "%lf %lf %lf", &masas[i], &excentricidades[i], &posiciones[i][0]);
        }
    }

    // Imprimir para verificar
    for (int i = 0; i < CUERPOS; i++) {
        printf("Cuerpo %d: masa = %.3e, excentricidad = %.8f, posicion = %.3e\n",
                i, masas[i], excentricidades[i], posiciones[i][0]);
        posiciones[i][1] = 0.0;
    }
    
    fclose(archivo);

    /**********************************************************************************************/
    /********************** REESCALAMIENTO DE LAS POSICIONES Y MASAS ******************************/
    /**********************************************************************************************/

    reescalar_posiciones(posiciones, CUERPOS);
    
    reescalar_masas(masas, CUERPOS);
    
    /**********************************************************************************************/
    /**********************  CÁLCULO DE LAS CONDICIONES INICIALES *********************************/
    /**********************************************************************************************/

    // Conociendo el valor de las excentricidades y la distancia respecto al sol calculamos
    // la velocidad inicial (la posición inicial es (x, 0)
    for (int i = 1; i < CUERPOS; i++)
    {
        velocidades[i][0] =  0;
        velocidades[i][1] = sqrt(masas[0] * (1 - excentricidades[i]) / (1 + excentricidades[i]) / posiciones[i][0]);
    }

    // Inicializamos las aceleraciones de los planetas en el instante 0 teniendo en cuenta las interacciones entre ellos
    for (int i = 0; i < CUERPOS; i++)// Recorremos todos los cuerpos
    {
        for (int a = 0; a < 2; a++)// Aplicamos para la coordenada x e y
        {
            aceleraciones[i][a] = 0;
            for (int j = 0; j < CUERPOS; j++)// Calculamos el efecto del cuerpo j sobre el i
            {
                if (i != j)
                {
                    Vector2D R;// Definimos R como la distancia entre los cuerpos i y j
                    R[0] = posiciones[i][0] - posiciones[j][0];
                    R[1] = posiciones[i][1] - posiciones[j][1];
                    aceleraciones[i][a] -= masas[j] * R[a] / pow(norma_cuad(R), 1.5);
                }
            }
        }            
    }

    // Calculamos la energía mecánica inicial de cada cuerpo
    for (int i = 0; i < CUERPOS; i++)
    {
        energia_total += 0.5 * masas[i] * norma_cuad(velocidades[i]);
            for (int j = i + 1; j < CUERPOS; j++)
            {
                    Vector2D R;
                    R[0] = posiciones[i][0] - posiciones[j][0];
                    R[1] = posiciones[i][1] - posiciones[j][1];
                    energia_total -= masas[i] * masas[j] / sqrt(norma_cuad(R));
            }
    }


    /************************************************************************************************/
    /********************************* APERTURA DEL FICHERO DE ESCRITURA ****************************/
    /************************************************************************************************/
    // Abrimos los ficheros donde queremos escribir los datos de la posición
    FILE *archivos[CUERPOS];
    char filename[20];
    for (int i = 0; i < CUERPOS; i++)
    {
        sprintf(filename, "Datos_cuerpos/cuerpo_%d.txt", i);
        archivos[i] = fopen(filename, "w");
        if (archivos[i] == NULL)
        {
            perror("Error al abrir el archivo");
            return 1;
        }
        
    }
    

    FILE *archivo_energia_total = fopen("Datos_sistema/energia_total.txt", "w");
    if (archivo_energia_total == NULL)
    {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Escribimos las condiciones iniciales en el fichero
    for (int i = 0; i < CUERPOS; i++)
    {
        fprintf(archivos[i], "POSICION_X\tPOSICION_Y\n");
        fprintf(archivos[i], "%f\t%f\n", posiciones[i][0], posiciones[i][1]);
    }

    fprintf(archivo_energia_total, "ENERGIA_TOTAL\n");
    fprintf(archivo_energia_total, "%.17f\n", energia_total);

    /***********************************************************************************************/
    /************* CALCULAMOS Y ESCRIBIMOS LA POSICIÓN, VELOCIDAD Y ACELERACIÓN ********************/
    /************* DE LOS PLANETAS EN TIEMPO t > O *************************************************/
    /***********************************************************************************************/
    
    // Iteramos el código el número de iteraciones elegidas
    for (int n = 0; n < iteraciones; n++)
    {
        // Calculamos la posición del cuerpo i
        for (int i = 0; i < CUERPOS; i++)
        {
            // Iteramos sobre las dos componentes de la posición
            for (int a = 0; a < 2; a++)
            {
                posiciones[i][a] +=  paso * velocidades[i][a] + 0.5 * paso * paso * aceleraciones[i][a];
            }
        }

        // Escribimos la posición de cada cuerpo en su fichero
        for (int i = 0; i < CUERPOS; i++)
        {
            fprintf(archivos[i], "%f\t%f\n", posiciones[i][0], posiciones[i][1]);
        }
        // Guardo los valores previos de las aceleraciones para el cálculo de la velocidad
        Vector2D aceleraciones_previa[CUERPOS];
        for (int a = 0; a < 2; a++)
        {
            for (int i = 0; i < CUERPOS; i++)
            {
                aceleraciones_previa[i][a] = aceleraciones[i][a];
            }
        }

        // Recorremos todos los cuerpos calculando sus aceleraciones y velocidades
        for (int i = 0; i < CUERPOS; i++)
        {
            // Reseteamos el valor de las aceleraciones a 0
            for (int a = 0; a < 2; a++)
            {
                aceleraciones[i][a] = 0.0;
            }
            
            // Calculamos la aceleración que genera j sobre i
            for (int j = 0; j < CUERPOS; j++)
            {
                if (j != i)
                {
                    // Calculamos la distancia entre los cuerpos i y j
                    Vector2D R;
                    for (int a = 0; a < 2; a++)
                    {
                        R[a] = posiciones[i][a] - posiciones[j][a];
                    }

                    // Calculamos la aceleración mediante la ley de gravitación universal
                    for (int a = 0; a < 2; a++)
                    {
                        aceleraciones[i][a] -=  masas[j] * R[a] / pow(norma_cuad(R), 1.5);
                    }
                }
            }
        }

        // Calculamos la velocidad de los cuerpos
        for (int i = 0; i < CUERPOS; i++)
        {
            for (int a = 0; a < 2; a++)
            {
                velocidades[i][a] += 0.5 * paso * (aceleraciones[i][a] + aceleraciones_previa[i][a]);
            }
        }

        // Escribimos en el fichero la energia mecanica de los cuerpos
        energia_total = 0.0;
        for (int i = 0; i < CUERPOS; i++)
        {
            energia_total += 0.5 * masas[i] * norma_cuad(velocidades[i]);
            for (int j = i + 1; j < CUERPOS; j++)
            {
                    Vector2D R;
                    R[0] = posiciones[i][0] - posiciones[j][0];
                    R[1] = posiciones[i][1] - posiciones[j][1];
                    energia_total -= masas[i] * masas[j] / sqrt(norma_cuad(R));
            }
        }
        fprintf(archivo_energia_total, "%.17f\n", energia_total);
    } 

    for (int i = 0; i < CUERPOS; i++)
    {
        // Cerramos los ficheros
        fclose(archivos[i]);
    }
    fclose(archivo_energia_total);

    // Liberamos el espacio de los punteros
    free(posiciones);
    free(velocidades);
    free(aceleraciones);
    double end_time = omp_get_wtime();
    printf("Tiempo de ejecucion: %.6f segundos\n", end_time - start_time);
    return 0;
}
