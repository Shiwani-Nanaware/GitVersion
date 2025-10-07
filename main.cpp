#include <iostream>
#include <string>
#include <stack>
#include <queue>
using namespace std;

// -------------------- BST for Searching --------------------
struct BSTNode {
    string key;
    BSTNode *left, *right;
    BSTNode(string k) : key(k), left(NULL), right(NULL) {}
};

class BST {
public:
    BSTNode* root;
    BST() : root(NULL) {}

    BSTNode* insert(BSTNode* node, string key) {
        if (!node) return new BSTNode(key);
        if (key < node->key)
            node->left = insert(node->left, key);
        else if (key > node->key)
            node->right = insert(node->right, key);
        return node;
    }

    bool search(BSTNode* node, string key) {
        if (!node) return false;
        if (node->key == key) return true;
        if (key < node->key) return search(node->left, key);
        else return search(node->right, key);
    }

    void inorder(BSTNode* node) {
        if (!node) return;
        inorder(node->left);
        cout << node->key << " ";
        inorder(node->right);
    }
};

// -------------------- Linked List Structures --------------------
struct File {
    string name;
    string content;
    File* next;
};

struct Repository {
    string repoName;
    File* fileHead;
    queue<string> tasks;
    Repository* next;
};

// -------------------- GitHub Class --------------------
class GitHub {
private:
    Repository* head;
    stack<string> undoStack;
    stack<string> redoStack;
    BST repoBST;  // For searching repositories

public:
    GitHub() { head = NULL; }

    void clearRedo() {
        while (!redoStack.empty()) redoStack.pop();
    }

    // -------------------- Repository Operations --------------------
    void createRepository(string name) {
        if (repoBST.search(repoBST.root, name)) {
            cout << " Repository already exists!\n";
            return;
        }
        Repository* newRepo = new Repository{name, NULL, queue<string>(), head};
        head = newRepo;
        repoBST.root = repoBST.insert(repoBST.root, name);
        undoStack.push("Created Repository: " + name);
        clearRedo();
        cout << " Repository '" << name << "' created.\n";
    }

    void deleteRepository(string name) {
        Repository* temp = head, *prev = NULL;
        while (temp && temp->repoName != name) {
            prev = temp;
            temp = temp->next;
        }
        if (!temp) {
            cout << " Repository not found.\n";
            return;
        }
        if (prev) prev->next = temp->next;
        else head = temp->next;
        undoStack.push("Deleted Repository: " + name);
        clearRedo();
        delete temp;
        cout << " Repository deleted successfully.\n";
    }

    void editRepository(string oldName, string newName) {
        Repository* temp = head;
        while (temp) {
            if (temp->repoName == oldName) {
                temp->repoName = newName;
                undoStack.push("Renamed Repository: " + oldName + " → " + newName);
                clearRedo();
                cout << " Repository renamed.\n";
                return;
            }
            temp = temp->next;
        }
        cout << " Repository not found.\n";
    }

    void showRepositories() {
        if (!head) {
            cout << " No repositories.\n";
            return;
        }
        Repository* temp = head;
        cout << "\n=== Repositories ===\n";
        while (temp) {
            cout << "- " << temp->repoName << endl;
            temp = temp->next;
        }
    }

    Repository* getRepository(string name) {
        Repository* temp = head;
        while (temp) {
            if (temp->repoName == name) return temp;
            temp = temp->next;
        }
        return NULL;
    }

    // -------------------- File Operations --------------------
    void createFile(Repository* repo, string fileName, string content, BST &fileBST) {
        if (fileBST.search(fileBST.root, fileName)) {
            cout << " File already exists!\n";
            return;
        }
        File* newFile = new File{fileName, content, repo->fileHead};
        repo->fileHead = newFile;
        fileBST.root = fileBST.insert(fileBST.root, fileName);
        undoStack.push("Created File: " + fileName + " in " + repo->repoName);
        clearRedo();
        cout << " File '" << fileName << "' created.\n";
    }

    void deleteFile(Repository* repo, string fileName) {
        File* temp = repo->fileHead, *prev = NULL;
        while (temp && temp->name != fileName) {
            prev = temp;
            temp = temp->next;
        }
        if (!temp) {
            cout << " File not found.\n";
            return;
        }
        if (prev) prev->next = temp->next;
        else repo->fileHead = temp->next;
        undoStack.push("Deleted File: " + fileName + " from " + repo->repoName);
        clearRedo();
        delete temp;
        cout << " File deleted successfully.\n";
    }

    void editFile(Repository* repo, string fileName, string newContent) {
        File* temp = repo->fileHead;
        while (temp) {
            if (temp->name == fileName) {
                temp->content = newContent;
                undoStack.push("Edited File: " + fileName + " in " + repo->repoName);
                clearRedo();
                cout << " File edited successfully.\n";
                return;
            }
            temp = temp->next;
        }
        cout << " File not found.\n";
    }

    void showFiles(Repository* repo) {
        if (!repo->fileHead) {
            cout << " No files in this repository.\n";
            return;
        }
        File* temp = repo->fileHead;
        cout << "\nFiles in '" << repo->repoName << "':\n";
        while (temp) {
            cout << "- " << temp->name << endl;
            temp = temp->next;
        }
    }

    // -------------------- Task Queue --------------------
    void addTask(Repository* repo, string task) {
        repo->tasks.push(task);
        undoStack.push("Added Task: " + task + " in " + repo->repoName);
        clearRedo();
        cout << " Task added.\n";
    }

