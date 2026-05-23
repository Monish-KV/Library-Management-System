#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>

class Book {
private:
    int bookID;
    std::string title;
    std::string author;
    bool isIssued;

public:
    Book(int id, std::string t, std::string a) 
        : bookID(id), title(t), author(a), isIssued(false) {}

    // Getters
    int getID() const { return bookID; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    bool getIssuedStatus() const { return isIssued; }

    // State Modifiers
    void issueBook() { isIssued = true; }
    void returnBook() { isIssued = false; }

    // Display formatted book information
    void displayBook() const {
        std::cout << std::left << std::setw(8) << bookID 
                  << std::setw(25) << title.substr(0, 23) 
                  << std::setw(20) << author.substr(0, 18) 
                  << (isIssued ? "Issued" : "Available") << "\n";
    }
};

class Member {
private:
    int memberID;
    std::string name;
    std::vector<int> borrowedBookIDs; // Tracks IDs of books currently borrowed

public:
    Member(int id, std::string n) : memberID(id), name(n) {}

    int getID() const { return memberID; }
    std::string getName() const { return name; }
    const std::vector<int>& getBorrowedBooks() const { return borrowedBookIDs; }

    void borrowBook(int bookID) {
        borrowedBookIDs.push_back(bookID);
    }

    bool returnBook(int bookID) {
        auto it = std::find(borrowedBookIDs.begin(), borrowedBookIDs.end(), bookID);
        if (it != borrowedBookIDs.end()) {
            borrowedBookIDs.erase(it);
            return true;
        }
        return false;
    }
};

class Library {
private:
    std::vector<Book> books;
    std::vector<Member> members;

    // Helper to transform strings to lowercase for case-insensitive matching
    std::string toLowerCase(std::string str) const {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

public:
    void addBook() {
        int id;
        std::string title, author;

        std::cout << "\nEnter Book ID (Numeric): ";
        while (!(std::cin >> id)) {
            std::cout << "Invalid input. Enter numbers only: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore();

        std::cout << "Enter Book Title: ";
        std::getline(std::cin, title);
        std::cout << "Enter Author Name: ";
        std::getline(std::cin, author);

        books.push_back(Book(id, title, author));
        std::cout << "Book successfully added to the catalog!\n";
    }

    void addMember() {
        int id;
        std::string name;

        std::cout << "\nEnter Member ID (Numeric): ";
        while (!(std::cin >> id)) {
            std::cout << "Invalid input. Enter numbers only: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore();

        std::cout << "Enter Member Full Name: ";
        std::getline(std::cin, name);

        members.push_back(Member(id, name));
        std::cout << "Member registered successfully!\n";
    }

    void searchBooks() const {
        int choice;
        std::cout << "\n1. Search by Title\n2. Search by Author\nChoose search type (1-2): ";
        std::cin >> choice;
        std::cin.ignore();

        std::string query;
        std::cout << "Enter search keyword: ";
        std::getline(std::cin, query);
        query = toLowerCase(query);

        std::cout << "\n------------------------------------------------------------\n";
        std::cout << std::left << std::setw(8) << "ID" << std::setw(25) << "Title" << std::setw(20) << "Author" << "Status\n";
        std::cout << "------------------------------------------------------------\n";

        bool found = false;
        for (const auto& book : books) {
            std::string matchField = (choice == 1) ? book.getTitle() : book.getAuthor();
            if (toLowerCase(matchField).find(query) != std::string::npos) {
                book.displayBook();
                found = true;
            }
        }
        if (!found) std::cout << "No matching records found.\n";
        std::cout << "------------------------------------------------------------\n";
    }

    void issueBook() {
        int memberID, bookID;
        std::cout << "\nEnter Member ID: ";
        std::cin >> memberID;
        std::cout << "Enter Book ID to Issue: ";
        std::cin >> bookID;

        // Find member
        auto memberIt = std::find_if(members.begin(), members.end(), [memberID](const Member& m) { return m.getID() == memberID; });
        if (memberIt == members.end()) {
            std::cout << "Error: Member ID not found.\n";
            return;
        }

        // Find book
        auto bookIt = std::find_if(books.begin(), books.end(), [bookID](const Book& b) { return b.getID() == bookID; });
        if (bookIt == books.end()) {
            std::cout << "Error: Book ID not found.\n";
            return;
        }

        // Verify status
        if (bookIt->getIssuedStatus()) {
            std::cout << "Error: Book is already issued to another member.\n";
            return;
        }

        // Execute transactions
        bookIt->issueBook();
        memberIt->borrowBook(bookID);
        std::cout << "Success: Book successfully issued to " << memberIt->getName() << ".\n";
    }

    void returnBook() {
        int memberID, bookID;
        std::cout << "\nEnter Member ID: ";
        std::cin >> memberID;
        std::cout << "Enter Book ID to Return: ";
        std::cin >> bookID;

        // Find member
        auto memberIt = std::find_if(members.begin(), members.end(), [memberID](const Member& m) { return m.getID() == memberID; });
        if (memberIt == members.end()) {
            std::cout << "Error: Member ID not found.\n";
            return;
        }

        // Find book
        auto bookIt = std::find_if(books.begin(), books.end(), [bookID](const Book& b) { return b.getID() == bookID; });
        if (bookIt == books.end()) {
            std::cout << "Error: Book ID not found.\n";
            return;
        }

        // Execute transaction loop records
        if (memberIt->returnBook(bookID)) {
            bookIt->returnBook();
            std::cout << "Success: Book successfully returned.\n";
        } else {
            std::cout << "Error: This specific member did not borrow this book.\n";
        }
    }
};

int main() {
    Library system;
    int choice;

    do {
        std::cout << "\n===================================";
        std::cout << "\n    LIBRARY MANAGEMENT SYSTEM      ";
        std::cout << "\n===================================";
        std::cout << "\n1. Add New Book";
        std::cout << "\n2. Register New Member";
        std::cout << "\n3. Search Books";
        std::cout << "\n4. Issue a Book";
        std::cout << "\n5. Return a Book";
        std::cout << "\n6. Exit Application";
        std::cout << "\n===================================";
        std::cout << "\nEnter Choice (1-6): ";

        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: system.addBook(); break;
            case 2: system.addMember(); break;
            case 3: system.searchBooks(); break;
            case 4: system.issueBook(); break;
            case 5: system.returnBook(); break;
            case 6: std::cout << "\nClosing System. Goodbye!\n"; break;
            default: std::cout << "\nInvalid choice option. Select 1 to 6.\n";
        }
    } while (choice != 6);

    return 0;
}
