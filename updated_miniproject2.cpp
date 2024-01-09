#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdexcept>

using namespace std;

class Book {
public:
    string title;
    string author;
    string isbn;

    Book(string t, string a, string i) : title(t), author(a), isbn(i) {}

    bool operator<(const Book& other) const {
        return title < other.title;
    }
};

class Loan {
public:
    string customerName;
    string isbn;
    time_t dueDate;

    Loan(string name, string i, time_t dd) : customerName(name), isbn(i), dueDate(dd) {}
};

class Library {
public:
    Library() {
        loadBooks();
        loadLoans();
    }

    ~Library() {
        saveBooks();
        saveLoans();
    }

    void addBook(Book b) {
        if (findBookByISBN(b.isbn) != nullptr) {
            throw runtime_error("A book with that ISBN already exists.");
        } else {
            books.push_back(b);
            cout << "Book added successfully." << endl;
        }
    }

    Book* findBookByISBN(string isbn) {
        for (auto& book : books) {
            if (book.isbn == isbn) {
                return &book;
            }
        }
        return nullptr;
    }

    // Search for books by title or author
    vector<Book> searchBooks(const string& query) {
        vector<Book> results;
        for (const auto& book : books) {
            if (book.title.find(query) != string::npos || book.author.find(query) != string::npos) {
                results.push_back(book);
            }
        }
        return results;
    }

    void removeBook(string isbn) {
        auto it = remove_if(books.begin(), books.end(), [&](const Book& book) {
            return book.isbn == isbn;
        });
        if (it != books.end()) {
            books.erase(it, books.end());
            cout << "Book removed successfully." << endl;
        } else {
            throw runtime_error("Book with ISBN " + isbn + " not found.");
        }
    }

    void listBooks() {
        if (books.empty()) {
            cout << "The library is empty." << endl;
        } else {
            sort(books.begin(), books.end());
            cout << "Books in the library:" << endl;
            for (const auto& book : books) {
                cout << "Title: " << book.title << endl;
                cout << "Author: " << book.author << endl;
                cout << "ISBN: " << book.isbn << endl;
                cout << "------------------------" << endl;
            }
        }
    }

    void lendBook(const string& customerName, const string& isbn, int daysToDue) {
        time_t now = time(0);
        time_t dueDate = now + daysToDue * 24 * 60 * 60;  // Calculate due date in seconds

        loans.push_back(Loan(customerName, isbn, dueDate));
        cout << "Book lent successfully." << endl;
    }

    void returnBook(const string& customerName, const string& isbn) {
        auto it = find_if(loans.begin(), loans.end(), [&](const Loan& loan) {
            return loan.customerName == customerName && loan.isbn == isbn;
        });

        if (it != loans.end()) {
            loans.erase(it);
            cout << "Book returned successfully." << endl;
        } else {
            throw runtime_error("Book with ISBN " + isbn + " not found in the loans.");
        }
    }

    // Search for loans by customer name
    vector<Loan> searchLoans(const string& customerName) {
        vector<Loan> results;
        for (const auto& loan : loans) {
            if (loan.customerName == customerName) {
                results.push_back(loan);
            }
        }
        return results;
    }

    void listLoans() {
        if (loans.empty()) {
            cout << "No books are currently on loan." << endl;
        } else {
            cout << "Books on loan:" << endl;
            for (const auto& loan : loans) {
                cout << "Customer Name: " << loan.customerName << endl;
                cout << "ISBN: " << loan.isbn << endl;
                cout << "Due Date: " << ctime(&loan.dueDate);
                cout << "------------------------" << endl;
            }
        }
    }

private:
    vector<Book> books;
    vector<Loan> loans;
    const string booksDataFile = "library_books.txt";
    const string loansDataFile = "library_loans.txt";

    void saveBooks() {
        ofstream file(booksDataFile, ios::out);
        if (!file.is_open()) {
            throw runtime_error("Failed to open books data file for writing.");
        }

        for (const auto& book : books) {
            file << book.title << " " << book.author << " " << book.isbn << endl;
        }

        file.close();
    }

