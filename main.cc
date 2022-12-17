#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
using namespace std;

// Struttura che rappresenta un corridoio con o senza ventola
struct Corridoio
{
    int destinazione;
    int Tmin;
    int Tmax = -1;               // per capire se è un corridoioio con ventola o no guardo Tmax
    bool ventolaAccesa = false;  // non accendiamo la ventola ad un livello x € (Tmin, Tmax)
    bool passaImpostore = false; // serve per capire se fa parte del cammino dell'impostore
};

// Struttura che rappresenta un nodo del grafo
struct Nodo
{
    vector<Corridoio*> adj = {};
    int distanzaS = -1;
    int distanzaI = -1;
    int predecessoreI = -1;
    int predecessoreS = -1;
};

// Struttura che contiene i dati da scrivere nel file di output
struct Risultato
{
    int vittoriaImpostore;
    int distanzaMinimaImpostore;
    int distanzaMinimaStudenti;
    vector<int> camminoImpostore;
    vector<int> camminoStudenti; // non fa parte dei dati di output ma penso sarà comodo averlo
};

void printNodi(vector<Nodo> nodi);
void leggiNodi(fstream &stream);
Risultato soluzione();
void stampaOutput(fstream &stream, Risultato &risultato);
void inizializzaDistanze(vector<Nodo> &G);
Corridoio* newCorridoio(int destinazione, int Tmin, int Tmax = -1);
void inputGraphPrint();
int dijkstraI();
void setPassaImpostore();
int dijkstraS();
void setNodesMetadataMenoUno();
bool setVentoleSoloStudenti();
Corridoio* getCorridoioPareggiante();

// Variabili globali:
int N, M, K;
int I, S, F;
// corridoi mi serve per tenere traccia dei corridoi in oridne di input
vector<Corridoio *> corridoi;
vector<Nodo> nodi;

int main(int argc, char *argv[])
{
    // checks if i/o files are specified
    string inputFilename;
    string outputFilename;
    if (argc >= 3)
    {
        inputFilename = argv[1];
        outputFilename = argv[2];
    }
    else
    {
        inputFilename = "input.txt";
        outputFilename = "output.txt";
    }

    fstream stream;
    stream.open(inputFilename, ios::in);
    // read N, M, K
    stream >> N >> M >> K;
    // read I, S, F
    stream >> I >> S >> F;

    nodi.resize(N);
    leggiNodi(stream);

    // close stream
    stream.close();

    // prints input graph
    // inputGraphPrint();

    // risoluzione problema

    Risultato risultato = soluzione();

    // output

    stream.open(outputFilename, ios::out);
    stampaOutput(stream, risultato);
    stream.close();

    return 0;
}

// Costruttore `Corridoio` siccome C++ 11 puzza
Corridoio* newCorridoio(int destinazione, int Tmin, int Tmax)
{
    Corridoio* c = new Corridoio;
    c->destinazione = destinazione;
    c->Tmin = Tmin;
    c->Tmax = Tmax;
    c->ventolaAccesa = false;
    c->passaImpostore = false;
    return c;
}

// Funzione che dato un vettore di precedenze restituisce il cammino (migliorabile)
vector<int> calcolaCamminoImpostore(){
     stack<int> pila;
     vector<int> cammino;
     int nodo = F;
     while(nodo != -1){
         pila.push(nodo);
         nodo = nodi[nodo].predecessoreI;
     }
     while(!pila.empty()){
         cammino.push_back(pila.top());
         pila.pop();
     }
     return cammino;
 }

// Dato un vettore, imposta tutti gli elementi a -1
//  void setMenoUno(vector<int> &v){
//      for (size_t i = 0; i < v.size(); i++)
//      {
//          v[i] = -1;
//      }
//  }

