#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::set<std::string> g_reserved;
std::set<char> g_separation_chars;
std::set<char> g_whitespaces;
std::set<std::string> g_attributes;
std::set<std::string> g_datatypes;
std::map<std::string, std::string> g_datatype_translations;

void init_globals() {
    std::string reserved[] = {
        "module", "interface", "void", "in", "out",
        "error", "bool",
        "char", "byte",
        "short", "word",
        "int", "dword",
        "long", "qword",
        "float", "double",
        "string"
    };
    for (int i = 0; i < sizeof(reserved) / sizeof(std::string); i++) {
        g_reserved.insert(reserved[i]);
    }

    std::string separation_chars = "(){};,/";
    for (int i = 0; i < separation_chars.length(); i++) {
        g_separation_chars.insert(separation_chars[i]);
    }

    std::string whitespaces = " \n\r\t";
    for (int i = 0; i < whitespaces.length(); i++) {
        g_whitespaces.insert(whitespaces[i]);
    }

    std::string datatypes[] = {
        "error", "bool",
        "char", "byte",
        "short", "word",
        "int", "dword",
        "long", "qword",
        "float", "double",
        "string"
    };
    for (int i = 0; i < sizeof(datatypes) / sizeof(std::string); i++) {
        g_datatypes.insert(datatypes[i]);
    }

    g_attributes.insert("in");
    g_attributes.insert("out");

    g_datatype_translations["error"] = "xcp_error_t";
    g_datatype_translations["bool"] = "xcp_bool_t";
    g_datatype_translations["byte"] = "unsigned char";
    g_datatype_translations["word"] = "unsigned short";
    g_datatype_translations["dword"] = "unsigned int";
    g_datatype_translations["string.in"] = "const char *";
    g_datatype_translations["string.out"] = "char *";
}

std::string find_translation(const std::string &datatype, const std::string &attribute) {
    if (g_datatype_translations.count(datatype + "." + attribute) != 0) {
        return g_datatype_translations[datatype + "." + attribute];
    }
    if (g_datatype_translations.count(datatype) != 0) {
        return g_datatype_translations[datatype];
    }
    return datatype;
}

