/*
* @utor Lagunez Nestor
Programa encargado de obtener Pi por el metodo de Leibniz empleando metodos std::async
*/


#include <iostream>
#include <iomanip>
#include <thread>
#include <future>
#include <vector>
#include <cmath>

double calculate_pi_leibniz(int start, int end)
{
    double sum = 0.0;
    for (int i = start; i < end; ++i)
    {
        int sign = pow(-1, i);
        double term = 1.0 / (i * 2 + 1);
        sum += sign * term;
    }

    return sum;
}

int main()
{
    int num_threads = std::thread::hardware_concurrency(); // Obtén el número de hilos soportado
    int num_terms = 1E8; // Ajusta la cantidad de términos según la precisión deseada
    int terms_per_thread = num_terms / num_threads; // Dividir términos entre hilos
    std::vector<std::future<double>> futures;
    int start;
    int end;
    for (int i = 0; i < num_threads; ++i)
    {

        start = i * terms_per_thread;
        end = (i == num_threads - 1) ? num_terms : (i + 1) * terms_per_thread;
        //std::async: std::async es una función en C++ que inicia una tarea asincrónica y devuelve un objeto std::future 
        // que eventualmente contendrá el resultado de la tarea. 
        //std::launch::async: Esto indica que deseas que la función se ejecute en un hilo separado de manera asíncrona. 
        //Esto puede permitir una ejecución concurrente de varias tareas.
        futures.emplace_back(std::async(std::launch::async, calculate_pi_leibniz, start, end));//agrega un nuevo elemento (en este caso, un std::future) 
                                                        //al final del vector futures. El vector futures se utiliza para almacenar los resultados 
                                                        //futuros de las tareas asíncronas que se están ejecutando en paralelo.
    }

    double total_sum = 0.0;
    for (int i = 0; i < num_threads; ++i)
    {
        total_sum += futures[i].get();
    }

    double pi_approx = total_sum * 4.0;

    std::cout << "Approximated pi using Leibniz series: " << std::setprecision(15) << pi_approx << std::endl;

    return 0;
}
