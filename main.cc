#include<iostream>
#include<fstream>
#include<vector>
using namespace std;


struct Corridoio{
    int destinazione;
    int Tmin;
    int Tmax = -1;
    bool ventola = false;
};

struct Nodo{
    vector<Corridoio> raggiungibili;
    int distanza = -1;
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

int main()
{
    //open `input.txt` file
    fstream stream;
    stream.open("input.txt", ios::in);

    //read N, M, K
    int N, M, K;
    stream >> N >> M >> K;

    //read I, S, F
    int I, S, F;
    stream >> I >> S >> F;


    vector<Nodo> nodi(N);
    leggiNodi(stream, nodi, M, K);
    
    
    //close `input.txt` file
    stream.close();
    

    //DEBUG print the input graph

    cout << N << " aule;" << endl;
    cout << M << " corridoi semplici;" << endl;
    cout << K << " corridoi con ventola;" << endl;
    cout << endl;
    cout << I << " aula impostore;" << endl;
    cout << S << " aula studenti;" << endl;
    cout << F << " FabLab;" << endl;
    cout << endl;
    printNodi(nodi); 

    //END DEBUG


    //risoluzione problema

    Risultato risultato = soluzione(nodi, I, S, F);

    //output

    stream.open("output.txt", ios::out);
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



int distanzaMinimaPesataDa(vector<Nodo> &nodi, int start, int F){
    /*  questa doveva essere l'implementazione dell'algoritmo di Dijkstra, ma è decisamnete troppo tardi
    vector<int> distanze(nodi.size());
    distanze[start] = -1;      //default value just to be sure
    vector<int> predecessori(nodi.size());
    //initialize predecessori, don't know if it's necessary
    for (size_t i = 0; i < predecessori.size(); i++)
    {
        predecessori[i] = -1;
    }
    //Dijkstra initialization
    for (size_t i = 0; i < nodi.size(); i++)
    {
        if (i == start)
            continue;
        else {
            if (is_adiacente(nodi[start].raggiungibili, i))
            {
                distanze[i] = costoMinimoCorridoio(nodi[start].raggiungibili, i);
                predecessori[i] = start;
            }
            else{
                distanze[i] = -8            //stands for infinite lmao
            }
            
        }
    }
    */

    return -1;
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
    //TODO everything

    return risultato;
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
        nodi[U].raggiungibili.push_back({V, T});
    }

    //read remaining corridoi con ventola
    for (size_t i = 0; i < K; i++)
    {
        int U, V, Tmin, Tmax;
        stream >> U >> V >> Tmin >> Tmax;
        nodi[U].raggiungibili.push_back({V, Tmin, Tmax, true});
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