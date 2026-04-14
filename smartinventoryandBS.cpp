#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

class User {
protected:
    string username;
    string password;
public:
    User(string u, string p) : username(u), password(p) {}
    virtual void welcomeMessage() { cout << "Welcome, User!" << endl; }
    bool validate(string u, string p) { return (username == u && password == p); }
};

class Admin : public User {
public:
    Admin() : User("admin", "admin123") {}
    void welcomeMessage() override {
        cout << "\033[32mLogin Successful! Welcome, Administrator\033[0m" << endl;
    }
};

class Product {
private: 
    int id;
    string name;
    double price;
    int stock;

public:
    Product(int i, string n, double p, int s) : id(i), name(n), price(p), stock(s) {}
    int getId() { return id; }
    string getName() { return name; }
    double getPrice() { return price; }
    int getStock() { return stock; }
    void reduceStock(int qty) { stock -= qty; }

    string getFileData() {
        return to_string(id) + "," + name + "," + to_string(price) + "," + to_string(stock);
    }
};

class BillingSystem {
private:
    vector<Product> inventory;
    Admin adminUser;
    const string filename = "inventory.txt";

public:
    BillingSystem() {
        loadFromFile(); // Load data as soon as system starts
        if (inventory.empty()) {
    
            inventory.push_back(Product(101, "Laptop", 800.00, 10));
            inventory.push_back(Product(102, "Mouse", 20.00, 50));
        }
    }

    // --- NEW: SAVE TO FILE ---
    void saveToFile() {
        ofstream outFile(filename);
        if (outFile.is_open()) {
            for (auto &p : inventory) {
                outFile << p.getId() << endl;
                outFile << p.getName() << endl;
                outFile << p.getPrice() << endl;
                outFile << p.getStock() << endl;
            }
            outFile.close();
            cout << "\033[33mData autosaved successfully.\033[0m" << endl;
        }
    }

    // --- NEW: LOAD FROM FILE ---
    void loadFromFile() {
        ifstream inFile(filename);
        if (!inFile) return; // If file doesn't exist, just return

        int id, stock;
        string name;
        double price;

        while (inFile >> id) {
            inFile.ignore(); // clear newline
            getline(inFile, name);
            inFile >> price;
            inFile >> stock;
            inventory.push_back(Product(id, name, price, stock));
        }
        inFile.close();
    }

    bool runLogin() {
        string u, p;
        cout << "\n--- SYSTEM LOGIN ---\nUsername: "; cin >> u;
        cout << "Password: "; cin >> p;
        if (adminUser.validate(u, p)) {
            adminUser.welcomeMessage();
            return true;
        }
        return false;
    }

    void showInventory() {
        cout << "\033[34m\nID\tNAME\t\tPRICE\tSTOCK\033[0m" << endl;
        for (auto &p : inventory) {
            cout << p.getId() << "\t" << left << setw(15) << p.getName() 
                 << "\t" << p.getPrice() << "\t" << p.getStock() << endl;
        }
    }

    void addProduct() {
        string name; double pr; int st;
        int newId = 100 + inventory.size() + 1;
        cout << "Product Name: "; cin.ignore(); getline(cin, name);
        cout << "Price: "; cin >> pr;
        cout << "Initial Stock: "; cin >> st;

        inventory.push_back(Product(newId, name, pr, st));
        saveToFile(); 
    }

    void createBill() {
        double grandTotal = 0;
        while (true) {
            showInventory();
            int id, qty;
            cout << "\nEnter Product ID (0 to finish): "; cin >> id;
            if (id == 0) break;

            for (auto &p : inventory) {
                if (p.getId() == id) {
                    cout << "Quantity: "; cin >> qty;
                    if (qty <= p.getStock()) {
                        p.reduceStock(qty);
                        grandTotal += (qty * p.getPrice());
                    } else {
                        cout << "Insufficient Stock!" << endl;
                    }
                }
            }
        }
        cout << "\033[32m\nGRAND TOTAL: $" << grandTotal << "\033[0m" << endl;
        saveToFile(); // Save inventory after stock reduction
    }
};

int main() {
    BillingSystem system;
    if (!system.runLogin()) return 0;

    int choice;
    while (true) {
        cout << "\n1. View | 2. Add | 3. Bill | 4. Exit\nSelection: ";
        cin >> choice;
        if (choice == 1) system.showInventory();
        else if (choice == 2) system.addProduct();
        else if (choice == 3) system.createBill();
        else if (choice == 4) break;
    }
    return 0;
}