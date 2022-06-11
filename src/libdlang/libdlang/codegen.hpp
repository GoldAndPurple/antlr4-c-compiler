#pragma once

#include <llvm/IR/IRBuilder.h>
#include <libdlang/ast.hpp>

namespace dlang {

class ASTVisitorCodegen : public ASTVisitor {
 public:
  llvm::LLVMContext* context;
  llvm::IRBuilder<>* builder;
  llvm::Module* modul;

  std::vector<llvm::Value*> vstack;
  std::map<std::string, llvm::Value*> current_vars;

  Scope* global;
  Scope* current_scope;

  ASTVisitorCodegen(Scope* g) : global(g) {
    context = new llvm::LLVMContext();
    modul = new llvm::Module("d-compiler", *context);
    builder = new llvm::IRBuilder<>(*context);
    current_scope = global;
  };

  llvm::Type* llvmtype(int v) {
    if (v == type_void) {
      return llvm::Type::getVoidTy(*context);
    } else if (v == type_int) {
      return llvm::Type::getInt32Ty(*context);
    } else if (v == type_float) {
      return llvm::Type::getFloatTy(*context);
    } else if (v == type_char) {
      return llvm::Type::getInt8Ty(*context);
      ;
    } else if (v == type_void_pointer) {
      return llvm::Type::getInt8Ty(*context);
      ;
    } else if (v == type_int_pointer) {
      return llvm::Type::getInt32PtrTy(*context);
    } else if (v == type_float_pointer) {
      return llvm::Type::getFloatPtrTy(*context);
      ;
    } else /* if (v == type_char_pointer) */ {
      return llvm::Type::getInt8PtrTy(*context);
      ;
    }
  }

  llvm::Value* visit(ASTNodeExpr* n) {
    n->accept(this);
    if (!vstack.empty()) {
      llvm::Value* value = vstack.back();
      vstack.pop_back();
      return value;
    }
    throw std::logic_error("internal error during expression generation");
  }

  void visit(ASTNodeProgram* n) { visit_children(n); }
  void visit(ASTNodeFuncDef* n) {
    current_scope = global->get_scope(n->name);

    std::vector<llvm::Type*> params;
    if (n->parameters) {
      for (auto t : n->parameters->param_types) {
        params.push_back(llvmtype(t));
      }
    }
    llvm::FunctionType* ft =
        llvm::FunctionType::get(llvmtype(n->returntype), params, false);
    llvm::Function* func = llvm::Function::Create(
        ft, llvm::Function::ExternalLinkage, n->name, modul);

    llvm::BasicBlock* bb =
        llvm::BasicBlock::Create(*context, "entry_" + n->name, func);
    builder->SetInsertPoint(bb);

    if (n->parameters) {
      int i = 0;
      for (auto& arg : func->args()) {
        arg.setName(n->parameters->param_names[i]);
        current_vars[n->parameters->param_names[i]] = &arg;
        if (!arg.getType()->isPointerTy()) {
          auto var = builder->CreateAlloca(arg.getType(), nullptr);
          builder->CreateStore(&arg, var);
        }
      }
      i++;
    }

    visit_children(n);
    current_scope = current_scope->parent;
    current_vars.clear();
  }

  void visit(ASTNodeFuncCall* n) {
    llvm::Function *call = modul->getFunction(n->name);
    std::vector<llvm::Value*> params;
    for (auto param : n->children){
      llvm::Value* p = visit((ASTNodeExpr*)(param));
      params.push_back(p);
    }
    builder->CreateCall(call,params);
  }

  void visit(ASTNodeDecl* n) {
    for (auto c : n->children) {
      if (c->getToken() == assignment) {
        /*
        n   ->    c [id] ->value
        decl->  assign   ->expression
        */
        std::string varname = (((ASTNodeAssign*)c)->id)->value;
        /* global var */
        if (current_scope == global && global->declared(varname)) {
          llvm::GlobalVariable* var;
          modul->getOrInsertGlobal(varname, llvmtype(global->get(varname)));
          var = modul->getNamedGlobal(varname);
          llvm::Constant* initValue = nullptr;
          if (global->get(varname) == type_int) {
            initValue = llvm::dyn_cast<llvm::Constant>(visit((ASTNodeExpr*)(c->children[0])));
            /* initValue = llvm::ConstantInt::get(
                *context,
                llvm::APInt(
                    32,
                    exprresult,
                    true)); */
          }
          var->setInitializer(initValue);
        }
      } else /* if token == id */ {
        std::string varname = ((ASTNodeIdentifier*)c)->value;
        auto variable = builder->CreateAlloca(
            llvmtype(current_scope->get(varname)), nullptr, varname);
        current_vars[varname] = variable;
      }
    }
  }

  void visit(ASTNodeAssign* n) {
    auto var = current_vars[n->id->value];
    llvm::Value* result = visit((ASTNodeExpr*)(n->children[0]));
    builder->CreateStore(result,var);
  }

  void visit(ASTNodeReturn* n){
    if (!n->children.empty()){
      llvm::Value* ret_expr = visit((ASTNodeExpr*)(n->children[0]));
      builder->CreateRet(ret_expr);
    } else {
      builder->CreateRet(nullptr);
    }
  }

  void visit(ASTNodeBinary* n) {
    llvm::Value* left = visit((ASTNodeExpr*)(n->children[0]));
    llvm::Value* right = visit((ASTNodeExpr*)(n->children[1]));
    llvm::Value* result;

    if (left->getType()->isIntegerTy()) {
      if (n->exprtype == "==") {
        result = builder->CreateICmpEQ(left, right);
      } else if (n->exprtype == "<") {
        result = builder->CreateICmpSLT(left, right);
      } else if (n->exprtype == "*") {
        result = builder->CreateMul(left, right);
      } else if (n->exprtype == "+") {
        result = builder->CreateAdd(left, right);
      }
      vstack.push_back(result);
    }
  }

  void visit(ASTNodeIdentifier* n) {
    llvm::Value* v;
    if (global->declared(n->value)) {
      llvm::GlobalVariable* global_var = modul->getNamedGlobal(n->value);
      v = builder->CreateLoad(global_var, n->value);
    } else { /*
       llvm::AllocaInst* local_var = context->GetVariable(n->value);
       v = builder->CreateLoad(local_var, n->value);
        */
      v = current_vars[n->value];
    }
    vstack.push_back(v);
  }

  void visit(ASTNodeInt* n) {
    vstack.push_back(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), n->value));
  }
  void visit(ASTNodeFloat* n) {
    vstack.push_back(
        llvm::ConstantFP::get(llvm::Type::getFloatTy(*context), n->value));
  }
  void visit(ASTNodeString* n) {
    const std::string str = n->value;
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context);
    llvm::Constant* constantString =
        llvm::ConstantDataArray::getString(*context, str, true);
    llvm::ArrayType* arrayType = llvm::ArrayType::get(i8, str.length() + 1);

    llvm::AllocaInst* allocaInst = builder->CreateAlloca(
        arrayType,
        llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(*context), uint64_t(str.length() + 1)));
    llvm::StoreInst* storeInst =
        builder->CreateStore(constantString, allocaInst);
    (void)storeInst;

    vstack.push_back(
        builder->CreateBitCast(allocaInst, llvm::Type::getInt8PtrTy(*context)));
  }
};

}  // namespace dlang