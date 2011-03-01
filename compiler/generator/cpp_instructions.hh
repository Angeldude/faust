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

#ifndef _CPP_INSTRUCTIONS_H
#define _CPP_INSTRUCTIONS_H

#include "loki/SafeFormat.h"

/**********************************************************************
			- code_gen.h : generic code generator (projet FAUST) -


		Historique :
		-----------

***********************************************************************/
using namespace std;

#include <string>
#include <list>
#include <set>
#include <map>

#include "instructions.hh"
#include "type_manager.hh"
#include "binop.hh"
#include "Text.hh"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

class CPPInstVisitor : public InstVisitor, public StringTypeManager {

    protected:

        int fTab;
        std::ostream* fOut;
        bool fFinishLine;

        virtual void tab1(int n, ostream& fout)
        {
            fout << '\n';
            while (n--) fout << '\t';
        }

        static map <string, int> gGlobalTable;

    public:

        CPPInstVisitor(std::ostream* out, int tab = 0)
            :fTab(tab), fOut(out), fFinishLine(true)
        {}

        virtual ~CPPInstVisitor()
        {}

        void Tab(int n) {fTab = n;}

        void EndLine()
        {
            if (fFinishLine) {
                *fOut << ";";
                tab1(fTab, *fOut);
            }
        }

        virtual void visit(AddMetaDeclareInst* inst)
        {
            *fOut << "interface->declare(" << "&" << inst->fZone <<", " << "\"" <<inst->fKey << "\"" << ", " <<  "\"" << inst->fValue << "\"" << ")"; EndLine();
        }

        virtual void visit(OpenboxInst* inst)
        {
            string name;
            switch (inst->fOrient) {
                case 0:
                    name = "interface->openVerticalBox"; break;
                case 1:
                    name = "interface->openHorizontalBox"; break;
                case 2:
                    name = "interface->openTabBox"; break;
            }
            *fOut << name << "(" << "\"" << inst->fName << "\"" << ")";
            EndLine();
        }

        virtual void visit(CloseboxInst* inst)
        {
            *fOut << "interface->closeBox();"; tab1(fTab, *fOut);
        }
        virtual void visit(AddButtonInst* inst)
        {
            if (inst->fType == AddButtonInst::kDefaultButton) {
                *fOut << "interface->addButton(" << "\"" << inst->fLabel << "\"" << "," << "&" << inst->fZone << ")"; EndLine();
            } else {
                *fOut << "interface->addCheckButton(" << "\"" << inst->fLabel << "\"" << "," << "&" << inst->fZone << ")"; EndLine();
            }
        }

        virtual void visit(AddSliderInst* inst)
        {
            string name;
            switch (inst->fType) {
                case AddSliderInst::kHorizontal:
                    name = "interface->addHorizontalSlider"; break;
                case AddSliderInst::kVertical:
                    name = "interface->addVerticalSlider"; break;
                case AddSliderInst::kNumEntry:
                    name = "interface->addNumEntry"; break;
            }
            if (strcmp(ifloat(), "float") == 0)
                *fOut << name << "(" << "\"" << inst->fLabel << "\"" << ", " << "&" << inst->fZone << ", " << checkFloat(inst->fInit) << ", " << checkFloat(inst->fMin) << ", " << checkFloat(inst->fMax) << ", " << checkFloat(inst->fStep) << ")";
            else
                *fOut << name << "(" << "\"" << inst->fLabel << "\"" << ", " << "&" << inst->fZone << ", " << inst->fInit << ", " << inst->fMin << ", " << inst->fMax << ", " << inst->fStep << ")";
            EndLine();
        }

        virtual void visit(AddBargraphInst* inst)
        {
            string name;
            switch (inst->fType) {
                case AddBargraphInst::kHorizontal:
                    name = "interface->addHorizontalBargraph"; break;
                case AddBargraphInst::kVertical:
                    name = "interface->addVerticalBargraph"; break;
            }
            if (strcmp(ifloat(), "float") == 0)
                *fOut << name << "(" << "\"" << inst->fLabel << "\"" << ", " << "&" << inst->fZone << ", "<< checkFloat(inst->fMin) << ", " << checkFloat(inst->fMax) << ")";
            else
                *fOut << name << "(" << "\"" << inst->fLabel << "\"" << ", " << "&" << inst->fZone << ", "<< inst->fMin << ", " << inst->fMax << ")";
            EndLine();
        }

