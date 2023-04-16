#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Node {
public:
    string name;
    bool usable;
    vector<Node*> dependents;

    Node(string name) {
        this->name = name;
        this->usable = true;
    }

    bool getUsable() {
        return this->usable;
    }

    void markUnusable() {
        this->usable = false;
    }

    void addDependent(Node* node) {
        dependents.push_back(node);
    }

    void removeDependent(Node* node) {
        int i = 0;
        for (Node* dep : node->dependents) {
            if (dep == node) {
                dependents.erase(dependents.begin() + i);
            }
            i++;
        }
    }

};

class ResourceManager {
public:

    unordered_map<string, Node*> nodes;

    // add a node to the map
    void addNode(string name) {
        nodes[name] = new Node(name);
    }

    // add link between item and resource
    void addLink(string from, string to) {
        nodes[from]->addDependent(nodes[to]);
    }


    void removeNode(string name) {

        Node* node = nodes[name];
        node->markUnusable();
        updateUsability();
        for (Node* dep : node->dependents) {
            dep->removeDependent(node);
        }

        nodes.erase(name);
        delete node;
    }

    void updateUsability() {
        for (auto& kv : nodes) {
            Node* node = kv.second;
            if (!node->getUsable()) {
                continue; // skip unusable nodes
            }
            bool usable = true;
            for (Node* dep : node->dependents) {
                if (!dep->usable) {
                    usable = false;
                    break;
                }
            }
            node->usable = usable;
        }
    }

    void printGraph() {
        for (auto& kv : nodes) {
            Node* node = kv.second;
            cout << node->name << ": ";
            if (node->getUsable()) {
                for (Node* dep : node->dependents) {
                    cout << dep->name << " ";
                }
            }
            else {
                cout << "X_X";
            }
            cout << "\n";
        }
    }

    void printUsability() {
        for (auto& kv : nodes) {
            Node* node = kv.second;
            if (node->getUsable()) {
                cout << node->name << " is usable \n";
            }
            else
            {
                cout << node->name << " is not usable \n";
            }
        }
    }
};

int main() {

    // variables
    ResourceManager rm;

    ifstream file("resource.txt");
    if (!file.is_open()) {
        throw runtime_error("Unable to open file");
    }

    string from = "";
    string to = "";
    string userInput = "";

    // add the items 
    while (file >> from >> to) {
        if (rm.nodes.find(from) == rm.nodes.end()) {
            rm.addNode(from);
        }
        if (rm.nodes.find(to) == rm.nodes.end()) {
            rm.addNode(to);
        }
        rm.addLink(from, to);
    }

    file.close();


    // user response time
    while (true) {
        // zero out variables to prevent errors
        userInput = "0";
        to = "0";
        from = "0";


        cout << "\n";
        rm.updateUsability();
        rm.printGraph();
        cout << "---------\n";
        rm.printUsability();
        cout << "---------\n";

        cout << "\n";
        cout << "What would you like to do?\n";
        cout << "To quit, type 'q' \n";
        cout << "To add new resources, type 'r' \n";
        cout << "To add new dependencies, type 'd' \n";
        cout << "To remove resources, type the name of the resource\n\n";

        cin >> userInput;
        cout << "\n";
        if (userInput == "q") { // Quit scenario
            break;
        }
        else if (userInput == "r") { //add resource

            cout << "What is the name of the resource you would like to add ? \n";
            cin >> userInput;

            if (rm.nodes.find(userInput) == rm.nodes.end()) { // resource does not exist
                rm.addNode(userInput);
                cout << userInput << " has been added!\n";
            }
            else { // resource exists already
                cout << userInput << " already exists\n";
            }
        }
        else if (userInput == "d") { // add dependency
            cout << "What item would you like to add a dependency to?\n";
            cin >> userInput;

            if (rm.nodes.find(userInput) == rm.nodes.end()) { // Main resource does not exist
                cout << userInput << " does not exist. Adding it now.\n";
                rm.addNode(userInput);
            }

            from = userInput;

            cout << "What item would you like " << from << " to be dependant on?\n";
            cin >> userInput;

            if (rm.nodes.find(userInput) == rm.nodes.end()) { //dependant resource does not exist
                cout << "That item does not exist. Adding it now.\n";
                rm.addNode(userInput);
            }

            to = userInput;

            cout << "Adding Link between " << from << " and " << to << "\n";
            rm.addLink(from, to);
            cout << from << "is now dependant on " << to << "\n";
        }
        else if (rm.nodes.find(userInput) != rm.nodes.end()) { // Remove resource
            rm.removeNode(userInput);
        }
    }
    return 0;
}