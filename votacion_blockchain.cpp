#include <iostream>
#include <vector>
#include <string>
using namespace std;


// Integrante 1

string aHexadecimal(unsigned long long numero){
    string simbolos = "0123456789abcdef";
    string resultado = "";

    for(int i = 0; i < 16; i++){
        resultado = simbolos[numero % 16] + resultado;
        numero = numero / 16;
    }
    return resultado;
}

class Voto{
public:
    string idVotante;
    string opcion;

    Voto(string id, string op){
        idVotante = id;
        opcion = op;
    }
};


class Block{
public:
    int indice;
    string hashAnterior;
    vector<Voto> votos;
    int nonce;
    string miHash;

    Block(int i, string anterior, vector<Voto> v){
        indice = i;
        hashAnterior = anterior;
        votos = v;
        nonce = 0;

        miHash = sacarHash();
    }

    string sacarHash(){
        string juntado = to_string(indice) + hashAnterior + to_string(nonce);

        for(int i = 0; i < votos.size(); i++){
            juntado = juntado + votos[i].idVotante + votos[i].opcion;
        }

        unsigned long long suma = 0;
        for(int i = 0; i < juntado.size(); i++){
            suma = suma * 31 + juntado[i];
        }
        return aHexadecimal(suma);
    }
};



// Integrante 2

class Blockchain{
private:
    static Blockchain* instancia;
    vector<Block> bloques;

    Blockchain(){
        vector<Voto> vacio;
        Block genesis(0, "0", vacio);
        bloques.push_back(genesis);
    }

public:

    static Blockchain* getInstance(){
        if(instancia == nullptr){
            instancia = new Blockchain();
        }
        return instancia;
    }

    Block ultimoBloque(){
        return bloques[bloques.size() - 1];
    }


    void agregarBloque(Block b){
        bloques.push_back(b);
    }

    void mineBlock(int dificultad){
        string ceros = "";
        for(int i = 0; i < dificultad; i++){
            ceros = ceros + "0";
        }

        Block& actual = bloques[bloques.size() - 1];

        while(actual.miHash.substr(0, dificultad) != ceros){
            actual.nonce++;
            actual.miHash = actual.sacarHash();
        }
    }

    bool isChainValid(){
        for(int i = 1; i < bloques.size(); i++){
            Block actual = bloques[i];
            Block anterior = bloques[i - 1];

            if(actual.miHash != actual.sacarHash()){
                return false;
            }
            if(actual.hashAnterior != anterior.miHash){
                return false;
            }
        }
        return true;
    }
};

Blockchain* Blockchain::instancia = nullptr;



// Integrante 3

class MesaElectoralObserver{
public:
    virtual void update(Block nuevoBloque) = 0;
};


class MesaElectoral : public MesaElectoralObserver{
public:
    string nombre;
    vector<Block> miCadena;

    MesaElectoral(string n){
        nombre = n;

        Block genesis = Blockchain::getInstance()->ultimoBloque();
        miCadena.push_back(genesis);
    }

    void update(Block nuevoBloque){
        Block ultimo = miCadena[miCadena.size() - 1];

        bool hashBien = (nuevoBloque.miHash == nuevoBloque.sacarHash());
        bool enlaceBien = (nuevoBloque.hashAnterior == ultimo.miHash);

        if(hashBien && enlaceBien){
            miCadena.push_back(nuevoBloque);
            cout << nombre << " recibio el bloque " << nuevoBloque.indice
                 << " y lo agrego. Ahora tiene " << miCadena.size() << " bloques." << endl;
        }else{
            cout << nombre << " rechazo el bloque porque no es valido." << endl;
        }
    }
};

class CentroElectoralSubject{
private:
    vector<MesaElectoralObserver*> mesas;

public:
    void attach(MesaElectoralObserver* m){
        mesas.push_back(m);
    }

    void notificarNuevoBloque(Block b){
        for(int i = 0; i < mesas.size(); i++){
            mesas[i]->update(b);
        }
    }
};


int main(){
    Blockchain* cadena = Blockchain::getInstance();

    MesaElectoral mesa1("Mesa Centro");
    MesaElectoral mesa2("Mesa Norte");
    MesaElectoral mesa3("Mesa Sur");

    CentroElectoralSubject red;
    red.attach(&mesa1);
    red.attach(&mesa2);
    red.attach(&mesa3);


    vector<Voto> votos;
    votos.push_back(Voto("id01", "Candidato A"));
    votos.push_back(Voto("id02", "Candidato B"));
    votos.push_back(Voto("id03", "Candidato A"));

    Block nuevo(1, cadena->ultimoBloque().miHash, votos);
    cadena->agregarBloque(nuevo);

    cout << "Minando el bloque de votos..." << endl;
    cadena->mineBlock(2);

    cout << "Bloque sellado con hash: " << cadena->ultimoBloque().miHash << endl;
    cout << "Nonce encontrado: " << cadena->ultimoBloque().nonce << endl;
    cout << endl;

    cout << "La Mesa Centro avisa a toda la red que mino un bloque:" << endl;
    red.notificarNuevoBloque(cadena->ultimoBloque());
    cout << endl;

    cout << "La cadena oficial es valida? " << (cadena->isChainValid() ? "Si" : "No") << endl;

    return 0;
}
