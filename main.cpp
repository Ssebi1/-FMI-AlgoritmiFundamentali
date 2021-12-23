#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <fstream>
#include <climits>
#include <limits.h>

using namespace std;

class Graf {
    int nrNoduri; // numarul de noduri ale grafului
    vector<vector<int>> matriceAdiacenta; // matrice de adiacenta( un vector de vectori; primul element din fiecare vector este -1 )
    bool esteOrientat; // esteOrientat = true -> graf orientat esle -> graf neorientat

public:
    Graf(int nrNoduri, const vector<vector<int>> &matriceAdiacenta, bool esteOrientat);

    Graf(int nrNoduri, bool esteOrientat);

    Graf(const Graf &graf);

    Graf() {};

    void citireGraf(istream &in, int nrMuchii);

    void adaugareMuchie(int startNode, int endNode);

    void eliminareMuchie(int startNode, int endNode);

    vector<int> distantaMinimaBFS(ostream &out, int start);

    int componenteConexe();

    int componenteConexeRecursiv(vector<int> &vizitate, int startPos = 1);

    vector<vector<int>> muchiiCritice_neeficient();

    vector<vector<int>> DFS_muchiiCritice();

    vector<int> DFS_sortareTopologica(ostream &out);

    int disjoint(int cod, int x, int y, vector<int> &parinte, vector<int> &rang);

    vector<int> dijkstra(vector<vector<pair<int, int>>> &matriceAdiacentaCosturi, int nodStart);

    vector<int> bellman_ford(vector<vector<pair<int, int>>> &matriceAdiacentaCosturi, int nodStart);

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
    void DFS(int nod, vector<int> &vizitate);

    void MuchieCritica(int nod, vector<int> &time, vector<int> &low_time, vector<int> &parent, vector<int> &vizitate,
                       vector<vector<int>> &result);

    void sortare_topologica(int nod, vector<int> &vizitate, vector<int> &result);

    int gasesteRadacina(int x, vector<int> parinte);

    int interogareCompresie(int x, vector<int> &parinte);

    void reuniune(int x, int y, vector<int> &parinte, vector<int> &rang);

};

/*
 * Constructor cu toti parametrii
 * nrNodurie = numarul de noduri ale grafului
 * matriceAdiacenta = matricea de adiacenta a grafului
 * esteOrientat = true -> graful este orientat; false -> graful este neorientat
 */
Graf::Graf(const int nrNoduri, const vector<vector<int>> &matriceAdiacenta, const bool esteOrientat) {
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
Graf::Graf(const int nrNoduri, const bool esteOrientat) {
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
void Graf::citireGraf(istream &in, const int nrMuchii) {
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
void Graf::adaugareMuchie(const int startNode, const int endNode) {
    if (!this->esteOrientat) {
        this->matriceAdiacenta[startNode].push_back(endNode);
        this->matriceAdiacenta[endNode].push_back(startNode);
    } else {
        this->matriceAdiacenta[startNode].push_back(endNode);
    }
}

void Graf::eliminareMuchie(const int startNode, const int endNode) {
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
vector<int> Graf::distantaMinimaBFS(ostream &out, int start) {
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

    // Returnam distanta
    return distanta;

}

/*
 * Parcurgere in inaltime
 * nod = nodul curent
 * vizitate = vector in care sunt marcate nodurile vizitate
 */
void Graf::DFS(const int nod, vector<int> &vizitate) {
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
int Graf::componenteConexeRecursiv(vector<int> &vizitate, const int startPos) {
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

vector<vector<int>> Graf::muchiiCritice_neeficient() {
    map<pair<int, int>, bool> muchiiVizitate;
    vector<vector<int>> result;

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
                        // cout << nod << " " << nodAdiacentCurent << '\n';
                        vector<int> pair;
                        pair.push_back(nod);
                        pair.push_back(nodAdiacentCurent);
                        result.push_back(pair);
                        break;
                    }
                }
                adaugareMuchie(nod, nodAdiacentCurent);
                muchiiVizitate[make_pair(nod, nodAdiacentCurent)] = true;
                muchiiVizitate[make_pair(nodAdiacentCurent, nod)] = true;
            }
        }
    }
    return result;
}

/*
 * O muchie [a,b] este muchie critica daca nu exista niciun drum de la descendentii lui b la a sau ascendentii sai
 */
void
Graf::MuchieCritica(const int nod, vector<int> &time, vector<int> &low_time, vector<int> &parent, vector<int> &vizitate,
                    vector<vector<int>> &result) {

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
                          vizitate, result); // facem verificarea si pentru nodulAdiacentCurent
            low_time[nod] = min(low_time[nod],
                                low_time[nodAdiacentCurent]); // actualizam low_time[nod] daca nodAdiacentCurent are muchie cu un parinte al nodului sau cu nodul
            if (low_time[nodAdiacentCurent] >
                time[nod]) { // conditia ca muchiia [nod,nodAdiacentCuret] sa fie critica (sa aiba drum la nod sau ascendentii sai)
//                cout << nod << " " << nodAdiacentCurent << '\n';
                vector<int> pair;
                pair.push_back(nod);
                pair.push_back(nodAdiacentCurent);
                result.push_back(pair);
            }
        } else if (nodAdiacentCurent != parent[nod]) {
            low_time[nod] = min(low_time[nod], time[nodAdiacentCurent]);
        }
    }
};

