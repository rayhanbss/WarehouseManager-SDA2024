// Judul Program    : Aplikasi Management Karyawan dan Barang BALIBUL WAREHOUSE V.2
// Nama             : Rayhan Bagus Sadewa
// NIM              : L0123119
// Kelas            : D

// header
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <stack>
#include <queue>
#include <list>
#include <set>
#include <map>
#include <cfloat>
#include <cstdlib>
#include <utility>
#include <windows.h>
#include <algorithm> 
#include <unordered_map> 

// standart namespace
using namespace std;

//============================================================ KARYAWAN ============================================================
// Struktur data untuk merepresentasikan informasi karyawan
struct Data {
    string nama, jabatan;
    char lokasi;
};

// Struktur data untuk node dalam pohon yang merepresentasikan struktur hirarki karyawan
struct Node_TREE {
    string id;
    Data data;
    list<Node_TREE*> children;

    Node_TREE(string id, Data data) : id(id), data(data) {}
};

// Fungsi untuk mengonversi kode jabatan menjadi string jabatan
string scan_jabatan(string jabatan) {
    if (jabatan == "1")
        return "Kepala Gudang";
    else if (jabatan == "2")
        return "Security";
    else if (jabatan == "3")
        return "Staff";
    else
        return "Unknown";
}

// Fungsi untuk mengonversi kode lokasi menjadi karakter lokasi
char scan_lokasi(string lokasi) {
    if (lokasi == "A")
        return 'A';
    else if (lokasi == "B")
        return 'B';
    else if (lokasi == "C")
        return 'C';
    else if (lokasi == "D")
        return 'D';
    else
        return '-';
}

// Fungsi untuk membaca data karyawan dari file dan menyimpannya dalam map
map<string, Data> scan_data(set<string>& id) {
    map<string, Data> map_karyawan;
    ifstream fin("data_karyawan.txt");
    if (!fin) {
        return map_karyawan;
    }

    string baris;
   
    while (getline(fin, baris)) {
        // Validasi ID
        string id_karyawan = baris.substr(0, 5);
        if (id.find(id_karyawan) != id.end()) {
            continue; // Lanjut ke entri berikutnya
        }

        // Buat data baru
        Data new_data;
        new_data.nama = baris.substr(6);
        new_data.jabatan = scan_jabatan(baris.substr(1, 1));
        new_data.lokasi = scan_lokasi(baris.substr(0, 1));

        // Tambahkan data ke map
        map_karyawan[id_karyawan] = new_data;
        id.insert(id_karyawan);
    }
    fin.close();
    return map_karyawan;
}

// Struktur data untuk merepresentasikan Disjoint Set
struct DisjointSet {
    unordered_map<string, string> parent;
    unordered_map<string, int> rank;
    unordered_map<string, int> size;

    // Fungsi untuk membuat set baru
    void makeSet(const string& x) {
        if (parent.find(x) == parent.end()) {
            parent[x] = x;
            rank[x] = 0;
            size[x] = 1;
        }
    }

    // Fungsi untuk mencari root dari elemen x
    string find(const string& x) const {
        if (parent.at(x) != x) {
            const_cast<DisjointSet*>(this)->parent[x] = find(parent.at(x));
        }
        return parent.at(x);
    }

    // Fungsi untuk menggabungkan dua set
    void unionSets(const string& x, const string& y) {
        string rootX = find(x);
        string rootY = find(y);

        if (rootX != rootY) {
            if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
                size[rootY] += size[rootX];
            } else if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
                size[rootX] += size[rootY];
            } else {
                parent[rootY] = rootX;
                rank[rootX]++;
                size[rootX] += size[rootY];
            }
        }
    }

    // Fungsi untuk mendapatkan ukuran set dari elemen x
    int getSetSize(const string& x) const {
        return size.at(find(x));
    }
};

// Fungsi data Disjoint untuk mencari jumlah karyawan pada setiap grup jabatan
DisjointSet data_disjoint(map<string, Data>& data_karyawan) {
    DisjointSet ds;
    
    for (const auto& pair : data_karyawan) {
        const string& id = pair.first;
        const Data& data = pair.second;
        
        ds.makeSet(id);
        ds.makeSet(data.jabatan);
        ds.unionSets(id, data.jabatan);
    }

    return ds;
}

