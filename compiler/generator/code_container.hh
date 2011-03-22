/************************************************************************
 ************************************************************************
    FAUST compiler
	Copyright (C) 2003-2004 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ************************************************************************
 ************************************************************************/

#ifndef _CODE_CONTAINER_H
#define _CODE_CONTAINER_H

/**********************************************************************
			- code_gen.h : generic code generator (projet FAUST) -


		Historique :
		-----------

***********************************************************************/
//using namespace std;

#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>

#include "instructions.hh"
#include "sigtype.hh"
#include "tlib.hh"
#include "description.hh"
#include "floats.hh"
#include "Text.hh"
#include "property.hh"
#include "function_builder.hh"
#include "code_loop.hh"
#include "fir_to_fir.hh"

class CodeContainer {

    protected:
        list <CodeContainer*> fSubContainers;

        int fNumInputs;
        int fNumOutputs;
        vector<int> fInputRates;
        vector<int> fOutputRates;

        // Declaration part
        BlockInst* fExtGlobalDeclarationInstructions;
        BlockInst* fGlobalDeclarationInstructions;
        BlockInst* fDeclarationInstructions;

        // Init method
        BlockInst* fInitInstructions;
        BlockInst* fPostInitInstructions;

        // To be used in delete method
        BlockInst* fDestroyInstructions;

        // Static init method
        BlockInst* fStaticInitInstructions;
        BlockInst* fPostStaticInitInstructions;

        // Compute method
        BlockInst* fComputeBlockInstructions;

        // Additionnal functions
        BlockInst* fComputeFunctions;

        // User interface
        BlockInst* fUserInterfaceInstructions;

        set<string>	fIncludeFileSet;
        set<string>	fLibrarySet;

        // DAG of loops
        CodeLoop* fCurLoop;

        property<CodeLoop*> fLoopProperty;     ///< loops used to compute some signals

        list<string> fUICode;
        list<string> fUIMacro;

        int fNumActives;       ///< number of active controls in the UI (sliders, buttons, etc.)
        int fNumPassives;      ///< number of passive widgets in the UI (bargraphs, etc.)

        int fSubContainerType;

        string fFullCount;

        void incUIActiveCount()    { fNumActives++; }
        void incUIPassiveCount()   { fNumPassives++; }

        void merge(set<string>& dst, set<string>& src)
        {
            set<string>::iterator i;
            for (i = src.begin(); i != src.end(); i++)  dst.insert(*i);
        }

        void collectIncludeFile(set<string>& S)
        {
            list<CodeContainer* >::iterator k;
            for (k = fSubContainers.begin(); k != fSubContainers.end(); k++) (*k)->collectIncludeFile(S);
            merge(S, fIncludeFileSet);
        }

        void collectLibrary(set<string>& S)
        {
            list<CodeContainer* >::iterator k;
            for (k = fSubContainers.begin(); k != fSubContainers.end(); k++) (*k)->collectLibrary(S);
            merge(S, fLibrarySet);
        }

        void generateDAGLoopAux(CodeLoop* loop, BlockInst* loop_code, DeclareVarInst* count, int loop_num, bool omp = false);

      public:

        CodeContainer();
        void initializeCodeContainer(int numInputs, int numOutputs);
        virtual ~CodeContainer();

        CodeLoop* getCurLoop() { return fCurLoop; }

        void openLoop(string index_name, int size = 0);
        void openLoop(Tree recsymbol, string index_name, int size = 0);
        void closeLoop(Tree sig = 0);

        int	inputs() 	{ return fNumInputs; }
        int outputs()	{ return fNumOutputs; }

        void setInputs(int inputs)      { fNumInputs = inputs; }
        void setOutputs(int outputs)    { fNumOutputs = outputs; }

        void setInputRate(int channel, int rate) { fInputRates[channel] = rate;}
        void setOutputRate(int channel, int rate) { fOutputRates[channel] = rate;}

        int getInputRate(int channel) { return fInputRates[channel];}
        int getOutputRate(int channel) { return fOutputRates[channel];}

        void addSubContainer(CodeContainer* container) { fSubContainers.push_back(container); }

        void addIncludeFile(const string& str)  { fIncludeFileSet.insert(str); }
        void addLibrary(const string& str)      { fLibrarySet.insert(str); }

        void printLibrary(ostream& fout);
         void printIncludeFile(ostream& fout);

        void setLoopProperty(Tree sig, CodeLoop* l);     ///< Store the loop used to compute a signal
        bool getLoopProperty(Tree sig, CodeLoop*& l);    ///< Returns the loop used to compute a signal

        void printGraphDotFormat(ostream& fout);

        void transformDAG(DispatchVisitor* visitor);
        void computeForwardDAG(lclgraph dag);
        void sortDeepFirstDAG(CodeLoop* l, set<CodeLoop*>& visited, list<CodeLoop*>& result);

        void generateLocalInputs(BlockInst* loop_code);
        void generateLocalOutputs(BlockInst* loop_code);

        DeclareFunInst* generateGetInputs(const string& name, bool isvirtual);
        DeclareFunInst* generateGetOutputs(const string& name, bool isvirtual);

        DeclareFunInst* generateGetInputRate(const string& name, bool isvirtual);
        DeclareFunInst* generateGetOutputRate(const string& name, bool isvirtual);

        LoadVarInst * loadFullCount(void)
        {
            return InstBuilder::genLoadStackVar(fFullCount);
        }

