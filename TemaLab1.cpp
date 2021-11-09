#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <fstream>

using namespace std;

class Graf {
    int nrNoduri; // numarul de noduri ale grafului
    vector<vector<int>> matriceAdiacenta; // matrice de adiacenta( un vector de vectori; primul element din fiecare vector este -1 )
    bool esteOrientat; // esteOrientat = true -> graf orientat esle -> graf neorientat

public:
    Graf(int nrNoduri, const vector<vector<int>> &matriceAdiacenta, bool esteOrientat);

    Graf(int nrNoduri, bool esteOrientat);

    Graf(const Graf &graf);

    void citireGraf(istream &in, int nrMuchii);

    void adaugareMuchie(int startNode, int endNode);

    void eliminareMuchie(int startNode, int endNode);

    void distantaMinimaBFS(ostream &out, int start);

    void DFS(int nod, vector<int> &vizitate);

    int componenteConexe();

    int componenteConexeRecursiv(vector<int> &vizitate, int startPos = 1);

    void muchiiCritice_neeficient();

    void DFS_muchiiCritice();

    void DFS_sortareTopologica(ostream &out);

    friend istream &operator>>(istream &in, Graf &graf) {
        cout << "\nEste graf orientat? (1 pentru DA, 0 pentru NU): ";
        in >> graf.esteOrientat;
        cout << "\nNumarul de noduri: ";
        in >> graf.nrNoduri;
        cout << "\nNumarul de muchii: ";
        int nrMuchii;
        in >> nrMuchii;
        for (int i = 1; i <= nrMuchii; i++) {
            cout << "Muchia numarul " << i << ": ";
            int startNode, endNode;
            in >> startNode >> endNode;
            graf.adaugareMuchie(startNode, endNode);
        }
        return in;
    }

    friend ostream &operator<<(ostream &out, Graf &graf) {
        if (graf.esteOrientat) {
            out << "Este graf orientat? DA";
        } else {
            out << "Este graf orientat? NU";
        }
        out << "\nNumar noduri: " << graf.nrNoduri;
        out << "\nMuchii:\n";
        for (int i = 1; i <= graf.nrNoduri; i++) {
            for (int j = 1; j < graf.matriceAdiacenta[i].size(); j++) {
                out << i << " " << graf.matriceAdiacenta[i][j] << '\n';
            }
        }
        return out;
    }

private:
    void MuchieCritica(int nod, int time[], int low_time[], int parent[], int vizitate[]);

    void sortare_topologica(int nod, int vizitate[], vector<int> &result);

};

/*
 * Constructor cu toti parametrii
 * nrNodurie = numarul de noduri ale grafului
 * matriceAdiacenta = matricea de adiacenta a grafului
 * esteOrientat = true -> graful este orientat; false -> graful este neorientat
 */
Graf::Graf(int nrNoduri, const vector<vector<int>> &matriceAdiacenta, bool esteOrientat) {
    this->nrNoduri = nrNoduri;
    this->matriceAdiacenta = matriceAdiacenta;
    this->esteOrientat = esteOrientat;
}

/*
 * Constructor cu toti parametrii
 * nrNodurie = numarul de noduri ale grafului
 * esteOrientat = true -> graful este orientat; false -> graful este neorientat
 * matricea de adiacenta este creata si fiecare linie initiata cu -1
 */
Graf::Graf(int nrNoduri, bool esteOrientat) {
    this->nrNoduri = nrNoduri;
    this->esteOrientat = esteOrientat;

    vector<int> v(1, -1);
    for (int i = 0; i <= nrNoduri; ++i) {
        matriceAdiacenta.push_back(v);
    }
}

/*
 * Constructor de copiere care copiaza din obiectul primit ca parametru in obiectul curent
 * graf = obiect de tipul Graf din care va fi copiat
 */
Graf::Graf(const Graf &graf) {
    this->nrNoduri = graf.nrNoduri;
    this->esteOrientat = graf.esteOrientat;
    this->matriceAdiacenta.clear();
    this->matriceAdiacenta = graf.matriceAdiacenta;
}

/*
 * Citeste o pereche x,y si adauga in matricea de adiacenta muchiile/arcele citite
 * in = stream-ul de intrare
 * nrMuchii = numarul de muchii/arce citite
 */
void Graf::citireGraf(istream &in, int nrMuchii) {
    int x, y;
    for (int i = 0; i < nrMuchii; i++) {
        in >> x >> y;
        matriceAdiacenta[x].push_back(y);
        if (!esteOrientat) {
            matriceAdiacenta[y].push_back(x);
        }
    }
}

/*
 * Addauga o muchie in graf
 * startNode = nodul din care porneste muchi
 * endNode = nodul in care ajunge muchia
 * In functie de tipul de graf, este actualizata matricea de adiacenta
 */
void Graf::adaugareMuchie(int startNode, int endNode) {
    if (!this->esteOrientat) {
        this->matriceAdiacenta[startNode].push_back(endNode);
        this->matriceAdiacenta[endNode].push_back(startNode);
    } else {
        this->matriceAdiacenta[startNode].push_back(endNode);
    }
}

