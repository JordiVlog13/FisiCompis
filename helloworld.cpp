#include <iostream>
#include <vector>
#include <cmath>
#include <fstream> // For file handling
using namespace std;

#define G 6.674e-11 
#define PI 3.14159265358979323846 
#define AU 1.496e11 
#define Msun 1.989e30 

double normalize(const vector<double>& r) // Changed parameter to vector for flexibility
{
	double norm = 0;
	for (size_t i = 0; i < r.size(); i++) // Use size_t for indexing
	{
		norm += r[i] * r[i];
	}
	return sqrt(norm);
}

vector<vector<double>> aceleracion(const vector<vector<double>>& r, const vector<double>& m) // Fixed parameter types
{
	int N = r.size(); // Number of planets
	int dim = r[0].size(); // Number of coordinates (x, y, z)
	vector<vector<double>> a(N, vector<double>(dim, 0.0)); // Initialize acceleration vector

	for (int part = 0; part < N; part++) // For each planet
	{
		for (int i = 0; i < dim; i++) // For each coordinate x, y, z
		{
			double sum = 0;
			for (int j = 0; j < N; j++) // Fixed missing loop condition
			{
				if (j != part) // Avoid self-interaction
				{
					vector<double> diff(dim);
					for (int k = 0; k < dim; k++) // Calculate difference vector
					{
						diff[k] = r[part][k] - r[j][k];
					}
					sum += m[j] * (diff[i] / pow(normalize(diff), 3)); // Sum of forces
				}
			}
			a[part][i] = sum; // Acceleration with gravitational constant
		}
	}
	return a; // Return acceleration of each planet
}

vector<double> EK(const vector<vector<double>>& v)//Para calcular la energía en cada momento
{
	int N = v.size(); // Number of planets
	int dim = v[0].size(); // Number of coordinates (x, y, z)
	vector<double> E_kin(N, 0.0); // Initialize kinetic energy vector

	for (int i = 0; i < N; i++) // For each planet
	{
		for (int j = 0; j < dim; j++) // For each coordinate x, y, z
		{
			E_kin[i] += 0.5 * v[i][j] * v[i][j]; // Kinetic energy calculation
		}
	}
	return E_kin; // Return kinetic energy of each planet

}

vector<double> EP(const vector<vector<double>>& r, const vector<double>& m)//Para calcular la energía potencial en cada momento
{
	int N = r.size(); // Number of planets
	int dim = r[0].size(); // Number of coordinates (x, y, z)
	vector<double> E_pot(N, 0.0); // Initialize potential energy vector

	for (int i = 0; i < N; i++) // For each planet
	{
		for (int j = 0; j < N; j++) // For each planet again for potential energy calculation
		{
			if (i != j) // Avoid self-interaction
			{
				double r_ij = 0;
				for (int k = 0; k < dim; k++) // Calculate distance between planets
				{
					r_ij += pow(r[i][k] - r[j][k], 2);
				}
				E_pot[i] -= G * m[i] * m[j] / sqrt(r_ij); // Potential energy calculation
			}
		}
	}
	return E_pot; // Return potential energy of each planet
}

vector<double> ETotal(const vector<double>& E_kin, const vector<double>& E_pot)//Para calcular la energía total en cada momento
{
	int N = E_kin.size(); // Number of planets
	vector<double> E_total(N, 0.0); // Initialize total energy vector

	for (int i = 0; i < N; i++) // For each planet
	{
		E_total[i] = E_kin[i] + E_pot[i]; // Total energy calculation
	}
	return E_total; // Return total energy of each planet
}

int main(void)
{
	//Lee los datos de posiciones (x,y), velocidades (vx, vy) y masas (m) de 
	//Los planetas, el archivo debe estar organizado de la siguiente manera:
	//x y vx vy m
	ifstream file("dat_planetas.txt");
	if (!file.is_open()) {
		cerr << "Error: Could not open dat_planetas.txt" << endl;
		return 1;
	}

	vector<vector<double>> r0; // To store initial positions
	vector<vector<double>> v0; // To store initial velocities
	vector<double> m; // To store masses

	double x, y, vx, vy, mass;
	while (file >> x >> y >> vx >> vy >> mass) {
		r0.push_back({x, y}); // Store x and y in r0
		v0.push_back({vx, vy}); // Store vx and vy in v0
		m.push_back(mass); // Store mass in m
	}

	file.close();

	// Debug output to verify data
	//cout << "Initial positions (r0):" << endl;
	//for (const auto& pos : r0) {
	//	cout << pos[0] << " " << pos[1] << endl;
	//}
//
	//cout << "Initial velocities (v0):" << endl;
	//for (const auto& vel : v0) {
	//	cout << vel[0] << " " << vel[1] << endl;
	//}
//
	//cout << "Masses (m):" << endl;
	//for (const auto& mass : m) {
	//	cout << mass << endl;
	//}


	// Reescalamiento de unidades
	for (size_t i = 0; i < r0.size(); i++) {
		r0[i][0] /= AU; // Convert x to AU
		r0[i][1] /= AU; // Convert y to AU
		v0[i][0] /= sqrt(G * Msun / AU); // Convert vx to AU/s
		v0[i][1] /= sqrt(G * Msun / AU); // Convert vy to AU/s
		m[i] /= Msun; // Convert mass to solar masses
	}

	// Algoritmo de Verlet
	double dt = 0.01; // Time step 
	double t = 0; // Initial time
	double tmax = 10; // Maximum time
	int N = r0.size(); // Number of planets

	vector<vector<double>> r = r0; // Current positions
	vector<vector<double>> v = v0; // Current velocities
	vector<double> E_kin(N, 0.0); // Kinetic energy
	vector<double> E_pot(N, 0.0); // Potential energy
	vector<double> E_total(N, 0.0); // Total energy
	vector<vector<double>> v_aux = v; // Initialize v_aux 
	ofstream output("planets_data.dat"); // Open output file once
	if (!output.is_open()) {
		cerr << "Error: Could not open planets_data.dat" << endl;
		return 1;
	}

	while (t < tmax) // Time loop
	{
		// Output positions to file
		for (int i = 0; i < N; i++) {
			output << r[i][0]  << " , " << r[i][1]  << endl; // Convert back to meters for output
		}
		output << endl; // Separate time steps with a blank line



		vector<vector<double>> a = aceleracion(r, m); // Calculate acceleration
		
		E_kin = EK(v); // Calculate kinetic energy
		E_pot = EP(r, m); // Calculate potential energy
		E_total = ETotal(E_kin, E_pot); // Calculate total energy
		//cout << "Total energy: " << E_total[0] << endl; // Output total energy
		//cout << "Kinetic energy: " << E_kin[0] << endl; // Output kinetic energy
		//cout << "Potential energy: " << E_pot[0] << endl; // Output potential energy
		//cout << "Time: " << t << endl; // Output time


		for (int i = 0; i < N; i++) // Update positions and velocities
		{
			for (int j = 0; j < 2; j++) // For x and y coordinates
			{
				v[i][j] = v_aux[i][j]; // Update velocity
				v_aux[i][j] += a[i][j] * dt; // Update velocity
				r[i][j] += 0.5 * (v[i][j]+v_aux[i][j]) * dt + 0.5 * a[i][j] * dt * dt; // Update position
				
			}
		}
		t += dt; // Increment time

		
	}

	output.close(); // Close output file
	return 0;
}