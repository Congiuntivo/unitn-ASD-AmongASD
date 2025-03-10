#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <utility>
using namespace std;

// Struttura che rappresenta un corridoio con o senza ventola
struct Corridoio
{
    int destinazione;
    int Tmin;
    int Tmax = -1;               // per capire se è un corridoioio con ventola o no guardo Tmax
    bool ventolaAccesa = false;  // non accendiamo la ventola ad un livello x € (Tmin, Tmax)
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
Corridoio* newCorridoio(int destinazione, int Tmin, int Tmax = -1);
void inputGraphPrint();
int dijkstraI();
int dijkstraS();
void setNodesMetadataMenoUno();
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

// Funzione che dato un vettore di precedenze restituisce il cammino (migliorabile)
vector<int> calcolaCamminoStudente(){
     stack<int> pila;
     vector<int> cammino;
     int nodo = F;
     while(nodo != -1){
         pila.push(nodo);
         nodo = nodi[nodo].predecessoreS;
     }
     while(!pila.empty()){
         cammino.push_back(pila.top());
         pila.pop();
     }
     return cammino;
}


void setVittoriaImpostore(Risultato &risultato){
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
}



// Funzione wrapper per l'algoritmo risolutivo
Risultato soluzione()
{
    // inizializza risultato e precedenti
    Risultato risultato = {-1, -1, -1, {}, {}};

    risultato.distanzaMinimaImpostore = dijkstraI();

    
    risultato.distanzaMinimaStudenti = dijkstraS();

    for(int i = 0; i < nodi.size(); i++){
        nodi[i].predecessoreI = -1;
        nodi[i].distanzaI = -1;
    }
    risultato.distanzaMinimaImpostore = dijkstraI();

    setVittoriaImpostore(risultato);

    //handle sconfitta/pareggio con percorso comune
    while((risultato.vittoriaImpostore == 0 || risultato.vittoriaImpostore == 2) && nodi[F].predecessoreI == nodi[F].predecessoreS){
        Corridoio* tmp = getCorridoioPareggiante();
        if (tmp == NULL)
        {
            //non posso fare altro
            break;
        }
        tmp->ventolaAccesa = true;
        setNodesMetadataMenoUno();
        risultato.distanzaMinimaImpostore = dijkstraI();
        risultato.distanzaMinimaStudenti = dijkstraS();
        for(int i = 0; i < nodi.size(); i++){
        nodi[i].predecessoreI = -1;
        nodi[i].distanzaI = -1;
        }
        risultato.distanzaMinimaImpostore = dijkstraI();
        if(risultato.distanzaMinimaStudenti < risultato.distanzaMinimaImpostore && risultato.vittoriaImpostore == 0){
            //ho già raggiunto il massimo che posso avere
            tmp->ventolaAccesa = false;
            setNodesMetadataMenoUno();
            risultato.distanzaMinimaImpostore = dijkstraI();
            risultato.distanzaMinimaStudenti = dijkstraS();
            for(int i = 0; i < nodi.size(); i++){
                nodi[i].predecessoreI = -1;
                nodi[i].distanzaI = -1;
            }
            risultato.distanzaMinimaImpostore = dijkstraI();
            setVittoriaImpostore(risultato);
            break;     
        }
        setVittoriaImpostore(risultato);
    }
   
    risultato.camminoImpostore = calcolaCamminoImpostore();

    return risultato;
}


Corridoio* getCorridoioPareggiante(){
    int current = F;
    int predecessore = nodi[current].predecessoreI;
    Corridoio* toReturn = NULL;
    while(predecessore != -1){
        for (int i = 0; i < nodi[predecessore].adj.size(); i++)
        {
            Corridoio* tmp = nodi[predecessore].adj[i];
            if(tmp->destinazione == current){
                if(tmp->Tmax != -1 && !tmp->ventolaAccesa){
                    toReturn = nodi[predecessore].adj[i];
                }
                else{
                    break;
                }
            }
        }
        current = predecessore;
        if(nodi[current].predecessoreI != nodi[current].predecessoreS){
            return toReturn;
        }
        predecessore = nodi[current].predecessoreI;
    }
    return toReturn;
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

bool studenteInVantaggio(int distanzaStudente, int distanzaImpostore){
    if (distanzaStudente == -1)
    {
        return false;
    }
    else if (distanzaImpostore == -1){
        return true;
    }
    else if (distanzaStudente < distanzaImpostore)
    {
        return true;
    }
    else
    {
        return false;
    }
    cout << "errore - studenteInVantaggio()" << endl;
}

int dijkstraS()
{
    priority_queue<pair<int,int>, std::vector<pair<int,int>>, std::greater<pair<int,int>>> coda;
    nodi[S].distanzaS = 0;
    int current, distanzaCurrent;
    coda.emplace(0, S);
    for (int i = 0; i < nodi[S].adj.size(); i++)
    {
        Corridoio* tmp = nodi[S].adj[i];
        tmp->ventolaAccesa = tmp->Tmax != -1;
    }
    while (!coda.empty())
    {
        current = coda.top().second;
        distanzaCurrent = coda.top().first;
        coda.pop();
        for (int i = 0; i < nodi[current].adj.size(); i++)
        {
            Corridoio* tmp = nodi[current].adj[i];
            int destinazione = tmp->destinazione;
            int costoCorridoio = tmp->ventolaAccesa ? tmp->Tmax : tmp->Tmin;
            bool vantaggioPrecedenteStudente = studenteInVantaggio(nodi[destinazione].distanzaS, nodi[destinazione].distanzaI);
            if (nodi[destinazione].distanzaS == -1 || nodi[destinazione].distanzaS > distanzaCurrent + costoCorridoio)
            {
                nodi[destinazione].distanzaS = nodi[current].distanzaS + costoCorridoio;
                coda.emplace(nodi[destinazione].distanzaS, destinazione);
                nodi[destinazione].predecessoreS = current;
                if (studenteInVantaggio(nodi[destinazione].distanzaS, nodi[destinazione].distanzaI) && !vantaggioPrecedenteStudente)
                {
                    for(int j = 0; j < nodi[destinazione].adj.size(); j++){
                        Corridoio* tmp = nodi[destinazione].adj[j];
                        tmp->ventolaAccesa = tmp->Tmax != -1;   //la setto a true se è con ventola
                    }
                }
            }
        }
    }
    return nodi[F].distanzaS;
}

int dijkstraI()
{
    // TODO implement with priority queue
    priority_queue<pair<int,int>, std::vector<pair<int,int>>, std::greater<pair<int,int>>> coda;
    nodi[I].distanzaI = 0;
    int current, distanzaCurrent;
    coda.emplace(0, I);
    while (!coda.empty())
    {
        current = coda.top().second;
        distanzaCurrent = coda.top().first;
        coda.pop();
        for (int i = 0; i < nodi[current].adj.size(); i++)
        {
            Corridoio* tmp = nodi[current].adj[i];
            int destinazione = tmp->destinazione;
            int costoCorridoio = tmp->ventolaAccesa ? tmp->Tmax : tmp->Tmin;
            if (nodi[destinazione].distanzaI == -1 || nodi[destinazione].distanzaI > (distanzaCurrent + costoCorridoio))
            {
                // setto la distanza
                nodi[destinazione].distanzaI = nodi[current].distanzaI + costoCorridoio;
                distanzaCurrent = nodi[destinazione].distanzaI;
                // aggiungo alla coda
                coda.emplace(nodi[destinazione].distanzaI, destinazione);
                // setto il predecessore
                nodi[destinazione].predecessoreI = current;
            }
        }
    }
    return nodi[F].distanzaI;
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