#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <allegro5/allegro.h>
using namespace std;

/*int* classes_by_id;
int* sprites_by_id;
char** names_by_id;
ALLEGRO_USTR** descs_by_id;
int** other_by_id;*/

class ObjectReference {
    private:
        ObjectReference* rnode;
        ObjectReference* lnode;
    public:
        int id;
        int class_type;
        int sprite_id;
        char* name;
        ALLEGRO_USTR* desc;
        int* other;
        ObjectReference(int i) : id(i) { rnode = NULL; lnode = NULL; }
        ObjectReference* search_node(int);
        ObjectReference* insert_node(ObjectReference*);
};

ObjectReference* ObjectReference::search_node(int i) {
    if (i < id) {
        return lnode;
    } else if (i > id) {
        return rnode;
    }
    return NULL;
}

ObjectReference* ObjectReference::insert_node(ObjectReference* oref) {
    if (oref->id < id) {
        if (lnode == NULL) {
            lnode = oref;
        } else {
            return lnode;
        }
    } else if (oref->id > id) {
        if (rnode == NULL) {
            rnode = oref;
        } else {
            return rnode;
        }
    }
    return NULL;
}

ObjectReference* root = NULL;

void load_objects() {
    cout << "[IO] Loading objects...\n";
    string line;
    char* s = new char[8];
    ifstream inpt("data/objects.txt");
    if (inpt.is_open()) {
        /*int len = 0;
        while (getline(inpt, line)) {
            if (line[0] != '/') {
                int index = line.find(' ');
                line.copy(s, index);
                int id = atoi(s);
                memset(&s[0], 0, sizeof(s));
                if (id > len) { len = id; }
            }
        }
        classes_by_id = new int[len];
        sprites_by_id = new int[len];
        names_by_id = new char*[len];
        descs_by_id = new ALLEGRO_USTR*[len];
        other_by_id = new int*[len];
        inpt.clear();
        inpt.seekg(0, ios::beg);*/
        while (getline(inpt, line)) {
            if (line[0] != '/') {
                cout << "[IO] << " << line << " >>\n";
                ObjectReference* oref;

                int index = line.find(' ')+1;
                line.copy(s, index-1);
                //int id = atoi(s)-1;
                oref = new ObjectReference(atoi(s));
                memset(&s[0], 0, sizeof(s));

                line.copy(s, line.find(' ', index)-index, index);
                //classes_by_id[id] = atoi(s);
                oref->class_type = atoi(s);
                memset(&s[0], 0, sizeof(s));
                index = line.find(' ', index)+1;

                line.copy(s, line.find(' ', index)-index, index);
                //sprites_by_id[id] = atoi(s);
                oref->sprite_id = atoi(s);
                memset(&s[0], 0, sizeof(s));
                index = line.find(' ', index)+2;

                //names_by_id[id] = new char[line.find('"', index)-index+1];
                oref->name = new char[line.find('"', index)-index+1];
                oref->name[line.find('"', index)-index] = '\0'; // if you don't do this weird shit happens. because.
                cout << "[IO] len = " << line.find('"', index)-index << "\n";
                memset(&(oref->name)[0], 0, sizeof(oref->name));
                line.copy(oref->name, line.find('"', index)-index, index);
                index = line.find('"', index)+3;
                cout << "[IO] name: " << oref->name << "\n";

                char* q = new char[line.find('"', index)-index+1];
                q[line.find('"', index)-index] = '\0'; // same as above. WHY YOU DO THIS
                line.copy(q, line.find('"', index)-index, index);
                //descs_by_id[id] = al_ustr_new(q);
                oref->desc = al_ustr_new(q);
                index = line.find('"', index)+2;
                cout << "[IO] desc: " << al_cstr(oref->desc) << "\n";

                int additional = 0;
                switch (oref->class_type) {
                    case 2:
                    case 4:
                        additional = 2;
                        break;
                    case 3:
                        additional = 1;
                        break;
                    case 7:
                        additional = 5;
                        break;
                }
                //other_by_id[id] = new int[additional];
                oref->other = new int[additional];
                for (int i = 0; i < additional; i++) {
                    if (i == additional-1) {
                        line.copy(s, line.length()-index, index);
                        //other_by_id[id][i] = atoi(s);
                        oref->other[i] = atoi(s);
                        cout << "[IO] other[" << i << "] = " << oref->other[i] << "\n";
                        memset(&s[0], 0, sizeof(s));
                    } else {
                        line.copy(s, line.find(' ', index)-index, index);
                        //other_by_id[id][i] = atoi(s);
                        oref->other[i] = atoi(s);
                        cout << "[IO] other[" << i << "] = " << oref->other[i] << "\n";
                        memset(&s[0], 0, sizeof(s));
                        index = line.find(' ', index)+1;
                    }
                }

                // insert reference into binary tree
                if (root == NULL) {
                    root = oref;
                } else {
                    ObjectReference* cnode = root;
                    while (cnode = cnode->insert_node(oref)) {}
                }
            }
        }
        inpt.close();
    }
    delete s;
}