        virtual void visit(LabelInst* inst)
        {
            *fOut << inst->fLabel;
            tab1(fTab, *fOut);
        }

        virtual void visit(DeclareVarInst* inst)
        {
            if (inst->fAddress->getAccess() & Address::kGlobal) {
                if (gGlobalTable.find(inst->fAddress->getName()) == gGlobalTable.end()) {
                    // If global is not defined
                    gGlobalTable[inst->fAddress->getName()] = 1;
                } else {
                    return;
                }
            }

            if (inst->fAddress->getAccess() & Address::kStaticStruct) {
                 *fOut << "static ";
            }

            if (inst->fAddress->getAccess() & Address::kVolatile) {
                 *fOut << "volatile ";
            }

            if (inst->fValue) {
                *fOut << generateType(inst->fTyped, inst->fAddress->getName()) << " = "; inst->fValue->accept(this); EndLine();
            } else {
                *fOut << generateType(inst->fTyped, inst->fAddress->getName()); EndLine();
            }
        }

        virtual void visit(RetInst* inst)
        {
            if (inst->fResult) {
                *fOut << "return "; inst->fResult->accept(this); EndLine();
            }
        }

        virtual void visit(DropInst* inst)
        {
            if (inst->fResult) {
                inst->fResult->accept(this); EndLine();
            }
        }

        virtual void visit(DeclareFunInst* inst)
        {
            if (gGlobalTable.find(inst->fName) != gGlobalTable.end())
                return;  // Already declared

            // Defined as macro in the architecture file...
            if (inst->fName == "min" || inst->fName == "max") {
                return;
            }

            // If function is actually a method (that is "xx::name"), then keep "xx::name" in gGlobalTable but print "name"
            string fun_name = inst->fName;
            size_t pos;
            if ((pos = inst->fName.find("::")) != string::npos) {
                fun_name = inst->fName.substr(pos + 2); // After the "::"
            }

            // Prototype
            if (inst->fType->fAttribute & FunTyped::kLocal)
                 *fOut << "inline ";

            if (inst->fType->fAttribute & FunTyped::kVirtual)
                 *fOut << "virtual ";

            *fOut << generateType(inst->fType->fResult, fun_name);
            *fOut << "(";
            list<NamedTyped*>::const_iterator it;
            int size = inst->fType->fArgsTypes.size(), i = 0;
            for (it = inst->fType->fArgsTypes.begin(); it != inst->fType->fArgsTypes.end(); it++, i++) {
                *fOut << generateType((*it));
                if (i < size - 1) *fOut << ", ";
            }

            if (inst->fCode->fCode.size() == 0) {
                *fOut << ");" << endl; ;  // Pure prototype
            } else {
                // Function body
                *fOut << ") {";
                    fTab++;
                    tab1(fTab, *fOut);
                    inst->fCode->accept(this);
                    fTab--;
                    tab1(fTab, *fOut);
                *fOut << "}";
                tab1(fTab, *fOut);
            }

            gGlobalTable[inst->fName] = 1;
        }

        virtual void visit(IndexedAddress* indexed)
        {
            indexed->fAddress->accept(this);
            *fOut << "[";
            indexed->fIndex->accept(this);
            *fOut << "]";
        }

        virtual void visit(CastAddress* casted)
        {
            if (gTypeNames.find(casted->fType) == gTypeNames.end())
               throw std::logic_error("casting to an undeclared type");

            string const & type = gTypeNames[casted->fType];
            string s;
            Loki::SPrintf(s, "reinterpret_cast<%s&>(")(type);
            *fOut << s;
            casted->fAddress->accept(this);
            *fOut << ")";
        }

        virtual void visit(LoadVarInst* inst)
        {
            NamedAddress* named = dynamic_cast<NamedAddress*>(inst->fAddress);
            IndexedAddress* indexed = dynamic_cast<IndexedAddress*>(inst->fAddress);

            if (named) {
                *fOut << named->getName();
            } else {
                /*
                *fOut << indexed->getName() << "[";
                indexed->fIndex->accept(this);
                *fOut << "]";
                */
                *fOut << indexed->getName();
                indexed->accept(this);
            }
        }

