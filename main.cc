#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
using namespace std;


struct Corridoio{
    int destinazione;
    int Tmin;
    int Tmax = -1;
    bool ventola = false;
};

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

struct Risultato
{
    int vittoriaImpostore;
    int distanzaMinimaImpostore;
    int distanzaMinimaStudenti;
    vector<int> potenzaVentole;     //potremmo avere un problema dato che vanno restituiti nell'ordine in cui sono stati trovati in input
    //TODO add roba facoltativa quando la implementiamo
};


void printNodi(vector<Nodo> nodi);
void leggiNodi(fstream &stream, vector<Nodo> &nodi, int M, int K);
Risultato soluzione(vector<Nodo> &nodi, int I, int S, int F);
void stampaOutput(fstream &stream, Risultato &risultato);
int bsf(vector<Nodo> &G, int n, int f);
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
    inputGraphPrint(nodi, N, M, K, I, S, F);


    //risoluzione problema

    Risultato risultato = soluzione(nodi, I, S, F);

    //output

    stream.open(outputFilename, ios::out);
    stampaOutput(stream, risultato);
    stream.close();


    return 0;
}


int costoMinimoCorridoio(vector<Corridoio> &corridoi, int destinazione){
    for (size_t i = 0; i < corridoi.size(); i++)
    {
        if (corridoi[i].destinazione == destinazione)
            return corridoi[i].Tmin;
    }
    return -1;
}

Corridoio newCorridoio(int destinazione, int Tmin, int Tmax, bool ventola){
    Corridoio c;
    c.destinazione = destinazione;
    c.Tmin = Tmin;
    c.Tmax = Tmax;
    c.ventola = ventola;
    return c;
}




bool is_adiacente(vector<Corridoio> &raggiungibili, int destinazione){
    for (size_t i = 0; i < raggiungibili.size(); i++)
    {
        if (raggiungibili[i].destinazione == destinazione)
            return true;
    }
    return false;
}


Risultato soluzione(vector<Nodo> &nodi, int I, int S, int F){
    Risultato risultato = {-1, -1, -1, {}};
    
    inizializzaDistanze(nodi);
    risultato.distanzaMinimaImpostore = bfs(nodi, I, F);
    
    inizializzaDistanze(nodi);
    risultato.distanzaMinimaStudenti = bfs(nodi, S, F);

    if(risultato.distanzaMinimaImpostore == risultato.distanzaMinimaStudenti){
        risultato.vittoriaImpostore = 0;
    }
    else if(risultato.distanzaMinimaImpostore < risultato.distanzaMinimaStudenti){
        risultato.vittoriaImpostore = 1;
    }else{
        risultato.vittoriaImpostore = 2;
    }

    return risultato;
}


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



void stampaOutput(fstream &stream, Risultato &risultato){
    stream << risultato.vittoriaImpostore << endl;
    stream << risultato.distanzaMinimaImpostore << " " << risultato.distanzaMinimaStudenti << endl;
    for (size_t i = 0; i < risultato.potenzaVentole.size(); i++) {
        cout << risultato.potenzaVentole[i] << " ";
    }
    // TODO aggiungere la roba facoltativa qundo implementata
}

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


// Aggiungere vettore hash precedenti, interi interessati (percorso impostore) vengono inseriti in uno stack e poi stampati prelevandone uno ad uno

int bfs(vector<Nodo> &G, int n, int f){

    queue<int> Q;
    G[n].distanza = 0;
    int u;
    Q.push(n);
    while(!Q.empty()){
        u = Q.front();
        Q.pop();
        for(int i=0; i<G[u].raggiungibili.size(); i++){
            Corridoio tmp = G[u].raggiungibili[i];
            int nodo = tmp.destinazione;
            if(G[nodo].distanza == -1 || G[nodo].distanza > (G[u].distanza + tmp.Tmin)){
                G[nodo].distanza = G[u].distanza + tmp.Tmin;
                Q.push(nodo);
            }
            if(G[nodo].distanza == G[u].distanza + 1){
                G[tmp].salti = G[u].num + 1;
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
