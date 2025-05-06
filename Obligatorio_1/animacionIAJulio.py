import numpy as np

def gravitational_force(pos, mass, G, central_mass):
    """
    Calculate the gravitational force on a planet due to a central body.
    """
    r = np.linalg.norm(pos)
    force_magnitude = -G * central_mass * mass / r**3
    return force_magnitude * pos

def verlet_algorithm(initial_positions, initial_velocities, masses, central_mass, G, dt, steps):
    """
    Simulate the motion of planets using Verlet's algorithm.
    
    Parameters:
        initial_positions: Array of initial positions [[x1, y1], [x2, y2], ...]
        initial_velocities: Array of initial velocities [[vx1, vy1], [vx2, vy2], ...]
        masses: Array of masses of the planets [m1, m2, ...]
        central_mass: Mass of the central body (e.g., the Sun)
        G: Gravitational constant
        dt: Time step
        steps: Number of time steps to simulate
    
    Returns:
        positions: Array of arrays with the positions of planets at each time step
    """
    num_planets = len(initial_positions)
    positions = np.zeros((steps, num_planets, 2))
    velocities = np.array(initial_velocities)
    positions[0] = np.array(initial_positions)
    
    # First step using initial velocity
    accelerations = np.array([gravitational_force(pos, masses[i], G, central_mass) for i, pos in enumerate(positions[0])])
    positions[1] = positions[0] + velocities * dt + 0.5 * accelerations * dt**2
    
    for t in range(1, steps - 1):
        new_accelerations = np.array([gravitational_force(pos, masses[i], G, central_mass) for i, pos in enumerate(positions[t])])
        positions[t + 1] = 2 * positions[t] - positions[t - 1] + new_accelerations * dt**2
        velocities = (positions[t + 1] - positions[t - 1]) / (2 * dt)  # Update velocities if needed
    
    return positions

# Example usage
if __name__ == "__main__":
    # Constants
    G = 6.67430e-11  # Gravitational constant
    central_mass = 1.989e30  # Mass of the Sun (kg)
    dt = 60 * 60  # Time step (1 hour)
    steps = 1000  # Number of steps

    # Initial conditions
    initial_positions = [[1.496e11, 0], [2.279e11, 0]]  # Positions of Earth and Mars (m)
    initial_velocities = [[0, 29780], [0, 24070]]  # Velocities of Earth and Mars (m/s)
    masses = [5.972e24, 6.417e23]  # Masses of Earth and Mars (kg)

    # Run simulation
    positions = verlet_algorithm(initial_positions, initial_velocities, masses, central_mass, G, dt, steps)

    # Print the first few positions
    print("First few positions of planets:")
    print(positions[:5])