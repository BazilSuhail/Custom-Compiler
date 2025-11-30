//===-- MyCustomLoopInterchange.cpp - Loop Interchange Pass ----------------===//
//
// Custom LLVM Transformation Pass
//
// This file implements a loop interchange optimization that swaps the execution
// order of perfectly nested loops to improve memory access patterns and cache
// locality.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

/// MyCustomLoopInterchange - Interchanges perfectly nested loops by swapping
/// their iteration spaces (induction variables and bounds).
///
/// Example transformation:
///   for (i = 0; i < N; i++)      =>     for (j = 0; j < M; j++)
///     for (j = 0; j < M; j++)             for (i = 0; i < N; i++)
///       A[i][j] = ...                       A[i][j] = ...
///
struct MyCustomLoopInterchange : public PassInfoMixin<MyCustomLoopInterchange> {

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);

    if (LI.empty())
      return PreservedAnalyses::all();

    // Find a perfectly nested 2-level loop (outer loop with exactly one subloop)
    Loop *Outer = nullptr;
    Loop *Inner = nullptr;
    for (Loop *L : LI) {
      if (L->getSubLoops().size() == 1) {
        Outer = L;
        Inner = L->getSubLoops()[0];
        break;
      }
    }

    if (!Outer || !Inner)
      return PreservedAnalyses::all();

    // Attempt interchange transformation
    if (performInterchange(Outer, Inner))
      return PreservedAnalyses::none();

    return PreservedAnalyses::all();
  }