// Fungsi untuk mencetak jumlah karyawan
void print_jumlah_karyawan(const DisjointSet& ds, const map<string, Data>& data_karyawan) {
    unordered_map<string, int> positionCounts;

    for (const auto& pair : data_karyawan) {
        const string& jabatan = pair.second.jabatan;
        if (positionCounts.find(jabatan) == positionCounts.end()) {
            positionCounts[jabatan] = ds.getSetSize(jabatan) - 1; 
        }
    }

    for (const auto& pair : positionCounts) {
        cout << "| " << left << setw(20) << pair.first << ": " << setw(3) << right << pair.second << " karyawan" << setw(70) << "" << "|" << endl;
    }
}

// Fungsi untuk memasukkan node anak ke dalam pohon
void insertion_tree(Node_TREE* parent, Node_TREE* child) {
    parent->children.push_back(child);
}

// Fungsi untuk membuat node baru sesuai dengan jabatan dan lokasi tertentu
Node_TREE* create_node(Node_TREE* root_karyawan, map<string, Data>& map_karyawan, string jabatan, char lokasi){
    Node_TREE* new_root_karyawan = nullptr;
    for (const auto& pair : map_karyawan) {
            if(pair.second.jabatan == jabatan){
                if(pair.second.lokasi == lokasi){
                    new_root_karyawan = new Node_TREE(pair.first, pair.second);
                    insertion_tree(root_karyawan,new_root_karyawan);
                }
            }
        }
    return root_karyawan;
}

// Fungsi untuk membangun pohon karyawan berdasarkan data karyawan yang ada
Node_TREE* build_tree_karyawan(map<string, Data>& map_karyawan) {
    Data data_root = {"-","-",'-'};
    Node_TREE* root_karyawan = new Node_TREE("root", data_root);
    
    for(char lokasi = 'A'; lokasi <= 'D'; ++lokasi) {
        Node_TREE* child_kepala = create_node(root_karyawan, map_karyawan, "Kepala Gudang", lokasi);
        
        Node_TREE* child_security = create_node(root_karyawan,map_karyawan, "Security", lokasi);
        
        Node_TREE* child_staff = create_node(root_karyawan,map_karyawan, "Staff", lokasi);
        
    }
    
    return root_karyawan;
}

// Fungsi untuk menghapus seluruh pohon karyawan dari memori
void delete_tree(Node_TREE* root_karyawan) {
    if (root_karyawan == nullptr) return;
    for (Node_TREE* child : root_karyawan->children) {
        delete_tree(child);
    }
    delete root_karyawan;
}

// Fungsi untuk mencari data karyawan berdasarkan ID
Data* search_data(Node_TREE* root_karyawan, string search) {
    if (root_karyawan == nullptr || root_karyawan->id == search) {
        return (root_karyawan == nullptr) ? nullptr : &(root_karyawan->data);
    }

    for (Node_TREE* child : root_karyawan->children) {
        Data* foundData = search_data(child, search);
        if (foundData != nullptr) {
            return foundData;
        }
    }

    return nullptr;
}

// Fungsi untuk mencari karyawan berdasarkan ID
void search_karyawan(Node_TREE* root_karyawan) {
    system("cls");
    while(1){
        cout << "============================================== CARI KARYAWAN ===============================================" << endl;
        string id;
        cout << "> Masukkan ID karyawan yang ingin dicari (0 untuk kembali) : ";
        cin >> id;
        
        if (id.compare("0") == 0) break;
        Data* foundData = search_data(root_karyawan, id);
        if (foundData != nullptr) {
            system("cls");
            cout << "> [!] Data ditemukan :" << endl;
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Karyawan" << setw(39) << "| Jabatan" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            cout << left << setw(2) << "| " << setw(13) << id << setw(2) << "| " << setw(37) << foundData->nama << setw(2) << "| " << setw(37) << foundData->jabatan << setw(9) << "| Gudang " << setw(4) << foundData->lokasi << setw(1) << "|" << endl;
            cout << "===========================================================================================================" << endl;
            cout << endl;
        } else {
            system("cls");
            cout << "> [!] Data karyawan tidak ditemukan." << endl;
        }
    }
}

// Fungsi untuk mencetak semua karyawan berdasarkan lokasi
void print_tree_karyawan(Node_TREE* root_karyawan, char lokasi) {
    if (!root_karyawan) return;
    if(root_karyawan->data.lokasi == lokasi){
    cout << left << setw(2) << "| " << setw(13) << root_karyawan->id << setw(2) << "| " << setw(37) << root_karyawan->data.nama << setw(2) << "| " << setw(37) << root_karyawan->data.jabatan << setw(9) << "| Gudang " << setw(4) <<root_karyawan->data.lokasi << setw(1) << "|" << endl;
    }
    const list<Node_TREE*>& children = root_karyawan->children;
    for (auto child : children) {
        print_tree_karyawan(child, lokasi);
    }
}