// Funzione wrapper per l'algoritmo risolutivo
Risultato soluzione()
{
    // inizializza risultato e precedenti
    Risultato risultato = {-1, -1, -1, {}, {}};

    risultato.distanzaMinimaImpostore = dijkstraI();
    // cout << "DEBUG - Dijkstra impostore: " << risultato.distanzaMinimaImpostore << endl;
    setPassaImpostore();

    risultato.distanzaMinimaStudenti = dijkstraS();
    // cout << "DEBUG - Dijkstra studenti: " << risultato.distanzaMinimaStudenti << endl;
    while (risultato.distanzaMinimaStudenti <= risultato.distanzaMinimaImpostore)
    {
        bool cambiate = setVentoleSoloStudenti();
        if (!cambiate && risultato.distanzaMinimaStudenti == risultato.distanzaMinimaImpostore)
        {
            if (nodi[F].predecessoreI != nodi[F].predecessoreS)
            {
                //pareggio unica via
                risultato.vittoriaImpostore = 0;
                break;
            }
            else{
                Corridoio* toCheck = getCorridoioPareggiante();
                if(toCheck == NULL){
                    //pareggio unica via
                    risultato.vittoriaImpostore = 0;
                    break;
                }
                else{
                    toCheck->ventolaAccesa = true;
                    setNodesMetadataMenoUno();
                    risultato.distanzaMinimaImpostore = dijkstraI();
                    setPassaImpostore();
                    risultato.distanzaMinimaStudenti = dijkstraS();
                    if (risultato.distanzaMinimaStudenti > risultato.distanzaMinimaImpostore)
                    {
                        risultato.vittoriaImpostore = 1;
                        break;
                    }
                    else if (risultato.distanzaMinimaStudenti < risultato.distanzaMinimaImpostore)
                    {
                        toCheck->ventolaAccesa = false;
                        setNodesMetadataMenoUno();
                        risultato.distanzaMinimaImpostore = dijkstraI();
                        setPassaImpostore();
                        risultato.distanzaMinimaStudenti = dijkstraS();
                        break;
                    }
                }
            }
        }
        else if(!cambiate){
            break;
        }  
        setNodesMetadataMenoUno();
        risultato.distanzaMinimaImpostore = dijkstraI();
        setPassaImpostore();
        risultato.distanzaMinimaStudenti = dijkstraS();
        // cout << "studenti = " << risultato.distanzaMinimaStudenti << endl;
        // cout << "impostore = " << risultato.distanzaMinimaImpostore << endl;
    }
    if (risultato.distanzaMinimaStudenti > risultato.distanzaMinimaImpostore)
    {
        risultato.vittoriaImpostore = 1;
    }
    else if (risultato.distanzaMinimaImpostore == risultato.distanzaMinimaStudenti)
    {
        risultato.vittoriaImpostore = 0;
    }
    else
    {
        risultato.vittoriaImpostore = 2;
    }
    

    risultato.camminoImpostore = calcolaCamminoImpostore();


    // setNodesMetadataMenoUno();
    // risultato.distanzaMinimaImpostore = dijkstraI();
    // setPassaImpostore();
    // risultato.distanzaMinimaStudenti = dijkstraS();

    // while(risultato.distanzaMinimaStudenti < 0){
    //     switch (risultato.distanzaMinimaStudenti)
    //     {
    //     case -1:
    //         setNodesMetadataMenoUno();
    //         risultato.distanzaMinimaImpostore = dijkstraI();
    //         setPassaImpostore();
    //         risultato.distanzaMinimaStudenti = dijkstraS();
    //         break;

    //     default:
    //         break;
    //     }
    // }

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

    cout << risultato.vittoriaImpostore << endl;
    cout << risultato.distanzaMinimaImpostore << endl;
    cout << risultato.distanzaMinimaStudenti << endl;


    return risultato;
}


