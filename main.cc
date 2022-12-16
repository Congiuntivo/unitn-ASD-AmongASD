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
    int Tmax = -1;
    bool ventola = false;
};

//Struttura che rappresenta un nodo del grafo
struct Nodo{
    vector<Corridoio> raggiungibili = {};
    int distanza = -1;
    int salti = 0;
    /*
    TODO provare a fare così
    -rimovere distanza

    -aggiungere questo:
    int distanza_studente = -1;
    int distanza_impostore = -1;
    */
};

//Struttura che contiene i dati da scrivere nel file di output
struct Risultato
{
    int vittoriaImpostore;
    int distanzaMinimaImpostore;
    int distanzaMinimaStudenti;
    vector<int> potenzaVentole;     //potremmo avere un problema dato che vanno restituiti nell'ordine in cui sono stati trovati in input
    vector<int> camminoImpostore;
    vector<int> camminoStudenti;    //non fa parte dei dati di output ma penso sarà comodo averlo
};


void printNodi(vector<Nodo> nodi);
void leggiNodi(fstream &stream, vector<Nodo> &nodi, int M, int K);
Risultato soluzione(vector<Nodo> &nodi, int I, int S, int F);
void stampaOutput(fstream &stream, Risultato &risultato);
int bfs(vector<Nodo> &G, int n, int f, vector<int> &precedenti);
void inizializzaDistanze(vector<Nodo> &G);
Corridoio newCorridoio(int destinazione, int Tmin, int Tmax = -1, bool ventola = false);
void inputGraphPrint(vector<Nodo> &nodi, int N, int M, int K, int I, int S, int F);

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
    int N, M, K;
    stream >> N >> M >> K;

    //read I, S, F
    int I, S, F;
    stream >> I >> S >> F;


    vector<Nodo> nodi(N);
    leggiNodi(stream, nodi, M, K);
    
    
    //close stream
    stream.close();
    

    //prints input graph
    // inputGraphPrint(nodi, N, M, K, I, S, F);


    //risoluzione problema

    Risultato risultato = soluzione(nodi, I, S, F);

    //output

    stream.open(outputFilename, ios::out);
    stampaOutput(stream, risultato);
    stream.close();


    return 0;
}

//Costruttore `Corridoio` siccome C++ 11 puzza
Corridoio newCorridoio(int destinazione, int Tmin, int Tmax, bool ventola){
    Corridoio c;
    c.destinazione = destinazione;
    c.Tmin = Tmin;
    c.Tmax = Tmax;
    c.ventola = ventola;
    return c;
}

