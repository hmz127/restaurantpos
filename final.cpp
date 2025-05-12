
#include <iostream>
#include <limits>
#include <string>
#include <fstream>

using namespace std;

const double TXRATE = 0.07;
const string USER = "admin";
const string PASS = "pass123";

// Helper function to convert int to string manually
string intToString(int num) {
    if (num == 0) {
        return "0";
    }
    string result;
    bool isNegative = num < 0;
    num = abs(num);
    while (num > 0) {
        result = char('0' + (num % 10)) + result;
        num /= 10;
    }
    if (isNegative) {
        result = "-" + result;
    }
    return result;
}

void printPadded(const string& text, int width) {
    cout << text;
    for (int i = (int)text.length(); i < width; i++) {
        cout << " ";
    }
}

void printPrice(double amount, int width) {
    int pkr = static_cast<int>(amount);
    int cents = static_cast<int>((amount - pkr) * 100 + 0.5);
    string amountStr = intToString(pkr) + ".";
    if (cents < 10) {
        amountStr += "0";
    }
    amountStr += intToString(cents);
    printPadded("Pkr" + amountStr, width);
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
        printPadded(intToString(id), 6);
        printPadded(name, 25);
        printPrice(price, 10);
        cout << "  Available: " << stock << endl;
    }

    bool canOrder(int quantity) const {
        return stock >= quantity;
    }
    void reduceStock(int quantity) {
        stock -= quantity;
    }
    void increaseStock(int quantity) {
        stock += quantity;
    }
    void setStock(int newStock) {
        stock = newStock;
    }
    int getID() const {
        return id;
    }
    double getPrice() const {
        return price;
    }
    int getStock() const {
        return stock;
    }
    string getName() const {
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

    void addProduct(const Product& product) {
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

    void showMenu() const {
        cout << "\n\t\t Today's Menu \n";
        cout << " ID   Dish Name             Price           Available\n";
        cout << " -----------------------------------------------------\n";
        for (int i = 0; i < productCount; ++i)
            products[i].display();
    }

    Product* findProduct(int id) {
        for (int i = 0; i < productCount; ++i)
            if (products[i].getID() == id)
                return &products[i];
        return nullptr;
    }

    void loadFromFile(const string& filename) {
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
          addProduct(Product(id, name, price, stock));
        }
    }

    void saveToFile(const string& filename) const {
        ofstream file(filename);
        for (int i = 0; i < productCount; ++i) {
         file << products[i].getID() << '\n' << products[i].getName() << '\n' << products[i].getPrice() << ' ' << products[i].getStock() << "\n\n";
        }
    }
};

class OrderItem {
    Product* product;
    int quantity;

public:
    OrderItem() : product(nullptr), quantity(0) {}
    OrderItem(Product* p, int qty) : product(p), quantity(qty) {}

    double getTotal() const {
        return product->getPrice() * quantity;
    }
    void print() const {
        printPadded(product->getName(), 25);
        cout << "x" << quantity;
        cout << "  Total: ";
        printPrice(getTotal(), 15);
        cout << endl;
    }
    Product* getProduct() const {
        return product;
    }
    int getQuantity() const {
        return quantity;
    }
    void increaseQuantity(int qty) {
        quantity += qty;
    }
};

class Cart {
    OrderItem* items;
    int itemCount;
    int itemCapacity;
    static int orderCounter;

public:
    Cart() {
        itemCapacity = 10;
        itemCount = 0;
        items = new OrderItem[itemCapacity];
    }
    ~Cart() {
        delete[] items;
    }

    void addItem(Product* product, int quantity) {
        for (int i = 0; i < itemCount; ++i) {
            if (items[i].getProduct()->getID() == product->getID()) {
                items[i].increaseQuantity(quantity);
                product->reduceStock(quantity);
                return;
            }
        }
        if (itemCount == itemCapacity) {
            int newCapacity = itemCapacity * 2;
            OrderItem* newArr = new OrderItem[newCapacity];
            for (int i = 0; i < itemCount; ++i)
                newArr[i] = items[i];
            delete[] items;
            items = newArr;
            itemCapacity = newCapacity;
        }
        items[itemCount++] = OrderItem(product, quantity);
        product->reduceStock(quantity);
    }

