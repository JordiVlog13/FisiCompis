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
	cout << "Initial positions (r0):" << endl;
	for (const auto& pos : r0) {
		cout << pos[0] << " " << pos[1] << endl;
	}

	cout << "Initial velocities (v0):" << endl;
	for (const auto& vel : v0) {
		cout << vel[0] << " " << vel[1] << endl;
	}

	cout << "Masses (m):" << endl;
	for (const auto& mass : m) {
		cout << mass << endl;
	}


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
	vector<vector<double>> v_mas = v0; // Previous velocities
	ofstream output("output.txt"); // Open output file once
	if (!output.is_open()) {
		cerr << "Error: Could not open output.txt" << endl;
		return 1;
	}

	while (t < tmax) // Time loop
	{
		vector<vector<double>> a = aceleracion(r, m); // Calculate acceleration
		vector<vector<double>> v_mas(N, vector<double>(2, 0.0)); // Initialize v_mas with the correct size

		for (int i = 0; i < N; i++) // Update positions and velocities
		{
			for (int j = 0; j < 2; j++) // For x and y coordinates
			{
				v[i][j] = v_mas[i][j]; // Update velocity
				v_mas[i][j] += a[i][j] * dt; // Update velocity
				r[i][j] += 0.5 * (v[i][j]+v_mas[i][j]) * dt + 0.5 * a[i][j] * dt * dt; // Update position
				
			}
		}
		t += dt; // Increment time

		// Output positions to file
					for (int i = 0; i < N; i++) {
				output << r[i][0] * AU << " " << r[i][1] * AU << " "; // Convert back to meters for output
			}
			output << endl;
	}

	output.close(); // Close output file
	return 0;
}