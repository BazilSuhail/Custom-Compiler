#include "ast.h"

/* Create a generic AST node */
ASTNode* create_node(NodeType type) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    node->type = type;
    return node;
}

/* Create literal nodes */
ASTNode* create_literal_int(int value) {
    ASTNode *node = create_node(NODE_LITERAL);
    node->data.literal.lit_type = LIT_INT;
    node->data.literal.value.int_val = value;
    return node;
}

ASTNode* create_literal_float(float value) {
    ASTNode *node = create_node(NODE_LITERAL);
    node->data.literal.lit_type = LIT_FLOAT;
    node->data.literal.value.float_val = value;
    return node;
}

ASTNode* create_literal_bool(int value) {
    ASTNode *node = create_node(NODE_LITERAL);
    node->data.literal.lit_type = LIT_BOOL;
    node->data.literal.value.bool_val = value;
    return node;
}

ASTNode* create_literal_char(char value) {
    ASTNode *node = create_node(NODE_LITERAL);
    node->data.literal.lit_type = LIT_CHAR;
    node->data.literal.value.char_val = value;
    return node;
}

ASTNode* create_literal_string(char *value) {
    ASTNode *node = create_node(NODE_LITERAL);
    node->data.literal.lit_type = LIT_STRING;
    node->data.literal.value.string_val = strdup(value);
    return node;
}

/* Create identifier node */
ASTNode* create_identifier(char *name) {
    ASTNode *node = create_node(NODE_IDENTIFIER);
    node->data.identifier.name = strdup(name);
    return node;
}