/*
 * Ia fiecare nod al grafului si apeleaza functia MuchiCritica
 */
vector<vector<int>> Graf::DFS_muchiiCritice() {
    // time = vector in care tinem minte momentul in care un nod a fost vizitat
    // low_time = vector in care tinem minte muchia vizitata cel mai devreme
    // parent = vector care retine parintele nodurilor
    vector<int> vizitate(nrNoduri + 1, 0), parent(nrNoduri + 1, -1), time(nrNoduri + 1), low_time(nrNoduri + 1);
    vector<vector<int>> result;

    // parcurgem fiecare nod al grafului
    for (int i = 1; i <= this->nrNoduri; i++) {
        if (!vizitate[i]) {
            MuchieCritica(i, time, low_time, parent, vizitate, result);
        }
    }
    return result;
}

void Graf::sortare_topologica(const int nod, vector<int> &vizitate, vector<int> &result) {
    vizitate[nod] = 1;

    // parcurgem fiecare nod adiacent nodului curent
    for (int i = 1; i < matriceAdiacenta[nod].size(); i++) {
        int nodCurentAdiacent = matriceAdiacenta[nod][i];
        if (!vizitate[nodCurentAdiacent]) {
            sortare_topologica(nodCurentAdiacent, vizitate, result);
        }
    }

    result.push_back(nod);
}

vector<int> Graf::DFS_sortareTopologica(ostream &out) {
    vector<int> vizitate(nrNoduri + 1, 0);
    vector<int> result;

    for (int i = 1; i <= this->nrNoduri; i++) {
        if (!vizitate[i]) {
            sortare_topologica(i, vizitate, result);
        }
    }
    return result;
}

/*
 * Pornind de la un nod dat ca parametru returneaza radicina arborelui in care se afla x
 */
int Graf::gasesteRadacina(int x, vector<int> parinte) {
    while (x != parinte[x]) {
        x = parinte[x];
    }
    return x;
}

/*
 * Dupa ce este gasita radacina arborelui care il contine pe x, se parcurge iar drumul de la x la radacina
 * si fiecare nod este legat de radacina.
 * Returneaza radacina
 */
int Graf::interogareCompresie(int x, vector<int> &parinte) {
    int radacina = gasesteRadacina(x, parinte); //gaseste radacina arborelui in care se afla nodul x
    // mergem din nodul initial x pana in radacina arborelui si unim toate nodurile de radacina
    while (x != parinte[x]) {
        int aux = parinte[x];
        parinte[x] = radacina;
        x = aux;
    }
    return radacina;
}

/*
 * Face reuniunea a doi arbori care au radacinile x si y, in functie de rangul arborilor
 */
void Graf::reuniune(int x, int y, vector<int> &parinte, vector<int> &rang) {
    // Unim arborele mai mic de cel mai mare
    if (rang[x] >= rang[y]) { // arborele cu radacina x este mai mare
        parinte[y] = x; // il facem pe x radacina arborelui reunit
        rang[y] = rang[x]; // crestem rangul arborelui mai mic
    } else if (rang[x] == rang[y]) { // arborii au dimensiuni egale
        parinte[x] = y;
        rang[x]++;
    } else if (rang[x] < rang[y]) { // arborele cu radacina y este mai mare
        parinte[x] = y; // il facem pe y radainca arborelui reunit
        rang[x] = rang[y]; // crestem rangul arborelui mai mic
    }
}

/*
 * Face o operatie in functie de codul primiti.
 * Returneaza: -1 daca a fost efectuata operatia de reuniune, 1 daca x si y sunt in aceeasi multime, 0 altfel
 */
int Graf::disjoint(int cod, int x, int y, vector<int> &parinte, vector<int> &rang) {
    int radacinaX = interogareCompresie(x, parinte), radacinaY = interogareCompresie(y, parinte);
    if (cod == 1) {
        reuniune(radacinaX, radacinaY, parinte, rang);
        return -1;
    } else if (cod == 2) {
        // daca nodurile au aceeasi radacina inseamna ca se afla in acelasi arbore, respectiv aceeasi multime
        if (radacinaX == radacinaY) {
            return 1;
        } else {
            return 0;
        }
    }
}