        virtual void visit(LoadVarAddressInst* inst)
        {
            NamedAddress* named = dynamic_cast<NamedAddress*>(inst->fAddress);
            IndexedAddress* indexed = dynamic_cast<IndexedAddress*>(inst->fAddress);

            if (named) {
                *fOut << "&" << named->getName();
            } else {
                *fOut << "&" << indexed->getName() << "[";
                indexed->fIndex->accept(this);
                *fOut << "]";
            }
        }

        virtual void visit(StoreVarInst* inst)
        {
            NamedAddress* named = dynamic_cast<NamedAddress*>(inst->fAddress);
            IndexedAddress* indexed = dynamic_cast<IndexedAddress*>(inst->fAddress);
            VectorAddress* vector = dynamic_cast<VectorAddress*>(inst->fAddress);

            if (named) {
                *fOut << named->getName() << " = ";
            } else if (indexed) {
                /*
                *fOut << indexed->getName() << "[";
                indexed->fAddress->accept(this);
                *fOut << "] = ";
                */
                *fOut << indexed->getName();
                indexed->accept(this);
                *fOut << " = ";
            } else if (vector) {
                throw std::runtime_error("cannot store to vector address");
            }
            inst->fValue->accept(this);
            EndLine();
        }

        virtual void visit(FloatNumInst* inst)
        {
            *fOut << checkFloat(inst->fNum);
        }

        virtual void visit(IntNumInst* inst)
        {
            *fOut << inst->fNum;
        }

        virtual void visit(BoolNumInst* inst)
        {
            *fOut << inst->fNum;
        }

        virtual void visit(DoubleNumInst* inst)
        {
            *fOut << inst->fNum;
        }

        virtual void visit(BinopInst* inst)
        {
            *fOut << "(";
            inst->fInst1->accept(this);
            *fOut << " ";
            *fOut << gBinOpTable[inst->fOpcode]->fName;
            *fOut << " ";
            inst->fInst2->accept(this);
            *fOut << ")";
        }

        virtual void visit(CastNumInst* inst)
        {
            *fOut << generateType(inst->fTyped);
            *fOut << "("; inst->fInst->accept(this);  *fOut << ")";
        }

        virtual void visit(FunCallInst* inst)
        {
            if (inst->fMethod) {
                list<ValueInst*>::const_iterator it =  inst->fArgs.begin();
                // Compile object arg
                (*it)->accept(this);
                *fOut << "->" << inst->fName << "(";
                list<ValueInst*>::const_iterator it1;
                int size = inst->fArgs.size() - 1, i = 0;
                for (it1 = ++it; it1 != inst->fArgs.end(); it1++, i++) {
                    // Compile argument
                    (*it1)->accept(this);
                    if (i < size - 1) *fOut << ", ";
                }
                *fOut << ")";
          } else {
                *fOut << inst->fName << "(";
                list<ValueInst*>::const_iterator it;
                int size = inst->fArgs.size(), i = 0;
                for (it = inst->fArgs.begin(); it != inst->fArgs.end(); it++, i++) {
                    // Compile argument
                    (*it)->accept(this);
                    if (i < size - 1) *fOut << ", ";
                }
                *fOut << ")";
            }
        }

        virtual void visit(Select2Inst* inst)
        {
            *fOut << "(";
            inst->fCond->accept(this);
            *fOut << "?";
            inst->fThen->accept(this);
            *fOut << ":";
            inst->fElse->accept(this);
            *fOut << ")";
        }

        virtual void visit(IfInst* inst)
        {
            *fOut << "if ";
            inst->fCond->accept(this);
            *fOut << " {";
                fTab++;
                tab1(fTab, *fOut);
                inst->fThen->accept(this);
                fTab--;
                tab1(fTab, *fOut);
            if (inst->fElse->fCode.size() > 0) {
                *fOut << "} else {";
                    fTab++;
                    tab1(fTab, *fOut);
                    inst->fElse->accept(this);
                    fTab--;
                    tab1(fTab, *fOut);
                *fOut << "}";
            } else {
                *fOut << "}";
            }
            tab1(fTab, *fOut);
        }

