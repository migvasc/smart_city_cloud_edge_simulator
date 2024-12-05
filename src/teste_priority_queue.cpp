#include <iostream>
#include <queue>
#include <vector>
#include <functional> // Para std::greater

int main() {
    // Definição da fila de prioridade (Min-Heap para menores valores no topo)
    std::priority_queue<double, std::vector<double>, std::greater<>> time_queue;

    // Inserindo valores (tempos)
    time_queue.push(12.0);
    time_queue.push(14.0);
    time_queue.push(15.0);
    time_queue.push(13.0);
    
    

    // Consultar o menor valor (topo) sem removê-lo
    std::cout << "Menor tempo na fila: " << time_queue.top() << std::endl;

    // Remover o menor valor
    time_queue.pop();

    // Consultar o novo menor valor após a remoção
    std::cout << "Novo menor tempo na fila: " << time_queue.top() << std::endl;

    return 0;
}