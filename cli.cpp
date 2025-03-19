#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <string>
#include <ctime>

using namespace std;

sqlite3* db;

void initialize_db() {
    char* errMsg;
    sqlite3_open("tasks.db", &db);
    const char* task_table = "CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, description TEXT, completed BOOLEAN);";
    const char* history_table = "CREATE TABLE IF NOT EXISTS task_history (id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TEXT, action TEXT, task_description TEXT);";
    sqlite3_exec(db, task_table, 0, 0, &errMsg);
    sqlite3_exec(db, history_table, 0, 0, &errMsg);
    sqlite3_close(db);
}

void execute_query(const char* query, const vector<string>& params = {}) {
    sqlite3_open("tasks.db", &db);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    for (size_t i = 0; i < params.size(); i++) {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_STATIC);
    }
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void log_history(const string& action, const string& task_description) {
    time_t now = time(0);
    string timestamp = ctime(&now);
    timestamp.pop_back(); // Remove newline
    execute_query("INSERT INTO task_history (timestamp, action, task_description) VALUES (?, ?, ?)", {timestamp, action, task_description});
}

void add_task(const string& description) {
    execute_query("INSERT INTO tasks (description, completed) VALUES (?, 0)", {description});
    log_history("Added", description);
    cout << "Task added successfully." << endl;
}

void edit_task(int task_id, const string& new_description) {
    execute_query("UPDATE tasks SET description = ? WHERE id = ?", {new_description, to_string(task_id)});
    log_history("Edited", new_description);
    cout << "Task updated successfully." << endl;
}

void complete_task(int task_id) {
    execute_query("UPDATE tasks SET completed = 1 WHERE id = ?", {to_string(task_id)});
    log_history("Completed", "Task ID: " + to_string(task_id));
    cout << "Task marked as completed." << endl;
}

void view_tasks() {
    sqlite3_open("tasks.db", &db);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT id, description, completed FROM tasks", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        bool completed = sqlite3_column_int(stmt, 2);
        cout << "[" << id << "] " << desc << " - " << (completed ? "Done" : "Pending") << endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void view_history() {
    sqlite3_open("tasks.db", &db);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT timestamp, action, task_description FROM task_history", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string action = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string task_description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        cout << timestamp << " - " << action << ": " << task_description << endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int main() {
    initialize_db();
    while (true) {
        cout << "\nTask Manager" << endl;
        cout << "1. Add Task" << endl;
        cout << "2. Edit Task" << endl;
        cout << "3. Complete Task" << endl;
        cout << "4. View Tasks" << endl;
        cout << "5. View Task History" << endl;
        cout << "6. Exit" << endl;
        
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        
        if (choice == 1) {
            string desc;
            cout << "Enter task description: ";
            getline(cin, desc);
            add_task(desc);
        } else if (choice == 2) {
            int task_id;
            string new_desc;
            cout << "Enter task ID to edit: ";
            cin >> task_id;
            cin.ignore();
            cout << "Enter new task description: ";
            getline(cin, new_desc);
            edit_task(task_id, new_desc);
        } else if (choice == 3) {
            int task_id;
            cout << "Enter task ID to complete: ";
            cin >> task_id;
            complete_task(task_id);
        } else if (choice == 4) {
            view_tasks();
        } else if (choice == 5) {
            view_history();
        } else if (choice == 6) {
            cout << "Goodbye!" << endl;
            break;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
    return 0;
}


// //gcc -c sqlite3.c -o sqlite3.o
// PS C:\se_ass3> g++ cli.cpp sqlite3.o -o cli
// PS C:\se_ass3> ./cl