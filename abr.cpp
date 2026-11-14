#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>

// ============================================================
// 1. Structure du noeud
// ============================================================
struct Node {
    int   value;
    Node* left;
    Node* right;

    Node(int v) : value(v), left(nullptr), right(nullptr) {}
};

// ============================================================
// 2. Classe Arbre Binaire de Recherche (ABR)
// ============================================================
class BST {
public:
    BST() : root_(nullptr) {}
    ~BST() { destroy(root_); }

    // Interdire la copie (pour éviter le double-free)
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    // ----- Insertion -----
    void insert(int x) {
        root_ = insertImpl(root_, x);
    }

    // ----- Suppression -----
    void remove(int x) {
        root_ = removeImpl(root_, x);
    }

    // ----- Recherche -----
    bool contains(int x) const {
        return containsImpl(root_, x);
    }

    // ----- Affichage trié -----
    void displayInOrder() const {
        std::cout << "In-order (trie) : ";
        inOrder(root_);
        std::cout << "\n";
    }

    // ----- Affichage de l'arbre -----
    void display() const {
        std::cout << "Affichage vertical (Arborescence) :\n";
        if (root_) {
            printTree(root_, "", false);
        }
    }

private:
    Node* root_;

    // --- Insertion récursive ---
    static Node* insertImpl(Node* n, int x) {
        if (!n) return new Node(x);
        if (x < n->value)      n->left  = insertImpl(n->left,  x);
        else if (x > n->value) n->right = insertImpl(n->right, x);
        return n;
    }

    // --- Suppression récursive ---
    static Node* removeImpl(Node* n, int x) {
        if (!n) return nullptr;

        if (x < n->value) {
            n->left = removeImpl(n->left, x);
        } else if (x > n->value) {
            n->right = removeImpl(n->right, x);
        } else {
            if (!n->left) {
                Node* rightChild = n->right;
                delete n;
                return rightChild;
            }
            if (!n->right) {
                Node* leftChild = n->left;
                delete n;
                return leftChild;
            }

            Node* successor = n->right;
            while (successor->left) {
                successor = successor->left;
            }
            n->value = successor->value;
            n->right = removeImpl(n->right, successor->value);
        }

        return n;
    }

    // --- Recherche récursive ---
    static bool containsImpl(const Node* n, int x) {
        if (!n) return false;
        if (x == n->value) return true;
        return x < n->value
            ? containsImpl(n->left, x)
            : containsImpl(n->right, x);
    }

    // --- Parcours in-order ---
    static void inOrder(const Node* n) {
        if (!n) return;
        inOrder(n->left);
        std::cout << n->value << " ";
        inOrder(n->right);
    }

    // --- Affichage en arborescence ---
    static void printTree(const Node* n, const std::string& prefixe, bool estGauche) {
        if (!n) return;
        
        std::cout << prefixe;
        std::cout << (estGauche ? "|-- " : "\\-- ");
        std::cout << n->value << "\n";
        
        std::string newPrefixe = prefixe + (estGauche ? "|   " : "    ");
        
        printTree(n->left,  newPrefixe, true);
        printTree(n->right, newPrefixe, false);
    }

    // --- Libération de la mémoire ---
    static void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
};

// ============================================================
// 3. Comparaison avec std::map
// ============================================================
void compareWithMap() {
    const int nombreValeurs = 5000;
    std::map<int, bool> map;
    BST arbre;

    auto debut = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= nombreValeurs; ++i) {
        arbre.insert(i);
    }
    auto fin = std::chrono::high_resolution_clock::now();
    auto dureeArbreInsertion =
        std::chrono::duration_cast<std::chrono::nanoseconds>(fin - debut);

    debut = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= nombreValeurs; ++i) {
        map.emplace(i, true);
    }
    fin = std::chrono::high_resolution_clock::now();
    auto dureeMapInsertion =
        std::chrono::duration_cast<std::chrono::nanoseconds>(fin - debut);

    volatile int recherchesArbre = 0;
    debut = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= nombreValeurs; ++i) {
        recherchesArbre += arbre.contains(i) ? 1 : 0;
    }
    fin = std::chrono::high_resolution_clock::now();
    auto dureeArbreRecherche =
        std::chrono::duration_cast<std::chrono::nanoseconds>(fin - debut);

    volatile int recherchesMap = 0;
    debut = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= nombreValeurs; ++i) {
        recherchesMap += map.find(i) != map.end() ? 1 : 0;
    }
    fin = std::chrono::high_resolution_clock::now();
    auto dureeMapRecherche =
        std::chrono::duration_cast<std::chrono::nanoseconds>(fin - debut);

    debut = std::chrono::high_resolution_clock::now();
    for (int i = nombreValeurs; i >= 1; --i) {
        arbre.remove(i);
    }
    fin = std::chrono::high_resolution_clock::now();
    auto dureeArbreSuppression =
        std::chrono::duration_cast<std::chrono::nanoseconds>(fin - debut);

    debut = std::chrono::high_resolution_clock::now();
    for (int i = nombreValeurs; i >= 1; --i) {
        map.erase(i);
    }
    fin = std::chrono::high_resolution_clock::now();
    auto dureeMapSuppression =
        std::chrono::duration_cast<std::chrono::nanoseconds>(fin - debut);

    const double arbreInsertionMs = dureeArbreInsertion.count() / 1'000'000.0;
    const double mapInsertionMs = dureeMapInsertion.count() / 1'000'000.0;
    const double arbreRechercheMs = dureeArbreRecherche.count() / 1'000'000.0;
    const double mapRechercheMs = dureeMapRecherche.count() / 1'000'000.0;
    const double arbreSuppressionMs = dureeArbreSuppression.count() / 1'000'000.0;
    const double mapSuppressionMs = dureeMapSuppression.count() / 1'000'000.0;

    std::cout << "\n===== Benchmark ABR vs std::map =====\n";
    std::cout << "Nombre de valeurs : " << nombreValeurs << "\n\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "------------------------------------------------\n";
    std::cout << "| Operation   | ABR personnel (ms) | std::map (ms) |\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "| Insertion   | " << std::setw(18) << arbreInsertionMs
              << " | " << std::setw(12) << mapInsertionMs << " |\n";
    std::cout << "| Recherche   | " << std::setw(18) << arbreRechercheMs
              << " | " << std::setw(12) << mapRechercheMs << " |\n";
    std::cout << "| Suppression | " << std::setw(18) << arbreSuppressionMs
              << " | " << std::setw(12) << mapSuppressionMs << " |\n";
    std::cout << "------------------------------------------------\n";
}

// ============================================================
// 4. Programme principal (Tests)
// ============================================================
int main() {
    BST arbre;

    // Insertion des valeurs
    const int valeurs[] = {1,2,3,4,5,6,7,8,9,10};
    for (int v : valeurs) {
        arbre.insert(v);
    }

    arbre.displayInOrder();
    std::cout << "\n";

    std::cout << "Avant la suppression :\n";
    arbre.display();
    std::cout << "\n";

    arbre.remove(5);

    std::cout << "Apres la suppression du noeud 5 :\n";
    arbre.display();

    compareWithMap();

    return 0;
}