/* Create binary operation node */
ASTNode* create_binary_op(OperatorType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_node(NODE_BINARY_OP);
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

/* Create unary operation node */
ASTNode* create_unary_op(OperatorType op, ASTNode *operand) {
    ASTNode *node = create_node(NODE_UNARY_OP);
    node->data.unary_op.op = op;
    node->data.unary_op.operand = operand;
    return node;
}

/* Create variable declaration node */
ASTNode* create_var_decl(ASTNode *type_spec, char *name, ASTNode *init) {
    ASTNode *node = create_node(NODE_VAR_DECL);
    node->data.var_decl.type_spec = type_spec;
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.init_expr = init;
    return node;
}

/* Create function declaration node */
ASTNode* create_func_decl(ASTNode *return_type, char *name, ASTNode *params, ASTNode *body) {
    ASTNode *node = create_node(NODE_FUNC_DECL);
    node->data.func_decl.return_type = return_type;
    node->data.func_decl.name = strdup(name);
    node->data.func_decl.params = params;
    node->data.func_decl.body = body;
    return node;
}

/* Create parameter node */
ASTNode* create_param(ASTNode *type_spec, char *name) {
    ASTNode *node = create_node(NODE_PARAM);
    node->data.param.type_spec = type_spec;
    node->data.param.name = strdup(name);
    return node;
}

/* Create type specification node */
ASTNode* create_type_spec(int is_const, DataType base_type) {
    ASTNode *node = create_node(NODE_TYPE_SPEC);
    node->data.type_spec.is_const = is_const;
    node->data.type_spec.base_type = base_type;
    return node;
}

/* Create function call node */
ASTNode* create_func_call(char *func_name, ASTNode *args) {
    ASTNode *node = create_node(NODE_FUNC_CALL);
    node->data.func_call.func_name = strdup(func_name);
    node->data.func_call.args = args;
    return node;
}

/* Create if statement node */
ASTNode* create_if_stmt(ASTNode *condition, ASTNode *then_block, ASTNode *else_block) {
    ASTNode *node = create_node(NODE_IF_STMT);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_block = then_block;
    node->data.if_stmt.else_block = else_block;
    return node;
}

/* Create while statement node */
ASTNode* create_while_stmt(ASTNode *condition, ASTNode *body) {
    ASTNode *node = create_node(NODE_WHILE_STMT);
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    return node;
}

/* Create do-while statement node */
ASTNode* create_do_while_stmt(ASTNode *body, ASTNode *condition) {
    ASTNode *node = create_node(NODE_DO_WHILE_STMT);
    node->data.do_while_stmt.body = body;
    node->data.do_while_stmt.condition = condition;
    return node;
}

/* Create for statement node */
ASTNode* create_for_stmt(ASTNode *init, ASTNode *condition, ASTNode *increment, ASTNode *body) {
    ASTNode *node = create_node(NODE_FOR_STMT);
    node->data.for_stmt.init = init;
    node->data.for_stmt.condition = condition;
    node->data.for_stmt.increment = increment;
    node->data.for_stmt.body = body;
    return node;
}

/* Create switch statement node */
ASTNode* create_switch_stmt(ASTNode *expr, ASTNode *cases) {
    ASTNode *node = create_node(NODE_SWITCH_STMT);
    node->data.switch_stmt.expr = expr;
    node->data.switch_stmt.cases = cases;
    return node;
}

/* Create case statement node */
ASTNode* create_case_stmt(ASTNode *value, ASTNode *stmts) {
    ASTNode *node = create_node(NODE_CASE_STMT);
    node->data.case_stmt.value = value;
    node->data.case_stmt.stmts = stmts;
    return node;
}

/* Create return statement node */
ASTNode* create_return_stmt(ASTNode *expr) {
    ASTNode *node = create_node(NODE_RETURN_STMT);
    node->data.return_stmt.expr = expr;
    return node;
}

/* Create break statement node */
ASTNode* create_break_stmt() {
    return create_node(NODE_BREAK_STMT);
}

/* Create continue statement node */
ASTNode* create_continue_stmt() {
    return create_node(NODE_CONTINUE_STMT);
}

/* Create block node */
ASTNode* create_block(ASTNode *declarations) {
    ASTNode *node = create_node(NODE_BLOCK);
    node->data.block.declarations = declarations;
    return node;
}

/* Create enum declaration node */
ASTNode* create_enum_decl(char *name, ASTNode *values) {
    ASTNode *node = create_node(NODE_ENUM_DECL);
    node->data.enum_decl.name = strdup(name);
    node->data.enum_decl.values = values;
    return node;
}

/* Create include directive node */
ASTNode* create_include_directive(char *file, int is_system) {
    ASTNode *node = create_node(NODE_INCLUDE_DIRECTIVE);
    node->data.include_dir.file = strdup(file);
    node->data.include_dir.is_system = is_system;
    return node;
}

/* Create define directive node */
ASTNode* create_define_directive(char *name, ASTNode *params, char *body) {
    ASTNode *node = create_node(NODE_DEFINE_DIRECTIVE);
    node->data.define_dir.name = strdup(name);
    node->data.define_dir.params = params;
    node->data.define_dir.body = body ? strdup(body) : NULL;
    return node;
}

/* Create list node */
ASTNode* create_list(NodeType list_type) {
    ASTNode *node = create_node(list_type);
    node->data.list.items = NULL;
    node->data.list.count = 0;
    node->data.list.capacity = 0;
    return node;
}

/* Append item to list */
void list_append(ASTNode *list, ASTNode *item) {
    if (list->data.list.count >= list->data.list.capacity) {
        int new_capacity = list->data.list.capacity == 0 ? 8 : list->data.list.capacity * 2;
        list->data.list.items = (ASTNode**)realloc(list->data.list.items, 
                                                    new_capacity * sizeof(ASTNode*));
        list->data.list.capacity = new_capacity;
    }
    list->data.list.items[list->data.list.count++] = item;
}

/* Helper function to print indentation */
static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

/* Get operator name as string */
static const char* get_op_name(OperatorType op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_ASSIGN: return "=";
        case OP_EQ: return "==";
        case OP_NE: return "!=";
        case OP_LT: return "<";
        case OP_GT: return ">";
        case OP_LE: return "<=";
        case OP_GE: return ">=";
        case OP_AND: return "&&";
        case OP_OR: return "||";
        case OP_NOT: return "!";
        case OP_BIT_AND: return "&";
        case OP_BIT_OR: return "|";
        case OP_BIT_XOR: return "^";
        case OP_BIT_NOT: return "~";
        case OP_LSHIFT: return "<<";
        case OP_RSHIFT: return ">>";
        case OP_INC: return "++";
        case OP_DEC: return "--";
        case OP_PLUS_UNARY: return "+";
        case OP_MINUS_UNARY: return "-";
        case OP_PRE_INC: return "++";
        case OP_PRE_DEC: return "--";
        case OP_POST_INC: return "++";
        case OP_POST_DEC: return "--";
        default: return "unknown";
    }
}