// Fungsi untuk mencetak karyawan berdasarkan lokasi yang dipilih pengguna
void print_karyawan(Node_TREE* root_karyawan, const DisjointSet& disjointSet_karyawan, const map<string, Data>& map_karyawan, int lokasi = 0){
    system("cls");
    while (1){

        cout << "============================================== LIST KARYAWAN ==============================================" << endl;
        cout << "[1] Gedung A [2] Gedung B  [3] Gedung C  [4] Gedung D [5] Seluruh Karyawan  [6] Jumlah Karyawan [0] Kembali"  << endl << endl;
        cout << "> Masukan lokasi karyawan : ";
        cin >> lokasi;

        cout << endl;
        cout << left << setw(6) << "ID" << setw(20) << "Nama" << setw(20) << "Jabatan" << setw(8) << "Lokasi" << endl;
        switch (lokasi)
        {
        case 1:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Jabatan" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_tree_karyawan(root_karyawan, 'A');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 2:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Jabatan" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_tree_karyawan(root_karyawan, 'B');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 3:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama " << setw(39) << "| Jabatan" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_tree_karyawan(root_karyawan, 'C');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 4:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama " << setw(39) << "| Jabatan" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_tree_karyawan(root_karyawan, 'D');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 5:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama " << setw(39) << "| Jabatan" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            for(char lokasi = 'A'; lokasi <= 'D'; ++lokasi){
                print_tree_karyawan(root_karyawan, lokasi);
            }
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 6:
            system("cls");
            cout << "============================================= JUMLAH KARYAWAN =============================================" << endl;
            print_jumlah_karyawan(disjointSet_karyawan, map_karyawan);
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 0:
            return;
            break;
        default:
            system("cls");
            cout << "> [!] Input salah." << endl;
            break;
        }
    }
}

//========================================================= GRAPH LOKASI =========================================================

// Enum inisiasi kota yang ada 
enum city_graph {
    KOTA_A,
    KOTA_B,
    KOTA_C,
    KOTA_D,
    KOTA_E,
    JUMLAH_KOTA
};

// Struktur data Edge untuk graf
struct Edge {
    int tujuan;
    double bobot;
};

// Struktur data Edge untuk algoritma MST
struct MSTEdge {
    int kota1, kota2;
    double bobot;
};

// Deklarasi graf
vector<vector<Edge>> graph(JUMLAH_KOTA);

// Inisiasi elemen graf dan bobotnya
void graph_initiation(){
    graph[KOTA_A].push_back({KOTA_B, 12.42});
    graph[KOTA_B].push_back({KOTA_A, 12.42});
    graph[KOTA_A].push_back({KOTA_C, INFINITE});
    graph[KOTA_C].push_back({KOTA_A, INFINITE});
    graph[KOTA_A].push_back({KOTA_D, INFINITE});
    graph[KOTA_D].push_back({KOTA_A, INFINITE});
    graph[KOTA_A].push_back({KOTA_E, 6.78});
    graph[KOTA_E].push_back({KOTA_A, 6.78});
    graph[KOTA_B].push_back({KOTA_C, 13.19});
    graph[KOTA_C].push_back({KOTA_B, 13.19});
    graph[KOTA_B].push_back({KOTA_D, 9.63});
    graph[KOTA_D].push_back({KOTA_B, 9.63});
    graph[KOTA_B].push_back({KOTA_E, 11.98});
    graph[KOTA_E].push_back({KOTA_B, 11.98});
    graph[KOTA_C].push_back({KOTA_D, 18.21});
    graph[KOTA_D].push_back({KOTA_C, 18.21});
    graph[KOTA_C].push_back({KOTA_E, INFINITE});
    graph[KOTA_E].push_back({KOTA_C, INFINITE});
    graph[KOTA_D].push_back({KOTA_E, 20.14});
    graph[KOTA_E].push_back({KOTA_D, 20.14});
}