bool is_valid_identifier(const std::string &token) {
    if (g_reserved.count(token) != 0) {
        return false;
    }
    if ((token.length() == 1) && (g_separation_chars.count(token[0]) != 0)) {
        return false;
    }
    for (int i = 0; i < token.length(); i++) {
        char c = token[i];
        if (!((('0' <= c) && (c <= '9')) || (c == '_') || (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')))) {
            return false;
        }
    }
    return true;
}

enum tkstate {
    tkstate_input,
    tkstate_input_or_comment_begin,
    tkstate_comment_line,
    tkstate_comment_block,
    tkstate_comment_block_or_end
};

struct position {
    int line;
    int col;
};

void pos_throw(const position &pos, const std::string &message) {
    std::ostringstream msg;
    msg << message << " at line " << pos.line << " column " << pos.col;
    throw std::runtime_error(msg.str().c_str());
}

void tokenize(std::istream &input, std::vector<std::pair<position, std::string> > &tokens) {
    tkstate state = tkstate_input;
    std::string token = "";
    char ch;
    position pos;
    pos.line = 1;
    pos.col = 1;
    while (input.get(ch).good()) {
        if (ch == '\n') {
            pos.line++;
            pos.col = 0;
        }
        if ((state == tkstate_input) || ((state == tkstate_input_or_comment_begin) && ((ch != '/') && (ch != '*')))) {
            if (state == tkstate_input_or_comment_begin) {
                position ipos = pos;
                ipos.col = ipos.col - 1;
                token = "/";
                tokens.push_back(std::pair<position, std::string>(ipos, token));
                token = "";
                state = tkstate_input;
            }
            if (g_whitespaces.count(ch) != 0) {
                if (token.length() != 0) {
                    position ipos = pos;
                    ipos.col = ipos.col - token.length();
                    tokens.push_back(std::pair<position, std::string>(ipos, token));
                    token = "";
                }
            } else if (g_separation_chars.count(ch) != 0) {
                if (token.length() != 0) {
                    position ipos = pos;
                    ipos.col = ipos.col - token.length();
                    tokens.push_back(std::pair<position, std::string>(ipos, token));
                    token = "";
                }
                if (state == tkstate_input) {
                    if (ch != '/') {
                        token = "";
                        token = token + ch;
                        tokens.push_back(std::pair<position, std::string>(pos, token));
                        token = "";
                    } else {
                        state = tkstate_input_or_comment_begin;
                    }
                }
            } else {
                token = token + ch;
            }
        } else if (state == tkstate_input_or_comment_begin) {
            if (ch == '/') {
                state = tkstate_comment_line;
            } else if (ch == '*') {
                state = tkstate_comment_block;
            }
        } else if (state == tkstate_comment_line) {
            if (ch == '\n') {
                state = tkstate_input;
            }
        } else if (state == tkstate_comment_block) {
            if (ch == '*') {
                state = tkstate_comment_block_or_end;
            }
        } else if (state == tkstate_comment_block_or_end) {
            if (ch == '/') {
                state = tkstate_input;
            } else {
                state = tkstate_comment_block;
            }
        }
        pos.col++;
    }
}

enum synstate {
    synstate_want_semicolon,
    synstate_want_module,
    synstate_want_interface,
    synstate_want_lbracket,
    synstate_want_rbracket_or_void,
    synstate_want_lparen,
    synstate_want_rparen_or_comma,
    synstate_want_attribute,
    synstate_want_datatype,
    synstate_want_identifier
};

enum synlevel {
    synlevel_base,
    synlevel_module,
    synlevel_interface,
    synlevel_message,
    synlevel_argument
};

struct argument_desc {
    std::string attribute;
    std::string datatype;
    std::string name;
};

std::string get_argument_syntax(const argument_desc &argument) {
    std::string syntax = "";
    bool pointer = argument.attribute == "out";
    syntax = syntax + "/* [" + argument.attribute + "] */ ";
    syntax = syntax + find_translation(argument.datatype, argument.attribute);
    syntax = syntax + " " + (pointer ? "*" : "");
    syntax = syntax + argument.name;
    return syntax;
}

struct message_desc {
    std::string name;
    std::vector<argument_desc> arguments;
};

struct interface_desc {
    std::string name;
    std::vector<message_desc> messages;
};

struct module_desc {
    std::string name;
    std::vector<interface_desc> interfaces;
};

void tab(std::ostream &o, int n) {
    for (int i = 0; i < n; i++) {
        o << "    ";
    }
}

void generate_header(const module_desc &m, std::ostream &o) {
    o << "/*" << std::endl;
    o << "** Generated using XCP-IDL compiler. DO NOT EDIT" << std::endl;
    o << "**" << std::endl;
    o << "** " << m.name << ".h: header file for \"" << m.name << "\" XCP module" << std::endl;
    o << "*/" << std::endl;
    o << std::endl;
    o << "#ifndef __XCPMODULE_" << m.name << "_H_INCLUDED" << std::endl;
    o << "#define __XCPMODULE_" << m.name << "_H_INCLUDED" << std::endl;
    o << std::endl;
    o << "#include <xcp.h>" << std::endl;
    o << std::endl;
    o << "#ifdef __cplusplus" << std::endl;
    o << "extern \"C\" {" << std::endl;
    o << "#endif" << std::endl;
    o << std::endl;
    o << "#ifdef _XCP_SERVER_STUB" << std::endl;
    o << "extern xcp_error_t xcp_init_module_" << m.name << "(void);" << std::endl;
    o << "extern xcp_error_t xcp_destroy_module_" << m.name << "(void);" << std::endl;
    o << std::endl;
    o << "extern xcp_error_t XCP_MODULE_" << m.name << "_INTERFACE_ROUTER(unsigned short, xcp_interface_t *);" << std::endl;
    o << "#endif" << std::endl;
    o << std::endl;
    for (int i = 0; i < m.interfaces.size(); i++) {
        const interface_desc &interface = m.interfaces[i];
        o << "/*" << std::endl;
        o << "** " << interface.name << " interface" << std::endl;
        o << "*/" << std::endl;
        o << std::endl;
        o << "#ifdef _XCP_SERVER_STUB" << std::endl;
        for (int j = 0; j < interface.messages.size(); j++) {
            const message_desc &message = interface.messages[j];
            o << "extern xcp_error_t " << interface.name << "_" << message.name << "_s(";
            if (message.arguments.size() == 0) {
                o << "void";
            }
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                o << get_argument_syntax(argument);
                if (k < message.arguments.size() - 1) {
                    o << ", ";
                }
            }
            o << ");" << std::endl;
        }
        o << "#endif" << std::endl;
        o << std::endl;
        for (int j = 0; j < interface.messages.size(); j++) {
            const message_desc &message = interface.messages[j];
            o << "extern xcp_error_t " << interface.name << "_" << message.name << "(xcp_client_t client";
            if (message.arguments.size() > 0) {
                o << ", ";
            }
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                o << get_argument_syntax(argument);
                if (k < message.arguments.size() - 1) {
                    o << ", ";
                }
            }
            o << ");" << std::endl;
        }
        o << std::endl;
    }
    o << "#ifdef __cplusplus" << std::endl;
    o << "}" << std::endl;
    o << "#endif" << std::endl;
    o << std::endl;
    o << "#endif" << std::endl;
}

