#include <regex>
#include "loader.h"

using namespace std;


FileLoader::FileLoader(Module* m) {
    module = m;
    add_key(string("this"));
    add_key(string("inherit"));
    add_key(string("Form"));
    add_key(string("Widget"));
    add_key(string("Menu"));
    add_key(string("Object"));
    add_key(string("Item"));
    add_key(string("Container"));
    add_key(string("Entity"));
    add_key(string("Attired"));
}

unsigned int FileLoader::add_key(string key) {
    unsigned int ikey = (unsigned int)keymap.size();
    keymap.insert( {{ key, ikey }} );
    return ikey;
}

unsigned int FileLoader::get_key(string key) {
    unordered_map<string, unsigned int>::iterator iter = keymap.find(key);
    if (iter == keymap.end()) {
        return add_key(key);
    } else {
        return iter->second;
    }
}


void FileLoader::load_define(Parametrized* data) {
    string params("params");
    string scripts("scripts");

    EndStatement ec;
    while ((ec = get_line()) != EndStatement::END_OF_FILE) {
        if (ec == EndStatement::CLOSE_BRACKET) { return; }
        if (ec == EndStatement::OPEN_BRACKET) {
            if (line.find(params) != string::npos) {
                load_params(data);
            } else if (line.find(scripts) != string::npos) {
                load_scripts(data);
            }
        }
    }
    throw string("Expected '}' before end of file.");
}

void FileLoader::load_params(Parametrized* data) {
    smatch match;
    regex assignment("(\w[\w\d]*)(\s*=\s*(.*))?");

    EndStatement ec;
    while ((ec = get_line()) != EndStatement::END_OF_FILE) {
        if (ec == EndStatement::CLOSE_BRACKET) { return; }
        if (ec == EndStatement::COMMA) {
            if (regex_match(line, match, assignment)) {
                unsigned int key = get_key(match.str(1));
                load_param_assignment(data, key, match.str(3));
            }
        }
    }
    throw string("Expected '}' before end of file.");
}

void FileLoader::load_param_assignment(Parametrized* data, unsigned int key, string substring) {
    if (substring.length() == 0) {
        data->set_param<Param*>(key, nullptr);
        return;
    }
    // do other stuff
}

void FileLoader::load_scripts(Parametrized* data) {
    smatch match;
    regex scr_decl("(\w[\w\d]*)");

    EndStatement ec;
    while ((ec = get_line()) != EndStatement::END_OF_FILE) {
        if (ec == EndStatement::CLOSE_BRACKET) { return; }
        if (ec == EndStatement::OPEN_BRACKET) {
            if (regex_match(line, match, scr_decl)) {
                unsigned int key = get_key(match.str(1));
                Script* scr = new Script();
                load_script(scr);
                //data->set_param(key, shared_ptr<Script>(scr));
            } else {
                throw string("Expected declaration of script name.");
            }
        } else {
            throw string("Expected declaration of script.");
        }
    }
    throw string("Expected '}' before end of file.");
}

void FileLoader::load_script(Script* scr) {
    smatch match;
    regex if_decl("\bif\s*\((.*\))");
    regex while_decl("\bwhile\s*\((.*\))");

    EndStatement ec;
    while ((ec = get_line()) != EndStatement::END_OF_FILE) {
        if (ec == EndStatement::CLOSE_BRACKET) { return; }
        if (ec == EndStatement::OPEN_BRACKET) {
            if (regex_match(line, match, if_decl)) {
                //load_cond_script(scr, match.str(1));
            }
        }
    }
    throw string("Expected '}' before end of file.");
}

void FileLoader::load(ALLEGRO_FILE* f) {
    file = f;
    line_count = 0;

    try {
        smatch match;
        regex define("def (\w.\[\w\d]+)");

        EndStatement ec;
        while ((ec = get_line()) != EndStatement::END_OF_FILE) {
            if (regex_match(line, match, define)) {
                unsigned int key = get_key(match.str(1));
                switch (ec) {
                    case EndStatement::OPEN_BRACKET:
                        load_define(module->get_data(key).get());
                    case EndStatement::SEMICOLON:
                        break;
                    default:
                        throw string("Unexpected character: expected ';' or '{'.");
                }
            }
        }
    } catch (string s) {
        // generate error message
    }
}

EndStatement FileLoader::get_line() {
    line.clear();
    char c;
    int comment = 0;
    smatch match;
    regex line_comment("\/\/$");
    regex full_comment("\/\*$");
    regex end_full_comment("\/\*.*\*\/$");

    while (al_fread(file, &c, sizeof(c)) == sizeof(c)) {
        if (c == '\n') { line_count++; }
        if (comment == 1) {
            if (c == '\n' && regex_match(line, match, line_comment)) {
                line.erase(match.position(0));
            }
        } else if (comment == 2) {
            if (c == '/' || c == '*') { line.append(&c); }
            if (regex_match(line, match, end_full_comment)) {
                line.erase(match.position(0));
            }
        } else {
            switch (c) {
                case ';':
                    return EndStatement::SEMICOLON;
                case ',':
                    return EndStatement::COMMA;
                case '{':
                    return EndStatement::OPEN_BRACKET;
                case '}':
                    return EndStatement::CLOSE_BRACKET;
                case '\n':
                    line.append(" ");
                    break;
                default:
                    line.append(&c);
                    break;
            }
            if (regex_match(line, line_comment)) { comment = 1; }
            else if (regex_match(line, full_comment)) { comment = 2; }
        }
    }
    if (al_feof(file)) {
        return EndStatement::END_OF_FILE;
    } else {
        throw string(al_ferrmsg(file));
    }
}