private:
  /// Find the canonical induction variable PHI node in the loop header.
  /// Returns the first PHI with exactly two incoming values.
  PHINode *findInductionPHI(Loop *L) {
    for (Instruction &I : *L->getHeader()) {
      if (PHINode *PN = dyn_cast<PHINode>(&I)) {
        if (PN->getNumIncomingValues() == 2)
          return PN;
      }
    }
    return nullptr;
  }

  /// Find the loop exit condition comparison instruction.
  ICmpInst *findExitCompare(Loop *L) {
    for (Instruction &I : *L->getHeader()) {
      if (ICmpInst *Cmp = dyn_cast<ICmpInst>(&I))
        return Cmp;
    }
    return nullptr;
  }

  /// Perform the loop interchange transformation.
  /// Returns true if successful, false otherwise.
  bool performInterchange(Loop *Outer, Loop *Inner) {
    // Locate induction PHI nodes
    PHINode *OuterPHI = findInductionPHI(Outer);
    PHINode *InnerPHI = findInductionPHI(Inner);
    if (!OuterPHI || !InnerPHI)
      return false;

    // Locate exit condition compares
    ICmpInst *OuterCmp = findExitCompare(Outer);
    ICmpInst *InnerCmp = findExitCompare(Inner);
    if (!OuterCmp || !InnerCmp)
      return false;

    // Get loop structure blocks
    BasicBlock *OuterPre = Outer->getLoopPreheader();
    BasicBlock *InnerPre = Inner->getLoopPreheader();
    BasicBlock *OuterLatch = Outer->getLoopLatch();
    BasicBlock *InnerLatch = Inner->getLoopLatch();
    if (!OuterPre || !InnerPre || !OuterLatch || !InnerLatch)
      return false;

    // Get PHI incoming value indices
    int oPreIdx = OuterPHI->getBasicBlockIndex(OuterPre);
    int oLatchIdx = OuterPHI->getBasicBlockIndex(OuterLatch);
    int iPreIdx = InnerPHI->getBasicBlockIndex(InnerPre);
    int iLatchIdx = InnerPHI->getBasicBlockIndex(InnerLatch);
    if (oPreIdx < 0 || oLatchIdx < 0 || iPreIdx < 0 || iLatchIdx < 0)
      return false;

    Value *OuterStart = OuterPHI->getIncomingValue(oPreIdx);
    Value *OuterNext = OuterPHI->getIncomingValue(oLatchIdx);
    Value *InnerStart = InnerPHI->getIncomingValue(iPreIdx);
    Value *InnerNext = InnerPHI->getIncomingValue(iLatchIdx);

    // Clone increment instructions to maintain SSA dominance
    Instruction *OuterInc = dyn_cast<Instruction>(OuterNext);
    Instruction *InnerInc = dyn_cast<Instruction>(InnerNext);
    if (!OuterInc || !InnerInc)
      return false;

    // Clone inner increment for outer loop
    Instruction *NewOuterInc = InnerInc->clone();
    NewOuterInc->insertBefore(OuterLatch->getTerminator());
    for (unsigned i = 0; i < NewOuterInc->getNumOperands(); ++i) {
      if (NewOuterInc->getOperand(i) == InnerPHI)
        NewOuterInc->setOperand(i, OuterPHI);
    }

    // Clone outer increment for inner loop
    Instruction *NewInnerInc = OuterInc->clone();
    NewInnerInc->insertBefore(InnerLatch->getTerminator());
    for (unsigned i = 0; i < NewInnerInc->getNumOperands(); ++i) {
      if (NewInnerInc->getOperand(i) == OuterPHI)
        NewInnerInc->setOperand(i, InnerPHI);
    }

    // Swap PHI start values and connect new increments
    OuterPHI->setIncomingValue(oPreIdx, InnerStart);
    OuterPHI->setIncomingValue(oLatchIdx, NewOuterInc);
    InnerPHI->setIncomingValue(iPreIdx, OuterStart);
    InnerPHI->setIncomingValue(iLatchIdx, NewInnerInc);

    // Update uses in loop bodies (swap induction variable references)
    swapInductionVariableUses(Outer, Inner, OuterPHI, InnerPHI, OuterCmp,
                               InnerCmp, NewOuterInc, NewInnerInc);

    // Swap loop bounds in exit conditions
    swapLoopBounds(OuterPHI, InnerPHI, OuterCmp, InnerCmp);

    // Remove old increment instructions if unused
    if (OuterInc->use_empty())
      OuterInc->eraseFromParent();
    if (InnerInc->use_empty())
      InnerInc->eraseFromParent();

    // Rename for readability
    OuterPHI->setName("j");
    InnerPHI->setName("i");
    NewOuterInc->setName("j.next");
    NewInnerInc->setName("i.next");

    return true;
  }

  /// Swap uses of outer and inner induction variables in loop bodies.
  void swapInductionVariableUses(Loop *Outer, Loop *Inner, PHINode *OuterPHI,
                                   PHINode *InnerPHI, ICmpInst *OuterCmp,
                                   ICmpInst *InnerCmp, Instruction *NewOuterInc,
                                   Instruction *NewInnerInc) {
    SmallVector<std::pair<Use *, Value *>, 32> UsesToUpdate;

    // Collect outer PHI uses in inner loop body
    for (Use &U : OuterPHI->uses()) {
      Instruction *UserInst = dyn_cast<Instruction>(U.getUser());
      if (!UserInst)
        continue;

      // Skip PHIs, compares, and new increment instructions
      if (isa<PHINode>(UserInst) || UserInst == OuterCmp ||
          UserInst == InnerCmp || UserInst == NewOuterInc ||
          UserInst == NewInnerInc)
        continue;

      // Uses in inner loop should reference inner PHI
      if (Inner->contains(UserInst->getParent()))
        UsesToUpdate.push_back({&U, InnerPHI});
    }

    // Collect inner PHI uses in outer loop body (excluding inner loop)
    for (Use &U : InnerPHI->uses()) {
      Instruction *UserInst = dyn_cast<Instruction>(U.getUser());
      if (!UserInst)
        continue;

      if (isa<PHINode>(UserInst) || UserInst == OuterCmp ||
          UserInst == InnerCmp || UserInst == NewOuterInc ||
          UserInst == NewInnerInc)
        continue;

      // Uses in outer loop (but not inner) should reference outer PHI
      if (Outer->contains(UserInst->getParent()) &&
          !Inner->contains(UserInst->getParent()))
        UsesToUpdate.push_back({&U, OuterPHI});
    }

    // Apply all collected updates
    for (auto &P : UsesToUpdate)
      P.first->set(P.second);
  }

  /// Swap the loop bound values in exit condition comparisons.
  void swapLoopBounds(PHINode *OuterPHI, PHINode *InnerPHI, ICmpInst *OuterCmp,
                      ICmpInst *InnerCmp) {
    for (unsigned i = 0; i < 2; ++i) {
      if (OuterCmp->getOperand(i) == OuterPHI) {
        Value *OuterBound = OuterCmp->getOperand(1 - i);
        for (unsigned j = 0; j < 2; ++j) {
          if (InnerCmp->getOperand(j) == InnerPHI) {
            Value *InnerBound = InnerCmp->getOperand(1 - j);
            OuterCmp->setOperand(1 - i, InnerBound);
            InnerCmp->setOperand(1 - j, OuterBound);
            return;
          }
        }
      }
    }
  }
};

} // anonymous namespace

//===----------------------------------------------------------------------===//
// Pass Registration
//===----------------------------------------------------------------------===//

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MyCustomLoopInterchange",
          LLVM_VERSION_STRING, [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "my-loop-interchange") {
                    FPM.addPass(MyCustomLoopInterchange());
                    return true;
                  }
                  return false;
                });
          }};
}
