
#include <iostream>
#include <limits>
#include <string>
#include <fstream>

using namespace std;

const double TXRATE = 0.07;
const string USER = "admin";
const string PASS = "pass123";



void printpadded(const string& text, int width) {
    cout << text;
    for (int i = (int)text.length(); i < width; i++) {
        cout << " ";
    }
}

void printprice(double amount, int width) {
    int pkr = static_cast<int>(amount);
    int cents = static_cast<int>((amount - pkr) * 100 + 0.5);
    string amountStr = to_string(pkr) + ".";
    if (cents < 10) {
        amountStr += "0";
    }
    amountStr += to_string(cents);
    printpadded("Pkr" + amountStr, width);
}

class Product {
    int id;
    string name;
    double price;
    int stock;

public:
    Product() : id(0), name(""), price(0), stock(0) {}
    Product(int id, string name, double price, int stock)
        : id(id), name(name), price(price), stock(stock) {}

    void display() const {
        printpadded(to_string(id), 6);
        printpadded(name, 25);
        printprice(price, 10);
        cout << "  Available: " << stock << endl;
    }

    bool canorder(int quantity) const {
        return stock >= quantity;
    }
    void reducestock(int quantity) {
        stock -= quantity;
    }
    void increasestock(int quantity) {
        stock += quantity;
    }
    void setstock(int newStock) {
        stock = newStock;
    }
    int getID() const {
        return id;
    }
    double getprice() const {
        return price;
    }
    int getstock() const {
        return stock;
    }
    string getname() const {
        return name;
    }
};

class Inventory {
    Product* products;
    int productCount;
    int productCapacity;

public:
    Inventory() {
        productCapacity = 10;
        productCount = 0;
        products = new Product[productCapacity];
    }
    ~Inventory() {
        delete[] products;
    }

    void addproduct(const Product& product) {
        if (productCount == productCapacity) {
            int newCapacity = productCapacity * 2;
            Product* newArr = new Product[newCapacity];
            for (int i = 0; i < productCount; ++i)
                newArr[i] = products[i];
            delete[] products;
            products = newArr;
            productCapacity = newCapacity;
        }
        products[productCount++] = product;
    }

    void showmenu() const {
        cout << "\n\t\t Today's Menu \n";
        cout << " ID   Dish Name             Price           Available\n";
        cout << " -----------------------------------------------------\n";
        for (int i = 0; i < productCount; ++i)
            products[i].display();
    }

    Product* findproduct(int id) {
        for (int i = 0; i < productCount; ++i)
            if (products[i].getID() == id)
                return &products[i];
        return nullptr;
    }

    void loadfile(const string& filename) {
        ifstream file(filename);
        if (!file)
            return;

        productCount = 0;
        int id, stock;
        double price;
        string name;

        while (file >> id) {
            file.ignore();
            getline(file, name);
            file >> price >> stock;
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            addproduct(Product(id, name, price, stock));
        }
    }

    void savefile(const string& filename) const {
        ofstream file(filename);
        for (int i = 0; i < productCount; ++i) {
            file << products[i].getID() << '\n' << products[i].getname() << '\n' << products[i].getprice() << ' ' << products[i].getstock() << "\n\n";
        }
    }
};

class Orderitem {
    Product* product;
    int quantity;

public:
    Orderitem() : product(nullptr), quantity(0) {}
    Orderitem(Product* p, int qty) : product(p), quantity(qty) {}

    double gettotal() const {
        return product->getprice() * quantity;
    }
    void print() const {
        printpadded(product->getname(), 25);
        cout << "x" << quantity;
        cout << "  Total: ";
        printprice(gettotal(), 15);
        cout << endl;
    }
    Product* getproduct() const {
        return product;
    }
    int getquantity() const {
        return quantity;
    }
    void increasequantity(int qty) {
        quantity += qty;
    }
};

class Cart {
    Orderitem* items;
    int itemCount;
    int itemCapacity;
    static int orderCounter;

public:
    Cart() {
        itemCapacity = 10;
        itemCount = 0;
        items = new Orderitem[itemCapacity];
    }
    ~Cart() {
        delete[] items;
    }