//Funzione che dato un vettore di precedenze restituisce il cammino (migliorabile)
vector<int> calcolaCammino(vector<int> precedenti, int f){
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


//Funzione wrapper per la logica risolutiva
Risultato soluzione(vector<Nodo> &nodi, int I, int S, int F){
    //inizializza risultato
    Risultato risultato = {-1, -1, -1, {}, {}, {}};
    risultato.camminoImpostore.resize(nodi.size());
    risultato.camminoStudenti.resize(nodi.size());
    for (int i = 0; i < nodi.size(); i++)
    {
        risultato.camminoImpostore[i] = -1;
        risultato.camminoStudenti[i] = -1;
    }
    //dichiara e inizializza precedenti
    vector<int> precedenti(nodi.size());
    for (int i = 0; i < nodi.size(); i++)
    {
        precedenti[i] = -1;
    }


    // inizializzaDistanze(nodi);  inutile dato che è già stato fatto in `leggiNodi`
    risultato.distanzaMinimaImpostore = bfs(nodi, I, F, precedenti);
    risultato.camminoImpostore = calcolaCammino(precedenti, F);
    
    //re-inizializza precedenti
    for (int i = 0; i < nodi.size(); i++)
    {
        precedenti[i] = -1;
    }
    inizializzaDistanze(nodi);
    risultato.distanzaMinimaStudenti = bfs(nodi, S, F, precedenti);
    risultato.camminoImpostore = calcolaCammino(precedenti, F);


    //PAREGGIO
    if(risultato.distanzaMinimaImpostore == risultato.distanzaMinimaStudenti){
        risultato.vittoriaImpostore = 0;
    }
    //VITTORIA IMPOSTORE
    else if(risultato.distanzaMinimaImpostore < risultato.distanzaMinimaStudenti){
        risultato.vittoriaImpostore = 1;
    }
    //VITTORIA STUDENTI
    else{
        risultato.vittoriaImpostore = 2;
    }

    return risultato;
}

//Debug - stampa il grafo e le variabili ricevute in input
void inputGraphPrint(vector<Nodo> &nodi, int N, int M, int K, int I, int S, int F){
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
    for (size_t i = 0; i < risultato.potenzaVentole.size(); i++) {
        stream << risultato.potenzaVentole[i] << " ";
    }
    stream << endl;
    // TODO aggiungere la roba facoltativa qundo implementata
}

//Dato lo stream, legge il file di input e crea il grafo nel vettore di nodi
void leggiNodi(fstream &stream, vector<Nodo> &nodi, int M, int K){
    //read corridoi semplici
    for (size_t i = 0; i < M; i++)
    {
        int U, V, T;
        stream >> U >> V >> T;
        nodi[U].raggiungibili.push_back(newCorridoio(V, T));
    }

    //read remaining corridoi con ventola
    for (size_t i = 0; i < K; i++)
    {
        int U, V, Tmin, Tmax;
        stream >> U >> V >> Tmin >> Tmax;
        nodi[U].raggiungibili.push_back(newCorridoio(V, Tmin, Tmax, true));
    }
}

//Debug - stampa il grafo
void printNodi(vector<Nodo> nodi)
{
    for (size_t i = 0; i < nodi.size(); i++)
    {
        cout << "Nodo " << i << " connesso a:" << endl;
        if (nodi[i].raggiungibili.size() == 0)
            cout << "\tnessun nodo" << endl;
        else
        for (size_t j = 0; j < nodi[i].raggiungibili.size(); j++)
        {
            if(nodi[i].raggiungibili[j].ventola)
                cout << "\t" << nodi[i].raggiungibili[j].destinazione << " con costo [" << 
                nodi[i].raggiungibili[j].Tmin << ", " << nodi[i].raggiungibili[j].Tmax << "];" << endl;
            else
                cout << "\t" << nodi[i].raggiungibili[j].destinazione << " con costo " << 
                nodi[i].raggiungibili[j].Tmin << ";" << endl;
        }
        cout << endl;
    }
}


//TODO Aggiungere vettore hash precedenti, interi interessati (percorso impostore) vengono inseriti in uno stack e poi stampati prelevandone uno ad uno
int bfs(vector<Nodo> &G, int n, int f, vector<int> &precedenti){
    //TODO change to priority queue
    queue<int> Q;
    G[n].distanza = 0;
    int u;
    Q.push(n);
    while(!Q.empty()){
        u = Q.front();
        Q.pop();
        //per tutti gli adiacenti di `u` setto la distanza
        for(int i=0; i<G[u].raggiungibili.size(); i++){
            Corridoio tmp = G[u].raggiungibili[i];
            int nodo = tmp.destinazione;
            //se la distanza non è stata ancora calcolata oppure è maggiore di quella calcolata in questo percorso
            if(G[nodo].distanza == -1 || G[nodo].distanza > (G[u].distanza + tmp.Tmin)){
                //setto la nuova distanza
                G[nodo].distanza = G[u].distanza + tmp.Tmin;
                //metto il nodo nella coda
                Q.push(nodo);
                precedenti[nodo] = u;
            }
        }
    }
    return G[f].distanza;
}

void inizializzaDistanze(vector<Nodo> &G){
    for (int i = 0; i < G.size(); i++){
        G[i].distanza = -1;
    }
}