void generate_client_stub(const module_desc &m, std::ostream &o) {
    o << "/*" << std::endl;
    o << "** Generated using XCP-IDL compiler. DO NOT EDIT" << std::endl;
    o << "**" << std::endl;
    o << "** " << m.name << "_c.c: client stub definition for \"" << m.name << "\" XCP module" << std::endl;
    o << "*/" << std::endl;
    o << std::endl;
    o << "#include \"" << m.name << ".h\"" << std::endl;
    o << std::endl;
    for (int i = 0; i < m.interfaces.size(); i++) {
        const interface_desc &interface = m.interfaces[i];
        for (int j = 0; j < interface.messages.size(); j++) {
            const message_desc &message = interface.messages[j];

            o << "xcp_error_t " << interface.name << "_" << message.name << "(xcp_client_t __client";
            if (message.arguments.size() > 0) {
                o << ", ";
            }
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                o << get_argument_syntax(argument);
                if (k < message.arguments.size() - 1) {
                    o << ", ";
                }
            }
            o << ") {" << std::endl;
            tab(o, 1); o << "xcp_error_t __err;" << std::endl;
            tab(o, 1); o << "xcp_dispatch_t __request, __response;" << std::endl;
            o << std::endl;
            tab(o, 1); o << "__err = xcp_client_invoke(__client, XCP_COMMAND(" << i << ", " << j << "), &__request, &__response);" << std::endl;
            tab(o, 1); o << "if (XCP_FAILED(__err)) {" << std::endl;
            tab(o, 2); o << "return __err;" << std::endl;
            tab(o, 1); o << "}" << std::endl;
            o << std::endl;

            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                if (argument.attribute == "in") {
                    tab(o, 1); o << "xcp_dispatch_put_" << argument.datatype << "(__request, " << argument.name << ");" << std::endl;
                }
            }
            tab(o, 1); o << "__err = xcp_dispatch_commit(__request);" << std::endl;
            tab(o, 1); o << "if (XCP_FAILED(__err)) {" << std::endl;
            tab(o, 2); o << "return __err;" << std::endl;
            tab(o, 1); o << "}" << std::endl;
            o << std::endl;
            tab(o, 1); o << "__err = xcp_dispatch_wait(__response);" << std::endl;
            tab(o, 1); o << "if (XCP_FAILED(__err)) {" << std::endl;
            tab(o, 2); o << "return __err;" << std::endl;
            tab(o, 1); o << "}" << std::endl;
            o << std::endl;
            tab(o, 1); o << "__err = xcp_dispatch_get_error(__response);" << std::endl;
            tab(o, 1); o << "if (XCP_SUCCEEDED(__err)) {" << std::endl;
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                if (argument.attribute == "out") {
                    tab(o, 2); o << "xcp_dispatch_get_" << argument.datatype << "(__response, " << argument.name << ");" << std::endl;
                }
            }
            tab(o, 2); o << "__err = xcp_dispatch_get_error(__response);" << std::endl;
            tab(o, 1); o << "}" << std::endl;
            tab(o, 1); o << "xcp_dispatch_close(__response);" << std::endl;
            o << std::endl;
            tab(o, 1); o << "return __err;" << std::endl;
            o << "}" << std::endl;
            o << std::endl;
        }
    }
}