        virtual void visit(ForLoopInst* inst)
        {
            *fOut << "for (";
                fFinishLine = false;
                inst->fInit->accept(this);
                *fOut << "; ";
                inst->fEnd->accept(this);
                *fOut << "; ";
                inst->fIncrement->accept(this);
                fFinishLine = true;
            *fOut << ") {";
                fTab++;
                tab1(fTab, *fOut);
                inst->fCode->accept(this);
                fTab--;
                tab1(fTab, *fOut);
            *fOut << "}";
            tab1(fTab, *fOut);
        }

        virtual void visit(WhileLoopInst* inst)
        {
            *fOut << "while ("; inst->fCond->accept(this); *fOut << ") {";
                fTab++;
                tab1(fTab, *fOut);
                inst->fCode->accept(this);
                fTab--;
                tab1(fTab, *fOut);
             *fOut << "}";
             tab1(fTab, *fOut);
        }

        virtual void visit(BlockInst* inst)
        {
            if (inst->fIndent) {
                *fOut << "{";
                fTab++;
                tab1(fTab, *fOut);
            }
            list<StatementInst*>::const_iterator it;
            for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
                (*it)->accept(this);
            }
            if (inst->fIndent) {
                fTab--;
                tab1(fTab, *fOut);
                *fOut << "}";
                tab1(fTab, *fOut);
            }
        }

        virtual void visit(SwitchInst* inst)
        {
            *fOut << "switch ("; inst->fCond->accept(this); *fOut << ") {";
                fTab++;
                tab1(fTab, *fOut);
                list<pair <int, BlockInst*> >::const_iterator it;
                for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
                    if ((*it).first == -1) { // -1 used to code "default" case
                        *fOut << "default: {";
                    } else {
                         *fOut << "case " << (*it).first << ": {";
                    }
                        fTab++;
                        tab1(fTab, *fOut);
                        ((*it).second)->accept(this);
                        if (!((*it).second)->hasReturn())
                            *fOut << "break;";
                        fTab--;
                        tab1(fTab, *fOut);
                    *fOut << "}";
                    tab1(fTab, *fOut);
                }
                fTab--;
                tab1(fTab, *fOut);
            *fOut << "}";
            tab1(fTab, *fOut);
        }

        map<Typed*, string> gTypeNames;
        virtual void visit(DeclareTypeInst* inst)
        {
            static int index = 0;
            BasicTyped* basicTyped = dynamic_cast<BasicTyped*>(inst->fType);
            if (basicTyped) {
                switch (basicTyped->getType()) {
                case Typed::kInt:
                    gTypeNames[basicTyped] = "int";
                    return;

                case Typed::kFloat:
                    gTypeNames[basicTyped] = "float";
                    return;

                case Typed::kDouble:
                    gTypeNames[basicTyped] = "double";
                    return;

                case Typed::kQuad:
                    gTypeNames[basicTyped] = "long double";
                    return;

                default:
                    assert(false);
                }
            }

            ArrayTyped* arrayTyped = dynamic_cast<ArrayTyped*>(inst->fType);

            if (arrayTyped) {
                assert(gTypeNames.find(arrayTyped->fType) != gTypeNames.end());
                string basicType = gTypeNames[arrayTyped->fType];

                stringstream typeName;
                typeName << "VecType" << index++;

                string code;
                Loki::SPrintf(code, "typedef %s %s[%d]")(basicType)(typeName.str())(arrayTyped->fSize);

                *fOut << code;
                EndLine();

                gTypeNames[arrayTyped] = typeName.str();
            }
        }
};

class CPPVecInstVisitor : public CPPInstVisitor {

    public:

        CPPVecInstVisitor(std::ostream* out, int tab = 0)
            :CPPInstVisitor(out, tab)
        {}

};

/**
 * Use the Apple Accelerate framework.
 *
 */

class CPPVecAccelerateInstVisitor : public CPPVecInstVisitor {

    protected:

        std::string fCurValue;
        Typed::VarType fCurType;

        std::map<int, string> fVecBinOpTable;
        std::map<int, string> fScalarBinOpTable;
        std::map<string, string> fFunctionTable;
        int fVecCounter;