    void loadBooks() {
        ifstream file(booksDataFile, ios::in);
        if (!file.is_open()) {
            throw runtime_error("Failed to open books data file for reading.");
        }

        while (!file.eof()) {
            string title, author, isbn;
            file >> title >> author >> isbn;
            if (title != "") {
                books.push_back(Book(title, author, isbn));
            }
        }

        file.close();
    }

    void saveLoans() {
        ofstream file(loansDataFile, ios::out);
        if (!file.is_open()) {
            throw runtime_error("Failed to open loans data file for writing.");
        }

        for (const auto& loan : loans) {
            file << loan.customerName << " " << loan.isbn << " " << loan.dueDate << endl;
        }

        file.close();
    }

    void loadLoans() {
        ifstream file(loansDataFile, ios::in);
        if (!file.is_open()) {
            throw runtime_error("Failed to open loans data file for reading.");
        }

        while (!file.eof()) {
            string customerName, isbn;
            time_t dueDate;
            file >> customerName >> isbn >> dueDate;
            if (customerName != "") {
                loans.push_back(Loan(customerName, isbn, dueDate));
            }
        }

        file.close();
    }
};

int main() {
    Library library;

    while (true) {
        cout << "1. Add Book" << endl;
        cout << "2. Remove Book" << endl;
        cout << "3. List Books" << endl;
        cout << "4. Search Books" << endl;
        cout << "5. Lend Book" << endl;
        cout << "6. Return Book" << endl;
        cout << "7. List Loans" << endl;
        cout << "8. Search Loans" << endl;
        cout << "9. Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        try {
            if (choice == 1) {
                string title, author, isbn;
                cout << "Enter title: ";
                cin >> title;
                cout << "Enter author: ";
                cin >> author;
                cout << "Enter ISBN: ";
                cin >> isbn;

                Book book(title, author, isbn);
                library.addBook(book);
            } else if (choice == 2) {
                string isbn;
                cout << "Enter ISBN of the book to remove: ";
                cin >> isbn;
                library.removeBook(isbn);
            } else if (choice == 3) {
                library.listBooks();
            } else if (choice == 4) {
                string query;
                cout << "Enter search query (title or author): ";
                cin.ignore(); // Consume the newline character
                getline(cin, query);
                vector<Book> results = library.searchBooks(query);
                if (results.empty()) {
                    cout << "No matching books found." << endl;
                } else {
                    cout << "Matching Books:" << endl;
                    for (const auto& book : results) {
                        cout << "Title: " << book.title << endl;
                        cout << "Author: " << book.author << endl;
                        cout << "ISBN: " << book.isbn << endl;
                        cout << "------------------------" << endl;
                    }
                }
            } else if (choice == 5) {
                string customerName, isbn;
                int daysToDue;
                cout << "Enter customer name: ";
                cin.ignore(); // Consume the newline character
                getline(cin, customerName);
                cout << "Enter ISBN of the book to lend: ";
                cin >> isbn;
                cout << "Enter days to due: ";
                cin >> daysToDue;
                library.lendBook(customerName, isbn, daysToDue);
            } else if (choice == 6) {
                string customerName, isbn;
                cout << "Enter customer name: ";
                cin.ignore(); // Consume the newline character
                getline(cin, customerName);
                cout << "Enter ISBN of the book to return: ";
                cin >> isbn;
                library.returnBook(customerName, isbn);
            } else if (choice == 7) {
                library.listLoans();
            } else if (choice == 8) {
                string customerName;
                cout << "Enter customer name for loan search: ";
                cin.ignore(); // Consume the newline character
                getline(cin, customerName);
                vector<Loan> results = library.searchLoans(customerName);
                if (results.empty()) {
                    cout << "No loans found for the customer." << endl;
                } else {
                    cout << "Loans for " << customerName << ":" << endl;
                    for (const auto& loan : results) {
                        cout << "Customer Name: " << loan.customerName << endl;
                        cout << "ISBN: " << loan.isbn << endl;
                        cout << "Due Date: " << ctime(&loan.dueDate);
                        cout << "------------------------" << endl;
                    }
                }
            } else {
                break;
            }
        } catch (const runtime_error& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    return 0;
}