vector<int> Graf::dijkstra(vector<vector<pair<int, int>>> &matriceAdiacentaCosturi, int nodStart) {
    vector<int> vizitate(nrNoduri + 1, 0), dist(nrNoduri + 1, INT_MAX);
    // heap in care retinem perechi de tipul (distanta,nod), unde distanta = distanta de la nodStart la nod
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    dist[nodStart] = 0; // distanta de la nodStart la el insusi este 0
    pq.push({0, nodStart});
    while (pq.size() > 0) {
        int nod = pq.top().second;
        pq.pop();
        if (vizitate[nod] == 0) {
            vizitate[nod] = 1;
            // parcurgem nodurile adiacente nodului curent
            for (auto it = matriceAdiacentaCosturi[nod].begin(); it != matriceAdiacentaCosturi[nod].end(); it++) {
                int nodAdiacent = it->first;
                int distanta = it->second; // distanta de la nodul curent la nodul adiacent
                // verificam daca distanta de la nodul curent la nodStart + distanta de la nodul curent la nodul adiacent este minima, caz in care o adaugam in heap
                if (dist[nodAdiacent] > dist[nod] + distanta) {
                    dist[nodAdiacent] = dist[nod] + distanta;
                    pq.push({dist[nodAdiacent], nodAdiacent});
                }
            }
        }
    }
    return dist;
}

vector<int> Graf::bellman_ford(vector<vector<pair<int, int>>> &matriceAdiacentaCosturi, int nodStart) {
    vector<int> vizitate(nrNoduri + 1, 0), dist(nrNoduri + 1, INT_MAX), inCoada(nrNoduri, 0);
    queue<int> q;

    q.push(nodStart); // punem primul nod in coada
    inCoada[nodStart] = 1; // marcam faptul ca nodul se afla in coada
    dist[nodStart] = 0; // distanta de la nod la el insusii este 0

    while (!q.empty()) {
        // Scoatem primul nod din coada
        int nod = q.front();
        q.pop();
        inCoada[nod] = 0;

        // Parcurgem nodurile adiacente nodului curent
        for (auto it = matriceAdiacentaCosturi[nod].begin(); it != matriceAdiacentaCosturi[nod].end(); it++) {
            int nodAdiacent = it->first;
            int distanta = it->second;
            if (dist[nodAdiacent] > dist[nod] + distanta) {
                dist[nodAdiacent] = dist[nod] + distanta;

                vizitate[nodAdiacent]++;
                if (vizitate[nodAdiacent] >= nrNoduri) { // s-a format ciclu
                    dist.clear();
                    return dist;
                }

                // Daca nu este in coada il adaugam
                if (inCoada[nodAdiacent] == 0) {
                    q.push(nodAdiacent);
                    inCoada[nodStart] = 1;
                }
            }
        }
    }
    return dist;
}

/* -------------------------------------------------------------- */

void infoarena_bfs() {
    ifstream f("bfs.in");
    ofstream g("bfs.out");

    int N, M, S;
    f >> N >> M >> S;
    Graf graf(N, true);
    graf.citireGraf(f, M);
    vector<int> distanta = graf.distantaMinimaBFS(g, S);
    for (int i = 1; i <= N; i++) {
        g << distanta[i] << " ";
    }
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
    vector<vector<int>> result = graf.DFS_muchiiCritice();
    for (int i = 0; i < result.size(); i++) {
        cout << result[i][0] << " " << result[i][1] << '\n';
    }
}

void infoarena_sortareTopologica() {
    ifstream f("sortaret.in");
    ofstream g("sortaret.out");

    int N, M;
    f >> N >> M;
    Graf graf(N, true);
    graf.citireGraf(f, M);
    vector<int> result = graf.DFS_sortareTopologica(g);

    for (int i = result.size() - 1; i >= 0; i--) {
        g << result[i] << " ";
    }
}

void infoarena_disjoint() {
    ifstream f("disjoint.in");
    ofstream g("disjoint.out");

    int n, m;
    f >> n >> m;

    Graf graf;
    vector<int> parinte(n + 1), rang(n + 1, 1); // initial fiecare arbore are rangul 1, avand doar un nod
    for (int i = 1; i <= n; i++) {
        parinte[i] = i; // initial fiecare nod este radacina
    }

    for (int i = 0; i < m; i++) {
        int cod, x, y;
        f >> cod >> x >> y;
        int result = graf.disjoint(cod, x, y, parinte, rang);
        if (result == 1) {
            g << "DA" << '\n';
        } else if (result == 0) {
            g << "NU" << '\n';
        }
    }
}