/* Get type name as string */
static const char* get_type_name(DataType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_DOUBLE: return "double";
        case TYPE_CHAR: return "char";
        case TYPE_BOOL: return "bool";
        case TYPE_VOID: return "void";
        default: return "unknown";
    }
}

/* Print AST recursively */
void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    
    print_indent(indent);
    
    switch (node->type) {
        case NODE_PROGRAM:
            printf("Program\n");
            if (node->data.list.count > 0) {
                for (int i = 0; i < node->data.list.count; i++) {
                    print_ast(node->data.list.items[i], indent + 1);
                }
            }
            break;
            
        case NODE_LITERAL:
            printf("Literal: ");
            switch (node->data.literal.lit_type) {
                case LIT_INT:
                    printf("%d (int)\n", node->data.literal.value.int_val);
                    break;
                case LIT_FLOAT:
                    printf("%f (float)\n", node->data.literal.value.float_val);
                    break;
                case LIT_BOOL:
                    printf("%s (bool)\n", node->data.literal.value.bool_val ? "true" : "false");
                    break;
                case LIT_CHAR:
                    printf("'%c' (char)\n", node->data.literal.value.char_val);
                    break;
                case LIT_STRING:
                    printf("%s (string)\n", node->data.literal.value.string_val);
                    break;
            }
            break;
            
        case NODE_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier.name);
            break;
            
        case NODE_BINARY_OP:
            printf("BinaryOp: %s\n", get_op_name(node->data.binary_op.op));
            print_ast(node->data.binary_op.left, indent + 1);
            print_ast(node->data.binary_op.right, indent + 1);
            break;
            
        case NODE_UNARY_OP:
            printf("UnaryOp: %s\n", get_op_name(node->data.unary_op.op));
            print_ast(node->data.unary_op.operand, indent + 1);
            break;
            
        case NODE_VAR_DECL:
            printf("VarDecl: %s\n", node->data.var_decl.name);
            print_ast(node->data.var_decl.type_spec, indent + 1);
            if (node->data.var_decl.init_expr) {
                print_indent(indent + 1);
                printf("Initializer:\n");
                print_ast(node->data.var_decl.init_expr, indent + 2);
            }
            break;
            
        case NODE_FUNC_DECL:
            printf("FuncDecl: %s\n", node->data.func_decl.name);
            print_indent(indent + 1);
            printf("Return Type:\n");
            print_ast(node->data.func_decl.return_type, indent + 2);
            if (node->data.func_decl.params) {
                print_indent(indent + 1);
                printf("Parameters:\n");
                print_ast(node->data.func_decl.params, indent + 2);
            }
            print_indent(indent + 1);
            printf("Body:\n");
            print_ast(node->data.func_decl.body, indent + 2);
            break;
            
        case NODE_PARAM:
            printf("Param: %s\n", node->data.param.name);
            print_ast(node->data.param.type_spec, indent + 1);
            break;
            
        case NODE_TYPE_SPEC:
            printf("TypeSpec: %s%s\n", 
                   node->data.type_spec.is_const ? "const " : "",
                   get_type_name(node->data.type_spec.base_type));
            break;
            
        case NODE_FUNC_CALL:
            printf("FuncCall: %s\n", node->data.func_call.func_name);
            if (node->data.func_call.args) {
                print_indent(indent + 1);
                printf("Arguments:\n");
                print_ast(node->data.func_call.args, indent + 2);
            }
            break;
            
        case NODE_IF_STMT:
            printf("IfStmt\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            print_ast(node->data.if_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Then:\n");
            print_ast(node->data.if_stmt.then_block, indent + 2);
            if (node->data.if_stmt.else_block) {
                print_indent(indent + 1);
                printf("Else:\n");
                print_ast(node->data.if_stmt.else_block, indent + 2);
            }
            break;
            
        case NODE_WHILE_STMT:
            printf("WhileStmt\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            print_ast(node->data.while_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Body:\n");
            print_ast(node->data.while_stmt.body, indent + 2);
            break;
            
        case NODE_DO_WHILE_STMT:
            printf("DoWhileStmt\n");
            print_indent(indent + 1);
            printf("Body:\n");
            print_ast(node->data.do_while_stmt.body, indent + 2);
            print_indent(indent + 1);
            printf("Condition:\n");
            print_ast(node->data.do_while_stmt.condition, indent + 2);
            break;
            
        case NODE_FOR_STMT:
            printf("ForStmt\n");
            if (node->data.for_stmt.init) {
                print_indent(indent + 1);
                printf("Init:\n");
                print_ast(node->data.for_stmt.init, indent + 2);
            }
            if (node->data.for_stmt.condition) {
                print_indent(indent + 1);
                printf("Condition:\n");
                print_ast(node->data.for_stmt.condition, indent + 2);
            }
            if (node->data.for_stmt.increment) {
                print_indent(indent + 1);
                printf("Increment:\n");
                print_ast(node->data.for_stmt.increment, indent + 2);
            }
            print_indent(indent + 1);
            printf("Body:\n");
            print_ast(node->data.for_stmt.body, indent + 2);
            break;
            
        case NODE_SWITCH_STMT:
            printf("SwitchStmt\n");
            print_indent(indent + 1);
            printf("Expression:\n");
            print_ast(node->data.switch_stmt.expr, indent + 2);
            print_indent(indent + 1);
            printf("Cases:\n");
            print_ast(node->data.switch_stmt.cases, indent + 2);
            break;
            
        case NODE_CASE_STMT:
            if (node->data.case_stmt.value) {
                printf("Case:\n");
                print_ast(node->data.case_stmt.value, indent + 1);
            } else {
                printf("Default:\n");
            }
            if (node->data.case_stmt.stmts) {
                print_ast(node->data.case_stmt.stmts, indent + 1);
            }
            break;
            
        case NODE_RETURN_STMT:
            printf("ReturnStmt\n");
            if (node->data.return_stmt.expr) {
                print_ast(node->data.return_stmt.expr, indent + 1);
            }
            break;
            
        case NODE_BREAK_STMT:
            printf("BreakStmt\n");
            break;
            
        case NODE_CONTINUE_STMT:
            printf("ContinueStmt\n");
            break;
            
        case NODE_BLOCK:
            printf("Block\n");
            if (node->data.block.declarations) {
                print_ast(node->data.block.declarations, indent + 1);
            }
            break;
            
        case NODE_ENUM_DECL:
            printf("EnumDecl: %s\n", node->data.enum_decl.name);
            if (node->data.enum_decl.values) {
                print_ast(node->data.enum_decl.values, indent + 1);
            }
            break;
            
        case NODE_INCLUDE_DIRECTIVE:
            printf("Include: %s%s%s\n",
                   node->data.include_dir.is_system ? "<" : "\"",
                   node->data.include_dir.file,
                   node->data.include_dir.is_system ? ">" : "\"");
            break;
            
        case NODE_DEFINE_DIRECTIVE:
            printf("Define: %s\n", node->data.define_dir.name);
            if (node->data.define_dir.params) {
                print_indent(indent + 1);
                printf("Parameters:\n");
                print_ast(node->data.define_dir.params, indent + 2);
            }
            if (node->data.define_dir.body) {
                print_indent(indent + 1);
                printf("Body: %s\n", node->data.define_dir.body);
            }
            break;
            
        case NODE_EXPR_STMT:
        case NODE_ASSIGN_STMT:
        case NODE_DECLARATION_LIST:
        case NODE_STATEMENT_LIST:
        case NODE_PARAM_LIST:
        case NODE_ARG_LIST:
        case NODE_ENUM_VALUE_LIST:
            // List nodes
            for (int i = 0; i < node->data.list.count; i++) {
                print_ast(node->data.list.items[i], indent);
            }
            break;
            
        default:
            printf("Unknown node type\n");
            break;
    }
}

