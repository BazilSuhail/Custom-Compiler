#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* AST Node Types */
typedef enum {
    // Program
    NODE_PROGRAM,
    
    // Declarations
    NODE_VAR_DECL,
    NODE_FUNC_DECL,
    NODE_PARAM,
    NODE_ENUM_DECL,
    NODE_INCLUDE_DIRECTIVE,
    NODE_DEFINE_DIRECTIVE,
    
    // Statements
    NODE_BLOCK,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_DO_WHILE_STMT,
    NODE_FOR_STMT,
    NODE_SWITCH_STMT,
    NODE_CASE_STMT,
    NODE_RETURN_STMT,
    NODE_BREAK_STMT,
    NODE_CONTINUE_STMT,
    NODE_EXPR_STMT,
    NODE_ASSIGN_STMT,
    
    // Expressions
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_FUNC_CALL,
    
    // Types
    NODE_TYPE_SPEC,
    
    // Lists
    NODE_DECLARATION_LIST,
    NODE_STATEMENT_LIST,
    NODE_PARAM_LIST,
    NODE_ARG_LIST,
    NODE_ENUM_VALUE_LIST
} NodeType;

/* Operator Types */
typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_ASSIGN,
    OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE,
    OP_AND, OP_OR, OP_NOT,
    OP_BIT_AND, OP_BIT_OR, OP_BIT_XOR, OP_BIT_NOT,
    OP_LSHIFT, OP_RSHIFT,
    OP_INC, OP_DEC,
    OP_PLUS_UNARY, OP_MINUS_UNARY,
    OP_PRE_INC, OP_PRE_DEC, OP_POST_INC, OP_POST_DEC
} OperatorType;

/* Data Types */
typedef enum {
    TYPE_INT, TYPE_FLOAT, TYPE_DOUBLE, TYPE_CHAR, TYPE_BOOL, TYPE_VOID
} DataType;

/* Forward declaration */
typedef struct ASTNode ASTNode;

/* AST Node Structure */
struct ASTNode {
    NodeType type;
    
    union {
        /* Literals */
        struct {
            enum { LIT_INT, LIT_FLOAT, LIT_BOOL, LIT_CHAR, LIT_STRING } lit_type;
            union {
                int int_val;
                float float_val;
                int bool_val;
                char char_val;
                char *string_val;
            } value;
        } literal;
        
        /* Identifier */
        struct {
            char *name;
        } identifier;
        
        /* Binary Operation */
        struct {
            OperatorType op;
            ASTNode *left;
            ASTNode *right;
        } binary_op;
        
        /* Unary Operation */
        struct {
            OperatorType op;
            ASTNode *operand;
        } unary_op;
        
        /* Variable Declaration */
        struct {
            ASTNode *type_spec;
            char *name;
            ASTNode *init_expr;  // NULL if not initialized
        } var_decl;
        
        /* Function Declaration */
        struct {
            ASTNode *return_type;
            char *name;
            ASTNode *params;     // param list or NULL
            ASTNode *body;       // block
        } func_decl;
        
        /* Parameter */
        struct {
            ASTNode *type_spec;
            char *name;
        } param;
        
        /* Type Specification */
        struct {
            int is_const;
            DataType base_type;
        } type_spec;
        
        /* Function Call */
        struct {
            char *func_name;
            ASTNode *args;       // arg list or NULL
        } func_call;
        
        /* If Statement */
        struct {
            ASTNode *condition;
            ASTNode *then_block;
            ASTNode *else_block; // NULL if no else
        } if_stmt;
        
        /* While Statement */
        struct {
            ASTNode *condition;
            ASTNode *body;
        } while_stmt;
        
        /* Do-While Statement */
        struct {
            ASTNode *body;
            ASTNode *condition;
        } do_while_stmt;
        
        /* For Statement */
        struct {
            ASTNode *init;       // var_decl, expr, or NULL
            ASTNode *condition;  // expr or NULL
            ASTNode *increment;  // expr or NULL
            ASTNode *body;
        } for_stmt;
        
        /* Switch Statement */
        struct {
            ASTNode *expr;
            ASTNode *cases;      // case list
        } switch_stmt;
        
        /* Case Statement */
        struct {
            ASTNode *value;      // NULL for default
            ASTNode *stmts;      // statement list
        } case_stmt;
        
        /* Return Statement */
        struct {
            ASTNode *expr;       // NULL for empty return
        } return_stmt;
        
        /* Block */
        struct {
            ASTNode *declarations;
        } block;
        
        /* Enum Declaration */
        struct {
            char *name;
            ASTNode *values;     // enum value list
        } enum_decl;
        
        /* Include Directive */
        struct {
            char *file;
            int is_system;       // 1 for <>, 0 for ""
        } include_dir;
        
        /* Define Directive */
        struct {
            char *name;
            ASTNode *params;     // param list or NULL
            char *body;          // macro body as string
        } define_dir;
        
        /* Lists */
        struct {
            ASTNode **items;
            int count;
            int capacity;
        } list;
    } data;
};

/* AST Construction Functions */
ASTNode* create_node(NodeType type);
ASTNode* create_literal_int(int value);
ASTNode* create_literal_float(float value);
ASTNode* create_literal_bool(int value);
ASTNode* create_literal_char(char value);
ASTNode* create_literal_string(char *value);
ASTNode* create_identifier(char *name);
ASTNode* create_binary_op(OperatorType op, ASTNode *left, ASTNode *right);
ASTNode* create_unary_op(OperatorType op, ASTNode *operand);
ASTNode* create_var_decl(ASTNode *type_spec, char *name, ASTNode *init);
ASTNode* create_func_decl(ASTNode *return_type, char *name, ASTNode *params, ASTNode *body);
ASTNode* create_param(ASTNode *type_spec, char *name);
ASTNode* create_type_spec(int is_const, DataType base_type);
ASTNode* create_func_call(char *func_name, ASTNode *args);
ASTNode* create_if_stmt(ASTNode *condition, ASTNode *then_block, ASTNode *else_block);
ASTNode* create_while_stmt(ASTNode *condition, ASTNode *body);
ASTNode* create_do_while_stmt(ASTNode *body, ASTNode *condition);
ASTNode* create_for_stmt(ASTNode *init, ASTNode *condition, ASTNode *increment, ASTNode *body);
ASTNode* create_switch_stmt(ASTNode *expr, ASTNode *cases);
ASTNode* create_case_stmt(ASTNode *value, ASTNode *stmts);
ASTNode* create_return_stmt(ASTNode *expr);
ASTNode* create_break_stmt();
ASTNode* create_continue_stmt();
ASTNode* create_block(ASTNode *declarations);
ASTNode* create_enum_decl(char *name, ASTNode *values);
ASTNode* create_include_directive(char *file, int is_system);
ASTNode* create_define_directive(char *name, ASTNode *params, char *body);
ASTNode* create_list(NodeType list_type);
void list_append(ASTNode *list, ASTNode *item);

/* AST Printing Functions */
void print_ast(ASTNode *node, int indent);
void free_ast(ASTNode *node);

#endif /* AST_H */
