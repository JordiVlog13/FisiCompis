import numpy as np

# Constantes físicas
G = 6.67430e-11  # Constante de gravitación universal (m^3 kg^-1 s^-2)
AU = 1.496e11    # Unidad astronómica (m)
M_sun = 1.989e30 # Masa del sol (kg)

def leer_datos(archivo):
    """
    Lee los datos de los planetas desde un archivo.
    Formato esperado: nombre, distancia (AU), velocidad orbital (m/s), masa (kg)
    """
    datos = np.loadtxt(archivo, delimiter=' ', usecols=(1, 2, 3))
    distancias = datos[:, 0] * AU
    velocidades = datos[:, 1]
    masas = datos[:, 2]
    return distancias, velocidades, masas

def calcular_aceleracion(posiciones, masas):
    """
    Calcula la aceleración gravitacional sobre cada planeta.
    """
    n = len(posiciones)
    aceleraciones = np.zeros((n, 2))
    for i in range(n):
        for j in range(n):
            if i != j:
                r_vec = posiciones[j] - posiciones[i]
                r_mag = np.linalg.norm(r_vec)
                aceleraciones[i] += G * masas[j] * r_vec / r_mag**3
    return aceleraciones

def verlet(posiciones, velocidades, masas, h, pasos):
    """
    Algoritmo de Verlet para integrar las ecuaciones de movimiento.
    """
    n = len(posiciones)
    trayectorias = np.zeros((pasos, n, 2))
    aceleraciones = calcular_aceleracion(posiciones, masas)

    for t in range(pasos):
        trayectorias[t] = posiciones
        posiciones += velocidades * h + 0.5 * aceleraciones * h**2
        nuevas_aceleraciones = calcular_aceleracion(posiciones, masas)
        velocidades += 0.5 * (aceleraciones + nuevas_aceleraciones) * h
        aceleraciones = nuevas_aceleraciones

    return trayectorias

def guardar_datos(trayectorias, archivo_salida):
    """
    Guarda las posiciones de los planetas en un archivo.
    """
    with open(archivo_salida, 'w') as f:
        for paso in trayectorias:
            for planeta in paso:
                f.write(f"{planeta[0]},{planeta[1]}\n")
            f.write("\n")

def main():
    # Leer datos de entrada
    archivo_entrada = "planetas_entrada.dat"
    distancias, velocidades, masas = leer_datos(archivo_entrada)

    # Inicializar posiciones y velocidades
    posiciones = np.zeros((len(distancias), 2))
    velocidades_vec = np.zeros((len(velocidades), 2))
    for i in range(len(distancias)):
        posiciones[i] = [distancias[i], 0]
        velocidades_vec[i] = [0, velocidades[i]]

    # Parámetros de simulación
    h = 1  # Paso de tiempo (s)
    pasos = 100  # Número de pasos

    # Ejecutar simulación
    trayectorias = verlet(posiciones, velocidades_vec, masas, h, pasos)

    # Guardar resultados
    archivo_salida = r"d:\DOCUMENTOS\GRANADA\4\COMPU\FisiCompis\planetas_datos.dat"
    guardar_datos(trayectorias, archivo_salida)

if __name__ == "__main__":
    main()
