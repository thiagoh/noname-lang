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

extern void yyerror(const char* error_msg);

/// LogError* - These are little helper functions for error handling.
ASTNode* logError(const char* str) {
  char msg[1024];
  sprintf(msg, "%s\n", str);
  yyerror(msg);
  // abort();
  // fprintf(stderr, "Error: %s\n", str);
  return NULL;
}

FunctionDefNode* logErrorF(const char* str) {
  logError(str);
  return nullptr;
}

AssignmentNode* logErrorV(const char* str) {
  logError(str);
  return nullptr;
}

void print_node_value(FILE* file, NodeValue& node_value) {
  if (node_value.getType() == TYPE_INT) {
    fprintf(file, "\n##########[print_node_value] %d", *(int*)node_value.getValue());
  } else if (node_value.getType() == TYPE_LONG) {
    fprintf(file, "\n##########[print_node_value] %ld", *(long*)node_value.getValue());
  } else if (node_value.getType() == TYPE_DOUBLE) {
    fprintf(file, "\n##########[print_node_value] %lf", *(double*)node_value.getValue());
  } else if (node_value.getType() == TYPE_STRING) {
    fprintf(file, "\n##########[print_node_value] %s", (*(std::string*)node_value.getValue()).c_str());
  } else {
    fprintf(file, "\n##########[print_node_value] could not print type %d", node_value.getType());
  }
}
void print_node_value(NodeValue& node_value) { print_node_value(stdout, node_value); }

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

VarNode* new_var_node(ASTContext* context, const std::string& name) {
  VarNode* new_node = new VarNode(context, name);
  return new_node;
}
AssignmentNode* new_assignment_node(ASTContext* context, const std::string& name, ExpNode* exp) {
  AssignmentNode* new_node = new AssignmentNode(context, name, exp);
  return new_node;
}
CallExprNode* new_call_node(ASTContext* context, const std::string& name, explist* exp_list) {
  CallExprNode* new_node = new CallExprNode(context, name, exp_list);
  return new_node;
}
AssignmentNode* new_declaration_node(ASTContext* context, const std::string& name) {
  AssignmentNode* new_node = new AssignmentNode(context, name, NULL);

  NodeValue* temp_node = context->getVariable(name);
  if (temp_node) {
    return logErrorV("\nVariable already exists in this context!");
  }

  context->store(name, temp_node);  // temp_node is null. It doesn't matter

  fprintf(stderr, "\n[new_assignment %s]", context->getName().c_str());

  return new_node;
}

FunctionDefNode* new_function_def(ASTContext* context, const std::string& name, arglist* arg_list, stmtlist* stmt_list,
                                  ExpNode* returnNode) {
  FunctionDefNode* new_node = new FunctionDefNode(context, name, arg_list, stmt_list, returnNode);

  ASTContext* parent = context->getParent();

  FunctionDefNode* functionNode = parent->getFunction(name);
  if (functionNode) {
    return logErrorF("\nFunction already exists in this context!");
  }

  parent->store(name, new_node);

  fprintf(stderr, "\n[new_function_def %s]", parent->getName().c_str());
  return new_node;
}

// UNNECESSARY
// void VarNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* VarNode::getValue() { return getContext()->getVariable(name); }

// UNNECESSARY
// void UnaryExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* UnaryExpNode::getValue() {
  NodeValue* rhs_value = rhs.get()->getValue();
  fprintf(stderr, "\n\n############ IMPLEMENT ME: UnaryExpNode::getValue ###########\n\n");
  return rhs_value;
}

// UNNECESSARY
// void BinaryExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* BinaryExpNode::getValue() {
  NodeValue* lhs_value = lhs.get()->getValue();
  NodeValue* rhs_value = rhs.get()->getValue();

  if (op == '+') {
  }

  return 0;
}

void AssignmentNode::eval() { getContext()->store(name, getValue()); }
NodeValue* AssignmentNode::getValue() { return rhs.get()->getValue(); }

// UNNECESSARY
// void CallExprNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* CallExprNode::getValue() {
  ASTContext* context = getContext();
  FunctionDefNode* functionNode = context->getFunction(getCallee());

  if (!functionNode) {
    fprintf(stderr, "\n\nThe called function was: '%s' BUT it wan not found on the context\n", getCallee().c_str());
    return 0;
  }

  ExpNode* returnNode = functionNode->getReturnNode();
  std::vector<std::unique_ptr<ExpNode>>* valueArgs = &getArgs();
  std::vector<std::unique_ptr<ExpNode>>::iterator itValueArg = valueArgs->begin();
  std::vector<std::unique_ptr<arg>>* signatureArgs = &functionNode->getArgs();
  std::vector<std::unique_ptr<arg>>::iterator itSignatureArg = signatureArgs->begin();
  std::vector<std::unique_ptr<ASTNode>>* bodyNodes = &functionNode->getBodyNodes();
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

    fprintf(stderr, "\n[## evaluating body: ASTNode of type %d]\n", bodyNode.get()->getType());
    ++itBodyNodes;
  }

  if (returnNode) {
    fprintf(stderr, "\n[## evaluating return]\n");
    return returnNode->getValue();
  } else {
    fprintf(stderr, "\n[## no return given]\n");
  }

  return nullptr;
}
