#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <list>
#include <stack>
#include <set>
#include <algorithm>
#include <limits>
#include <climits>

#define INF INT_MAX

using namespace std;

class Graph {
    class Node {
        public:
            int index;
            int minWeight;
            int maxWeight;
            // default constructor
            Node() = default;
            Node(int index, int minWeight, int maxWeight) : index(index) , minWeight(minWeight), maxWeight(maxWeight) {} 
    };

    private:
        int numVertices;
        int Istart;
        int Sstart;
        int fabLab;
        vector<int> parent;
        vector<int> distance;
        vector<bool> visited;
        stack<int> labPath;
        int INodeCounter;
        int Idistance;
        int Sdistance;
        vector<Node*> fanReferences;
        vector<vector<Node*> > nodes;
    public:
        Graph(int rooms, int IStart, int Sstart, int fabLab);
        void addEdge(int src, int dest, int minWeight, int maxWeight, bool isComplex);
        void play();
        void dijkstra(int start, bool isSus, bool isISecondTime);
};

// Initialize graph
Graph::Graph(int rooms, int IStart, int Sstart, int fabLab) {
    numVertices = rooms;
    this->Istart = IStart; 
    this->Sstart = Sstart;
    this->fabLab = fabLab;
    vector<int> parent(numVertices, -1);

    vector<int> distance(numVertices, INF);
    vector<bool> visited(numVertices, false);
    this->parent = parent;
    this->distance = distance;
    this->visited = visited;
    nodes.resize(rooms);
}

// Add edges
void Graph::addEdge(int src, int dest, int minWeight, int maxWeight, bool isComplex = false) {
    Node* node = new Node(dest, minWeight, maxWeight);
    nodes.at(src).push_back(node);
    if(isComplex)
        fanReferences.push_back(node);
}

void Graph::dijkstra(int start, bool isSus, bool isISecondTime = false) {
    /* Per tenere conto dei vertici visitati usiamo un set ordinato
    L'ordinamento è applicato al primo elemento della coppia, per
    cui salviamo il peso nel primo elemento e il numero nel nodo nel 
    secondo elemento */
    set< pair<int, int> > setds;
    // Inseriamo l'origine nel set
    setds.insert(make_pair(0, start));
    // Il nodo di partenza ha distanza 0 da se stesso
    distance.at(start) = 0;
 
    /* Finchè il set non diventa vuoto */
    while (!setds.empty()) {
        // Prendiamo il primo elemento dal set, il quale sarà l'arco
        // con distanza minima, questo grazie all'ordinamento interno del set
        pair<int, int> tmp = *(setds.begin());
        // e successivamento lo eliminiamo
        setds.erase(setds.begin());
        int u = tmp.second;
        // se siamo arrivati al fablab ci fermiamo
        if(u != fabLab) {
            // iniziamo a scorrere tutti i figli di u
            for (auto& node : nodes.at(u)) {
                // ci prendiamo l'indice di quel nodo (figlio di u)
                int v = node->index;
                // se il nodo di partenza è lo studente allora settiamo
                // il il range sinistro al massimo
                if(!isSus) node->minWeight = node->maxWeight;
                // prendiamo il peso del range minore se impostore
                // minore se studente
                int weight = isSus ? node->minWeight : node->maxWeight;
    
                // se la distanza di v salvata precedentemente
                // è maggiore di quella scoperta ora (padre di v + peso del link tra u e v)
                if (distance.at(v) > distance.at(u) + weight) {
                    // se avevamo inserito in precedenza il nodo (ovviamente con peso maggiore)
                    // lo eliminiamo e diciamo NO ora tu passi da qui
                    if (distance.at(v) != INF)
                        setds.erase(setds.find(make_pair(distance.at(v), v)));
    
                    // allora aggiorniamo la distanza
                    distance.at(v) = distance.at(u) + weight;
                    // e il vettore dei padri
                    if(isISecondTime)
                        parent.at(v) = u;
                    // e inseriamo il nodo nel set
                    setds.insert(make_pair(distance.at(v), v));
                }
            }
        }
    }

    if(isSus) {
        Idistance = distance.at(fabLab);
    }
    else 
        Sdistance = distance.at(fabLab);
}

void Graph::play() {
    ofstream output("output.txt");
    dijkstra(Istart, true);
    fill(distance.begin(), distance.end(), INF);
    dijkstra(Sstart, false);
    fill(distance.begin(), distance.end(), INF);
    dijkstra(Istart, true, true);
    
    // prima riga
    if(Idistance < Sdistance) {
        //cout << "1: Vince Sus" << endl;
        output << 1 << endl;
    }
    else if(Idistance == Sdistance) {
        //cout << "0: Pareggio" << endl;
        output << 0 << endl;
    }
    else {
        //cout << "2: Vince Studente" << endl;
        output << 2 << endl;
    }

    //seconda riga
    output << Idistance << " " << Sdistance << endl;
    
    //terza riga
    for(auto node : fanReferences) {
       output << node->minWeight << " ";
    }
    output << endl;

    INodeCounter = 0;
    int iterator = fabLab;
    while (iterator != -1) {
        ++INodeCounter;
        labPath.push(iterator);
        iterator = parent.at(iterator);
    }

    //quarta riga
    output << INodeCounter << endl;

    //quinta riga
    for(int i = 0; i < INodeCounter; i++) {
        int element = labPath.top();
        output << element << " ";
        labPath.pop();
    }
    output.close();
}

int main(int argc, char const *argv[]) {
    int rooms, simpleCorridor, complexCorridor, Istart, Sstart, fabLab;
    ifstream input("input.txt");
    input >> rooms >> simpleCorridor >> complexCorridor >> Istart >> Sstart >> fabLab;

    Graph graph(rooms, Istart, Sstart, fabLab);
    
    int minWeight, maxWeight;
    for(int i = 0; i < simpleCorridor; i++){
        input >> Istart >> Sstart >> minWeight;
        graph.addEdge(Istart, Sstart, minWeight, minWeight);
    }

    for(int i = 0; i < complexCorridor; i++){
        input >> Istart >> Sstart >> minWeight >> maxWeight;
        graph.addEdge(Istart, Sstart, minWeight, maxWeight, true);
    }

    graph.play();
    input.close();
    return 0;
}