// Algoritma Djikstra untuk menjari rute terpendek pada graf 
vector<double> dijkstra(vector<vector<Edge>>& graph, int awal, int jumlah_kota = JUMLAH_KOTA) {
    vector<double> jarak(jumlah_kota, DBL_MAX);
    jarak[awal] = 0;

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push(make_pair(0, awal));

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        for (const Edge& edge : graph[u]) {
            int v = edge.tujuan;
            double bobot = edge.bobot;

            if (jarak[u] + bobot < jarak[v]) {
                jarak[v] = jarak[u] + bobot;
                pq.push(make_pair(jarak[v], v));
            }
        }
    }
    return jarak;
}

// Algoritma Prim untuk mencari minimum spanning tree untuk mencari jalur distribusi yang menghubungkan setiap kota
vector<MSTEdge> primMST(const vector<vector<Edge>>& graph) {
    vector<bool> inMST(JUMLAH_KOTA, false);
    vector<double> key(JUMLAH_KOTA, INFINITE);
    vector<int> parent(JUMLAH_KOTA, -1);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({0, KOTA_A});
    key[KOTA_A] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        inMST[u] = true;

        for (const Edge& edge : graph[u]) {
            int v = edge.tujuan;
            double weight = edge.bobot;

            if (!inMST[v] && key[v] > weight) {
                key[v] = weight;
                pq.push({key[v], v});
                parent[v] = u;
            }
        }
    }

    vector<MSTEdge> mst;
    for (int i = 1; i < JUMLAH_KOTA; ++i) {
        if (parent[i] != -1) {
            mst.push_back({parent[i], i, key[i]});
        }
    }
    return mst;
}

// Fungsi Untuk mencetak MST
void printMST(const vector<MSTEdge>& mst) {
    cout << "============================================ JALUR  DISTRIBUSI ============================================" << endl;
    cout << left << setw(35) << "| Kota" << setw(35) << "| Kota" << setw(35) << "| Jarak " << " |" << endl;
    for (const MSTEdge& edge : mst) {
        cout << "| " << left << setw(33) << static_cast<char>(edge.kota1 + 'A') << "| " << setw(33) << static_cast<char>(edge.kota2 + 'A') << "| " << setw(30) << edge.bobot << " KM" << " |" << endl;
    }
    cout << "===========================================================================================================" << endl;
}

// Fungsi untuk menginisiasi dan mencetak MST
void initiate_and_print_MST() {
    vector<MSTEdge> mst = primMST(graph);
    printMST(mst);
}

//============================================================ BARANG ============================================================
// Struktur data untuk merepresentasikan informasi barang

// Struktur data product untuk data barang 
struct Product
{
    string id, nama;
    char lokasi;
    int stock;
};

// Struktur data untuk node dalam pohon yang merepresentasikan struktur data BST (Binary Search Tree)
struct Node_BST {
    Product barang;
    Node_BST* left;
    Node_BST* right;

    Node_BST(Product barang) : barang(barang), left(nullptr), right(nullptr) {}
};

// Struktur data untuk permintaan pengiriman atau restock
struct Order {
    stack<Product*> stack_product;
    int home, destination;
};

// Fungsi selection sort untuk mengurutkan data yang tak terurut pada scan product
void selectionSort(list<Product>& list_barang) {
    for (auto it = list_barang.begin(); it != list_barang.end(); ++it) {
        auto min_it = it;
        for (auto jt = next(it); jt != list_barang.end(); ++jt) {
            if (jt->id < min_it->id) {
                min_it = jt;
            }
        }
        if (min_it != it) {
            swap(*it, *min_it);
        }
    }
}

// Fungsi untuk membaca data barang dari file dan menyimpannya dalam list
list<Product> scan_product(set<string>& id){
    list<Product> list_barang;
    ifstream fin("data_barang.txt");
    if (!fin) {
        return list_barang;
    }

    string baris;
   
    while (getline(fin, baris)) {
        // Validasi ID
        string id_barang = baris.substr(0, 4);
        if (id.find(id_barang) != id.end()) {
            continue; // Lanjut ke entri berikutnya
        }

        // Buat data baru
        Product new_product;
        new_product.id = id_barang;
        new_product.stock = stoi(baris.substr(5,8));
        new_product.nama = baris.substr(9);
        new_product.lokasi = scan_lokasi(baris.substr(0, 1));

        // Tambahkan data ke list
        list_barang.push_back(new_product);
        id.insert(id_barang);
    }
    fin.close();

    // Urutkan list menggunakan selection sort
    selectionSort(list_barang);

    return list_barang;
};
 
