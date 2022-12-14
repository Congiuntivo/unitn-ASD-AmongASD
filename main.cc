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

void printNodi(vector<Nodo> nodi);
void leggiNodi(fstream &stream, vector<Nodo> &nodi, int M, int K);


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

    //TODO solution

    //TODO output

    return 0;
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