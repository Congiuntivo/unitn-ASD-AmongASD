#include<iostream>
#include<fstream>
#include<vector>
using namespace std;


struct Corridoio{
    int U;
    int V;
    int Tmin;
    int Tmax;
    bool ventola;
};

void printCorridoio(Corridoio c);

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



    vector<Corridoio> corridoi(M + K);

    //read corridoi semplici
    for (size_t i = 0; i < M; i++)
    {
        int U, V, Tmin;
        stream >> U >> V >> Tmin;
        corridoi.push_back(Corridoio{U, V, Tmin, -1, false});
    }

    //read remaining corridoi con ventola
    for (size_t i = 0; i < K; i++)
    {
        int U, V, Tmin, Tmax;
        stream >> U >> V >> Tmin >> Tmax;
        corridoi.push_back(Corridoio{U, V, Tmin, Tmax, true});
    }
    

    //DEBUG

    cout << N << " aule;" << endl;
    cout << M << " corridoi semplici;" << endl;
    cout << K << " corridoi con ventola;" << endl;
    cout << endl;
    cout << I << " aula impostore;" << endl;
    cout << S << " aula studenti;" << endl;
    cout << F << " FabLab;" << endl;
    cout << endl;  
    cout << "Corridoi:" << endl;
    for (size_t i = 0; i < corridoi.size(); i++)
    {
        printCorridoio(corridoi[i]);
    }

    //END DEBUG


    //close `input.txt` file
    stream.close();



    return 0;
}


void printCorridoio(Corridoio c)
{
    if(c.ventola){
        cout << c.U << " --> " << c.V << ", costo = [" << c.Tmin << ", " << c.Tmax << "] " << endl;
    }
    else{
        cout << c.U << " --> " << c.V << ", costo = " << c.Tmin << endl;
    }
}