void Graf::eliminareMuchie(int startNode, int endNode) {
    int len = this->matriceAdiacenta[startNode].size();
    for (int i = 1; i < len; i++) {
        if (this->matriceAdiacenta[startNode][i] == endNode) {
            this->matriceAdiacenta[startNode][i] = this->matriceAdiacenta[startNode][len - 1];
            this->matriceAdiacenta[startNode].pop_back();
        }
    }

    if (!esteOrientat) {
        len = this->matriceAdiacenta[endNode].size();
        for (int i = 0; i < len; i++) {
            if (this->matriceAdiacenta[endNode][i] == startNode) {
                this->matriceAdiacenta[endNode][i] = this->matriceAdiacenta[endNode][len - 1];
                this->matriceAdiacenta[endNode].pop_back();
            }
        }
    }
}


/*
 * Calculeaza distanta minima de la fiecare nod la start si o afiseaza
 * out = stream-ul de iesire
 * start = nodul de start
 */
void Graf::distantaMinimaBFS(ostream &out, int start) {
    // Vector pentru distanta in care fiecare element este initial -1
    vector<int> distanta(nrNoduri + 1, -1);
    distanta[start] = 0;

    // Queue in care salvam elementele care trebuiesc vizitate. Cand se goleste, nu mai avem nimic de vizitat
    queue<int> queue;
    queue.push(start);

    while (!queue.empty()) {
        start = queue.front();
        queue.pop();
        for (auto it: matriceAdiacenta[start]) { // parcurgem fiecare nod adiacent al nodului curent
            // Daca nu a fost vizitat inca
            if (distanta[it] == -1) {
                distanta[it] = distanta[start] + 1;
                queue.push(it); // Il adaugam in coada pentru a fi vizitat
            }
        }
    }

    // Afisam distanta
    for (int i = 1; i <= nrNoduri; i++) {
        out << distanta[i] << " ";
    }
}

/*
 * Parcurgere in inaltime
 * nod = nodul curent
 * vizitate = vector in care sunt marcate nodurile vizitate
 */
void Graf::DFS(int nod, vector<int> &vizitate) {
    vizitate[nod] = 1;
    for (int i = 1; i < matriceAdiacenta[nod].size(); i++) { // parcurgem nodurile adiacente nodului curent
        if (vizitate[matriceAdiacenta[nod][i]] == 0) { // daca nu a fost vizitat
            DFS(matriceAdiacenta[nod][i], vizitate);
        }
    }
}

/*
 * Functie care ia fiecare nod nevizitat al grafului si cu ajutorul DFS viziteaza toate nodurile care au drum catre nodurile curent (practic viziteaza o componenta conexa)
 * Returneaza numarul de componente conexe
 */
int Graf::componenteConexe() {
    int ct = 0;
    vector<int> vizitate(nrNoduri + 1, 0);
    for (int i = 1; i <= nrNoduri; i++) {
        if (vizitate[i] == 0) {
            ct++;
            DFS(i, vizitate);
        }
    }
    return ct;
}

/*
 * Functie recursiva care ia fiecare nod nevizitat si aplica DFS din el
 * Se opreste in momentul in care trece de ultimul nod
 * vizitate = vectorul in care sunt marcate nodurile vizitate
 * startPos = un index folosit pentru a parcurge nodurile, are valoare default 1
 * Returneaza numarul de componente conexe
 */
int Graf::componenteConexeRecursiv(vector<int> &vizitate, int startPos) {
    if (startPos == nrNoduri + 1) {
        return 0;
    } else {
        if (vizitate[startPos] == 0) { // i nu exista in map => nu a fost vizitat
            DFS(startPos, vizitate);
            return 1 + componenteConexeRecursiv(vizitate, startPos + 1);
        } else {
            return componenteConexeRecursiv(vizitate, startPos + 1);
        }
    }
}

void Graf::muchiiCritice_neeficient() {
    map<pair<int, int>, bool> muchiiVizitate;

    // parcurgem fiecare muchie a grafului si o eliminam
    for (int nod = 1; nod <= this->nrNoduri; nod++) {
        for (int j = 1; j < this->matriceAdiacenta[nod].size(); j++) {
            int nodAdiacentCurent = this->matriceAdiacenta[nod][j];
            if (muchiiVizitate.find(make_pair(nod, nodAdiacentCurent)) == muchiiVizitate.end()) {
                eliminareMuchie(nod, nodAdiacentCurent);
                vector<int> vizitateAux(this->nrNoduri + 1, 0);
                DFS(nod, vizitateAux);
                for (int i = 1; i <= this->nrNoduri; i++) {
                    if (vizitateAux[i] == 0) {
                        cout << nod << " " << nodAdiacentCurent << '\n';
                        break;
                    }
                }
                adaugareMuchie(nod, nodAdiacentCurent);
                muchiiVizitate[make_pair(nod, nodAdiacentCurent)] = true;
                muchiiVizitate[make_pair(nodAdiacentCurent, nod)] = true;
            }
        }
    }
}

/*
 * O muchie [a,b] este muchie critica daca nu exista niciun drum de la descendentii lui b la a sau ascendentii sai
 */