// Fungsi untuk menyisipkan node ke dalam pohon BST
void insertion(Node_BST*& root_barang, Product barang) {
    if (root_barang == nullptr) {
        root_barang = new Node_BST(barang);
    } else {
        if (barang.id < root_barang->barang.id) {
            insertion(root_barang->left, barang);
        } else {
            insertion(root_barang->right, barang);
        }
    }
}

// Fungsi untuk mencari data barang berdasarkan ID
Product* search_product(Node_BST* root_barang, string search) {
    if (root_barang == nullptr || root_barang->barang.id == search) {
        return (root_barang == nullptr) ? nullptr : &(root_barang->barang);
    }

    if (search < root_barang->barang.id) {
        return search_product(root_barang->left, search);
    } else {
        return search_product(root_barang->right, search);
    }
}

// Fungsi untuk mencari barang berdasarkan ID
void search_bst(Node_BST* root_barang) {
    system("cls");
    while(1){
        string id;
        cout << "=============================================== CARI BARANG ================================================" << endl;
        cout << "> Masukkan ID barang yang ingin dicari (0 utuk kembali) : ";
        cin >> id;

        if(id.compare("0") == 0) break;

        Product* found_product = search_product(root_barang, id);
        if (found_product != nullptr) {
            system("cls");
            cout << "> [!] Data ditemukan :" << endl;
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            cout << left << setw(2) << "| " << setw(13) << found_product->id << setw(2) << "| " << setw(37) << found_product->nama << setw(2) << "| " << setw(37) << found_product->stock << setw(9) << "| Gudang " << setw(4) << found_product->lokasi << setw(1) << "|" << endl;
            cout << "===========================================================================================================" << endl;
            cout << endl;
        } else {
            system("cls");
            cout << "> [!] Barang tidak ditemukan." << endl;
        }
    }
}

// Fungsi untuk membangun pohon BST berdasarkan data barang yang ada
Node_BST* build_bst(list<Product>& list_barang) {
    Node_BST* root_barang = nullptr;
    for (const auto& pair : list_barang) {
        insertion(root_barang, pair);
    }
    return root_barang;
}

// Fungsi untuk menghapus seluruh pohon BST dari memori
void delete_bst(Node_BST*& root_barang) {
    if (root_barang == nullptr) return;
    delete_bst(root_barang->left);
    delete_bst(root_barang->right);
    delete root_barang;
    root_barang = nullptr;
}

// Fungsi untuk mencetak daftar barang berdasarkan lokasi
void print_list_barang(Node_BST* root_barang, char lokasi) {
    if (root_barang == nullptr) {
        return; 
    }
    if (root_barang->barang.lokasi == lokasi) {
        cout << left << setw(2) << "| " << setw(13) << root_barang->barang.id << setw(2) << "| " << setw(37) << root_barang->barang.nama << setw(2) << "| " << setw(37) << root_barang->barang.stock << setw(9) << "| Gudang " << setw(4) << root_barang->barang.lokasi << setw(1) << "|" << endl;
    }

    print_list_barang(root_barang->left, lokasi);
    print_list_barang(root_barang->right, lokasi);
}

// Fungsi untuk mencetak barang berdasarkan lokasi yang dipilih pengguna
void print_barang(Node_BST* root_barang, int lokasi = 0){
    system("cls");
    while (1){
        cout << "=============================================== LIST BARANG ===============================================" << endl;
        cout << "[1] Gudang A      [2] Gudang B      [3] Gudang C      [4] Gudang D      [5] Seluruh Produk      [0] Kembali" << endl << endl;
        cout << "> Masukan lokasi gudang : ";
        cin >> lokasi;

        cout << endl;
        
        switch (lokasi)
        {
        case 1:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_list_barang(root_barang, 'A');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 2:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_list_barang(root_barang, 'B');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 3:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_list_barang(root_barang, 'C');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 4:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            print_list_barang(root_barang, 'D');
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 5:
            system("cls");
            cout << "===========================================================================================================" << endl;
            cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
            for(int i = 0; i < 4; i++){
                print_list_barang(root_barang, 'A' + i);
            }
            cout << "===========================================================================================================" << endl;
            cout << endl;
            break;
        case 0:
            return;
        default:
            system("cls");
            cout << "> [!] Input salah." << endl << endl;
            break;
        }
    }
}