void generate_server_stub(const module_desc &m, std::ostream &o) {
    o << "/*" << std::endl;
    o << "** Generated using XCP-IDL compiler. DO NOT EDIT" << std::endl;
    o << "**" << std::endl;
    o << "** " << m.name << "_s.c: server stub definition for \"" << m.name << "\" XCP module" << std::endl;
    o << "*/" << std::endl;
    o << std::endl;
    o << "#define _XCP_SERVER_STUB" << std::endl;
    o << std::endl;
    o << "#include \"" << m.name << ".h\"" << std::endl;
    o << std::endl;
    for (int i = 0; i < m.interfaces.size(); i++) {
        const interface_desc &interface = m.interfaces[i];
        o << "xcp_interface_t __XCP_MODULE_" << m.name << "_INTERFACE_" << interface.name << " = NULL;" << std::endl;
        o << std::endl;
        for (int j = 0; j < interface.messages.size(); j++) {
            const message_desc &message = interface.messages[j];
            o << "xcp_error_t __XCP_MODULE_" << m.name << "_MESSAGE_CALLBACK_" << interface.name << "_" << message.name << "(xcp_dispatch_t __request, xcp_dispatch_t __response) {" << std::endl;
            tab(o, 1); o << "xcp_error_t __err;" << std::endl;
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                std::string ctype = "";
                if ((argument.datatype == "string") && (argument.attribute == "in")) {
                    ctype = "char *";
                } else {
                    ctype = find_translation(argument.datatype, argument.attribute);
                }
                tab(o, 1); o << ctype << " " << argument.name << ";" << std::endl;
            }
            o << std::endl;
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                if (argument.attribute == "in") {
                    tab(o, 1); o << "xcp_dispatch_get_" << argument.datatype << "(__request, &" << argument.name << ");" << std::endl;
                }
            }
            tab(o, 1); o << "xcp_dispatch_close(__request);" << std::endl;
            o << std::endl;
            tab(o, 1); o << "__err = " << interface.name << "_" << message.name << "_s(";
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                if (argument.attribute == "out") {
                    o << "&";
                }
                o << argument.name;
                if (k < message.arguments.size() - 1) {
                    o << ", ";
                }
            }
            o << ");" << std::endl;
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                if ((argument.datatype == "string") && (argument.attribute == "in")) {
                    tab(o, 1); o << "xcp_free(" << argument.name << ");" << std::endl;
                }
            }
            for (int k = 0; k < message.arguments.size(); k++) {
                const argument_desc &argument = message.arguments[k];
                if (argument.attribute == "out") {
                    tab(o, 1); o << "xcp_dispatch_put_" << argument.datatype << "(__response, " << argument.name << ");" << std::endl;
                    if (argument.datatype == "string") {
                        tab(o, 1); o << "xcp_free(" << argument.name << ");" << std::endl;
                    }
                }
            }
            o << std::endl;
            tab(o, 1); o << "return __err;" << std::endl;
            o << "}" << std::endl;
            o << std::endl;
        }
        o << "xcp_error_t __XCP_MODULE_" << m.name << "_MESSAGE_ROUTER_" << interface.name << "(unsigned short message_id, xcp_message_callback_t *callback) {" << std::endl;
        tab(o, 1); o << "xcp_message_callback_t callback_func = NULL;" << std::endl;
        o << std::endl;
        tab(o, 1); o << "switch (message_id) {" << std::endl;
        for (int j = 0; j < interface.messages.size(); j++) {
            const message_desc &message = interface.messages[j];
            tab(o, 1); o << "case " << j << ":" << std::endl;
            tab(o, 2); o << "callback_func = __XCP_MODULE_" << m.name << "_MESSAGE_CALLBACK_" << interface.name << "_" << message.name << ";" << std::endl;
            tab(o, 2); o << "break;" << std::endl;
        }
        tab(o, 1); o << "}" << std::endl;
        tab(o, 1); o << "if (callback_func != NULL) {" << std::endl;
        tab(o, 2); o << "*callback = callback_func;" << std::endl;
        tab(o, 2); o << "return XCP_S_OK;" << std::endl;
        tab(o, 1); o << "}" << std::endl;
        o << std::endl;
        tab(o, 1); o << "return XCP_E_NOTIMPL;" << std::endl;
        o << "}" << std::endl;
        o << std::endl;
    }
    o << "xcp_error_t xcp_init_module_" << m.name << "(void) {" << std::endl;
    tab(o, 1); o << "xcp_error_t err = XCP_S_OK;" << std::endl;
    o << std::endl;
    for (int i = 0; i < m.interfaces.size(); i++) {
        const interface_desc &interface = m.interfaces[i];
        tab(o, 1); o << "err = XCP_SUCCEEDED(err) ? xcp_interface_create(__XCP_MODULE_" << m.name << "_MESSAGE_ROUTER_" << interface.name << ", &__XCP_MODULE_" << m.name << "_INTERFACE_" << interface.name << ") : err;" << std::endl;
    }
    tab(o, 1); o << "if (XCP_FAILED(err)) {" << std::endl;
    tab(o, 2); o << "xcp_destroy_module_" << m.name << "();" << std::endl;
    tab(o, 2); o << "return err;" << std::endl;
    tab(o, 1); o << "}" << std::endl;
    o << std::endl;
    tab(o, 1); o << "return XCP_S_OK;" << std::endl;
    o << "}" << std::endl;
    o << std::endl;
    o << "xcp_error_t xcp_destroy_module_" << m.name << "(void) {" << std::endl;
    for (int i = 0; i < m.interfaces.size(); i++) {
        const interface_desc &interface = m.interfaces[i];
        tab(o, 1); o << "xcp_interface_destroy(__XCP_MODULE_" << m.name << "_INTERFACE_" << interface.name << ");" << std::endl;
    }
    o << std::endl;
    tab(o, 1); o << "return XCP_S_OK;" << std::endl;
    o << "}" << std::endl;
    o << std::endl;
    o << "xcp_error_t XCP_MODULE_" << m.name << "_INTERFACE_ROUTER(unsigned short interface_id, xcp_interface_t *interface) {" << std::endl;
    tab(o, 1); o << "xcp_interface_t interface_obj = NULL;" << std::endl;
    o << std::endl;
    tab(o, 1); o << "switch (interface_id) {" << std::endl;
    for (int i = 0; i < m.interfaces.size(); i++) {
        const interface_desc &interface = m.interfaces[i];
        tab(o, 1); o << "case " << i << ":" << std::endl;
        tab(o, 2); o << "interface_obj = __XCP_MODULE_" << m.name << "_INTERFACE_" << interface.name << ";" << std::endl;
        tab(o, 2); o << "break;" << std::endl;
    }
    tab(o, 1); o << "}" << std::endl;
    tab(o, 1); o << "if (interface_obj != NULL) {" << std::endl;
    tab(o, 2); o << "*interface = interface_obj;" << std::endl;
    tab(o, 2); o << "return XCP_S_OK;" << std::endl;
    tab(o, 1); o << "}" << std::endl;
    o << std::endl;
    tab(o, 1); o << "return XCP_E_NOTIMPL;" << std::endl;
    o << "}" << std::endl;
    o << std::endl;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "usage: " << argv[0] << " <input file>" << std::endl;
        return 1;
    }

    try {
        init_globals();

        std::string input_name = argv[1];
        std::ifstream input(input_name.c_str());

        if (input.is_open()) {
            std::cout << "Parsing IDL source..." << std::endl;

            std::vector<std::pair<position, std::string> > tokens;
            tokenize(input, tokens);

            module_desc main_module;
            interface_desc interface;
            message_desc message;
            argument_desc argument;

            synlevel level = synlevel_base;
            synstate state = synstate_want_module;
            for (int i = 0; i < tokens.size(); i++) {
                position pos = tokens[i].first;
                std::string token = tokens[i].second;

                if (level == synlevel_base) {
                    if (state == synstate_want_module) {
                        if (token == "module") {
                            state = synstate_want_identifier;
                        } else {
                            pos_throw(pos, "expecting `module` keyword");
                        }
                    } else if (state == synstate_want_identifier) {
                        if (is_valid_identifier(token)) {
                            main_module.name = token;
                            state = synstate_want_semicolon;
                        } else {
                            pos_throw(pos, "invalid identifier");
                        }
                    } else if (state == synstate_want_semicolon) {
                        if (token == ";") {
                            level = synlevel_module;
                            state = synstate_want_interface;
                        } else {
                            pos_throw(pos, "expecting semicolon");
                        }
                    }
                } else if (level == synlevel_module) {
                    if (state == synstate_want_interface) {
                        if (token == "interface") {
                            level = synlevel_interface;
                            state = synstate_want_identifier;
                            interface.name = "";
                            interface.messages.clear();
                        } else {
                            pos_throw(pos, "expecting `interface` keyword");
                        }
                    }
                } else if (level == synlevel_interface) {
                    if (state == synstate_want_identifier) {
                        if (is_valid_identifier(token)) {
                            interface.name = token;
                            state = synstate_want_lbracket;
                        } else {
                            pos_throw(pos, "invalid identifier");
                        }
                    } else if (state == synstate_want_lbracket) {
                        if (token == "{") {
                            state = synstate_want_rbracket_or_void;
                        } else {
                            pos_throw(pos, "expecting left bracket");
                        }
                    } else if (state == synstate_want_rbracket_or_void) {
                        if (token == "void") {
                            level = synlevel_message;
                            state = synstate_want_identifier;
                            message.name = "";
                            message.arguments.clear();
                        } else if (token == "}") {
                            main_module.interfaces.push_back(interface);
                            state = synstate_want_semicolon;
                        } else {
                            pos_throw(pos, "expecting right bracket or `void` keyword");
                        }
                    } else if (state == synstate_want_semicolon) {
                        if (token == ";") {
                            level = synlevel_module;
                            state = synstate_want_interface;
                        } else {
                            pos_throw(pos, "expecting semicolon");
                        }
                    }
                } else if (level == synlevel_message) {
                    if (state == synstate_want_identifier) {
                        if (is_valid_identifier(token)) {
                            state = synstate_want_lparen;
                            message.name = token;
                        } else {
                            pos_throw(pos, "invalid identifier");
                        }
                    } else if (state == synstate_want_lparen) {
                        if (token == "(") {
                            level = synlevel_argument;
                            state = synstate_want_attribute;
                            argument.attribute = "";
                            argument.datatype = "";
                            argument.name = "";
                        } else {
                            pos_throw(pos, "expecting left parenthese");
                        }
                    }
                } else if (level == synlevel_argument) {
                    if (state == synstate_want_attribute) {
                        if (g_attributes.count(token) != 0) {
                            argument.attribute = token;
                            state = synstate_want_datatype;
                        } else {
                            pos_throw(pos, "invalid attribute");
                        }
                    } else if (state == synstate_want_datatype) {
                        if (g_datatypes.count(token) != 0) {
                            argument.datatype = token;
                            state = synstate_want_identifier;
                        } else {
                            pos_throw(pos, "invalid datatype");
                        }
                    } else if (state == synstate_want_identifier) {
                        if (is_valid_identifier(token)) {
                            argument.name = token;
                            state = synstate_want_rparen_or_comma;
                        } else {
                            pos_throw(pos, "invalid identifier");
                        }
                    } else if (state == synstate_want_rparen_or_comma) {
                        if (token == ",") {
                            message.arguments.push_back(argument);
                            state = synstate_want_attribute;
                            argument.attribute = "";
                            argument.datatype = "";
                            argument.name = "";
                        } else if (token == ")") {
                            message.arguments.push_back(argument);
                            state = synstate_want_semicolon;
                        } else {
                            pos_throw(pos, "expecting right parenthese or comma");
                        }
                    } else if (state == synstate_want_semicolon) {
                        if (token == ";") {
                            interface.messages.push_back(message);
                            level = synlevel_interface;
                            state = synstate_want_rbracket_or_void;
                        } else {
                            pos_throw(pos, "expecting semicolon");
                        }
                    }
                }
            }

            std::ofstream header((main_module.name + ".h").c_str());
            if (header.is_open()) {
                std::cout << "Generating header..." << std::endl;
                generate_header(main_module, header);
            } else {
                throw std::runtime_error("unable to write header file");
            }

            std::ofstream client_stub((main_module.name + "_c.c").c_str());
            if (client_stub.is_open()) {
                std::cout << "Generating client stub..." << std::endl;
                generate_client_stub(main_module, client_stub);
            } else {
                throw std::runtime_error("unable to write client stub file");
            }

            std::ofstream server_stub((main_module.name + "_s.c").c_str());
            if (server_stub.is_open()) {
                std::cout << "Generating server stub..." << std::endl;
                generate_server_stub(main_module, server_stub);
            } else {
                throw std::runtime_error("unable to write server stub file");
            }
        } else {
            std::ostringstream msg;
            msg << "failed opening the file: " << input_name;
            throw std::runtime_error(msg.str().c_str());
        }
    } catch (const std::exception &e) {
        std::cout << "E: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