/* Free AST memory */
void free_ast(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_LITERAL:
            if (node->data.literal.lit_type == LIT_STRING) {
                free(node->data.literal.value.string_val);
            }
            break;
            
        case NODE_IDENTIFIER:
            free(node->data.identifier.name);
            break;
            
        case NODE_BINARY_OP:
            free_ast(node->data.binary_op.left);
            free_ast(node->data.binary_op.right);
            break;
            
        case NODE_UNARY_OP:
            free_ast(node->data.unary_op.operand);
            break;
            
        case NODE_VAR_DECL:
            free(node->data.var_decl.name);
            free_ast(node->data.var_decl.type_spec);
            free_ast(node->data.var_decl.init_expr);
            break;
            
        case NODE_FUNC_DECL:
            free(node->data.func_decl.name);
            free_ast(node->data.func_decl.return_type);
            free_ast(node->data.func_decl.params);
            free_ast(node->data.func_decl.body);
            break;
            
        case NODE_PARAM:
            free(node->data.param.name);
            free_ast(node->data.param.type_spec);
            break;
            
        case NODE_FUNC_CALL:
            free(node->data.func_call.func_name);
            free_ast(node->data.func_call.args);
            break;
            
        case NODE_IF_STMT:
            free_ast(node->data.if_stmt.condition);
            free_ast(node->data.if_stmt.then_block);
            free_ast(node->data.if_stmt.else_block);
            break;
            
        case NODE_WHILE_STMT:
            free_ast(node->data.while_stmt.condition);
            free_ast(node->data.while_stmt.body);
            break;
            
        case NODE_DO_WHILE_STMT:
            free_ast(node->data.do_while_stmt.body);
            free_ast(node->data.do_while_stmt.condition);
            break;
            
        case NODE_FOR_STMT:
            free_ast(node->data.for_stmt.init);
            free_ast(node->data.for_stmt.condition);
            free_ast(node->data.for_stmt.increment);
            free_ast(node->data.for_stmt.body);
            break;
            
        case NODE_SWITCH_STMT:
            free_ast(node->data.switch_stmt.expr);
            free_ast(node->data.switch_stmt.cases);
            break;
            
        case NODE_CASE_STMT:
            free_ast(node->data.case_stmt.value);
            free_ast(node->data.case_stmt.stmts);
            break;
            
        case NODE_RETURN_STMT:
            free_ast(node->data.return_stmt.expr);
            break;
            
        case NODE_BLOCK:
            free_ast(node->data.block.declarations);
            break;
            
        case NODE_ENUM_DECL:
            free(node->data.enum_decl.name);
            free_ast(node->data.enum_decl.values);
            break;
            
        case NODE_INCLUDE_DIRECTIVE:
            free(node->data.include_dir.file);
            break;
            
        case NODE_DEFINE_DIRECTIVE:
            free(node->data.define_dir.name);
            free_ast(node->data.define_dir.params);
            free(node->data.define_dir.body);
            break;
            
        case NODE_DECLARATION_LIST:
        case NODE_STATEMENT_LIST:
        case NODE_PARAM_LIST:
        case NODE_ARG_LIST:
        case NODE_ENUM_VALUE_LIST:
        case NODE_PROGRAM:
            for (int i = 0; i < node->data.list.count; i++) {
                free_ast(node->data.list.items[i]);
            }
            free(node->data.list.items);
            break;
            
        default:
            break;
    }
    
    free(node);
}
