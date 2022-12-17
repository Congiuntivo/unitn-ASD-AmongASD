#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<stack>
using namespace std;

//Struttura che rappresenta un corridoio con o senza ventola
struct Corridoio{
    int destinazione;
    int Tmin;
    int Tmax = -1;                      //per capire se è un corridoioio con ventola o no guardo Tmax
    bool ventolaAccesa = false;         //non accendiamo la ventola ad un livello x € (Tmin, Tmax)
    bool passaImpostore = false;        //serve per capire se fa parte del cammino dell'impostore
};


//Struttura che rappresenta un nodo del grafo
struct Nodo{
    vector<Corridoio> adj = {};
    int distanzaS = -1;
    int distanzaI = -1;
    int predecessoreI = -1;
    int predecessoreS = -1;
};

//Struttura che contiene i dati da scrivere nel file di output
struct Risultato
{
    int vittoriaImpostore;
    int distanzaMinimaImpostore;
    int distanzaMinimaStudenti;
    vector<int> camminoImpostore;
    vector<int> camminoStudenti;    //non fa parte dei dati di output ma penso sarà comodo averlo
};


void printNodi(vector<Nodo> nodi);
void leggiNodi(fstream &stream);
Risultato soluzione();
void stampaOutput(fstream &stream, Risultato &risultato);
int bfs(vector<Nodo> &G, int n, int f, vector<int> &precedenti);
void inizializzaDistanze(vector<Nodo> &G);
Corridoio newCorridoio(int destinazione, int Tmin, int Tmax = -1);
void inputGraphPrint();
int dijkstraI();


//Variabili globali:
int N, M, K;
int I, S, F;
//corridoi mi serve per tenere traccia dei corridoi in oridne di input
vector<Corridoio*> corridoi;
vector<Nodo> nodi;


int main(int argc, char *argv[])
{    
    //checks if i/o files are specified
    string inputFilename;
    string outputFilename;
    if(argc >= 3){
        inputFilename = argv[1];
        outputFilename = argv[2];
    }else{
        inputFilename = "input.txt";
        outputFilename = "output.txt";
    }

    fstream stream;
    stream.open(inputFilename, ios::in);
    //read N, M, K
    stream >> N >> M >> K;
    //read I, S, F
    stream >> I >> S >> F;

    corridoi.resize(M+K);
    nodi.resize(N);
    leggiNodi(stream);
    
    
    //close stream
    stream.close();
    

    //prints input graph
    inputGraphPrint();


    //risoluzione problema

    Risultato risultato = soluzione();

    //output

    stream.open(outputFilename, ios::out);
    // stampaOutput(stream, risultato);
    stream.close();


    return 0;
}

//Costruttore `Corridoio` siccome C++ 11 puzza
Corridoio newCorridoio(int destinazione, int Tmin, int Tmax){
    Corridoio c;
    c.destinazione = destinazione;
    c.Tmin = Tmin;
    c.Tmax = Tmax;
    c.ventolaAccesa = false;
    c.passaImpostore = false;
    return c;
}

//Funzione che dato un vettore di precedenze restituisce il cammino (migliorabile)
vector<int> calcolaCammino(vector<int> &precedenti, int f){
    stack<int> pila;
    vector<int> cammino;
    int nodo = f;
    while(nodo != -1){
        pila.push(nodo);
        nodo = precedenti[nodo];
    }
    while(!pila.empty()){
        cammino.push_back(pila.top());
        pila.pop();
    }
    return cammino;
}

//Dato un vettore, imposta tutti gli elementi a -1
void setMenoUno(vector<int> &v){
    for (size_t i = 0; i < v.size(); i++)
    {
        v[i] = -1;
    }
}


//Funzione wrapper per l'algoritmo risolutivo
Risultato soluzione(){
    //inizializza risultato e precedenti
    Risultato risultato = {-1, -1, -1, {}, {}};

    risultato.distanzaMinimaImpostore = dijkstraI();
    cout << "DEBUG - Dijkstra impostore: " << risultato.distanzaMinimaImpostore << endl;


    // risultato.camminoImpostore = calcolaCammino(precedenti, F);
    
    // //re-inizializza precedenti e distanze
    // setMenoUno(precedenti);
    // inizializzaDistanze(nodi);
    // risultato.distanzaMinimaStudenti = bfs(nodi, S, F, precedenti);
    // risultato.camminoStudenti = calcolaCammino(precedenti, F);


    // //PAREGGIO
    // if(risultato.distanzaMinimaImpostore == risultato.distanzaMinimaStudenti){
    //     risultato.vittoriaImpostore = 0;
    // }
    // //VITTORIA IMPOSTORE
    // else if(risultato.distanzaMinimaImpostore < risultato.distanzaMinimaStudenti){
    //     risultato.vittoriaImpostore = 1;
    // }
    // //VITTORIA STUDENTI
    // else{
    //     risultato.vittoriaImpostore = 2;
    // }

    // return risultato;
}


