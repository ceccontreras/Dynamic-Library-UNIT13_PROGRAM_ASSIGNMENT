// Author:      Carlos Campos
// Date:        04/19/2026
// Program:     Dynamic Library
// Description: A menu-driven Library Inventory System demonstrating operator
//              overloading, dynamic memory management, deep copy, friend
//              functions, 'this' pointer, function templates, class templates,
//              and file I/O.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <limits>
#include <iomanip>

using namespace std;


// Book Class
class Book {
private:
    string title;
    char*  author;  // dynamically allocated C-string
    double price;
    int    stock;

    // Safely allocate and copy a C-string into author
    void allocateAuthor(const char* src) {
        if (src) {
            author = new char[strlen(src) + 1];
            strcpy(author, src);
        } else {
            author = new char[1];
            author[0] = '\0';
        }
    }

public:
    // Default constructor
    Book() : title(""), price(0.0), stock(0) {
        author = new char[1];
        author[0] = '\0';
    }

    // Parameterized constructor
    Book(const string& t, const char* a, double p, int s) : title(t), price(p), stock(s) {
        allocateAuthor(a);
    }

    // Copy constructor — deep copy
    Book(const Book& other) : title(other.title), price(other.price), stock(other.stock) {
        allocateAuthor(other.author);
    }

    // Copy assignment operator — free old memory, then deep copy
    Book& operator=(const Book& other) {
        if (this != &other) {           // guard against self-assignment
            delete[] author;            // release old heap memory
            title  = other.title;
            price  = other.price;
            stock  = other.stock;
            allocateAuthor(other.author);
        }
        return *this;
    }

    // Destructor — free dynamically allocated author memory
    ~Book() {
        delete[] author;
    }

    // ---- Setters (use this-> to make the pointer explicit) ----
    void setTitle (const string& t) { this->title = t; }
    void setAuthor(const char*   a) { delete[] author; allocateAuthor(a); }
    void setPrice (double p)        { this->price = p; }
    void setStock (int    s)        { this->stock = s; }

    // ---- Getters ----
    string      getTitle () const { return this->title;  }
    const char* getAuthor() const { return this->author; }
    double      getPrice () const { return this->price;  }
    int         getStock () const { return this->stock;  }

    // Pre-increment: increase stock by 1, return *this
    Book& operator++() {
        ++this->stock;
        return *this;
    }

    // Equality: compare title and author only
    bool operator==(const Book& other) const {
        return (this->title == other.title &&
                strcmp(this->author, other.author) == 0);
    }

    // Friend: formatted output
    friend ostream& operator<<(ostream& out, const Book& b);

    // Friend: prompted input
    friend istream& operator>>(istream& in, Book& b);
};


// Overloaded << — display one book in readable format
ostream& operator<<(ostream& out, const Book& b) {
    out << "Title:  " << b.title                                     << "\n"
        << "Author: " << b.author                                    << "\n"
        << "Price:  $" << fixed << setprecision(2) << b.price        << "\n"
        << "Stock:  " << b.stock                                     << "\n";
    return out;
}