    public:

        CPPVecAccelerateInstVisitor(std::ostream* out, int tab = 0)
            :CPPVecInstVisitor(out, tab)
        {
            fVecBinOpTable[kAdd] = "vDSP_vadd";
            fVecBinOpTable[kSub] = "vDSP_vsub";
            fVecBinOpTable[kMul] = "vDSP_vmul";
            fVecBinOpTable[kDiv] = "vDSP_vdiv";

            fFunctionTable["sin"] = "vvsin";
            fFunctionTable["sinf"] = "vvsinf";
            fFunctionTable["cos"] = "vvcos";
            fFunctionTable["cosf"] = "vvcosf";
            fFunctionTable["tan"] = "vvatan";
            fFunctionTable["tanf"] = "vvatanf";
            fFunctionTable["log"] = "vvlog";
            fFunctionTable["logf"] = "vvlogf";
            fFunctionTable["log10"] = "vvlog10";
            fFunctionTable["log10f"] = "vvlog10f";

            //fFunctionTable["log2"] = "native_log2";
            //fFunctionTable["log2f"] = "native_log2";

            fFunctionTable["exp"] = "vvexp";
            fFunctionTable["expf"] = "vvexpf";
            fFunctionTable["pow"] = "vvpow";
            fFunctionTable["powf"] = "vvpowf";
            fFunctionTable["sqrt"] = "vvrsqrt";
            fFunctionTable["sqrtf"] = "vvrsqrtf";
            fFunctionTable["fabsf"] = "vvfabf";
            fFunctionTable["floor"] = "vvfloor";
            fFunctionTable["floorf"] = "vvfloorf";
            fFunctionTable["fmodf"] = "vvfmodf";

            fVecCounter = 0;
        }

        ~CPPVecAccelerateInstVisitor()
        {}

        inline string generateNameVec()
        {
            return subst("vec$0_", T(fVecCounter++));
        }

        virtual void visit(LoadVarInst* inst)
        {
             // Keep name as value
            fCurValue = inst->fAddress->getName();
            //cerr << "inst->fAddress->getName " << inst->fAddress->getName() << std::endl;
            // Keep type
            assert(gVarTable.find(inst->fAddress->getName()) != gVarTable.end());
            fCurType = gVarTable[inst->fAddress->getName()]->getType();
        }

        virtual void visit(FloatNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kFloat;
        }

        virtual void visit(IntNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kInt;
        }

        virtual void visit(BoolNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kDouble;
        }

        virtual void visit(DoubleNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kBool;
        }

        virtual void visit(BinopInst* inst)
        {
            // Keep result of first arg compilation
            inst->fInst1->accept(this);
            std::string res1 = fCurValue;

            // Keep result of second arg compilation
            inst->fInst2->accept(this);
            std::string res2 = fCurValue;

            // Generate new result symbol, both arguments are equal, so fCurType is the one of last evaluated one
            fCurValue = generateNameVec();
            *fOut << fTypeDirectTable[fCurType] << " " << fCurValue << "[" << inst->fSize << "]"; EndLine();

            // Generate stream
            if (inst->fInst1->fSize >= 1 && inst->fInst2->fSize >= 1) {
                // Full vector operation
                cerr << "inst->fOpcode " << inst->fOpcode  << endl;
                *fOut << fVecBinOpTable[inst->fOpcode] << "(" << res1 << ", 1, " << res2 << ", 1, " << fCurValue << ", 1, " << inst->fSize << ")";
            } else if (inst->fInst1->fSize > 1) {
                // Scalar-Vec operation
                // TODO
                // *fOut << fScalarBinOpTable[inst->fOpcode] << "(" << res1 << ", 1, " << res2 << fCurValue << ", 1, " << inst->fSize << ")";
            } else {
                // Scalar-Vec operation
            }

            EndLine();
        }