int dijkstraI(){
    //TODO implement with priority queue
    queue<int> coda;
    nodi[I].distanzaI = 0;
    int current;
    coda.push(I);
    while(!coda.empty()){
        current = coda.front();
        coda.pop();
        for(int i=0; i<nodi[current].adj.size(); i++){
            Corridoio tmp = nodi[current].adj[i];
            int destinazione = tmp.destinazione;
            //se la ventola è accesa, il costo è Tmax, altrimenti Tmin
            int costoCorridoio = tmp.ventolaAccesa ? tmp.Tmax : tmp.Tmin;
            if(nodi[destinazione].distanzaI == -1 || nodi[destinazione].distanzaI > (nodi[current].distanzaI + costoCorridoio)){
                //setto la distanza
                nodi[destinazione].distanzaI = nodi[current].distanzaI + costoCorridoio;
                //aggiungo alla coda
                coda.push(destinazione);
                //setto il predecessore
                nodi[destinazione].predecessoreI = current;
            }
        }
    }
    return nodi[F].distanzaI;
}


//Debug - stampa il grafo e le variabili ricevute in input
void inputGraphPrint(){
    cout << N << " aule;" << endl;
    cout << M << " corridoi semplici;" << endl;
    cout << K << " corridoi con ventola;" << endl;
    cout << endl;
    cout << I << " aula impostore;" << endl;
    cout << S << " aula studenti;" << endl;
    cout << F << " FabLab;" << endl;
    cout << endl;
    printNodi(nodi); 
}

//Dato lo stream, scrive nel file di output il contenuto di `risultato` come richiesto
void stampaOutput(fstream &stream, Risultato &risultato){
    stream << risultato.vittoriaImpostore << endl;
    stream << risultato.distanzaMinimaImpostore << " " << risultato.distanzaMinimaStudenti << endl;
    for (size_t i = M; i < corridoi.size(); i++) {
        if (corridoi[i]->ventolaAccesa) {
            stream << corridoi[i]->Tmax << " ";
        }
        else {
            stream << corridoi[i]->Tmin << " ";
        }
    }
    stream << endl;
    stream << risultato.camminoImpostore.size() << endl;
    for (size_t i = 0; i < risultato.camminoImpostore.size(); i++) {
        stream << risultato.camminoImpostore[i] << " ";
    }
}

//Dato lo stream, legge il file di input e crea il grafo nel vettore di nodi
void leggiNodi(fstream &stream){
    //read corridoi semplici
    for (size_t i = 0; i < M; i++)
    {
        int U, V, T;
        stream >> U >> V >> T;
        nodi[U].adj.push_back(newCorridoio(V, T));
        corridoi.push_back(&nodi[U].adj[nodi[U].adj.size()-1]);
    }

    //read remaining corridoi con ventola
    for (size_t i = 0; i < K; i++)
    {
        int U, V, Tmin, Tmax;
        stream >> U >> V >> Tmin >> Tmax;
        nodi[U].adj.push_back(newCorridoio(V, Tmin, Tmax));
        corridoi.push_back(&nodi[U].adj[nodi[U].adj.size()-1]);
    }
}

//Debug - stampa il grafo
void printNodi(vector<Nodo> nodi)
{
    for (size_t i = 0; i < nodi.size(); i++)
    {
        cout << "Nodo " << i << " connesso a:" << endl;
        if (nodi[i].adj.size() == 0)
            cout << "\tnessun nodo" << endl;
        else
        for (size_t j = 0; j < nodi[i].adj.size(); j++)
        {
            if(nodi[i].adj[j].Tmax != -1)
                cout << "\t" << nodi[i].adj[j].destinazione << " con costo [" << 
                nodi[i].adj[j].Tmin << ", " << nodi[i].adj[j].Tmax << "];" << endl;
            else
                cout << "\t" << nodi[i].adj[j].destinazione << " con costo " << 
                nodi[i].adj[j].Tmin << ";" << endl;
        }
        cout << endl;
    }
}


// int bfs(vector<Nodo> &G, int n, int f, vector<int> &precedenti){
//     //TODO change to priority queue
//     queue<int> Q;
//     G[n].distanza = 0;
//     int u;
//     Q.push(n);
//     while(!Q.empty()){
//         u = Q.front();
//         Q.pop();
//         //per tutti gli adiacenti di `u` setto la distanza
//         for(int i=0; i<G[u].adj.size(); i++){
//             Corridoio tmp = G[u].adj[i];
//             int nodo = tmp.destinazione;
//             //se la distanza non è stata ancora calcolata oppure è maggiore di quella calcolata in questo percorso
//             if(G[nodo].distanza == -1 || G[nodo].distanza > (G[u].distanza + tmp.Tmin)){
//                 //setto la nuova distanza
//                 G[nodo].distanza = G[u].distanza + tmp.Tmin;
//                 //metto il nodo nella coda
//                 Q.push(nodo);
//                 //setto il predecessore
//                 precedenti[nodo] = u;
//             }
//         }
//     }
//     return G[f].distanza;
// }


// void inizializzaDistanze(vector<Nodo> &G){
//     for (int i = 0; i < G.size(); i++){
//         G[i].distanza = -1;
//     }
// }