// Overloaded >> — read a book from the user with prompts
istream& operator>>(istream& in, Book& b) {
    string authorStr;

    cout << "  Enter title:  ";
    getline(in, b.title);

    cout << "  Enter author: ";
    getline(in, authorStr);
    b.setAuthor(authorStr.c_str());

    cout << "  Enter price:  ";
    while (!(in >> b.price) || b.price < 0.0) {
        cout << "  Invalid — enter a positive number: ";
        in.clear();
        in.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "  Enter stock:  ";
    while (!(in >> b.stock) || b.stock < 0) {
        cout << "  Invalid — enter a non-negative integer: ";
        in.clear();
        in.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    in.ignore(numeric_limits<streamsize>::max(), '\n'); // flush trailing newline

    return in;
}


// Function Template — returns the larger of two values
template <typename T>
T maxValue(T a, T b) {
    return (a > b) ? a : b;
}


// Class Template — stores a single value of any type
template <typename T>
class Box {
private:
    T value;
public:
    Box(T v) : value(v) {}
    T    getValue() const { return value; }
    void display()  const { cout << "Box contains: " << value << "\n"; }
};

// Specialization for Book — display the title instead of the whole object
template <>
class Box<Book> {
private:
    Book value;
public:
    Box(const Book& v) : value(v) {}
    Book getValue() const { return value; }
    void display()  const {
        cout << "Box<Book> contains a book titled: "
             << value.getTitle() << "\n";
    }
};


// Utility helpers
void flushInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Find the index of a book by title; returns -1 if not found
int findBook(const vector<Book>& inv, const string& title) {
    int index = 0;
    for (const Book& b : inv) {
        if (b.getTitle() == title)
            return index;
        index++;
    }
    return -1;
}

// ============================================================
// Menu Option 1 — Load books from books.txt
// Each record is exactly 4 lines: title / author / price / stock
// Blank lines between records are tolerated.
// ============================================================
void loadBooks(vector<Book>& inv) {
    ifstream inFile("books.txt");
    if (!inFile) {
        cout << "  Error: 'books.txt' not found.\n";
        return;
    }

    inv.clear();    // discard old inventory

    string title, authorStr, priceStr, stockStr;
    int count = 0;

    while (true) {
        // Skip any blank lines to reach the title line
        title = "";
        while (title.empty()) {
            if (!getline(inFile, title)) goto loadDone; // end of file
        }

        if (!getline(inFile, authorStr) ||
            !getline(inFile, priceStr)  ||
            !getline(inFile, stockStr)) {
            cout << "  Warning: incomplete record at end of file — skipped.\n";
            break;
        }

        try {
            double p = stod(priceStr);
            int    s = stoi(stockStr);
            inv.emplace_back(title, authorStr.c_str(), p, s);
            ++count;
        } catch (...) {
            cout << "  Warning: malformed record skipped.\n";
        }
    }

    loadDone:
    inFile.close();
    cout << "  Books successfully loaded from file.\n";
    cout << "  " << count << " book(s) available in inventory.\n";
}


// Menu Option 7 — Save books to books.txt
void saveBooks(const vector<Book>& inv) {
    ofstream outFile("books.txt");
    if (!outFile) {
        cout << "  Error: Could not write to 'books.txt'.\n";
        return;
    }

    for (const Book& b : inv) {
        outFile << b.getTitle()  << "\n"
                << b.getAuthor() << "\n"
                << fixed << setprecision(2) << b.getPrice() << "\n"
                << b.getStock()  << "\n";
    }

    outFile.close();
    cout << "  Books successfully saved to 'books.txt'.\n";
}


// Menu Option 2 — Display all books
void displayAllBooks(const vector<Book>& inv) {
    if (inv.empty()) {
        cout << "  Inventory is empty. Load or add books first.\n";
        return;
    }
    cout << "\n===== INVENTORY (" << inv.size() << " book(s)) =====\n";
    int number = 1;
    for (const Book& b : inv) {
        cout << "\n[" << number << "]\n" << b;
        number++;
    }
    cout << "=====================================\n";
}


// Menu Option 3 — Add a new book via overloaded >>
void addBook(vector<Book>& inv) {
    cout << "\n--- Add New Book ---\n";
    Book newBook;
    cin >> newBook;     // uses overloaded >> operator

    // Warn about duplicate title (same title AND author = true duplicate)
    int idx = findBook(inv, newBook.getTitle());
    if (idx != -1 && inv[idx] == newBook) {
        cout << "  Warning: a book with this title and author already exists.\n";
    }

    inv.push_back(newBook);
    cout << "  Book added successfully.\n";
}

// Menu Option 4 — Compare two books with ==
void compareBooks(const vector<Book>& inv) {
    if (inv.empty()) {
        cout << "  Inventory is empty.\n";
        return;
    }

    string t1, t2;
    cout << "  Enter title of first book:  ";
    getline(cin, t1);
    cout << "  Enter title of second book: ";
    getline(cin, t2);

    int i1 = findBook(inv, t1);
    int i2 = findBook(inv, t2);

    if (i1 == -1) { cout << "  Book \"" << t1 << "\" not found.\n"; return; }
    if (i2 == -1) { cout << "  Book \"" << t2 << "\" not found.\n"; return; }

    if (inv[i1] == inv[i2])
        cout << "  Result: The books are equal.\n";
    else
        cout << "  Result: The two books are NOT equal.\n";
}


// Menu Option 5 — Increase stock with ++book
void increaseStock(vector<Book>& inv) {
    if (inv.empty()) {
        cout << "  Inventory is empty.\n";
        return;
    }

    string title;
    cout << "  Enter book title: ";
    getline(cin, title);

    int idx = findBook(inv, title);
    if (idx == -1) {
        cout << "  Book \"" << title << "\" not found.\n";
        return;
    }

    ++inv[idx];     // pre-increment operator overload
    cout << "  Stock increased. Updated record:\n" << inv[idx];
}


// Menu Option 6 — Template demonstrations
void templateDemo(const vector<Book>& inv) {
    // Function template
    cout << "\n--- Template Demonstrations ---\n";
    cout << "maxValue(10, 25) = " << maxValue(10, 25) << "\n";

    // Class template: Box<int>
    Box<int> intBox(7);
    intBox.display();

    // Class template: Box<Book>
    if (!inv.empty()) {
        Box<Book> bookBox(inv[0]);
        bookBox.display();
    } else {
        Book demo("Hamlet", "William Shakespeare", 12.99, 5);
        Box<Book> bookBox(demo);
        bookBox.display();
    }
}

// Display menu
void showMenu() {
    cout << "\n=============================\n";
    cout << "  LIBRARY INVENTORY SYSTEM   \n";
    cout << "=============================\n";
    cout << "  1. Load Books from File\n";
    cout << "  2. Display All Books\n";
    cout << "  3. Add a New Book\n";
    cout << "  4. Compare Two Books\n";
    cout << "  5. Increase Stock (++ Operator)\n";
    cout << "  6. Template Demonstrations\n";
    cout << "  7. Save Books and Exit\n";
    cout << "=============================\n";
    cout << "  Choice: ";
}

// Main
int main() {
    vector<Book> inventory;
    int choice = 0;

    cout << "Welcome to the Library Inventory System!\n";

    do {
        showMenu();

        // Validate menu input
        while (!(cin >> choice) || choice < 1 || choice > 7) {
            cout << "  Invalid input. Enter a number 1-7: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline

        switch (choice) {
            case 1: loadBooks(inventory);       break;
            case 2: displayAllBooks(inventory); break;
            case 3: addBook(inventory);         break;
            case 4: compareBooks(inventory);    break;
            case 5: increaseStock(inventory);   break;
            case 6: templateDemo(inventory);    break;
            case 7:
                saveBooks(inventory);
                cout << "  Exiting program. Goodbye!\n";
                break;
            default:
                cout << "  Invalid choice.\n";
                break;
        }

    } while (choice != 7);

    return 0;
}