    void viewTasks(Repository* repo) {
        if (repo->tasks.empty()) {
            cout << " No tasks.\n";
            return;
        }
        queue<string> temp = repo->tasks;
        cout << "\nTasks in '" << repo->repoName << "':\n";
        while (!temp.empty()) {
            cout << "- " << temp.front() << endl;
            temp.pop();
        }
    }

    void removeTask(Repository* repo) {
        if (repo->tasks.empty()) {
            cout << " No tasks to remove.\n";
            return;
        }
        string task = repo->tasks.front();
        repo->tasks.pop();
        undoStack.push("Removed Task: " + task + " from " + repo->repoName);
        clearRedo();
        cout << " Task removed.\n";
    }

    // -------------------- Search using BST --------------------
    void searchRepository(string name) {
        bool found = repoBST.search(repoBST.root, name);
        cout << (found ? " Repository found.\n" : " Repository not found.\n");
    }

    void searchFile(Repository* repo, string fileName) {
        BST fileBST;
        File* temp = repo->fileHead;
        while (temp) {
            fileBST.root = fileBST.insert(fileBST.root, temp->name);
            temp = temp->next;
        }
        bool found = fileBST.search(fileBST.root, fileName);
        cout << (found ? " File found.\n" : " File not found.\n");
    }

    // -------------------- Undo/Redo & History --------------------
    void undo() {
        if (undoStack.empty()) {
            cout << " No action to undo.\n";
            return;
        }
        string action = undoStack.top();
        undoStack.pop();
        redoStack.push(action);
        cout << " Undo: " << action << endl;
    }

    void redo() {
        if (redoStack.empty()) {
            cout << " No action to redo.\n";
            return;
        }
        string action = redoStack.top();
        redoStack.pop();
        undoStack.push(action);
        cout << " Redo: " << action << endl;
    }

    void showHistory() {
        if (undoStack.empty()) {
            cout << " No commit history.\n";
            return;
        }
        stack<string> temp = undoStack;
        cout << "\n=== Commit History ===\n";
        while (!temp.empty()) {
            cout << temp.top() << endl;
            temp.pop();
        }
    }
};

// -------------------- MAIN --------------------
int main() {
    GitHub git;
    int choice;
    string repoName, newRepoName, fileName, content, task;

    cout << "=== Mini GitHub using Linked List, Stack, Queue & BST ===\n";

    do {
        cout << "\n1. Create Repository\n2. Edit Repository\n3. Delete Repository\n4. Show Repositories";
        cout << "\n5. Manage Repository\n6. Search Repository\n7. Show Commit History\n8. Undo\n9. Redo\n10. Exit";
        cout << "\nEnter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter repository name: ";
                getline(cin, repoName);
                git.createRepository(repoName);
                break;

            case 2:
                cout << "Enter old name: ";
                getline(cin, repoName);
                cout << "Enter new name: ";
                getline(cin, newRepoName);
                git.editRepository(repoName, newRepoName);
                break;

            case 3:
                cout << "Enter repository to delete: ";
                getline(cin, repoName);
                git.deleteRepository(repoName);
                break;

            case 4:
                git.showRepositories();
                break;

            case 5: {
                cout << "Enter repository to manage: ";
                getline(cin, repoName);
                Repository* repo = git.getRepository(repoName);
                if (!repo) {
                    cout << " Repository not found.\n";
                    break;
                }
                BST fileBST;
                int subChoice;
                do {
                    cout << "\n--- Manage Repository: " << repoName << " ---\n";
                    cout << "1. Create File\n2. Edit File\n3. Delete File\n4. Show Files\n5. Add Task\n6. View Tasks\n7. Remove Task\n8. Search File\n9. Back\n";
                    cout << "Enter choice: ";
                    cin >> subChoice;
                    cin.ignore();

                    switch (subChoice) {
                        case 1:
                            cout << "Enter file name: ";
                            getline(cin, fileName);
                            cout << "Enter content: ";
                            getline(cin, content);
                            git.createFile(repo, fileName, content, fileBST);
                            break;
                        case 2:
                            cout << "Enter file name: ";
                            getline(cin, fileName);
                            cout << "Enter new content: ";
                            getline(cin, content);
                            git.editFile(repo, fileName, content);
                            break;
                        case 3:
                            cout << "Enter file name: ";
                            getline(cin, fileName);
                            git.deleteFile(repo, fileName);
                            break;
                        case 4:
                            git.showFiles(repo);
                            break;
                        case 5:
                            cout << "Enter task: ";
                            getline(cin, task);
                            git.addTask(repo, task);
                            break;
                        case 6:
                            git.viewTasks(repo);
                            break;
                        case 7:
                            git.removeTask(repo);
                            break;
                        case 8:
                            cout << "Enter file name to search: ";
                            getline(cin, fileName);
                            git.searchFile(repo, fileName);
                            break;
                    }
                } while (subChoice != 9);
                break;
            }

            case 6:
                cout << "Enter repository name to search: ";
                getline(cin, repoName);
                git.searchRepository(repoName);
                break;

            case 7:
                git.showHistory();
                break;

            case 8:
                git.undo();
                break;

            case 9:
                git.redo();
                break;

            case 10:
                cout << " Exiting Mini GitHub... Goodbye!\n";
                break;
        }

    } while (choice != 10);
}
