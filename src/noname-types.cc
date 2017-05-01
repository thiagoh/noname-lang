#include "noname-utils.h"
#include "noname-types.h"
#include "noname-jit.h"
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
using namespace llvm::orc;

extern FILE* fin;

namespace noname {

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
extern std::unique_ptr<NonameJIT> TheJIT;

std::string pow_function_name("_noname_function_pow");

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

stmtlist_t* new_stmt_list(ASTContext* context) {
  stmtlist_t* head_stmt_list = (stmtlist_t*)malloc(sizeof(stmtlist_t));

  if (!head_stmt_list) {
    yyerror("out of space");
    exit(0);
  }
  head_stmt_list->first = 0;
  head_stmt_list->last = 0;
  return head_stmt_list;
}
stmtlist_t* new_stmt_list(ASTContext* context, ASTNode* ast_node) {
  stmtlist_t* head_stmt_list = (stmtlist_t*)malloc(sizeof(stmtlist_t));
  stmtlist_node_t* new_node = (stmtlist_node_t*)malloc(sizeof(stmtlist_node_t));

  if (!head_stmt_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = ast_node;
  head_stmt_list->first = new_node;
  head_stmt_list->last = new_node;
  return head_stmt_list;
}

stmtlist_t* new_stmt_list(ASTContext* context, stmtlist_t* head_stmt_list, ASTNode* ast_node) {
  stmtlist_node_t* new_node = (stmtlist_node_t*)malloc(sizeof(stmtlist_node_t));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = ast_node;
  head_stmt_list->last->next = new_node;
  head_stmt_list->last = new_node;
  return head_stmt_list;
}

explist_t* new_exp_list(ASTContext* context) {
  explist_t* head_exp_list = (explist_t*)malloc(sizeof(explist_t));

  if (!head_exp_list) {
    yyerror("out of space");
    exit(0);
  }
  head_exp_list->first = 0;
  head_exp_list->last = 0;
  return head_exp_list;
}

explist_t* new_exp_list(ASTContext* context, ExpNode* exp_node) {
  explist_t* head_exp_list = (explist_t*)malloc(sizeof(explist_t));
  explist_node_t* new_node = (explist_node_t*)malloc(sizeof(explist_node_t));

  if (!head_exp_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = exp_node;
  head_exp_list->first = new_node;
  head_exp_list->last = new_node;
  return head_exp_list;
}

explist_t* new_exp_list(ASTContext* context, explist_t* head_exp_list, ExpNode* exp_node) {
  explist_node_t* new_node = (explist_node_t*)malloc(sizeof(explist_node_t));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->node = exp_node;
  head_exp_list->last->next = new_node;
  head_exp_list->last = new_node;
  return head_exp_list;
}

arglist_t* new_arg_list(ASTContext* context) {
  arglist_t* head_arg_list = (arglist_t*)malloc(sizeof(arglist_t));

  if (!head_arg_list) {
    yyerror("out of space");
    exit(0);
  }
  head_arg_list->first = 0;
  head_arg_list->last = 0;
  return head_arg_list;
}

arglist_t* new_arg_list(ASTContext* context, arg_t* arg) {
  arglist_t* head_arg_list = (arglist_t*)malloc(sizeof(arglist_t));
  arglist_node_t* new_node = (arglist_node_t*)malloc(sizeof(arglist_node_t));

  if (!head_arg_list || !new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->arg = arg;
  head_arg_list->first = new_node;
  head_arg_list->last = new_node;
  return head_arg_list;
}
arglist_t* new_arg_list(ASTContext* context, arglist_t* head_arg_list, arg_t* arg) {
  arglist_node_t* new_node = (arglist_node_t*)malloc(sizeof(arglist_node_t));

  if (!new_node) {
    yyerror("out of space");
    exit(0);
  }
  new_node->arg = arg;
  head_arg_list->last->next = new_node;
  head_arg_list->last = new_node;
  return head_arg_list;
}

arg_t* create_new_arg(ASTContext* context, char* arg_name) {
  arg_t* new_arg = (arg_t*)malloc(sizeof(arg_t));

  if (!new_arg) {
    yyerror("out of space");
    exit(0);
  }
  new_arg->name = arg_name;
  return new_arg;
}

arg_t* new_arg(ASTContext* context, char* arg_name, ASTNode* default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = default_value;
  return new_arg;
}

arg_t* new_arg(ASTContext* context, char* arg_name, double default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new NumberExpNode(context, default_value);
  return new_arg;
}
arg_t* new_arg(ASTContext* context, char* arg_name, long default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new NumberExpNode(context, default_value);
  return new_arg;
}
arg_t* new_arg(ASTContext* context, char* arg_name, char* default_value) {
  arg_t* new_arg = create_new_arg(context, arg_name);
  new_arg->default_value = new StringExpNode(context, default_value);
  return new_arg;
}

VarExpNode* new_var_node(ASTContext* context, const std::string name) {
  VarExpNode* new_node = new VarExpNode(context, name);
  return new_node;
}
AssignmentNode* new_assignment_node(ASTContext* context, const std::string name, ExpNode* exp) {
  AssignmentNode* new_node = new AssignmentNode(context, name, exp);
  return new_node;
}
CallExpNode* new_call_node(ASTContext* context, const std::string name, explist_t* exp_list) {
  CallExpNode* new_node = new CallExpNode(context, name, exp_list);
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

ASTNode* new_function_def(ASTContext* context, const std::string name, arglist_t* arg_list, stmtlist_t* stmt_list,
                          ExpNode* returnNode) {
  FunctionDefNode* new_node = new FunctionDefNode(context, name, arg_list, stmt_list, returnNode);

  ASTNode* check_result = new_node->check();

  if (check_result && isa<ErrorNode>(*check_result)) {
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

NodeValue* NumberExpNode::getValue() {
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

NodeValue* StringExpNode::getValue() {
  NodeValue* node = new NodeValue(value);
  return node;
}

// UNNECESSARY
// void* VarExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* VarExpNode::getValue() {
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

void* DeclarationNode::eval() {
  NodeValue* node_value = nullptr;

  getContext()->store(name, node_value);

  return node_value;
}

void* AssignmentNode::eval() {
  NodeValue* node_value = getValue();

  getContext()->update(name, node_value);

  if (yydebug >= 2) {
    fprintf(stdout, "\n\n############ updated %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
  }

  return node_value;
}

void* DeclarationAssignmentNode::eval() {
  NodeValue* node_value = getValue();

  getContext()->store(name, node_value);

  if (yydebug >= 2) {
    fprintf(stdout, "\n\n############ stored %s on context %s \n\n", name.c_str(), getContext()->getName().c_str());
  }

  return node_value;
}

NodeValue* AssignmentNode::getValue() { return rhs.get()->getValue(); }

// UNNECESSARY
// void* CallExpNode::eval() { NodeValue* node_value = getValue(); }
NodeValue* CallExpNode::getValue() {
  ASTContext* call_exp_context = getContext();
  FunctionDefNode* function_node = call_exp_context->getFunction(getCallee());

  if (!function_node) {
    fprintf(stdout, "\n\nThe called function was: '%s' BUT it wan not found on the context\n", getCallee().c_str());
    return 0;
  }

  // ASTContext* function_context = function_node->getContext();
  // ASTContext* temp_context = new ASTContext("temp_context", call_exp_context, function_context);
  // function_node->setContext(temp_context);

  ExpNode* returnNode = function_node->getReturnNode();
  std::vector<std::unique_ptr<ExpNode>>* value_args = &getArgs();
  std::vector<std::unique_ptr<ExpNode>>::iterator it_value_args = value_args->begin();
  std::vector<std::unique_ptr<arg_t>>* signature_args = &function_node->getArgs();
  std::vector<std::unique_ptr<arg_t>>::iterator it_signature_args = signature_args->begin();
  std::vector<std::unique_ptr<ASTNode>>* body_nodes = &function_node->getBodyNodes();
  std::vector<std::unique_ptr<ASTNode>>::iterator it_body_nodes = body_nodes->begin();

  for (; it_signature_args != signature_args->end() || it_value_args != value_args->end();) {
    std::unique_ptr<ExpNode>& value_arg = *it_value_args;
    std::unique_ptr<arg_t>& signature_arg = *it_signature_args;

    call_exp_context->store(signature_arg.get()->name, value_arg.get()->getValue());
    // temp_context->store(signature_arg.get()->name, value_arg.get()->getValue());

    ++it_signature_args;
    ++it_value_args;
  }

  for (; it_body_nodes != body_nodes->end();) {
    std::unique_ptr<ASTNode>& body_node = *it_body_nodes;

    body_node.get()->eval();

    if (yydebug >= 1) {
      fprintf(stdout, "\n[## evaluating body: ASTNode of type %s]\n",
              ASTNode::toString(body_node.get()->getKind()).c_str());
    }
    ++it_body_nodes;
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

  // delete temp_context;
  // function_node->setContext(function_context);

  return nullptr;
}

void* ASTNodeProcessorStrategy::process(ASTNode* node) {
  NodeValue* return_value = (NodeValue*)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}

void* ExpNodeProcessorStrategy::process(ASTNode* node) {
  NodeValue* return_value = (NodeValue*)node->eval();

  // if (auto* FnIR = FnAST->codegen()) {
  //   fprintf(stderr, "Read top-level expression:");
  //   FnIR->print(errs());
  //   fprintf(stderr, "\n");

  //   // JIT the module containing the anonymous expression, keeping a handle so
  //   // we can free it later.
  //   auto H = TheJIT->addModule(std::move(TheModule));
  //   InitializeModuleAndPassManager();

  //   // Search the JIT for the __anon_expr symbol.
  //   auto ExprSymbol = TheJIT->findSymbol("__anon_expr");
  //   assert(ExprSymbol && "Function not found");

  //   // Get the symbol's address and cast it to the right type (takes no
  //   // arguments, returns a double) so we can call it as a native function.
  //   double (*FP)() = (double (*)())(intptr_t)ExprSymbol.getAddress();
  //   fprintf(stderr, "Evaluated to %f\n", FP());

  //   // Delete the anonymous expression module from the JIT.
  //   TheJIT->removeModule(H);
  // }

  print_node_value(stdout, return_value);
  return nullptr;
}
void* AssignmentNodeProcessorStrategy::process(ASTNode* node) {
  NodeValue* return_value = (NodeValue*)node->eval();
  print_node_value(stdout, return_value);
  return nullptr;
}
void* CallExpNodeProcessorStrategy::process(ASTNode* node) {
  CallExpNode* call_exp_node = (CallExpNode*)node;
  FunctionDefNode* function_def_node = node->getContext()->getFunction(call_exp_node->getCallee());

  if (function_def_node) {
    if (yydebug >= 2) {
      fprintf(stdout, "\nThe called function was: '%s'\n", function_def_node->getName().c_str());
    }

    NodeValue* return_value = (NodeValue*)call_exp_node->eval();
    print_node_value(stdout, return_value);

  } else {
    fprintf(stderr, "\nError: The function %s was not found int the context\n", call_exp_node->getCallee().c_str());
  }
  return nullptr;
}

void* ImportNodeProcessorStrategy::process(ASTNode* node) {
  ImportNode* import_node = (ImportNode*)node;

  char* file_path = get_file_path(import_node->getFilename().c_str());
  char* const_file_path[] = {file_path};
  // const char *const_file_path = file_path;
  FILE* opened_file = fopen(*const_file_path, "r");

  if (opened_file != NULL) {
    if (is_file_already_imported(*const_file_path)) {
      if (yydebug >= 3) {
        fprintf(stdout, "\nNOTICE: File '%s' already imported\n", file_path);
      }
    } else {
      imported_files.push_back(file_path);
      read_from_file_import = true;
      fin = opened_file;
    }

  } else {
    fprintf(stderr, "\nError: File '%s' could not be opened.\n", file_path);
  }

  free(file_path);
  return nullptr;
}

// initialization
ProcessorStrategy* astNodeProcessorStrategy = new ASTNodeProcessorStrategy();
ProcessorStrategy* expNodeProcessorStrategy = new ExpNodeProcessorStrategy();
ProcessorStrategy* assignmentNodeProcessorStrategy = new AssignmentNodeProcessorStrategy();
ProcessorStrategy* callNodeProcessorStrategy = new CallExpNodeProcessorStrategy();
ProcessorStrategy* importNodeProcessorStrategy = new ImportNodeProcessorStrategy();

//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//

Value* NodeValue::codegen() {
  Value* value = nullptr;

  if (type == TYPE_DOUBLE) {
    value = ConstantFP::get(TheContext, APFloat(*(double*)value));
  } else if (type == TYPE_FLOAT) {
    value = ConstantFP::get(TheContext, APFloat(*(float*)value));
  } else if (type == TYPE_LONG) {
    // APInt (unsigned numBits, uint64_t val, bool isSigned=false)
    value = ConstantInt::get(TheContext, APInt(CHAR_BIT * sizeof(long), *(long*)value, true));
  } else if (type == TYPE_INT) {
    // APInt (unsigned numBits, uint64_t val, bool isSigned=false)
    value = ConstantInt::get(TheContext, APInt(CHAR_BIT * sizeof(int), *(int*)value, true));
  } else if (type == TYPE_SHORT) {
    value = ConstantInt::get(TheContext, APInt(CHAR_BIT * sizeof(short), *(short*)value, true));
  } else if (type == TYPE_CHAR) {
    value = ConstantInt::get(TheContext, APInt(CHAR_BIT * sizeof(char), *(char*)value, true));
  }

  return value;
}

Value* NumberExpNode::codegen() {
  NodeValue* node = this->getValue();

  if (!node) {
    fprintf(stdout, "\n\n############ could not resolve number expression");
    return nullptr;
  }

  return node->codegen();
}

Value* StringExpNode::codegen() {
  NodeValue* node = this->getValue();

  if (!node) {
    fprintf(stdout, "\n\n############ could not resolve string expression");
    return nullptr;
  }

  return node->codegen();
}

Value* VarExpNode::codegen() {
  NodeValue* node = getContext()->getVariable(name);

  if (!node) {
    fprintf(stdout, "\n\n############ could not find %s on context %s \n\n", name.c_str(),
            getContext()->getName().c_str());
    return nullptr;
  }

  return node->codegen();
}

Value* BinaryExpNode::CreatePow(Value* L, Value* R, const char* name = "call_pow_tmp") {
  Function* pow_function = TheModule->getFunction(pow_function_name);

  if (!pow_function) {
    // result = LogErrorV("Unknown function referenced");
    return nullptr;
  }

  std::vector<Value*> args_values;
  args_values.push_back(L);
  args_values.push_back(R);

  return Builder.CreateCall(pow_function, args_values, name);
}

Value* BinaryExpNode::codegen() {
  NodeValue* lhs_node_value = lhs.get()->getValue();
  NodeValue* rhs_node_value = rhs.get()->getValue();

  int result_type = get_adequate_result_type(lhs_node_value, rhs_node_value);
  void* lhs_value = lhs_node_value->getValue(result_type);
  void* rhs_value = rhs_node_value->getValue(result_type);
  Value* result = nullptr;

  Value* L = lhs_node_value->codegen();
  Value* R = rhs_node_value->codegen();

  if (!L || !R) {
    return result;
  }

  if (result_type == TYPE_DOUBLE || result_type == TYPE_FLOAT) {
    if (op == '+') {
      result = Builder.CreateFAdd(L, R, "addtmp");
    } else if (op == '-') {
      result = Builder.CreateFSub(L, R, "addtmp");
    } else if (op == '*') {
      result = Builder.CreateFMul(L, R, "multmp");
    } else if (op == '/') {
      result = Builder.CreateFDiv(L, R, "divtmp");
    } else if (op == '^') {
      result = CreatePow(L, R);
    }

  } else if (result_type == TYPE_LONG || result_type == TYPE_INT || result_type == TYPE_SHORT ||
             result_type == TYPE_CHAR) {
    int typed_lhs_value = *(int*)lhs_value;
    int typed_rhs_value = *(int*)rhs_value;

    if (op == '+') {
      result = Builder.CreateAdd(L, R, "addtmp");
    } else if (op == '-') {
      result = Builder.CreateSub(L, R, "addtmp");
    } else if (op == '*') {
      result = Builder.CreateMul(L, R, "multmp");
    } else if (op == '/') {
      result = Builder.CreateSDiv(L, R, "divtmp");

      // cast from int to double
      // Builder.CreateSIToFP(result, Type * DestTy, const Twine& Name = "")

    } else if (op == '^') {
      result = CreatePow(L, R);
    }

    // } else if (result_type == TYPE_STRING) {
    //   std::string typed_lhs_value = *(std::string*)lhs_value;
    //   std::string typed_rhs_value = *(std::string*)rhs_value;

    //   if (op == '+') {
    //     result = new NodeValue(typed_lhs_value + typed_rhs_value);
    //   }
  }

  return result;
}

Value* UnaryExpNode::codegen() {
  NodeValue* rhs_node_value = rhs.get()->getValue();
  Value* R = rhs_node_value->codegen();
  fprintf(stdout, "\n\n############ IMPLEMENT ME: UnaryExpNode::codegen ###########\n\n");
  return R;
}
}