    void additem(Product* product, int quantity) {
        for (int i = 0; i < itemCount; ++i) {
            if (items[i].getproduct()->getID() == product->getID()) {
                items[i].increasequantity(quantity);
                product->reducestock(quantity);
                return;
            }
        }
        if (itemCount == itemCapacity) {
            int newCapacity = itemCapacity * 2;
            Orderitem* newArr = new Orderitem[newCapacity];
            for (int i = 0; i < itemCount; ++i)
                newArr[i] = items[i];
            delete[] items;
            items = newArr;
            itemCapacity = newCapacity;
        }
        items[itemCount++] = Orderitem(product, quantity);
        product->reducestock(quantity);
    }

    void printreceipt() const {
        cout << "\n\t\t Final Bill\n";
        double subtotal = 0;

        for (int i = 0; i < itemCount; ++i) {
            items[i].print();
            subtotal += items[i].gettotal();
        }

        double tax = subtotal * TXRATE;
        cout << "\nSubtotal: ";
        printprice(subtotal, 25);
        cout << "\nTax (7%): ";
        printprice(tax, 25);
        cout << "\nTotal:    ";
        printprice(subtotal + tax, 25);
        cout << "\n==============================\n";
    }

    void savereceipt() const {
        ofstream file("receipt_" + to_string(orderCounter++) + ".txt");
        file << "\t\t Order Receipt\n";
        for (int i = 0; i < itemCount; ++i) {
            file << items[i].gettotal() << " - "
                << items[i].getproduct()->getname()
                << " x" << items[i].getquantity() << '\n';
        }
    }

    bool empty() const { return itemCount == 0; }
    void clear() { itemCount = 0; }

    void listitems() const {
        if (itemCount == 0) {
            cout << "Your order is empty.\n";
            return;
        }
        cout << "\nYour current order:\n";
        cout << "No.  Dish                     Quantity\n";
        cout << "--------------------------------------\n";
        for (int i = 0; i < itemCount; ++i) {
            cout << (i + 1) << ".   ";
            printpadded(items[i].getproduct()->getname(), 25);
            cout << items[i].getquantity() << endl;
        }
    }
    double gettotal() const {
        double subtotal = 0;
        for (int i = 0; i < itemCount; ++i) {
            subtotal += items[i].gettotal();
        }
        double tax = subtotal * TXRATE;
        return subtotal + tax;
    }

    bool removeitem(size_t index) {
        if (index >= 1 && index <= (size_t)itemCount) {
            Product* p = items[index - 1].getproduct();
            int qty = items[index - 1].getquantity();
            p->increasestock(qty);
            for (int i = index - 1; i < itemCount - 1; ++i) {
                items[i] = items[i + 1];
            }
            --itemCount;
            return true;
        }
        return false;
    }
};
// --- Payment Base Class ---
class Payment {
public:
    virtual void pay(double amount) = 0;
    virtual ~Payment() {}
};

// --- Cash Payment Derived Class ---
class CashPayment : public Payment {
public:
    void pay(double amount) override {
        cout << "Please pay PKR " << amount << " in cash.\n";
        cout << "Payment received. Thank you!\n";
    }
};

// --- Card Payment Derived Class ---
class CardPayment : public Payment {
public:
    void pay(double amount) override {
        string cardNumber;
        cout << "Enter Card Number: ";
        cin >> cardNumber;
        cout << "Processing payment of PKR " << amount << " from card " << cardNumber << "...\n";
        cout << "Payment successful! Thank you!\n";
    }
};

int Cart::orderCounter = 1001;

class POSSystem {
    Inventory menu;
    Cart cart;

    bool admin() {
        int attempts = 3;
        while (attempts > 0) {
            cout << "\nAdmin Username ('q' to cancel): ";
            string username;
            cin >> username;

            if (username == "q") return false;

            cout << "Admin Password: ";
            string password;
            cin >> password;
            if (username == USER && password == PASS) {
                cout << "Access granted.\n";
                return true;
            }
            cout << "Invalid credentials. Attempts left: " << --attempts << endl;
        }
        cout << "Access denied.\n";
        return false;
    }

