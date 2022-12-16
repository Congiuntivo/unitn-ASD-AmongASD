#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<stack>
#include<unordered_set>
using namespace std;

//Struttura che rappresenta un corridoio con o senza ventola
struct Corridoio{
    int destinazione;
    int Tmin;
    int Tmax = -1;
    bool ventola = false;
    int potenza = -1;
    bool impostore = false;
    
};

/*
PSEUDO CODICE:

SetVentoleMinimo()
camminoStudentiPrecedente = {}
camminoImpostorePrecedente = {}
camminoStudenti = {}
camminoImpostore = {}
ImpostoreNonVince = chiVince(&camminoStudenti, &camminoImpostore)
camminiCambiati = cambiatoCammino(&camminoStudenti, &camminoStudentiPrecedente) || cambiatoCammino(&camminoImpostore, &camminoImpostorePrecedente)
while (ImpostoreNonVince && camminiCambiati):
    impostaVentoleStudenteMax()             //imposta le ventole del cammino dello studente al massimo (ventole€)
    camminoStudentiPrecedente = camminoStudenti
    camminoImpostorePrecedente = camminoImpostore
    camminoStudenti = {}
    camminoImpostore = {}
    ImpostoreNonVince
 = chiVince(&camminoStudenti, &camminoImpostore)
    camminiCambiati = cambiatoCammino(&camminoStudenti, &camminoStudentiPrecedente) || cambiatoCammino(&camminoImpostore, &camminoImpostorePrecedente)
corridoio = getPrimoCorridoioConVentolaInComune()
while (ImpostoreNonVince && corridoio != NULL):
    //sono sicuro che se hanno un nodo in comune il cammino successivo sarà lo stesso e che, siccome l'impostore sta perdendo o pareggiando,
    //lo studente sarà o in vantaggio o a pari distanza
    nodoInComune = corridoio.partenza
    //lo studente è in vantaggio, alzo la ventola al massimo, rendendo il percorso sconveniente anche per l'impostore, ma 
    //spero che l'impostore abbia un percorso alternativo più conveniente
    if (nodoInComune.distanzaStudente < nodoInComune.distanzaImpostore):
        corridoio.impostaVentolaMax()    
    //sono a pari distanza
    else:
        //TODO!!
        //se alzo al massimo la ventola potrei perdere, nel caso in cui lo studente trovi un percorso alternativo più conveniente, perciò keep calm
        //se lascio la ventola al minimo potrei non ottenere una potenziale vittoria
        //qindi dovrei fare Dijkstra senza considerare quel corridoio per vedere quali sono le alternative
*/

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
void leggiNodi(fstream &stream, vector<Nodo> &nodi, int M, int K, vector<Corridoio*> &corridoi);
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

    vector<Corridoio*> corridoi(M+K);
    vector<Nodo> nodi(N);
    leggiNodi(stream, nodi, M, K, corridoi);
    
    
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

//Costruttore `Corridoio` siccome C++ 11 puzza
Corridoio newCorridoio(int destinazione, int Tmin, int Tmax, bool ventola){
    Corridoio c;
    c.destinazione = destinazione;
    c.Tmin = Tmin;
    c.Tmax = Tmax;
    c.ventola = ventola;
    c.potenza = Tmin;
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
Risultato soluzione(vector<Nodo> &nodi, int I, int S, int F){
    //inizializza risultato e precedenti
    Risultato risultato = {-1, -1, -1, {}, {}, {}};
    risultato.camminoImpostore.resize(nodi.size());
    risultato.camminoStudenti.resize(nodi.size());
    vector<int> precedenti(nodi.size());
    for (int i = 0; i < nodi.size(); i++)
    {
        risultato.camminoImpostore[i] = -1;
        risultato.camminoStudenti[i] = -1;
        precedenti[i] = -1;
    }

    risultato.distanzaMinimaImpostore = bfs(nodi, I, F, precedenti);
    risultato.camminoImpostore = calcolaCammino(precedenti, F);
    
    //re-inizializza precedenti e distanze
    setMenoUno(precedenti);
    inizializzaDistanze(nodi);
    risultato.distanzaMinimaStudenti = bfs(nodi, S, F, precedenti);
    risultato.camminoStudenti = calcolaCammino(precedenti, F);


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
    stream << risultato.camminoImpostore.size() << endl;
    for (size_t i = 0; i < risultato.camminoImpostore.size(); i++) {
        stream << risultato.camminoImpostore[i] << " ";
    }
}

//Dato lo stream, legge il file di input e crea il grafo nel vettore di nodi
void leggiNodi(fstream &stream, vector<Nodo> &nodi, int M, int K, vector<Corridoio*> &corridoi){
    //read corridoi semplici
    for (size_t i = 0; i < M; i++)
    {
        int U, V, T;
        stream >> U >> V >> T;
        nodi[U].raggiungibili.push_back(newCorridoio(V, T));
        corridoi.push_back(&nodi[U].raggiungibili[nodi[U].raggiungibili.size()-1]);
    }

    //read remaining corridoi con ventola
    for (size_t i = 0; i < K; i++)
    {
        int U, V, Tmin, Tmax;
        stream >> U >> V >> Tmin >> Tmax;
        nodi[U].raggiungibili.push_back(newCorridoio(V, Tmin, Tmax, true));
        corridoi.push_back(&nodi[U].raggiungibili[nodi[U].raggiungibili.size()-1]);
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
                //setto il predecessore
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