// Fungsi untuk membuat stack barang berdasarkan barang yang dipilih pengguna
void create_stack(Node_BST* root_barang, stack<Product*>& stack_barang, int in_out) {
    string req;
    int stock = 0;

    while (true) {
        cout << "> Masukkan ID barang (0 untuk jika selesai) : ";
        cin >> req;
        if (req == "0") break;

        Product* barang = search_product(root_barang, req);
        if (!barang) {
            cout << "> [!] Barang tidak ditemukan. Silakan coba lagi." << endl;
            continue;
        }

        Product* new_barang = new Product(*barang);
        
        while (true) {
            cout << "> Masukkan jumlah barang : ";
            cin >> stock;
            if(in_out ==1){
                if (stock > 0 && stock < 1000) break;
                cout << "> [!] Input jumlah barang invalid (0 < jumlah < 999)." << endl;
            }else{
                if (stock > 0 && stock < barang->stock) break;
                cout << "> [!] Input jumlah barang invalid (0 < jumlah < " << barang->stock << ")." << endl;
            }
            
        }

        stack_barang.push(new_barang);
        stack_barang.top()->stock = stock;
        stock = 0;
        cout << "> [!] Barang berhasil ditambahkan." << endl;
    }
}

// Fungsi untuk mencetak antrian barang
void print_queue(const queue<Order>& queue_barang) {
    int batch = 1;
    queue<Order> temp_queue = queue_barang;  // Create a copy to iterate

    while (!temp_queue.empty()) {
        const Order& order = temp_queue.front();
        
        cout << "================================================ BATCH " << right << setw(2) << batch++ << " =================================================" << endl;
        cout << left << setw(18) << "| ID" << setw(45) << "| Nama Produk" << setw(43) << "| Stok" << setw(1) << "|" << endl;
        cout << left << setw(18) << "|-----------------" << setw(45) << "|--------------------------------------------" << setw(43) << "|------------------------------------------" << setw(1) << "|" << endl;

        stack<Product*> temp_stack = order.stack_product;  // Create a copy to iterate
        while (!temp_stack.empty()) {
            Product* barang = temp_stack.top();
            temp_stack.pop();
            cout << left << setw(2) << "| "  << setw(16) << barang->id  << setw(2) << "| "  << setw(43) << barang->nama << setw(2) << "| "  << setw(41) << barang->stock << setw(1)  << "|" << endl;
        }

        // Calculate distance using dijkstra
        vector<double> distances = dijkstra(graph, order.home, JUMLAH_KOTA);
        double distance = distances[order.destination];

        cout << "============================================================================= Jarak Pengiriman : " << right << setw(4) << fixed << setprecision(2) << distance << " KM =" << endl;
        
        temp_queue.pop();
    }
}

// Fungsi untuk mengubah data barang dalam txt sesuai dengan data yang telah diubah
void change_txt(Product* barang){
    ifstream fin("data_barang.txt");
    if (!fin) {
        cerr << "Gagal membuka file!" << endl;
        return;
    }
    
    string idToUpdate = barang->id;
    string newStock = to_string(barang->stock);

    vector<string> lines;
    string line;
    while (getline(fin, line)) {
        if (line.find(idToUpdate) != string::npos) {
            size_t pos = line.find(" ");
            string temp = string(3 - newStock.length(), '0') + newStock;
            if (pos != string::npos) {
                line.replace(pos + 1, 3,  temp);
            }
        }
        lines.push_back(line);
    }
    fin.close();

    ofstream fout("data_barang.txt");
    if (!fout) {
        cerr << "Gagal membuka file untuk penulisan!" << endl;
        return;
    }

    for (const string& l : lines) {
        fout << l << endl;
    }
    fout.close();
}

// Fungsi untuk menambahkan stok barang
void restock_barang(Node_BST* root_barang, queue<Order>& queue_in) {
    system("cls");
    cout << "===========================================================================================================" << endl;
    cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
    for(int i = 0; i < 4; i++){
        print_list_barang(root_barang, 'A' + i);
    }
    cout << "===========================================================================================================" << endl;
    cout << endl;

    int source_city;
    cout << "============================================= PILIH KOTA ASAL =============================================" << endl;
    cout << "[0] Kota A               [1] Kota B              [2] Kota C              [3] Kota D              [4] Kota E" << endl;
    cout << "> Masukan Pilihan Anda : ";
    cin >> source_city;

    stack<Product*> new_stack;
    create_stack(root_barang, new_stack, 1);
    
    Order new_order;
    new_order.home = source_city;
    new_order.destination = 0;
    new_order.stack_product = new_stack;

    queue_in.push(new_order);
}

