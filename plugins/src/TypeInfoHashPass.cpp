#include <vector>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

struct ModifyTypeInfo : public ModulePass {
  static char ID;

  ModifyTypeInfo() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    std::vector<GlobalValue *> GlobalValuesToModify;

    for (GlobalValue &GlobalValue : M.global_values()) {
      if (!GlobalValue.getName().startswith("_ZTS")) // type_info::name
        continue;

      GlobalValuesToModify.push_back(&GlobalValue);
    }

    for (GlobalValue *GlobalValue : GlobalValuesToModify) {
      LLVMContext &Context = M.getContext();

      StringRef GlobalName = GlobalValue->getName();
      std::hash<std::string> hasher;
      size_t typeHash = hasher(GlobalName.str());
      std::string newName = Twine::utohexstr(typeHash).str().substr(0, 8);

      GlobalVariable *GVar = M.getNamedGlobal(GlobalName);
      if (GVar && GVar->hasInitializer()) {
        Constant *NewInit =
            ConstantDataArray::getString(Context, newName, true);

        GlobalVariable *NewGV = new GlobalVariable(
            M, NewInit->getType(), true, GVar->getLinkage(), NewInit,
            GVar->getName(), nullptr, GVar->getThreadLocalMode(),
            GVar->getAddressSpace(), GVar->isExternallyInitialized());
        NewGV->setDSOLocal(GVar->isDSOLocal());

        GVar->replaceAllUsesWith(NewGV);
        GVar->eraseFromParent();
      }
    }

    return false;
  }
};

} // namespace

char ModifyTypeInfo::ID = 0;
static RegisterPass<ModifyTypeInfo> _X("modify-type-info",
                                       "Modify Type Info Pass");