Corridoio* getCorridoioPareggiante(){
    int current = F;
    int predecessore = nodi[current].predecessoreI;
    while(predecessore != -1){
        for (int i = 0; i < nodi[predecessore].adj.size(); i++)
        {
            Corridoio* tmp = nodi[predecessore].adj[i];
            if(tmp->destinazione == current){
                if(tmp->Tmax != -1 && !tmp->ventolaAccesa){
                    return nodi[predecessore].adj[i];
                }
                else{
                    break;
                }
            }
        }
        current = predecessore;
        if(nodi[current].predecessoreI != nodi[current].predecessoreS){
            return NULL;
        }
        predecessore = nodi[current].predecessoreI;
    }
    return NULL;
}


void setNodesMetadataMenoUno()
{
    for (int i = 0; i < nodi.size(); i++)
    {
        nodi[i].distanzaI = -1;
        nodi[i].distanzaS = -1;
        nodi[i].predecessoreI = -1;
        nodi[i].predecessoreS = -1;
    }
}

int dijkstraS()
{
    queue<int> coda;
    nodi[S].distanzaS = 0;
    int current;
    coda.push(S);
    while (!coda.empty())
    {
        current = coda.front();
        coda.pop();
        for (int i = 0; i < nodi[current].adj.size(); i++)
        {
            Corridoio* tmp = nodi[current].adj[i];
            int destinazione = tmp->destinazione;
            // se non passa l'impostore ed è una ventola, la sparo al massimo
            //  if(!tmp.passaImpostore && tmp.Tmax != -1 && (nodi[current].distanzaS < nodi[current].distanzaI)){
            //      tmp.ventolaAccesa = true;
            // }
            // //altrimenti se passa l'impostore ed è una ventola:
            // else if(tmp.passaImpostore && tmp.Tmax != -1 && !tmp.ventolaAccesa){
            //     //se vince lo studente, la alzo al massimo e rifaccio dijkstraI
            //     if (nodi[current].distanzaS < nodi[current].distanzaI){
            //         tmp.ventolaAccesa = true;
            //         //Ritorna -1 se vince lo studente
            //         cout << "current : " << current << " destinazione : " << destinazione << endl;
            //         cout << "DEBUG - Rifare DijkstraI - vinceva lo studente" << endl;
            //         return -1;
            //     }
            //     //se pareggiano devo vedere
            //     else if (nodi[current].distanzaS == nodi[current].distanzaI){
            //         //CAZZI AMARI
            //     }
            // }
            int costoCorridoio = tmp->ventolaAccesa ? tmp->Tmax : tmp->Tmin;
            if (nodi[destinazione].distanzaS == -1 || nodi[destinazione].distanzaS > nodi[current].distanzaS + costoCorridoio)
            {
                nodi[destinazione].distanzaS = nodi[current].distanzaS + costoCorridoio;
                coda.push(destinazione);
                nodi[destinazione].predecessoreS = current;
            }
        }
    }
    return nodi[F].distanzaS;
}

int dijkstraI()
{
    // TODO implement with priority queue
    queue<int> coda;
    nodi[I].distanzaI = 0;
    int current;
    coda.push(I);
    while (!coda.empty())
    {
        current = coda.front();
        coda.pop();
        for (int i = 0; i < nodi[current].adj.size(); i++)
        {
            Corridoio* tmp = nodi[current].adj[i];
            int destinazione = tmp->destinazione;
            // se la ventola è accesa, il costo è Tmax, altrimenti Tmin   ---- !!!controllare se è corretto!!!
            int costoCorridoio = tmp->ventolaAccesa ? tmp->Tmax : tmp->Tmin;
            if (nodi[destinazione].distanzaI == -1 || nodi[destinazione].distanzaI > (nodi[current].distanzaI + costoCorridoio))
            {
                // setto la distanza
                nodi[destinazione].distanzaI = nodi[current].distanzaI + costoCorridoio;
                // aggiungo alla coda
                coda.push(destinazione);
                // setto il predecessore
                nodi[destinazione].predecessoreI = current;
            }
        }
    }
    return nodi[F].distanzaI;
}