// Fungsi untuk mengeluarkan barang
void kirim_barang(Node_BST* root_barang, queue<Order>& queue_out) {
    system("cls");
    cout << "===========================================================================================================" << endl;
    cout << left << setw(15) << "| ID" << setw(39) << "| Nama Produk" << setw(39) << "| Stok" << setw(13) << "| Lokasi" << setw(1) << "|" << endl;
    for(int i = 0; i < 4; i++){
        print_list_barang(root_barang, 'A' + i);
    }
    cout << "===========================================================================================================" << endl;
    cout << endl;

    int dest_city;
    cout << "============================================ PILIH KOTA TUJUAN ============================================" << endl;
    cout << "[1] Kota B                      [2] Kota C                       [3] Kota D                      [4] Kota E" << endl;
    cout << "> Masukan Pilihan Anda : ";
    cin >> dest_city;

    vector<double> distances = dijkstra(graph, KOTA_A);
    cout << "Jarak terpendek dari Gudang (Kota A) ke Kota " << (char)('A' + dest_city) << ": " << fixed << setprecision(2) << distances[dest_city] << " km" << endl;

    stack<Product*> new_stack;
    create_stack(root_barang, new_stack, 1);
    
    Order new_order;
    new_order.home = 0;
    new_order.destination = dest_city;
    new_order.stack_product = new_stack;

    queue_out.push(new_order);
}

// Fungsi untuk menambahkan atau mengeluarkan barang setela dikonfirmasi
void in_out_product(Node_BST*& root_barang, queue<Order>& queue_barang, int in_out) {
    if(queue_barang.empty()) return;

    Order order = queue_barang.front();
    queue_barang.pop();

    stack<Product*>& stack_barang = order.stack_product;

    while (!stack_barang.empty()) {
        Product* new_product = stack_barang.top();
        stack_barang.pop();

        Product* existing_product = search_product(root_barang, new_product->id);
        if (existing_product) {
            if (in_out == 1){
                existing_product->stock += new_product->stock;
                change_txt(existing_product);
                cout << "> [!] Stok produk " << new_product->nama << " berhasil ditambahkan." << endl;
            } else {
                if (existing_product->stock >= new_product->stock) {
                    existing_product->stock -= new_product->stock;
                    change_txt(existing_product);
                    cout << "> [!] Stok produk " << new_product->nama << " berhasil dikeluarkan." << endl;
                } else {
                    cout << "> [!] Stok produk " << new_product->nama << " tidak mencukupi." << endl;
                }
            }
        } else {
            cout << "> [!] Produk " << new_product->id << " tidak ditemukan." << endl;
        }
        delete new_product;
    }
}

// Fungsi untuk mengonfirmasi barang masuk atau keluar dari antrian
void confirm_in_out(Node_BST*& root_barang, queue<Order>& queue_barang, int in_out){
    int input;
    system("cls");
    while(1){
        print_queue(queue_barang);
        cout << "======================================== KONFIRMASI ANTRIAN BARANG ========================================" << endl;
        cout << "[1] Konfirmasi antrian batch pertama             [2] Konfirmasi semua antrian batch             [0] Kembali" << endl << endl;
        cout << "> Masukan pilihan anda : ";
        cin >> input;

        switch(input){
            case 1: 
                system("cls");
                if(queue_barang.empty()) break;
                in_out_product(root_barang, queue_barang, in_out);
                break;
            case 2:
                system("cls");
                if(queue_barang.empty()) break;
                while(!queue_barang.empty()){
                    in_out_product(root_barang, queue_barang, in_out);
                }
                break;
            case 0:
                return;
            default:
                system("cls");
                cout << "> [!] Input salah." << endl;
            break;
        }
    }
}

// Fungsi untuk memindahkan barang masuk atau keluar dari antrian
void movement_product(Node_BST*& root_barang, queue<Order>& queue_in, queue<Order>& queue_out) {
    while (1) {
        system("cls");
        int input = 0;
        cout << "============================================= MOVEMENT BARANG =============================================" << endl;
        cout << "[1] Konfirmasi antrian barang masuk            [2] Konfirmasi antrian barang keluar             [0] Kembali" << endl << endl;
        cout << "> Masukan pilihan anda : ";
        cin >> input;

        switch (input) {
        case 1:
            if(!queue_in.empty()){
                confirm_in_out(root_barang, queue_in, 1);
            }else{
                cout << "> [!] Antrian barang kosong." << endl;
            }
            break;
        case 2:
            if(!queue_out.empty()){
                confirm_in_out(root_barang, queue_out, 2);
            }else{
                cout << "> [!] Antrian barang kosong." << endl;
            }
            break;
        case 0:
            return;
        default:
            cout << "> [!] Input salah." << endl;
            break;
        }
    }
}

