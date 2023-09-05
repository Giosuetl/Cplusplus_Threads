
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>


/*

Este código ilustra la comunicación entre hilos utilizando una cola de mensajes y variables de condición para 
garantizar que los hilos se sincronicen correctamente al enviar y recibir mensajes.
*/

/*
Se define una estructura Message que contiene un enumerador saludo con dos valores : Hola y Adios.Esta estructura
representa los mensajes que se enviarán entre los hilos.También incluye un campo numHilo para identificar el número
de hilo que envía el mensaje.
*/
struct Message {
    enum class saludo {
        Hola ,
        Adios,

    };
    saludo id;
    int numHilo;
};
/*
Se declara una variable global std::condition_variable convar que se utilizará para sincronizar la comunicación entre
los hilos.También se declara un mutex m para proteger las secciones críticas al acceder a la cola de mensajes msg_queue.
*/
std::condition_variable convar;//variable de condcion global
std::mutex m;//mutex, que va a proteger las secciones criticas

//Se crea una cola msg_queue que contendrá los mensajes que se enviarán entre los hilos.
std::queue<Message> msg_queue;//datos que seran modificados

/*
La función GetMessage se encarga de obtener un mensaje de la cola de mensajes.Para hacerlo, adquiere un std::unique_lock
del mutex m y luego espera en la condición convar hasta que la cola de mensajes no esté vacía.Una vez que se cumple esa
condición, obtiene el primer mensaje de la cola, lo elimina de la cola y lo devuelve.
*/
Message GetMessage() {//esperara a que se modifiquen los datos
    Message message;
    std::unique_lock<std::mutex> lock(m);//acquire lock

    convar.wait(lock, []() {return not msg_queue.empty(); });//pasamos el bloqueo unico en su argumento tambien podria ser: convar.wait(lock);, esyo hara que el hilo se duerma y desbloqueara el mutex y luego se dormira
    message = msg_queue.front();
    msg_queue.pop();

    return message;
}

/*
La función PutMessage toma un mensaje como argumento y bloquea el mutex m utilizando std::lock_guard para proteger el acceso
concurrente a la cola de mensajes. Luego, agrega el mensaje a la cola y notifica a la condición convar que se ha modificado
la cola de mensajes.

*/
void PutMessage(const Message& message) {
    std::lock_guard<std::mutex> lk(m);//bloquea el mutex ponemos la proteccion de la cerradura en su propio alcance para que la 
                                      //cerradura se libere
    msg_queue.push(message);//la funcion miembro push va a bloquear el mutex llamara a la funcion insercion del estandar miembro de la cola
    convar.notify_one();//notificmos la condicion variable, eso enviara una señal variable de condicion diciendo que los datos han sido modificados
}

/*
Las funciones hilo1 y hilo2 son las funciones que ejecutarán los hilos. hilo1 es un bucle infinito que obtiene mensajes de
la cola y los imprime en función de si son saludos de "Hola" o "Adios". hilo2 permite al usuario elegir entre enviar un
mensaje de "Hola" o "Adios" al otro hilo y lo coloca en la cola usando PutMessage.
*/
void hilo2(int numHilo)
{

    std::cout << "decir hola:1\ndecir adios:0\n";
    int op;
    std::cin >> op;
    if(op)
        PutMessage(Message{ Message::saludo::Hola,numHilo });
    else
        PutMessage(Message{ Message::saludo::Adios,numHilo });

    return;
}

void hilo1(int numHilo)
{

    bool bandera = 1;
    while (bandera) {
        const Message msg = GetMessage();
        switch (msg.id) {
        case Message::saludo::Hola:
            std::cout << "recivido en hilo: "<< numHilo << ", el hilo " << msg.numHilo << " dice hola\n";
            bandera = 0;
            break;
        case Message::saludo::Adios:
            std::cout << "recivido en hilo:" << numHilo << ", el hilo " << msg.numHilo << " dice adios\n";
            bandera = 0;
            break;

        default:
            break;
        }
    }
    return;
}


/*
En la función main, se crean dos hilos, h1 y h2, que ejecutan las funciones hilo1 y hilo2, respectivamente.
Los hilos se unen a través de h1.join() y h2.join(), lo que garantiza que el programa principal espere a que ambos hilos
finalicen antes de salir.

*/
int main() {
    std::thread h1(hilo1,1);
    std::thread h2(hilo2,2);

    h1.join();
    h2.join();

    return 0;
}