        void generateDAGLoop(BlockInst* loop_code, DeclareVarInst* count);

        /* can be overridden by subclasses to reorder the FIR before the actual code generation */
        virtual void processFIR(void) {}

        // Fill code for each method
        StatementInst* pushDeclare(StatementInst* inst)
        {
            fDeclarationInstructions->pushBackInst(inst);
            // TODO : add inter-loop vectors in current loop
            return inst;
        }

        StatementInst* pushGlobalDeclare(StatementInst* inst)
        {
            fGlobalDeclarationInstructions->pushBackInst(inst);
            return inst;
        }

        StatementInst* pushExtGlobalDeclare(StatementInst* inst)
        {
            fExtGlobalDeclarationInstructions->pushBackInst(inst);
            return inst;
        }

        ValueInst* pushFunction(const string& name, Typed::VarType result, vector<Typed::VarType>& types, const list<ValueInst*>& args);

        void generateExtGlobalDeclarations(InstVisitor* visitor)
        {
            if (fExtGlobalDeclarationInstructions->fCode.size() > 0) {
                fExtGlobalDeclarationInstructions->accept(visitor);
            }
        }

        void generateGlobalDeclarations(InstVisitor* visitor)
        {
            if (fGlobalDeclarationInstructions->fCode.size() > 0) {
                fGlobalDeclarationInstructions->accept(visitor);
            }
        }

        void generateDeclarations(InstVisitor* visitor)
        {
           if (fDeclarationInstructions->fCode.size() > 0) {
                // Sort arrays to be at the begining
                fDeclarationInstructions->fCode.sort(sortArrayDeclarations);
                fDeclarationInstructions->accept(visitor);
            }
        }

        void generateInit(InstVisitor* visitor)
        {
            if (fInitInstructions->fCode.size() > 0) {
                fInitInstructions->accept(visitor);
            }

            if (fPostInitInstructions->fCode.size() > 0) {
                fPostInitInstructions->accept(visitor);
            }
        }

        void generateStaticInit(InstVisitor* visitor)
        {
            if (fStaticInitInstructions->fCode.size() > 0) {
                fStaticInitInstructions->accept(visitor);
            }

            if (fPostStaticInitInstructions->fCode.size() > 0) {
                fPostStaticInitInstructions->accept(visitor);
            }
        }

        void generateUserInterface(InstVisitor* visitor)
        {
            if (fUserInterfaceInstructions->fCode.size() > 0) {
                fUserInterfaceInstructions->accept(visitor);
            }
        }

        void generateComputeFunctions(InstVisitor* visitor)
        {
            if (fComputeFunctions->fCode.size() > 0) {
                fComputeFunctions->accept(visitor);
            }
        }

        void generateComputeBlock(InstVisitor* visitor)
        {
            if (fComputeBlockInstructions->fCode.size() > 0) {
                fComputeBlockInstructions->accept(visitor);
            }
        }

        void generateDestroy(InstVisitor* visitor)
        {
            if (fDestroyInstructions->fCode.size() > 0) {
                fDestroyInstructions->accept(visitor);
            }
        }

        StatementInst* pushInitMethod(StatementInst* inst) { fInitInstructions->pushBackInst(inst); return inst; }
        StatementInst* pushPostInitMethod(StatementInst* inst) { fPostInitInstructions->pushBackInst(inst); return inst; }
        StatementInst* pushFrontInitMethod(StatementInst* inst) { fInitInstructions->pushFrontInst(inst); return inst; }
        StatementInst* pushDestroyMethod(StatementInst* inst) { fDestroyInstructions->pushBackInst(inst); return inst; }
        StatementInst* pushStaticInitMethod(StatementInst* inst) { fStaticInitInstructions->pushBackInst(inst); return inst; }
        StatementInst* pushPostStaticInitMethod(StatementInst* inst) { fPostStaticInitInstructions->pushBackInst(inst); return inst; }
        StatementInst* pushComputeBlockMethod(StatementInst* inst) { fComputeBlockInstructions->pushBackInst(inst); return inst; }
        StatementInst* pushUserInterfaceMethod(StatementInst* inst) { fUserInterfaceInstructions->pushBackInst(inst); return inst; }

        StatementInst* pushComputePreDSPMethod(StatementInst* inst)     { return fCurLoop->pushComputePreDSPMethod(inst); }
        StatementInst* pushComputeDSPMethod(StatementInst* inst)        { return fCurLoop->pushComputeDSPMethod(inst); }
        StatementInst* pushComputePostDSPMethod(StatementInst* inst)    { return fCurLoop->pushComputePostDSPMethod(inst); }

        void generateSubContainers()
        {
            list<CodeContainer*>::const_iterator it;
            for (it = fSubContainers.begin(); it != fSubContainers.end(); it++) {
                (*it)->produceInternal();
            }
        }

        int getSubContainerType() const
        {
            return fSubContainerType;
        }


        // UI construction
        void addUIMacro(const string& str)  { fUIMacro.push_back(str); }
        void addUICode(const string& str)	{ fUICode.push_back(str); }

        virtual CodeContainer* createInternalContainer(const string& name, int sub_container_type) = 0;

        virtual void produceInternal() = 0;
        virtual void produceClass() {}

        virtual void dump() {}
        virtual void dump(ostream* dst) {}

};

inline bool isElement(const set<CodeLoop*>& S, CodeLoop* l)
{
	return S.find(l) != S.end();
}

#endif