// Fungsi untuk mengelola barang berdasarkan pilihan pengguna
void manage_product(Node_BST* root_barang, queue<Order>& queue_in, queue<Order>& queue_out) {
    int input = 0;
    system("cls");
    while (1) {
        cout << "============================================== MANAGE BARANG ==============================================" << endl;
        cout << "[1] Restock Barang    [2] Kirim Barang    [3] Check Movement Barang    [4] Jalur Distribusi     [0] Kembali" << endl << endl;
        cout << "> Masukan pilihan anda : ";
        cin >> input;

        cout << endl;
        switch (input) {
        case 1:
            restock_barang(root_barang, queue_in);
            system("cls");
            break;
        case 2:
            kirim_barang(root_barang, queue_out);
            system("cls");
            break;
        case 3:
            movement_product(root_barang, queue_in, queue_out);
            system("cls");
            break;
        case 4:
            system("cls");
            initiate_and_print_MST();
            break;
        case 0:
            return;
        default:
            cout << "> [!] Input salah." << endl;
            break;
        }
    }
}

// Fungsi untuk membersihkan antrian barang
void clear_queue(queue<Order>& queue_barang) {
    while (!queue_barang.empty()) {
        Order& order = queue_barang.front();
        while (!order.stack_product.empty()) {
            delete order.stack_product.top();
            order.stack_product.pop();
        }
        queue_barang.pop();
    }
}

// Fungsi utama program
void home(){
    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);

    set<string> id_karyawan;
    set<string> id_barang;

    map<string, Data> map_karyawan = scan_data(id_karyawan);
    list<Product> list_barang = scan_product(id_barang);

    DisjointSet disjointSet_karyawan = data_disjoint(map_karyawan);

    Node_TREE* root_karyawan = build_tree_karyawan(map_karyawan);
    Node_BST* root_barang = build_bst(list_barang);

    graph_initiation();

    queue<Order> queue_in, queue_out;

    while(1){
        system("cls");
        int menu;
        cout << "===========================================================================================================" << endl;
        cout << "     ____  ___    __    ________  __  ____     _       _____    ____  ________  ______  __  _______ ______ " << endl
             << "    / __ )/   |  / /   /  _/ __ )/ / / / /    | |     / /   |  / __ \\/ ____/ / / / __ \\/ / / / ___// ____/ " << endl
             << "   / __  / /| | / /    / // __  / / / / /     | | /| / / /| | / /_/ / __/ / /_/ / / / / / / /\\__ \\/ __/    " << endl
             << "  / /_/ / ___ |/ /____/ // /_/ / /_/ / /___   | |/ |/ / ___ |/ _, _/ /___/ __  / /_/ / /_/ /___/ / /___    " << endl
             << " /_____/_/  |_/_____/___/_____/\\____/_____/   |__/|__/_/  |_/_/ |_/_____/_/ /_/\\____/\\____//____/_____/    " << endl << endl;
        cout << "===========================================================================================================" << endl;                                                                                                   

        cout << "[1] List Karyawan   [2] Cari Karyawan   [3] List Barang    [4] Cari Barang   [5] Kelola Barang   [0] Keluar" << endl << endl;
        cout << "> Masukan pilihan anda : ";
        cin >> menu;

        switch (menu)
        {
        case 1:
            print_karyawan(root_karyawan, disjointSet_karyawan, map_karyawan);

            break;
        case 2:
            search_karyawan(root_karyawan);
            break;
        case 3:
            print_barang(root_barang);
            break;
        case 4:
            search_bst(root_barang);
            break;
        case 5:
            manage_product(root_barang, queue_in, queue_out);
            break;
        case 0:
            delete_tree(root_karyawan);
            delete_bst(root_barang);
            clear_queue(queue_in);
            clear_queue(queue_out);
            exit(0);
        default:
            cout << "> [!] Input salah." << endl;
            break;
        }
    }
}

// Fungsi utama
int main(){
    home();
    return 0;
}