    void adminmenu() {
        while (true) {
            cout << "\n--- Management Console ---\n";
            cout << "1. Add New Dish\n";
            cout << "2. Update Stock\n";
            cout << "3. Return\n";
            cout << "Choose option: ";
            int choice;
            cin >> choice;
            if (choice == 1) {
                int id;
                string name;
                double price;
                int stock;
                cout << "New Dish ID: ";
                cin >> id;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer before getline

                cout << "Dish Name: ";
                getline(cin, name);

                cout << "Price: ";
                cin >> price;
                cout << "Initial Stock: ";
                cin >> stock;
                menu.addproduct(Product(id, name, price, stock));
                menu.savefile("menu.txt");
                cout << "Dish added successfully.\n";
            }
            else if (choice == 2) {
                cout << "Enter Dish ID: ";
                int id;
                cin >> id;
                Product* p = menu.findproduct(id);
                if (p) {
                    cout << "New Stock Level: ";
                    int stock;
                    cin >> stock;
                    p->setstock(stock);
                    menu.savefile("menu.txt");
                    cout << "Stock updated.\n";
                }
                else {
                    cout << "Dish not found!\n";
                }
            }
            else if (choice == 3) {
                break;
            }
            else {
                cout << "Invalid choice, try again.\n";
            }
        }
    }

public:

    POSSystem() {
        menu.loadfile("menu.txt");
        // If menu is empty, add default dishes
        if (menu.findproduct(1) == nullptr && menu.findproduct(2) == nullptr && menu.findproduct(3) == nullptr) {
            menu.addproduct(Product(1, "Margherita Pizza", 1299, 10));
            menu.addproduct(Product(2, "Caesar Salad", 849, 15));
            menu.addproduct(Product(3, "Lemonade", 399, 20));
            menu.savefile("menu.txt");
        }
    }

    void start() {
        while (true) {
            cout << "\n=== Tasty Bites POS System ===\n";
            cout << "1. View Menu\n";
            cout << "2. Take Order\n";
            cout << "3. Cancel Order Item\n";
            cout << "4. Checkout\n";
            cout << "5. Admin Login\n";
            cout << "6. Exit\n";
            cout << "Choose option: ";
            int choice;
            cin >> choice;
            if (choice == 1) {
                menu.showmenu();
            }
            else if (choice == 2) {
                char more = 'y';
                while (more == 'y' || more == 'Y') {
                    cout << "Enter Dish ID: ";
                    int id;
                    cin >> id;

                    Product* p = menu.findproduct(id);
                    if (p) {
                        cout << "Quantity: ";
                        int qty;
                        cin >> qty;
                        if (qty <= 0) {
                            cout << "Quantity must be positive.\n";
                            continue;
                        }
                        if (p->canorder(qty)) {
                            cart.additem(p, qty);
                            cout << "Added to order!\n";
                        }
                        else {
                            cout << "Only " << p->getstock() << " available\n";
                        }
                    }
                    else {
                        cout << "Invalid Dish ID!\n";
                    }
                    cout << "Add another item? (y/n): ";
                    cin >> more;
                }
            }
            else if (choice == 3) {
                if (cart.empty()) {
                    cout << "Your order is empty. Nothing to cancel.\n";
                }
                else {
                    cart.listitems();
                    cout << "Enter item number to cancel (0 to return): ";
                    int itemNum;
                    cin >> itemNum;
                    if (itemNum == 0) {
                        cout << "Returning to main menu.\n";
                    }
                    else if (cart.removeitem(static_cast<size_t>(itemNum))) {
                        cout << "Item removed from order.\n";
                    }
                    else {
                        cout << "Invalid item number.\n";
                    }
                }
            }
            else if (choice == 4) {
                if (cart.empty()) {
                    cout << "Your order is empty.\n";
                }
                else {
                    cart.printreceipt();
                    double totalAmount = cart.gettotal();
                    cout << "\nSelect Payment Method:\n1. Cash\n2. Card\nChoice: ";
                    int payChoice;
                    cin >> payChoice;
                    Payment* payment = nullptr;
                    if (payChoice == 1) {
                        payment = new CashPayment();
                    }
                    else if (payChoice == 2) {
                        payment = new CardPayment();
                    }
                    else {
                        cout << "Invalid payment method.\n";
                    }
                    if (payment) {
                        payment->pay(totalAmount);
                        delete payment;
                        cart.savereceipt();
                        cart.clear();
                    }
                }
            }

            else if (choice == 5) {
                if (admin()) {
                    adminmenu();
                }
            }
            else if (choice == 6) {
                cout << "Thank you for using our system! Goodbye.\n";
                break;
            }
            else {
                cout << "Invalid choice, please try again.\n";
            }
        }
    }
};

int main() {
    POSSystem pos;
    pos.start();
    return 0;
}