        virtual void visit(CastNumInst* inst)
        {
            // Compile value to cast, result in fCurValue
            inst->fInst->accept(this);

            BasicTyped* basic_typed = dynamic_cast<BasicTyped*>(inst->fTyped);

            switch (basic_typed->fType) {

                case Typed::kFloat: {
                    string res = generateNameVec();
                    *fOut << fTypeDirectTable[Typed::kFloat] << " " << res << "[" << inst->fSize << "]"; EndLine();
                    switch (fCurType) {

                        case Typed::kInt:
                            *fOut << "vDSP_vflt32(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                        case Typed::kFloat:
                            // No supposed to happen
                            assert(false);
                            break;

                         case Typed::kDouble:
                            *fOut << "vDSP_vspdp(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         default:
                            // No supposed to happen
                            cerr << "CurType " << fTypeDirectTable[fCurType] << endl;
                            assert(false);
                            break;
                    }
                    fCurType = Typed::kFloat;
                    fCurValue = res;
                    break;
                }

                case Typed::kInt: {
                    string res = generateNameVec();
                    //*fOut << fTypeDirectTable[Typed::kInt] << " " << res << "[" << inst->fSize << "]"; EndLine();
                    switch (fCurType) {

                        case Typed::kInt:
                            // No supposed to happen
                            assert(false);
                            break;

                        case Typed::kFloat:
                            *fOut << "vDSP_vfixr32(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         case Typed::kDouble:
                            *fOut << "vDSP_vfixr32D(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         default:
                            // No supposed to happen
                            assert(false);
                            break;
                    }
                    fCurType = Typed::kInt;
                    fCurValue = res;
                    break;
                }

                case Typed::kDouble: {
                    string res = generateNameVec();
                    *fOut << fTypeDirectTable[Typed::kDouble] << " " << res << "[" << inst->fSize << "]"; EndLine();
                    switch (fCurType) {

                        case Typed::kInt:
                            *fOut << "vDSP_vflt32D(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                        case Typed::kFloat:
                            *fOut << "vDSP_vdpsp(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         case Typed::kDouble:
                            // No supposed to happen
                            assert(false);
                            break;

                         default:
                            // No supposed to happen
                            assert(false);
                            break;
                    }
                    fCurType = Typed::kDouble;
                    fCurValue = res;
                    break;
                }

                case Typed::kQuad:
                    // No supposed to happen
                    assert(false);
                    break;

                default:
                    // No supposed to happen
                    assert(false);
                    break;

            }
            EndLine();
        }

        virtual void visit(FunCallInst* inst)
        {
            string res = generateNameVec();
            *fOut << fTypeDirectTable[fCurType] << " " << res << "[" << inst->fSize << "]"; EndLine();

            if (inst->fMethod) {
                list<ValueInst*>::const_iterator it =  inst->fArgs.begin();
                // Compile object arg
                (*it)->accept(this);
                *fOut << "->" << ((fFunctionTable.find(inst->fName) != fFunctionTable.end()) ? fFunctionTable[inst->fName] : inst->fName) << "(";
                list<ValueInst*>::const_iterator it1;
                int i = 0;
                // Add result in parameter list
                *fOut << res;
                for (it1 = ++it; it1 != inst->fArgs.end(); it1++, i++) {
                    // Compile argument
                    (*it1)->accept(this);
                    *fOut << ", ";
                }
                // Add "count" in parameter list
                *fOut << inst->fSize;
                *fOut << ")";
            } else {
                *fOut << ((fFunctionTable.find(inst->fName) != fFunctionTable.end()) ? fFunctionTable[inst->fName] : inst->fName) << "(";
                list<ValueInst*>::const_iterator it;
                int i = 0;
                // Add result in parameter list
                *fOut << res;
                for (it = inst->fArgs.begin(); it != inst->fArgs.end(); it++, i++) {
                    // Compile argument
                    (*it)->accept(this);
                    *fOut << ", ";
                }
                // Add "count" in parameter list
                *fOut << inst->fSize;
                *fOut << ")";
            }

            fCurValue = res;
            // fCurType does not change
        }

        virtual void visit(Select2Inst* inst)
        {
            // Compile condition, result in fCurValue
            inst->fCond->accept(this);
            std::string cond_value = fCurValue;

            // Compile then branch, result in fCurValue
            inst->fThen->accept(this);
            std::string then_value = fCurValue;

            // Compile else branch, result in fCurValue
            inst->fElse->accept(this);
            std::string else_value = fCurValue;

            // TODO
        }
};

#endif
