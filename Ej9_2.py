import numpy as np
import sympy as sp
import matplotlib.pyplot as plt
import scipy.constants as const
#hola que tal
#Definimos constantes
q = const.e
hbar = const.hbar/q
kT = const.k * 298/q

#Parámetros del problema
eta = 0.5
E_F = -5.0
HOMO = -5.5
Gamma_S = Gamma_D =0.1
tau_S = hbar / Gamma_S
tau_D = hbar / Gamma_D
N0 = 2 #Consideramos en el eq el nivel está lleno
g = 2 #Tomamos la densidad de estados como valor discreto al considerar la delta 

def fermi(E, mu): #Distribución de Fermi
    return 1 / (1 + np.exp((E - mu)/ kT))

def calculate_N(U, mu_S, mu_D): #Calculamos el número de electrones
    E = HOMO + U
    f_S = fermi(E, mu_S)
    f_D = fermi(E, mu_D)
    return g * (tau_D * f_S + tau_S * f_D  ) / (tau_S + tau_D)


def calculate_U(V, q2_Ces): #Calculamos el potencial U de forma iterativa para cada caso 
    alpha = 0.1
    tol = 1e-6 #tolerancia
    
    mu_S = E_F + eta * V
    mu_D = E_F - (1 - eta) * V

    if q2_Ces == 0:
        return 0, calculate_N(0, mu_S, mu_D)
    
    U_old = 0 #U guess
    while True:      #Bucle que continúa hasta obtener una diferencia de valores menor que tol
        N = calculate_N(U_old, mu_S, mu_D)
        U_calc = q2_Ces * (N - N0)
        U_new = U_old + alpha * (U_calc - U_old)
        
        if abs(U_new - U_old) < tol:
            break
        
        U_old = U_new

    return U_new,N


def calculate_I(V,U): #Calculamos la corriente
    mu_S = E_F + eta * V
    mu_D = E_F - (1 - eta) * V
    E = HOMO + U
    f_S = fermi(E, mu_S)
    f_D = fermi(E, mu_D)
    return q * g * (1/(tau_S + tau_D)) * (f_S - f_D)

#Definimos vectores de las magnitudes que nos interesan
V = np.linspace(-2,2,1000)      #Damos valores al potencial aplicado
I_0 = np.zeros_like(V)          # Intensidad caso q²/C_ES = 0
I_1 = np.zeros_like(V)          # Intensidad caso q²/C_ES = 1
G_0 = np.zeros_like(V)          #Conductancia caso q²/C_ES = 0
G_1 = np.zeros_like(V)          #Intensidad caso q²/C_ES = 1
U_v = np.zeros_like(V)          #Valores de U

#Bucle por cada valor del potencial aplicado calcula U e I
for i, V0 in enumerate(V):
    U, _ = calculate_U(V0, q2_Ces=0)
    I_0[i] = calculate_I(V0,U)

    U, _ = calculate_U(V0, q2_Ces=1.0)
    U_v[i] = U
    I_1[i] = calculate_I(V0,U)

G_0 = np.gradient(I_0,V)    #Calculamos la conductancia dI/dV
G_1 = np.gradient(I_1,V)

# Graficamos
plt.figure(figsize=(12, 6))

# Current-Voltage
plt.subplot(1, 2, 1)
plt.plot(V, I_0 * 1e6, 'b-', label='q²/C_ES = 0')
plt.plot(V, I_1 * 1e6, 'r--', label='q²/C_ES = 1 eV')
plt.xlabel('Voltage (V)')
plt.ylabel('Current (μA)')
plt.title('I-V')
plt.legend()
plt.grid(True)

# Conductance-Voltage
plt.subplot(1, 2, 2)
plt.plot(V, G_0 * 1e6, 'b-')
plt.plot(V, G_1 * 1e6, 'r--')
plt.xlabel('Voltage (V)')
plt.ylabel('Conductance (μS)')
plt.title('G-V')
plt.grid(True)

plt.tight_layout()
plt.show()