ObjectReference* search_nodes(int id) {
    ObjectReference* cnode = root;
    do {
        if (cnode == NULL) { return NULL; }
        if (cnode->id == id) { return cnode; }
    } while (cnode = cnode->search_node(id));
}

int get_class_by_id(int id) {
    ObjectReference* node = search_nodes(id);
    if (node == NULL) {
        return 1;
    } else {
        return node->class_type;
    }
    /*if (id > sizeof(classes_by_id) || id <= 0) {
        return 1;
    } else {
        return classes_by_id[id-1];
    }*/
}

int get_sprite_by_id(int id) {
    ObjectReference* node = search_nodes(id);
    if (node == NULL) {
        return 0;
    } else {
        return node->sprite_id;
    }
    /*if (id > sizeof(sprites_by_id) || id <= 0) {
        return 0;
    } else {
        return sprites_by_id[id-1];
    }*/
}

char* get_name_by_id(int id) {
    ObjectReference* node = search_nodes(id);
    if (node == NULL) {
        return "perfectly generic object";
    } else {
        cout << "[IO] id = " << id << ", name = " << node->name << "\n";
        return node->name;
    }
    /*if (id > sizeof(names_by_id) || id <= 0) {
        return "perfectly generic object";
    } else {
        return names_by_id[id-1];
    }*/
}

ALLEGRO_USTR* get_desc_by_id(int id) {
    ObjectReference* node = search_nodes(id);
    if (node == NULL) {
        return al_ustr_new("A perfectly generic object. You seriously don't think there could be a more generic object. "
            "(Also, the fact that you're looking at it means SOMETHING fucked up.)");
    } else {
        return node->desc;
    }
    /*if (id > sizeof(names_by_id) || id <= 0) {
        return al_ustr_new("A perfectly generic object. You seriously don't think there could be a more generic object. "
            "(Also, the fact that you're looking at it means SOMETHING fucked up.)");
    } else {
        return descs_by_id[id-1];
    }*/
}

int* get_params_by_id(int id) {
    ObjectReference* node = search_nodes(id);
    if (node == NULL) {
        return NULL;
    } else {
        return node->other;
    }
    /*if (id > sizeof(other_by_id) || id <= 0) {
        return NULL;
    } else {
        return other_by_id[id-1];
    }*/
}

/*
void save() {
    ofstream output;
    output.open("filename.txt", ios::trunc);
    if (output.is_open()) {
        output << "I don't know what goes here yet!!!";
        // save code goes here
        output.close();
    }
}

void load() {
    ifstream input;
    input.open("filename.txt");
    if (input.is_open()) {
        while (getline(input,line)) {

        }
        // load code goes here
        input.close();
    }
}
*/