void Graf::MuchieCritica(int nod, int time[], int low_time[], int parent[], int vizitate[]) {

    static int t = 0;
    vizitate[nod] = 1; // vizitam nodul
    t++;
    low_time[nod] = t;
    time[nod] = low_time[nod];

    // parcurgem fiecare nod adiacent nodului curent
    for (int i = 1; i < matriceAdiacenta[nod].size(); i++) {
        int nodAdiacentCurent = matriceAdiacenta[nod][i];
        if (!vizitate[nodAdiacentCurent]) { // daca nu e vizitat
            parent[nodAdiacentCurent] = nod; // tinem minte parintele
            MuchieCritica(nodAdiacentCurent, time, low_time, parent,
                          vizitate); // facem verificarea si pentru nodulAdiacentCurent
            low_time[nod] = min(low_time[nod],
                                low_time[nodAdiacentCurent]); // actualizam low_time[nod] daca nodAdiacentCurent are muchie cu un parinte al nodului sau cu nodul
            if (low_time[nodAdiacentCurent] >
                time[nod]) { // conditia ca muchiia [nod,nodAdiacentCuret] sa fie critica (sa aiba drum la nod sau ascendentii sai)
                cout << nod << " " << nodAdiacentCurent << '\n';
            }
        } else if (nodAdiacentCurent != parent[nod]) {
            low_time[nod] = min(low_time[nod], time[nodAdiacentCurent]);
        }
    }
};

/*
 * Ia fiecare nod al grafului si apeleaza functia MuchiCritica
 */
void Graf::DFS_muchiiCritice() {
    // time = vector in care tinem minte momentul in care un nod a fost vizitat
    // low_time = vector in care tinem minte muchia vizitata cel mai devreme
    // parent = vector care retine parintele nodurilor
    int time[this->nrNoduri + 1], low_time[this->nrNoduri + 1], parent[this->nrNoduri + 1], vizitate[
            this->nrNoduri + 1];
    for (int i = 0; i <= this->nrNoduri; i++) {
        parent[i] = -1;
        vizitate[i] = 0;
    }

    // parcurgem fiecare nod al grafului
    for (int i = 1; i <= this->nrNoduri; i++) {
        if (!vizitate[i]) {
            MuchieCritica(i, time, low_time, parent, vizitate);
        }
    }
}

void Graf::sortare_topologica(int nod, int vizitate[], vector<int> &result) {
    vizitate[nod] = 1;

    // parcurgem fiecare nod adiacent nodului curent
    for (int i = 0; i < matriceAdiacenta[nod].size(); i++) {
        int nodCurentAdiacent = matriceAdiacenta[nod][i];
        if (!vizitate[nodCurentAdiacent]) {
            sortare_topologica(nodCurentAdiacent, vizitate, result);
        }
    }

    result.push_back(nod);
}

void Graf::DFS_sortareTopologica(ostream &out) {
    int vizitate[this->nrNoduri + 1];
    vector<int> result;

    for (int i = 1; i <= this->nrNoduri; i++) {
        vizitate[i] = 0;
    }

    for (int i = 1; i <= this->nrNoduri; i++) {
        if (!vizitate[i]) {
            sortare_topologica(i, vizitate, result);
        }
    }

    for (int i = result.size() - 1; i >= 0; i--) {
        out << result[i] << " ";
    }
}

/* -------------------------------------------------------------- */

void infoarena_bfs() {
    ifstream f("bfs.in");
    ofstream g("bfs.out");

    int N, M, S;
    f >> N >> M >> S;
    Graf graf(N, true);
    graf.citireGraf(f, M);
    graf.distantaMinimaBFS(g, S);
}

void infoarena_dfs() {
    ifstream f("dfs.in");
    ofstream g("dfs.out");

    int N, M, S;
    f >> N >> M;
    Graf graf(N, false);
    graf.citireGraf(f, M);
//    map<int,bool> vizitate;
    g << graf.componenteConexe();
}

void leetcode_CriticalConnections_neeficient() {
    ifstream f("dfs.in");
    int n, nrMuchii;
    f >> n;
    f >> nrMuchii;
    Graf graf(n, false);
    graf.citireGraf(f, nrMuchii);
    graf.muchiiCritice_neeficient();
}

void leetcode_CriticalConnections() {
    ifstream f("dfs.in");
    int n, nrMuchii;
    f >> n;
    f >> nrMuchii;
    Graf graf(n, false);
    graf.citireGraf(f, nrMuchii);
    graf.DFS_muchiiCritice();
}

void infoarena_sortareTopologica() {
    ifstream f("sortaret.in");
    ofstream g("sortaret.out");

    int N, M;
    f >> N >> M;
    Graf graf(N, true);
    graf.citireGraf(f, M);
    graf.DFS_sortareTopologica(g);
}

/* -------------------------------------------------------------- */

int main() {
//    infoarena_bfs();
//    infoarena_dfs();
//    leetcode_CriticalConnections();
//    leetcode_CriticalConnections_neeficient();
//    infoarena_sortareTopologica();
    return 0;
}