    void printReceipt() const {
        cout << "\n\t\t Final Bill\n";
        double subtotal = 0;

        for (int i = 0; i < itemCount; ++i) {
            items[i].print();
            subtotal += items[i].getTotal();
        }

        double tax = subtotal * TXRATE;
        cout << "\nSubtotal: ";
        printPrice(subtotal, 25);
        cout << "\nTax (7%): ";
        printPrice(tax, 25);
        cout << "\nTotal:    ";
        printPrice(subtotal + tax, 25);
        cout << "\n==============================\n";
    }

    void saveReceipt() const {
        ofstream file("receipt_" + intToString(orderCounter++) + ".txt");
        file << "\t\t Order Receipt\n";
        for (int i = 0; i < itemCount; ++i) {
            file << items[i].getTotal() << " - "
                << items[i].getProduct()->getName()
                << " x" << items[i].getQuantity() << '\n';
        }
    }

    bool empty() const { return itemCount == 0; }
    void clear() { itemCount = 0; }

    void listItems() const {
        if (itemCount == 0) {
            cout << "Your order is empty.\n";
            return;
        }
        cout << "\nYour current order:\n";
        cout << "No.  Dish                     Quantity\n";
        cout << "--------------------------------------\n";
        for (int i = 0; i < itemCount; ++i) {
            cout << (i + 1) << ".   ";
            printPadded(items[i].getProduct()->getName(), 25);
            cout << items[i].getQuantity() << endl;
        }
    }

    bool removeItem(size_t index) {
        if (index >= 1 && index <= (size_t)itemCount) {
            Product* p = items[index - 1].getProduct();
            int qty = items[index - 1].getQuantity();
            p->increaseStock(qty);
            for (int i = index - 1; i < itemCount - 1; ++i) {
                items[i] = items[i + 1];
            }
            --itemCount;
            return true;
        }
        return false;
    }
};

int Cart::orderCounter = 1001;

class POSSystem {
    Inventory menu;
    Cart cart;

    bool authenticateAdmin() {
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

   void adminMenu() {
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
             menu.addProduct(Product(id, name, price, stock));
             menu.saveToFile("menu.txt");
             cout << "Dish added successfully.\n";
            }
       else if (choice == 2) {
             cout << "Enter Dish ID: ";
             int id;
             cin >> id;
              Product* p = menu.findProduct(id);
        if (p) {
              cout << "New Stock Level: ";
              int stock;
              cin >> stock;
              p->setStock(stock);
              menu.saveToFile("menu.txt");
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
        menu.loadFromFile("menu.txt");
        // If menu is empty, add default dishes
   if (menu.findProduct(1) == nullptr && menu.findProduct(2) == nullptr && menu.findProduct(3) == nullptr) {
        menu.addProduct(Product(1, "Margherita Pizza", 1299, 10));
        menu.addProduct(Product(2, "Caesar Salad", 849, 15));
        menu.addProduct(Product(3, "Lemonade", 399, 20));
        menu.saveToFile("menu.txt");
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
               menu.showMenu();
            }
       else if (choice == 2) {
            char more = 'y';
            while (more == 'y' || more == 'Y') {
              cout << "Enter Dish ID: ";
              int id;
               cin >> id;

           Product* p = menu.findProduct(id);
               if (p) {
                cout << "Quantity: ";
                 int qty;
                 cin >> qty;
                 if (qty <= 0) {
                     cout << "Quantity must be positive.\n";
                      continue;
                    }
                if (p->canOrder(qty)) {
                      cart.addItem(p, qty);
                       cout << "Added to order!\n";
                     }
                else {
                    cout << "Only " << p->getStock() << " available\n";
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
                    cart.listItems();
                    cout << "Enter item number to cancel (0 to return): ";
                    int itemNum;
                    cin >> itemNum;
                    if (itemNum == 0) {
                        cout << "Returning to main menu.\n";
                    }
                    else if (cart.removeItem(static_cast<size_t>(itemNum))) {
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
                    cart.printReceipt();
                    cart.saveReceipt();
                    cart.clear();
                }
            }
            else if (choice == 5) {
                if (authenticateAdmin()) {
                    adminMenu();
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

