#include <iostream>
#include <string>
#include <iomanip>
#include "sqlite3.h"

using namespace std;

// Global database connection
sqlite3* db;

// Student structure
struct Student {
    int rollno;
    string name;
    int marks1;
    int marks2;
    int marks3;
    int marks4;
    int marks5;
    int totalmarks;
};

// Function to initialize database
void initializeDB() {
    // Open database
    int rc = sqlite3_open("students.db", &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }
    
    // Create table if not exists
    const char* sql = "CREATE TABLE IF NOT EXISTS Students ("
                     "rollno INTEGER PRIMARY KEY,"
                     "name TEXT NOT NULL,"
                     "marks1 INTEGER,"
                     "marks2 INTEGER,"
                     "marks3 INTEGER,"
                     "marks4 INTEGER,"
                     "marks5 INTEGER,"
                     "totalmarks INTEGER);";
                     
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        exit(1);
    }
}

// Function to add a student
void addStudent() {
    Student st;
    
    cout << "Enter name: ";
    cin >> st.name;
    
    cout << "Enter rollno: ";
    cin >> st.rollno;
    
    cout << "Enter marks1: ";
    cin >> st.marks1;
    
    cout << "Enter marks2: ";
    cin >> st.marks2;
    
    cout << "Enter marks3: ";
    cin >> st.marks3;
    
    cout << "Enter marks4: ";
    cin >> st.marks4;
    
    cout << "Enter marks5: ";
    cin >> st.marks5;
    
    st.totalmarks = st.marks1 + st.marks2 + st.marks3 + st.marks4 + st.marks5;
    
    // Prepare SQL statement
    string sql = "INSERT INTO Students VALUES (" + 
                to_string(st.rollno) + ", '" + 
                st.name + "', " + 
                to_string(st.marks1) + ", " + 
                to_string(st.marks2) + ", " + 
                to_string(st.marks3) + ", " + 
                to_string(st.marks4) + ", " + 
                to_string(st.marks5) + ", " + 
                to_string(st.totalmarks) + ");";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Student added successfully..." << endl;
    }
}

// Function to update marks
void updateMarks() {
    int rollno;
    Student st;
    bool found = false;
    
    cout << "\n-----Update marks-----\n";
    cout << "Enter rollno to update: ";
    cin >> rollno;
    
    // First check if student exists
    string checkSql = "SELECT * FROM Students WHERE rollno = " + to_string(rollno) + ";";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, checkSql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return;
    }
    
    // Get existing student
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        st.rollno = rollno;
        st.name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    }
    
    sqlite3_finalize(stmt);
    
    if (!found) {
        cout << "\nStudent not found...\n";
        return;
    }
    
    // Get new marks
    cout << "Enter the new set of marks\n";
    cout << "Enter marks1: ";
    cin >> st.marks1;
    
    cout << "Enter marks2: ";
    cin >> st.marks2;
    
    cout << "Enter marks3: ";
    cin >> st.marks3;
    
    cout << "Enter marks4: ";
    cin >> st.marks4;
    
    cout << "Enter marks5: ";
    cin >> st.marks5;
    
    st.totalmarks = st.marks1 + st.marks2 + st.marks3 + st.marks4 + st.marks5;
    
    // Update database
    string updateSql = "UPDATE Students SET "
                      "marks1 = " + to_string(st.marks1) + ", "
                      "marks2 = " + to_string(st.marks2) + ", "
                      "marks3 = " + to_string(st.marks3) + ", "
                      "marks4 = " + to_string(st.marks4) + ", "
                      "marks5 = " + to_string(st.marks5) + ", "
                      "totalmarks = " + to_string(st.totalmarks) + " "
                      "WHERE rollno = " + to_string(rollno) + ";";
    
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, updateSql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "\nMarks updated successfully...\n";
    }
}

// Callback function for displaying students
static int displayCallback(void* data, int argc, char** argv, char** azColName) {
    // Format: Rollno, Name, Subject1, Subject2, Subject3, Subject4, Subject5, Total Marks
    cout << setw(10) << left << (argv[0] ? argv[0] : "NULL") << " ";
    cout << setw(30) << left << (argv[1] ? argv[1] : "NULL") << " ";
    cout << setw(20) << left << (argv[2] ? argv[2] : "NULL") << " ";
    cout << setw(20) << left << (argv[3] ? argv[3] : "NULL") << " ";
    cout << setw(20) << left << (argv[4] ? argv[4] : "NULL") << " ";
    cout << setw(20) << left << (argv[5] ? argv[5] : "NULL") << " ";
    cout << setw(20) << left << (argv[6] ? argv[6] : "NULL") << " ";
    cout << setw(20) << left << (argv[7] ? argv[7] : "NULL") << endl;
    
    return 0;
}

// Function to display all students
void display() {
    cout << "\n---Student details----\n";
    cout << setw(10) << left << "Rollno" << " ";
    cout << setw(30) << left << "Name" << " ";
    cout << setw(20) << left << "Subject1" << " ";
    cout << setw(20) << left << "Subject2" << " ";
    cout << setw(20) << left << "Subject3" << " ";
    cout << setw(20) << left << "Subject4" << " ";
    cout << setw(20) << left << "Subject5" << " ";
    cout << setw(20) << left << "Total Marks" << endl;
    
    string sql = "SELECT * FROM Students ORDER BY rollno;";
    char* errMsg = nullptr;
    
    int rc = sqlite3_exec(db, sql.c_str(), displayCallback, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}

// Teacher menu
void teacher() {
    int choice;
    
    cout << "\n1. Add student\n";
    cout << "2. Update marks\n";
    cout << "3. Display marks\n";
    cout << "Enter choice: ";
    cin >> choice;
    
    switch (choice) {
        case 1:
            addStudent();
            break;
        case 2:
            updateMarks();
            break;
        case 3:
            display();
            break;
        default:
            cout << "Invalid input\n";
    }
}

// Student menu (only view functionality)
void student() {
    display();
}

// Main function
int main() {
    // Initialize database
    initializeDB();
    
    int choice;
    while (true) {
        cout << "\n1. Teacher\n";
        cout << "2. Student\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                teacher();
                break;
            case 2:
                student();
                break;
            case 0:
                sqlite3_close(db);
                return 0;
            default:
                cout << "Invalid input\n";
        }
    }
    
    return 0;
}