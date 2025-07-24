#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

class Task {
private:
    static int nextId;
    int id;
    string description;
    bool completed;

public:
    Task(const string& desc) 
        : id(nextId++), description(desc), completed(false) {}
    
    Task(int taskId, const string& desc, bool comp)
        : id(taskId), description(desc), completed(comp) { 
        if (taskId >= nextId) nextId = taskId + 1;
    }

    int getId() const { return id; }
    string getDescription() const { return description; }
    bool isCompleted() const { return completed; }

    void setDescription(const string& desc) { description = desc; }
    void markCompleted() { completed = true; }
    void markPending() { completed = false; }

    void display() const {
        cout << setw(4) << id << " | "
                  << (completed ? "Done" : "Pending") << " | " 
                  << description << endl;
    }

    string toString() const {
        return to_string(id) + "," + description + "," + 
               (completed ? "1" : "0");
    }

    static Task fromString(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> tokens;
        
        while (getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() == 3) {
            return Task(stoi(tokens[0]), tokens[1], tokens[2] == "1");
        }
        return Task("");
    }
};

int Task::nextId = 1;

class TaskManager {
private:
    vector<Task> tasks;
    string filename;

public:
    TaskManager(const string& file = "tasks.txt") : filename(file) {
        loadTasks();
    }

    ~TaskManager() {
        saveTasks();
    }

    void addTask() {
        string description;
        
        cout << "\nEnter task description: ";
        cin.ignore();
        getline(cin, description);
        
        if (description.empty()) {
            cout << "Task description cannot be empty!\n";
            return;
        }

        tasks.emplace_back(description);
        cout << "Task added successfully!\n";
    }

    void markTaskCompleted() {
        if (tasks.empty()) {
            cout << "No tasks available!\n";
            return;
        }

        viewPendingTasks();
        
        int taskId;
        cout << "\nEnter task ID to mark as completed: ";
        cin >> taskId;

        auto it = find_if(tasks.begin(), tasks.end(),
                              [taskId](const Task& t) { return t.getId() == taskId; });

        if (it != tasks.end()) {
            if (it->isCompleted()) {
                cout << "Task is already completed!\n";
            } else {
                it->markCompleted();
                cout << "Task marked as completed!\n";
            }
        } else {
            cout << "Task with ID " << taskId << " not found!\n";
        }
    }

    void viewAllTasks() const {
        if (tasks.empty()) {
            cout << "\nNo tasks available!\n";
            return;
        }

        cout << "\nALL TASKS:\n";
        cout << string(40, '=') << endl;
        cout << setw(4) << "ID" << " | "
                  << "STATUS | DESCRIPTION" << endl;
        cout << string(40, '-') << endl;

        for (const auto& task : tasks) {
            task.display();
        }
        cout << string(40, '=') << endl;
    }

    void viewPendingTasks() const {
        vector<Task> pendingTasks;
        copy_if(tasks.begin(), tasks.end(), back_inserter(pendingTasks),
                    [](const Task& t) { return !t.isCompleted(); });

        if (pendingTasks.empty()) {
            cout << "\nNo pending tasks! Great job!\n";
            return;
        }

        cout << "\nPENDING TASKS:\n";
        cout << string(40, '=') << endl;
        cout << setw(4) << "ID" << " | "
                  << "STATUS | DESCRIPTION" << endl;
        cout << string(40, '-') << endl;

        for (const auto& task : pendingTasks) {
            task.display();
        }
        cout << string(40, '=') << endl;
    }

    void viewCompletedTasks() const {
        vector<Task> completedTasks;
        copy_if(tasks.begin(), tasks.end(), back_inserter(completedTasks),
                    [](const Task& t) { return t.isCompleted(); });

        if (completedTasks.empty()) {
            cout << "\nNo completed tasks yet!\n";
            return;
        }

        cout << "\nCOMPLETED TASKS:\n";
        cout << string(40, '=') << endl;
        cout << setw(4) << "ID" << " | "
                  << "STATUS | DESCRIPTION" << endl;
        cout << string(40, '-') << endl;

        for (const auto& task : completedTasks) {
            task.display();
        }
        cout << string(40, '=') << endl;
    }

    void deleteTask() {
        if (tasks.empty()) {
            cout << "No tasks available!\n";
            return;
        }

        viewAllTasks();
        
        int taskId;
        cout << "\nEnter task ID to delete: ";
        cin >> taskId;

        auto it = find_if(tasks.begin(), tasks.end(),
                              [taskId](const Task& t) { return t.getId() == taskId; });

        if (it != tasks.end()) {
            tasks.erase(it);
            cout << "Task deleted successfully!\n";
        } else {
            cout << "Task with ID " << taskId << " not found!\n";
        }
    }

    void showStatistics() const {
        int total = tasks.size();
        int completed = count_if(tasks.begin(), tasks.end(),
                                     [](const Task& t) { return t.isCompleted(); });
        int pending = total - completed;

        cout << "\nTASK STATISTICS:\n";
        cout << string(30, '=') << endl;
        cout << "Total Tasks: " << total << endl;
        cout << "Completed: " << completed << endl;
        cout << "Pending: " << pending << endl;
        
        if (total > 0) {
            double completionRate = (double)completed / total * 100;
            cout << "Completion Rate: " << fixed << setprecision(1) 
                      << completionRate << "%" << endl;
        }
        cout << string(30, '=') << endl;
    }

    void saveTasks() const {
        ofstream file(filename);
        if (file.is_open()) {
            for (const auto& task : tasks) {
                file << task.toString() << endl;
            }
            file.close();
        }
    }

    void loadTasks() {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    Task task = Task::fromString(line);
                    if (!task.getDescription().empty()) {
                        tasks.push_back(task);
                    }
                }
            }
            file.close();
        }
    }
};

class MenuSystem {
private:
    TaskManager taskManager;

public:
    void displayMenu() const {
        cout << "\nTASK MANAGER\n";
        cout << string(40, '=') << endl;
        cout << "1. Add Task\n";
        cout << "2. Mark Task as Completed\n";
        cout << "3. View All Tasks\n";
        cout << "4. View Pending Tasks\n";
        cout << "5. View Completed Tasks\n";
        cout << "6. Delete Task\n";
        cout << "7. Show Statistics\n";
        cout << "8. Exit\n";
        cout << string(40, '=') << endl;
        cout << "Choose an option (1-8): ";
    }

    void run() {
        int choice;
        
        do {
            displayMenu();
            cin >> choice;

            switch (choice) {
                case 1:
                    taskManager.addTask();
                    break;
                case 2:
                    taskManager.markTaskCompleted();
                    break;
                case 3:
                    taskManager.viewAllTasks();
                    break;
                case 4:
                    taskManager.viewPendingTasks();
                    break;
                case 5:
                    taskManager.viewCompletedTasks();
                    break;
                case 6:
                    taskManager.deleteTask();
                    break;
                case 7:
                    taskManager.showStatistics();
                    break;
                case 8:
                    cout << "\nThank you for using Task Manager! Goodbye!\n";
                    break;
                default:
                    cout << "Invalid choice! Please select 1-8.\n";
            }
                if (choice != 8) {
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();

            }

        } while (choice != 8);
    }
};

int main() {
    MenuSystem menu;
    menu.run();
    return 0;
}