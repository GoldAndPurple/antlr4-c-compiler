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

  llvm::Value* visit_value(ASTNodeExpr* n) {
    n->accept(this);
    if (!vstack.empty()) {
      llvm::Value* value = vstack.back();
      vstack.pop_back();
      return value;
    }
    throw std::runtime_error("internal error during expression generation");
  }

  void visit(ASTNodeProgram* n) {
    /* declare printf */
    std::vector<llvm::Type*> params = {llvm::Type::getInt8PtrTy(*context)};
    llvm::FunctionType* signature =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), params, true);
    llvm::Function::Create(
        signature, llvm::Function::ExternalLinkage, "printf", modul);
    /* declare scanf */
    std::vector<llvm::Type*> params_2 = {llvm::Type::getInt8PtrTy(*context)};
    llvm::FunctionType* signature_2 = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context), params_2, true);
    llvm::Function::Create(
        signature_2, llvm::Function::ExternalLinkage, "scanf", modul);

    visit_children(n);
  }
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
        auto pname = n->parameters->param_names[i];
        arg.setName(pname);

        auto var =
            builder->CreateAlloca(arg.getType(), nullptr, pname + "_ptr");
        builder->CreateStore(&arg, var);
        current_vars[pname] = var;
      }
      i++;
    }

    visit_children(n);
    current_scope = current_scope->parent;
    current_vars.clear();
  }

  void visit(ASTNodeFuncCall* n) {
    llvm::Function* call = modul->getFunction(n->name);
    std::vector<llvm::Value*> params;
    for (auto param : n->children[0]->children) {
      llvm::Value* p = visit_value((ASTNodeExpr*)(param));
      params.push_back(p);
    }
    vstack.push_back(builder->CreateCall(call, params));
  }

  void visit(ASTNodeDecl* n) {
    for (auto c : n->children) {
      if (c->getToken() == assignment) {
        /*
        n   ->    c [id] ->value
        decl-> assign[id]->expression
        */
        std::string varname = (((ASTNodeAssign*)c)->id)->value;
        /* global var */
        if (current_scope == global && global->declared(varname)) {
          llvm::GlobalVariable* var;
          llvm::Type* to_alloc = nullptr;
          if (((ASTNodeAssign*)c)->id->array_idx > 0) {
            to_alloc = llvm::ArrayType::get(
                llvmtype(global->get(varname)),
                ((ASTNodeAssign*)c)->id->array_idx);
          } else {
            to_alloc = llvmtype(global->get(varname));
          }
          modul->getOrInsertGlobal(varname, to_alloc);
          var = modul->getNamedGlobal(varname);
          llvm::Constant* initValue = nullptr;
          if (global->get(varname) == type_int) {
            initValue = llvm::dyn_cast<llvm::Constant>(
                visit_value((ASTNodeExpr*)(c->children[0])));
          } else {
            throw std::runtime_error("Type float not supported");
          }
          var->setInitializer(initValue);
        } else {
          /* in function */
          llvm::Type* to_alloc = nullptr;
          if (((ASTNodeAssign*)c)->id->array_idx > 0) {
            to_alloc = llvm::ArrayType::get(
                llvmtype(current_scope->get(varname)),
                ((ASTNodeAssign*)c)->id->array_idx);
          } else {
            to_alloc = llvmtype(current_scope->get(varname));
          }
          auto variable = builder->CreateAlloca(to_alloc, nullptr, varname);
          current_vars[varname] = variable;
          visit((ASTNodeAssign*)c);
        }
      } else /* if token == id */ {
        std::string varname = ((ASTNodeIdentifier*)c)->value;
        llvm::Type* to_alloc = nullptr;
        if (((ASTNodeIdentifier*)c)->array_idx > 0) {
          to_alloc = llvm::ArrayType::get(
              llvmtype(current_scope->get(varname)),
              ((ASTNodeIdentifier*)c)->array_idx);
        } else {
          to_alloc = llvmtype(current_scope->get(varname));
        }
        auto variable = builder->CreateAlloca(to_alloc, nullptr, varname);
        current_vars[varname] = variable;
        current_vars[varname] = variable;
      }
    }
  }

  void visit(ASTNodeAssign* n) {
    auto var = current_vars[n->id->value];
    llvm::Value* result = visit_value((ASTNodeExpr*)(n->children[0]));
    if (n->id->array_idx > -1) {
      auto target_idx = llvm::ConstantInt::get(
          llvm::Type::getInt64Ty(*context), n->id->array_idx);
      auto zero_idx =
          llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0);
      var = builder->CreateGEP(
          var, llvm::ArrayRef<llvm::Value*>{zero_idx, target_idx});
    }
    builder->CreateStore(result, var);
  }

  void visit(ASTNodeReturn* n) {
    if (!n->children.empty()) {
      llvm::Value* ret_expr = visit_value((ASTNodeExpr*)(n->children[0]));
      builder->CreateRet(ret_expr);
    } else {
      builder->CreateRetVoid();
    }
  }

  void visit(ASTNodeConditional* n) {
    llvm::Value* cond = visit_value((ASTNodeExpr*)(n->condition));
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* then_bb =
        llvm::BasicBlock::Create(*context, "if_true", func);

    /* can only support a single ELSE */
    if ((n->elseif != nullptr) && (n->elseif->condition == nullptr)) {
      llvm::BasicBlock* else_bb =
          llvm::BasicBlock::Create(*context, "if_false", func);
      builder->CreateCondBr(cond, then_bb, else_bb);
      llvm::BasicBlock* cont_bb =
          llvm::BasicBlock::Create(*context, "if_continue", func);

      builder->SetInsertPoint(then_bb);
      visit_children(n);
      builder->CreateBr(cont_bb);

      builder->SetInsertPoint(else_bb);
      visit_children(n->elseif);
      builder->CreateBr(cont_bb);

      builder->SetInsertPoint(cont_bb);
      return;

    } else if ((n->elseif != nullptr) && (n->elseif->condition != nullptr)) {
      throw std::runtime_error("codegen for 'else if' blocks not supported");
    }

    llvm::BasicBlock* cont_bb =
        llvm::BasicBlock::Create(*context, "if_continue", func);
    builder->CreateCondBr(cond, then_bb, cont_bb);

    builder->SetInsertPoint(then_bb);
    visit_children(n);
    builder->CreateBr(cont_bb);

    builder->SetInsertPoint(cont_bb);
  }

  void visit(ASTNodeLoop* n) {
    llvm::Value* cond = visit_value((ASTNodeExpr*)(n->condition));
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* body_bb =
        llvm::BasicBlock::Create(*context, "while_body", func);
    llvm::BasicBlock* after_bb =
        llvm::BasicBlock::Create(*context, "while_continue", func);
    builder->CreateCondBr(cond, body_bb, after_bb);

    builder->SetInsertPoint(body_bb);
    visit_children(n);
    cond = visit_value((ASTNodeExpr*)(n->condition));
    builder->CreateCondBr(cond, body_bb, after_bb);

    builder->SetInsertPoint(after_bb);
  }

  void visit(ASTNodeBinary* n) {
    llvm::Value* left = visit_value((ASTNodeExpr*)(n->children[0]));
    llvm::Value* right = visit_value((ASTNodeExpr*)(n->children[1]));
    llvm::Value* result;

    if (left->getType()->isIntegerTy()) {
      if (n->op == "==") {
        result = builder->CreateICmpEQ(left, right);
      } else if (n->op == "!=") {
        result = builder->CreateICmpNE(left, right);
      } else if (n->op == "<") {
        result = builder->CreateICmpSLT(left, right);
      } else if (n->op == ">") {
        result = builder->CreateICmpSGT(left, right);
      } else if (n->op == "*") {
        result = builder->CreateMul(left, right);
      } else if (n->op == "/") {
        result = builder->CreateSDiv(left, right);
      } else if (n->op == "+") {
        result = builder->CreateAdd(left, right);
      } else if (n->op == "-") {
        result = builder->CreateSub(left, right);
      } else {
        throw std::runtime_error(
            n->op + " operator is not supported in this expression");
      }
      vstack.push_back(result);
    }
  }

  void visit(ASTNodeIdentifier* n) {
    llvm::Value* v;
    if (global->declared(n->value)) {
      llvm::GlobalVariable* global_var = modul->getNamedGlobal(n->value);
      if (n->array_idx > -1) {
        auto target_idx = llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(*context), n->array_idx);
        auto zero_idx =
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0);
        v = builder->CreateGEP(
            global_var, llvm::ArrayRef<llvm::Value*>{zero_idx, target_idx});
        v = builder->CreateLoad(v);
      } else {
        v = builder->CreateLoad(global_var);
      }
    } else {
      if (n->referenced) {
        v = builder->CreateIntToPtr(
            current_vars[n->value], llvm::Type::getInt32PtrTy(*context));
      } else {
        if (n->array_idx > -1) {
          auto target_idx = llvm::ConstantInt::get(
              llvm::Type::getInt64Ty(*context), n->array_idx);
          auto zero_idx =
              llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0);
          v = builder->CreateGEP(
              current_vars[n->value], llvm::ArrayRef<llvm::Value*>{zero_idx, target_idx});
          v = builder->CreateLoad(v);
        } else {
          v = builder->CreateLoad(current_vars[n->value]);
        }
      }
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
    throw std::runtime_error("Type float not supported");
  }
  void visit(ASTNodeString* n) {
    vstack.push_back(builder->CreateGlobalStringPtr((n->value), "str"));
  }
};

}  // namespace dlang