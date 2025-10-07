#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <ctime>
using namespace std;

// -------------------- Commit History --------------------
struct Commit
{
    string action;
    string user;
    string date;
    Commit *prev, *next;
    Commit(string a, string u) : action(a), user(u), prev(NULL), next(NULL) 
{
        time_t now = time(0);
        date = ctime(&now);
        date.pop_back(); // remove newline
    }
};

struct CommitHistory 
{
    Commit* head = NULL;
    Commit* tail = NULL;

    void addCommit(string action, string user) 
{
        Commit* c = new Commit(action, user);
        if (!head) head = tail = c;
        else 
        {
            tail->next = c;
            c->prev = tail;
            tail = c;
        }
    }

    void showCommits()
{
        if (!head) { cout << "No commits yet.\n"; return; }
        cout << "\n=== Commit History ===\n";
        Commit* temp = head;
        while (temp) {
            cout << "[" << temp->date << "] " << temp->user << " - " << temp->action << endl;
            temp = temp->next;
        }
    }
};

// -------------------- BST for Repository Searching --------------------
struct BSTNode {
    string key;
    BSTNode *left, *right;
    BSTNode(string k) : key(k), left(NULL), right(NULL) {}
};

class BST {
public:
    BSTNode* root = NULL;

    BSTNode* insert(BSTNode* node, string key) {
        if (!node) return new BSTNode(key);
        if (key < node->key) node->left = insert(node->left, key);
        else if (key > node->key) node->right = insert(node->right, key);
        return node;
    }

    bool search(BSTNode* node, string key) {
        if (!node) return false;
        if (node->key == key) return true;
        if (key < node->key) return search(node->left, key);
        else return search(node->right, key);
    }
};

// -------------------- Linked List for Files & Repositories --------------------
struct File {
    string name, content;
    File* next;
    File(string n, string c) : name(n), content(c), next(NULL) {}
};

struct Repository {
    string repoName;
    File* fileHead;
    queue<string> tasks;
    Repository* next;
    Repository(string n) : repoName(n), fileHead(NULL), next(NULL) {}
};

// -------------------- Operation Struct for Undo/Redo --------------------
struct Operation {
    string type;       // createRepo, deleteRepo, createFile, editFile, deleteFile, addTask, removeTask
    string repoName;
    string fileName;
    string content;
};

// -------------------- GitHub Simulation --------------------
class GitHub {
private:
    Repository* head = NULL;
    BST repoBST;
    stack<Operation> undoStack, redoStack;
    CommitHistory commits;
    string currentUser = "Shiwani";

public:
    // -------------------- Helper Functions --------------------
    Repository* findRepo(string name) {
        Repository* temp = head;
        while (temp) {
            if (temp->repoName == name) return temp;
            temp = temp->next;
        }
        return NULL;
    }

    File* findFile(Repository* repo, string name) {
        File* temp = repo->fileHead;
        while (temp) {
            if (temp->name == name) return temp;
            temp = temp->next;
        }
        return NULL;
    }

    // -------------------- Repository Operations --------------------
    void createRepository(string name) {
        if (repoBST.search(repoBST.root, name)) { cout << "Repository already exists!\n"; return; }
        Repository* newRepo = new Repository(name);
        newRepo->next = head;
        head = newRepo;
        repoBST.root = repoBST.insert(repoBST.root, name);
        commits.addCommit("Created Repository: " + name, currentUser);
        undoStack.push({"deleteRepo", name, "", ""});
        cout << "Repository '" << name << "' created.\n";
    }

    void deleteRepository(string name) {
        Repository* temp = head, *prev = NULL;
        while (temp && temp->repoName != name) { prev = temp; temp = temp->next; }
        if (!temp) { cout << "Repository not found.\n"; return; }
        undoStack.push({"createRepo", temp->repoName, "", ""});
        if (prev) prev->next = temp->next; else head = temp->next;
        commits.addCommit("Deleted Repository: " + name, currentUser);
        delete temp;
        cout << "Repository deleted.\n";
    }

    void showRepositories() {
        if (!head) { cout << "No repositories.\n"; return; }
        Repository* temp = head;
        cout << "\nRepositories:\n";
        while (temp) { cout << "- " << temp->repoName << endl; temp = temp->next; }
    }

    // -------------------- File Operations --------------------
    void createFile(Repository* repo, string fileName, string content) {
        if (findFile(repo, fileName)) { cout << "File already exists!\n"; return; }
        File* newFile = new File(fileName, content);
        newFile->next = repo->fileHead;
        repo->fileHead = newFile;
        commits.addCommit("Created File: " + fileName, currentUser);
        undoStack.push({"deleteFile", repo->repoName, fileName, ""});
        cout << "File created successfully.\n";
    }

    void deleteFile(Repository* repo, string fileName) {
        File* temp = repo->fileHead, *prev = NULL;
        while (temp && temp->name != fileName) { prev = temp; temp = temp->next; }
        if (!temp) { cout << "File not found.\n"; return; }
        undoStack.push({"createFile", repo->repoName, temp->name, temp->content});
        if (prev) prev->next = temp->next; else repo->fileHead = temp->next;
        commits.addCommit("Deleted File: " + fileName, currentUser);
        delete temp;
        cout << "File deleted successfully.\n";
    }