void infoarena_dijkstra() {
    ifstream f("dijkstra.in");
    ofstream g("dijkstra.out");

    int n, m, nodStart = 1;
    f >> n >> m;
    vector<vector<pair<int, int>>> matriceAdiacentaCosturi(m); // matrice de adiacenta care retine si costul unei muchii
    int a, b, c;
    for (int i = 0; i < m; i++) {
        f >> a >> b >> c;
        matriceAdiacentaCosturi[a].push_back(make_pair(b, c));
    }
    Graf graf(n, true);
    vector<int> dist = graf.dijkstra(matriceAdiacentaCosturi, nodStart);
    for (int i = 1; i <= n; i++) {
        if (i != nodStart) {
            if (dist[i] != INT_MAX) {
                g << dist[i] << " ";
            } else {
                g << 0 << " ";
            }
        }
    }
}

void infoarena_bellman_ford() {
    ifstream f("sortaret.in");
    ofstream g("sortaret.out");

    int n, m, nodStart = 1;
    f >> n >> m;
    Graf graf(n, true);
    vector<vector<pair<int, int>>> matriceAdiacentaCosturi(m); // matrice de adiacenta care retine si costul unui drum
    int a, b, c;
    for (int i = 0; i < m; i++) {
        f >> a >> b >> c;
        matriceAdiacentaCosturi[a].push_back(make_pair(b, c));
    }
    vector<int> dist = graf.bellman_ford(matriceAdiacentaCosturi, nodStart);
    if (dist.empty()) {
        g << "Ciclu negativ!";
        return;
    }
    for (int i = 1; i <= n; i++) {
        if (i != nodStart) {
            if (dist[i] != INT_MAX) {
                g << dist[i] << " ";
            } else {
                g << 0 << " ";
            }
        }
    }
}

void infoarena_muzeu() {
    ifstream f("muzeu.in");
    ofstream g("muzeu.out");
    int n;
    f >> n;
    queue<pair<int, int>> queue;
    int matrice[n][n];
    int vizitate[n][n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            char x;
            f >> x;

            vizitate[i][j] = 0;
            if (x == '#') {
                matrice[i][j] = -2;
            } else if (x == 'P') {
                matrice[i][j] = 0;
                queue.push(make_pair(i, j)); // Initial punem in queue toti paznicii
            } else {
                matrice[i][j] = -1; // Initial marcam toate camere ca nevizitabile
            }
        }
    }

    // Parcurgem queue-ul de paznici
    // De la fiecare paznic merge in toate directiile(sus, jos, stanga, dreapta) si elementul vizitat il adaugam in queue
    while (!queue.empty()) {
        int i = queue.front().first, j = queue.front().second, linie, coloana;
        queue.pop();
        vizitate[i][j] = 1;

        // Sus - scadem 1 din linie
        linie = i - 1;
        coloana = j;
        if (linie >= 0 && vizitate[linie][coloana] == 0) {
            if (matrice[linie][coloana] != -2 && matrice[linie][coloana] != 0) {
                vizitate[linie][coloana] = 1;
                queue.push(make_pair(linie, coloana));
                matrice[linie][coloana] = matrice[i][j] + 1;
            }
        }

        // Jos - adunam 1 la linie
        linie = i + 1;
        coloana = j;
        if (linie < n && vizitate[linie][coloana] == 0) {
            if (matrice[linie][coloana] != -2 && matrice[linie][coloana] != 0) {
                vizitate[linie][coloana] = 1;
                queue.push(make_pair(linie, coloana));
                matrice[linie][coloana] = matrice[i][j] + 1;
            }
        }

        // Stanga - scadem 1 din coloana
        linie = i;
        coloana = j - 1;
        if (coloana >= 0 && vizitate[linie][coloana] == 0) {
            if (matrice[linie][coloana] != -2 && matrice[linie][coloana] != 0) {
                vizitate[linie][coloana] = 1;
                queue.push(make_pair(linie, coloana));
                matrice[linie][coloana] = matrice[i][j] + 1;
            }
        }

        // Dreapta - adunam 1 la coloana
        linie = i;
        coloana = j + 1;
        if (coloana < n && vizitate[linie][coloana] == 0) {
            if (matrice[linie][coloana] != -2 && matrice[linie][coloana] != 0) {
                vizitate[linie][coloana] = 1;
                queue.push(make_pair(linie, coloana));
                matrice[linie][coloana] = matrice[i][j] + 1;
            }
        }
    }

    // Afisare
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            g << matrice[i][j] << ' ';
        }
        g << '\n';
    }
}

/* -------------------------------------------------------------- */

int main() {
//    infoarena_bfs();
//    infoarena_dfs();
//    leetcode_CriticalConnections();
//    leetcode_CriticalConnections_neeficient();
//    infoarena_sortareTopologica();
//    infoarena_disjoint();
//    infoarena_dijkstra();
//    infoarena_bellman_ford();
//    infoarena_muzeu();
    return 0;
}