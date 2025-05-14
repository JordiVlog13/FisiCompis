# include <iostream>

class Schrodinger
{
private:
    int N;
public:
    Schrodinger(int N_) : N(N_) {};
    ~Schrodinger();
};


class Simulacion
{
private:
    Schrodinger FuncionDeOnda;
    int n_ciclos;
    double s;
    double h;
    double lambda;
    double FuncionInicial;
public:
    Simulacion(int n_ciclos_, double lambda_, int N_) : FuncionDeOnda(N_) {};
    ~Simulacion() {};
};



int main()
{
    return 0;
}