    void editFile(Repository* repo, string fileName, string newContent) {
        File* temp = findFile(repo, fileName);
        if (!temp) { cout << "File not found.\n"; return; }
        undoStack.push({"editFile", repo->repoName, fileName, temp->content});
        temp->content = newContent;
        commits.addCommit("Edited File: " + fileName, currentUser);
        cout << "File edited successfully.\n";
    }

    void showFiles(Repository* repo) {
        if (!repo->fileHead) { cout << "No files.\n"; return; }
        File* temp = repo->fileHead;
        cout << "\nFiles in " << repo->repoName << ":\n";
        while (temp) { cout << "- " << temp->name << endl; temp = temp->next; }
    }

    // -------------------- Task Operations --------------------
    void addTask(Repository* repo, string task) {
        repo->tasks.push(task);
        commits.addCommit("Added Task: " + task, currentUser);
        undoStack.push({"removeTask", repo->repoName, "", task});
        cout << "Task added.\n";
    }

    void removeTask(Repository* repo) {
        if (repo->tasks.empty()) { cout << "No tasks.\n"; return; }
        string t = repo->tasks.front();
        repo->tasks.pop();
        commits.addCommit("Removed Task: " + t, currentUser);
        undoStack.push({"addTask", repo->repoName, "", t});
        cout << "Task removed.\n";
    }

    void viewTasks(Repository* repo) {
        if (repo->tasks.empty()) { cout << "No tasks.\n"; return; }
        queue<string> q = repo->tasks;
        cout << "\nTasks in " << repo->repoName << ":\n";
        while (!q.empty()) { cout << "- " << q.front() << endl; q.pop(); }
    }

    // -------------------- Undo/Redo --------------------
    void undo() {
        if (undoStack.empty()) { cout << "Nothing to undo.\n"; return; }
        Operation op = undoStack.top(); undoStack.pop();
        redoStack.push(op);

        Repository* r = findRepo(op.repoName);
        if (op.type == "deleteRepo") deleteRepository(op.repoName);
        else if (op.type == "createRepo") createRepository(op.repoName);
        else if (op.type == "deleteFile" && r) deleteFile(r, op.fileName);
        else if (op.type == "createFile" && r) createFile(r, op.fileName, op.content);
        else if (op.type == "editFile" && r) editFile(r, op.fileName, op.content);
        else if (op.type == "addTask" && r) addTask(r, op.content);
        else if (op.type == "removeTask" && r) removeTask(r);
        cout << "Undo performed.\n";
    }

    void redo() {
        if (redoStack.empty()) { cout << "Nothing to redo.\n"; return; }
        Operation op = redoStack.top(); redoStack.pop();
        undoStack.push(op);
        cout << "Redo performed. Reapply action: " << op.type << endl;
    }

    void showHistory() { commits.showCommits(); }
};

// -------------------- MAIN --------------------
int main() {
    GitHub git;
    int choice;
    string repoName, fileName, content, task;

    do {
        cout << "\n=== Mini GitHub ===\n";
        cout << "1. Create Repository\n2. Delete Repository\n3. Show Repositories\n4. Manage Repository\n5. Undo\n6. Redo\n7. Show History\n8. Exit\nEnter choice: ";
        cin >> choice; cin.ignore();

        switch(choice) {
        case 1:
            cout << "Repository name: "; getline(cin, repoName);
            git.createRepository(repoName); break;
        case 2:
            cout << "Repository name: "; getline(cin, repoName);
            git.deleteRepository(repoName); break;
        case 3:
            git.showRepositories(); break;
        case 4: {
            cout << "Repository to manage: "; getline(cin, repoName);
            Repository* repo = git.findRepo(repoName);
            if (!repo) { cout << "Repository not found.\n"; break; }

            int sub;
            do {
                cout << "\n--- Manage " << repoName << " ---\n";
                cout << "1. Create File\n2. Edit File\n3. Delete File\n4. Show Files\n";
                cout << "5. Add Task\n6. Remove Task\n7. View Tasks\n8. Undo\n9. Redo\n10. Back\nEnter: ";
                cin >> sub; cin.ignore();
                switch(sub) {
                    case 1: cout << "File name: "; getline(cin, fileName);
                            cout << "Content: "; getline(cin, content);
                            git.createFile(repo, fileName, content); break;
                    case 2: cout << "File name: "; getline(cin, fileName);
                            cout << "New content: "; getline(cin, content);
                            git.editFile(repo, fileName, content); break;
                    case 3: cout << "File name: "; getline(cin, fileName);
                            git.deleteFile(repo, fileName); break;
                    case 4: git.showFiles(repo); break;
                    case 5: cout << "Task: "; getline(cin, task);
                            git.addTask(repo, task); break;
                    case 6: git.removeTask(repo); break;
                    case 7: git.viewTasks(repo); break;
                    case 8: git.undo(); break;
                    case 9: git.redo(); break;
                }
            } while(sub != 10);
            break;
        }
        case 5: git.undo(); break;
        case 6: git.redo(); break;
        case 7: git.showHistory(); break;
        case 8: cout << "Exiting...\n"; break;
        }
    } while(choice != 8);
}
