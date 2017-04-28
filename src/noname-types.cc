#include "noname-utils.h"
#include "noname-types.h"
#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

namespace noname {

/// LogError* - These are little helper functions for error handling.
ASTNode* logError(const char* str) {
  char msg[1024];
  sprintf(msg, "%s\n", str);
  yyerror(msg);
  // abort();
  // fprintf(stdout, "Error: %s\n", str);
  return nullptr;
}

FunctionDefNode* logErrorF(const char* str) {
  logError(str);
  return nullptr;
}

AssignmentNode* logErrorV(const char* str) {
  logError(str);
  return nullptr;
}

ASTNode* logError(ErrorNode* error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

NodeValue* logErrorNV(ErrorNode* error_node) {
  logError(error_node->what().c_str());
  return nullptr;
}

void print_node_value(FILE* file, NodeValue* node_value) {
  if (yydebug >= 1) {
    if (!node_value) {
      fprintf(file, "\n##########[print_node_value] undef");
    } else if (node_value->getType() == TYPE_INT) {
      fprintf(file, "\n##########[print_node_value] %d", *(int*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_LONG) {
      fprintf(file, "\n##########[print_node_value] %ld", *(long*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_DOUBLE) {
      fprintf(file, "\n##########[print_node_value] %lf", *(double*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_STRING) {
      fprintf(file, "\n##########[print_node_value] %s", (*(std::string*)node_value->getRawValue()).c_str());
    } else {
      fprintf(file == stdout ? stderr : file, "\n##########[print_node_value] [WARN] could not print type %d",
              node_value->getType());
    }
  } else {
    if (!node_value) {
      fprintf(file, "undef");
    } else if (node_value->getType() == TYPE_INT) {
      fprintf(file, "%d", *(int*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_LONG) {
      fprintf(file, "%ld", *(long*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_DOUBLE) {
      fprintf(file, "%lf", *(double*)node_value->getRawValue());
    } else if (node_value->getType() == TYPE_STRING) {
      fprintf(file, "%s", (*(std::string*)node_value->getRawValue()).c_str());
    } else {
      fprintf(file == stdout ? stderr : file, "[WARN] could not print type %d", node_value->getType());
    }
  }
}
void print_node_value(NodeValue* node_value) { print_node_value(stdout, node_value); }

// ReturnNode* new_return(ASTContext* context, ExpNode* exp_node) {
//   ReturnNode* new_node = new ReturnNode(context, exp_node);
//   return new_node;
// }

stmtlist* new_stmt_list(ASTContext* context) {
  stmtlist* head_stmt_list = (stmtlist*)malloc(sizeof(stmtlist));

  if (!head_stmt_list) {
    yyerror("out of space");
    exit(0);
  }
  head_stmt_list->first = 0;
  head_stmt_list->last = 0;
  return head_stmt_list;
}
stmtlist* new_stmt_list(ASTContext* context, ASTNode* ast_node) {
  stmtlist* head_stmt_list = (stmtlist*)malloc(sizeof(stmtlist));
  stmtlist_node* new_node = (stmtlist_node*)malloc(sizeof(stmtlist_node));

  if (!head_stmt_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = ast_node;
  head_stmt_list->first = new_node;
  head_stmt_list->last = new_node;
  return head_stmt_list;
}

stmtlist* new_stmt_list(ASTContext* context, stmtlist* head_stmt_list, ASTNode* ast_node) {
  stmtlist_node* new_node = (stmtlist_node*)malloc(sizeof(stmtlist_node));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = ast_node;
  head_stmt_list->last->next = new_node;
  head_stmt_list->last = new_node;
  return head_stmt_list;
}

explist* new_exp_list(ASTContext* context) {
  explist* head_exp_list = (explist*)malloc(sizeof(explist));

  if (!head_exp_list) {
    yyerror("out of space");
    exit(0);
  }
  head_exp_list->first = 0;
  head_exp_list->last = 0;
  return head_exp_list;
}

explist* new_exp_list(ASTContext* context, ExpNode* exp_node) {
  explist* head_exp_list = (explist*)malloc(sizeof(explist));
  explist_node* new_node = (explist_node*)malloc(sizeof(explist_node));

  if (!head_exp_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = exp_node;
  head_exp_list->first = new_node;
  head_exp_list->last = new_node;
  return head_exp_list;
}

explist* new_exp_list(ASTContext* context, explist* head_exp_list, ExpNode* exp_node) {
  explist_node* new_node = (explist_node*)malloc(sizeof(explist_node));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = exp_node;
  head_exp_list->last->next = new_node;
  head_exp_list->last = new_node;
  return head_exp_list;
}

arglist* new_arg_list(ASTContext* context) {
  arglist* head_arg_list = (arglist*)malloc(sizeof(arglist));

  if (!head_arg_list) {
    yyerror("out of space");
    exit(0);
  }
  head_arg_list->first = 0;
  head_arg_list->last = 0;
  return head_arg_list;
}

arglist* new_arg_list(ASTContext* context, arg* arg) {
  arglist* head_arg_list = (arglist*)malloc(sizeof(arglist));
  arglist_node* new_node = (arglist_node*)malloc(sizeof(arglist_node));

  if (!head_arg_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->arg = arg;
  head_arg_list->first = new_node;
  head_arg_list->last = new_node;
  return head_arg_list;
}
arglist* new_arg_list(ASTContext* context, arglist* head_arg_list, arg* arg) {
  arglist_node* new_node = (arglist_node*)malloc(sizeof(arglist_node));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->arg = arg;
  head_arg_list->last->next = new_node;
  head_arg_list->last = new_node;
  return head_arg_list;
}

arg* create_new_arg(ASTContext* context, char* arg_name) {
  arg* new_arg = (arg*)malloc(sizeof(arg));

  if (!new_arg) {
    yyerror("out of space");
    exit(0);
  }
  new_arg->name = arg_name;
  return new_arg;
}

arg* new_arg(ASTContext* context, char* arg_name, ASTNode* defaultValue) {
  arg* new_arg = create_new_arg(context, arg_name);
  new_arg->defaultValue = defaultValue;
  return new_arg;
}

arg* new_arg(ASTContext* context, char* arg_name, double defaultValue) {
  arg* new_arg = create_new_arg(context, arg_name);
  new_arg->defaultValue = new NumberNode(context, defaultValue);
  return new_arg;
}
arg* new_arg(ASTContext* context, char* arg_name, long defaultValue) {
  arg* new_arg = create_new_arg(context, arg_name);
  new_arg->defaultValue = new NumberNode(context, defaultValue);
  return new_arg;
}
arg* new_arg(ASTContext* context, char* arg_name, char* defaultValue) {
  arg* new_arg = create_new_arg(context, arg_name);
  new_arg->defaultValue = new StringNode(context, defaultValue);
  return new_arg;
}

VarNode* new_var_node(ASTContext* context, const std::string name) {
  VarNode* new_node = new VarNode(context, name);
  return new_node;
}
AssignmentNode* new_assignment_node(ASTContext* context, const std::string name, ExpNode* exp) {
  AssignmentNode* new_node = new AssignmentNode(context, name, exp);
  return new_node;
}
CallExprNode* new_call_node(ASTContext* context, const std::string name, explist* exp_list) {
  CallExprNode* new_node = new CallExprNode(context, name, exp_list);
  return new_node;
}
AssignmentNode* new_declaration_node(ASTContext* context, const std::string name) {
  AssignmentNode* new_node = new AssignmentNode(context, name, NULL);

  NodeValue* temp_node = context->getVariable(name);
  if (temp_node) {
    return logErrorV("\nVariable already exists in this context!");
  }

  context->store(name, temp_node);  // temp_node is null. It doesn't matter, it's only a declaration

  if (yydebug >= 1) {
    fprintf(stdout, "\n[new_assignment %s]", context->getName().c_str());
  }
  return new_node;
}

ImportNode* new_import(ASTContext* context, std::string filename) {
  fprintf(stderr, "\n[new_import %s]", filename.c_str());
  ImportNode* new_node = new ImportNode(context, filename);

  return new_node;
}

ASTNode* new_function_def(ASTContext* context, const std::string name, arglist* arg_list, stmtlist* stmt_list,
                          ExpNode* returnNode) {
  FunctionDefNode* new_node = new FunctionDefNode(context, name, arg_list, stmt_list, returnNode);

  ASTNode* check_result = new_node->check();

  if (check_result && is_of_type<ErrorNode>(*check_result)) {
    // logError((ErrorNode*)check_result);
    return check_result;
  }

  ASTContext* parent = context->getParent();
  parent->store(name, new_node);
  if (yydebug >= 1) {
    fprintf(stdout, "\n[new_function_def %s]", parent->getName().c_str());
  }

  return new_node;
}

NodeValue* NumberNode::getValue() {
  NodeValue* node = nullptr;

  if (type == TYPE_DOUBLE) {
    node = new NodeValue(*(double*)value);
  } else if (type == TYPE_LONG) {
    node = new NodeValue(*(long*)value);
  } else if (type == TYPE_INT) {
    node = new NodeValue(*(int*)value);
  } else if (type == TYPE_FLOAT) {
    node = new NodeValue(*(float*)value);
  } else if (type == TYPE_SHORT) {
    node = new NodeValue(*(short*)value);
  } else if (type == TYPE_CHAR) {
    node = new NodeValue(*(char*)value);
  }

  return node;
}

NodeValue* StringNode::getValue() {
  NodeValue* node = new NodeValue(value);
  return node;
}

// UNNECESSARY
// void* VarNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* VarNode::getValue() {
  NodeValue* node = getContext()->getVariable(name);

  if (!node) {
    fprintf(stdout, "\n\n############ could not find %s on context %s \n\n", name.c_str(),
            getContext()->getName().c_str());
  }

  return node;
}

// UNNECESSARY
// void* UnaryExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* UnaryExpNode::getValue() {
  NodeValue* rhs_value = rhs.get()->getValue();
  fprintf(stdout, "\n\n############ IMPLEMENT ME: UnaryExpNode::getValue ###########\n\n");
  return rhs_value;
}

bool both_of_type(NodeValue* lhs, NodeValue* rhs, int type) {
  return lhs && rhs && lhs->getType() == type && rhs->getType() == type;
}

bool any_of_type(NodeValue* lhs, NodeValue* rhs, int type) {
  return lhs && rhs && (lhs->getType() == type || rhs->getType() == type);
}

bool match_to_types(NodeValue* lhs, NodeValue* rhs, int type1, int type2) {
  return lhs && rhs &&
         ((lhs->getType() == type1 && rhs->getType() == type2) || (lhs->getType() == type2 && rhs->getType() == type1));
}

int get_adequate_result_type(NodeValue* lhs, NodeValue* rhs) {
  if (any_of_type(lhs, rhs, TYPE_DOUBLE)) {
    return TYPE_DOUBLE;
  }
  if (both_of_type(lhs, rhs, TYPE_FLOAT)) {
    return TYPE_FLOAT;
  }
  if (match_to_types(lhs, rhs, TYPE_FLOAT, TYPE_LONG)) {
    return TYPE_DOUBLE;
  }
  if (any_of_type(lhs, rhs, TYPE_FLOAT)) {
    return TYPE_FLOAT;
  }
  if (any_of_type(lhs, rhs, TYPE_LONG)) {
    return TYPE_LONG;
  }
  if (any_of_type(lhs, rhs, TYPE_INT)) {
    return TYPE_INT;
  }
  if (any_of_type(lhs, rhs, TYPE_SHORT)) {
    return TYPE_SHORT;
  }
  if (any_of_type(lhs, rhs, TYPE_CHAR)) {
    return TYPE_CHAR;
  }
  return -1;
}

// UNNECESSARY
// void* BinaryExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* BinaryExpNode::getValue() {
  NodeValue* lhs_node_value = lhs.get()->getValue();
  NodeValue* rhs_node_value = rhs.get()->getValue();
  int result_type = get_adequate_result_type(lhs_node_value, rhs_node_value);
  void* lhs_value = lhs_node_value->getValue(result_type);
  void* rhs_value = rhs_node_value->getValue(result_type);
  NodeValue* result = nullptr;
  if (result_type == TYPE_DOUBLE) {
    double typed_lhs_value = *(double*)lhs_value;
    double typed_rhs_value = *(double*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue(pow(typed_lhs_value, typed_rhs_value));
    }
  } else if (result_type == TYPE_FLOAT) {
    float typed_lhs_value = *(float*)lhs_value;
    float typed_rhs_value = *(float*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue(pow(typed_lhs_value, typed_rhs_value));
    }
  } else if (result_type == TYPE_LONG) {
    long typed_lhs_value = *(long*)lhs_value;
    long typed_rhs_value = *(long*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((long)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_INT) {
    int typed_lhs_value = *(int*)lhs_value;
    int typed_rhs_value = *(int*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((int)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_SHORT) {
    short typed_lhs_value = *(short*)lhs_value;
    short typed_rhs_value = *(short*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((short)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_CHAR) {
    char typed_lhs_value = *(char*)lhs_value;
    char typed_rhs_value = *(char*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    } else if (op == '-') {
      result = new NodeValue(typed_lhs_value - typed_rhs_value);
    } else if (op == '*') {
      result = new NodeValue(typed_lhs_value * typed_rhs_value);
    } else if (op == '/') {
      result = new NodeValue(typed_lhs_value / typed_rhs_value);
    } else if (op == '^') {
      result = new NodeValue((char)pow(typed_lhs_value, typed_rhs_value));
    }

  } else if (result_type == TYPE_STRING) {
    std::string typed_lhs_value = *(std::string*)lhs_value;
    std::string typed_rhs_value = *(std::string*)rhs_value;

    if (op == '+') {
      result = new NodeValue(typed_lhs_value + typed_rhs_value);
    }
  }

  return result;
}

void* AssignmentNode::eval() {
  NodeValue* node_value = getValue();

  getContext()->store(name, node_value);

  if (yydebug >= 2) {
    fprintf(stdout, "\n\n############ stored %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
  }

  return node_value;
}

NodeValue* AssignmentNode::getValue() { return rhs.get()->getValue(); }

// UNNECESSARY
// void* CallExprNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* CallExprNode::getValue() {
  ASTContext* context = getContext();
  FunctionDefNode* function_node = context->getFunction(getCallee());

  if (!function_node) {
    fprintf(stdout, "\n\nThe called function was: '%s' BUT it wan not found on the context\n", getCallee().c_str());
    return 0;
  }

  ExpNode* returnNode = function_node->getReturnNode();
  std::vector<std::unique_ptr<ExpNode>>* valueArgs = &getArgs();
  std::vector<std::unique_ptr<ExpNode>>::iterator itValueArg = valueArgs->begin();
  std::vector<std::unique_ptr<arg>>* signatureArgs = &function_node->getArgs();
  std::vector<std::unique_ptr<arg>>::iterator itSignatureArg = signatureArgs->begin();
  std::vector<std::unique_ptr<ASTNode>>* bodyNodes = &function_node->getBodyNodes();
  std::vector<std::unique_ptr<ASTNode>>::iterator itBodyNodes = bodyNodes->begin();

  for (; itSignatureArg != signatureArgs->end() || itValueArg != valueArgs->end();) {
    std::unique_ptr<ExpNode>& valueArg = *itValueArg;
    std::unique_ptr<arg>& signatureArg = *itSignatureArg;

    context->store(signatureArg.get()->name, valueArg.get()->getValue());

    ++itSignatureArg;
    ++itValueArg;
  }

  for (; itBodyNodes != bodyNodes->end();) {
    std::unique_ptr<ASTNode>& bodyNode = *itBodyNodes;
    bodyNode.get()->eval();
    if (yydebug >= 1) {
      fprintf(stdout, "\n[## evaluating body: ASTNode of type %d]\n", bodyNode.get()->getType());
    }
    ++itBodyNodes;
  }

  if (returnNode) {
    if (yydebug >= 1) {
      fprintf(stdout, "\n[## evaluating return]\n");
    }
    return returnNode->getValue();
  } else {
    if (yydebug >= 1) {
      fprintf(stdout, "\n[## no return given]\n");
    }
  }

  return nullptr;
}
}