void setPassaImpostore()
{
    for (int i = 0; i < corridoi.size(); i++)
    {
        Corridoio *tmp = corridoi[i];
        tmp->passaImpostore = false;
    }
    int current = F;
    int predecessore = nodi[current].predecessoreI;
    while (predecessore != -1)
    {
        for (int i = 0; i < nodi[predecessore].adj.size(); i++)
        {
            if (nodi[predecessore].adj[i]->destinazione == current)
            {
                nodi[predecessore].adj[i]->passaImpostore = true;
                break;
            }
        }
        current = predecessore;
        predecessore = nodi[current].predecessoreI;
    }
}

bool setVentoleSoloStudenti()
{
    bool cambiate = false;
    int current = F;
    int predecessore = nodi[current].predecessoreS;
    while (predecessore != -1)
    {
        for (int i = 0; i < nodi[predecessore].adj.size(); i++)
        {
            Corridoio* tmp = nodi[predecessore].adj[i];
            // se è un corridoio attraversato da studenti, ha una ventola e non è accesa
            if (tmp->destinazione == current && tmp->Tmax != -1 && !tmp->ventolaAccesa)
            {
                if (!tmp->passaImpostore)
                {
                    nodi[predecessore].adj[i]->ventolaAccesa = true;
                    cambiate = true;
                }
                else
                {
                    if (nodi[predecessore].distanzaS < nodi[predecessore].distanzaI)
                    {
                        nodi[predecessore].adj[i]->ventolaAccesa = true;
                        cambiate = true;
                    }
                }
            }
        }
        current = predecessore;
        predecessore = nodi[current].predecessoreI;
    }
    return cambiate;
}

// Debug - stampa il grafo e le variabili ricevute in input
void inputGraphPrint()
{
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

// Dato lo stream, scrive nel file di output il contenuto di `risultato` come richiesto
void stampaOutput(fstream &stream, Risultato &risultato)
{
    stream << risultato.vittoriaImpostore << endl;
    stream << risultato.distanzaMinimaImpostore << " " << risultato.distanzaMinimaStudenti << endl;
    for (size_t i = M; i < corridoi.size(); i++)
    {
        if (corridoi[i]->ventolaAccesa)
        {
            stream << corridoi[i]->Tmax << " ";
        }
        else
        {
            stream << corridoi[i]->Tmin << " ";
        }
    }
    stream << endl;
    stream << risultato.camminoImpostore.size() << endl;
    for (size_t i = 0; i < risultato.camminoImpostore.size(); i++)
    {
        stream << risultato.camminoImpostore[i] << " ";
    }
}

// Dato lo stream, legge il file di input e crea il grafo nel vettore di nodi
void leggiNodi(fstream &stream)
{
    // read corridoi semplici
    for (size_t i = 0; i < M; i++)
    {
        int U, V, T;
        stream >> U >> V >> T;
        nodi[U].adj.push_back(newCorridoio(V, T));
        corridoi.push_back(nodi[U].adj[nodi[U].adj.size() - 1]);
    }

    // read remaining corridoi con ventola
    for (size_t i = 0; i < K; i++)
    {
        int U, V, Tmin, Tmax;
        stream >> U >> V >> Tmin >> Tmax;
        nodi[U].adj.push_back(newCorridoio(V, Tmin, Tmax));
        corridoi.push_back(nodi[U].adj[nodi[U].adj.size() - 1]);
    }
}

// Debug - stampa il grafo
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
                if (nodi[i].adj[j]->Tmax != -1)
                    cout << "\t" << nodi[i].adj[j]->destinazione << " con costo [" << nodi[i].adj[j]->Tmin << ", " << nodi[i].adj[j]->Tmax << "];" << endl;
                else
                    cout << "\t" << nodi[i].adj[j]->destinazione << " con costo " << nodi[i].adj[j]->Tmin << ";" << endl;
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
