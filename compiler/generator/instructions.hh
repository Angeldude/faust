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

#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

/**********************************************************************
			- code_gen.h : generic code generator (projet FAUST) -


		Historique :
		-----------

***********************************************************************/

using namespace std;

#include <string>
#include <list>
#include <set>
#include <stack>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>
#include <stdio.h>

#include "binop.hh"
#include "property.hh"
#include "Text.hh"

// ============================
// Generic instruction visitor
// ============================

struct Printable;
struct NullInst;
struct DeclareVarInst;
struct DeclareFunInst;
struct DeclareTypeInst;
struct LoadVarInst;
struct LoadVarAddressInst;
struct StoreVarInst;
struct FloatNumInst;
struct IntNumInst;
struct BoolNumInst;
struct DoubleNumInst;
struct BinopInst;
struct CastNumInst;
struct RetInst;
struct DropInst;

struct FunCallInst;
struct Select2Inst;
struct IfInst;
struct ForLoopInst;
struct WhileLoopInst;
struct BlockInst;
struct SwitchInst;

// User interface
struct AddMetaDeclareInst;
struct OpenboxInst;
struct CloseboxInst;
struct AddButtonInst;
struct AddSliderInst;
struct AddBargraphInst;
struct LabelInst;

struct Typed;
struct Address;
struct ValueInst;
struct StatementInst;

struct BasicTyped;
struct NamedTyped;
struct FunTyped;
struct ArrayTyped;

struct NamedAddress;
struct IndexedAddress;
struct CastAddress;

// Globals
extern map<string, Typed*> gVarTable;

// =========
// Visitors
// =========

class InstVisitor {

    public:

        InstVisitor()
        {}
        virtual ~InstVisitor()
        {}

        virtual void visit(Printable* inst) {}
        virtual void visit(NullInst* inst) {}

        // Declarations
        virtual void visit(DeclareVarInst* inst) {}
        virtual void visit(DeclareFunInst* inst) {}
        virtual void visit(DeclareTypeInst* inst) {}

        // Memory
        virtual void visit(LoadVarInst* inst) {}
        virtual void visit(LoadVarAddressInst* inst) {}
        virtual void visit(StoreVarInst* inst) {}

        // Addresses
        virtual void visit(NamedAddress* address) {}
        virtual void visit(IndexedAddress* address) {}
        virtual void visit(CastAddress* address) {}

        // Primitives : numbers
        virtual void visit(FloatNumInst* inst) {}
        virtual void visit(IntNumInst* inst) {}
        virtual void visit(BoolNumInst* inst) {}
        virtual void visit(DoubleNumInst* inst) {}

        // Numerical computation
        virtual void visit(BinopInst* inst) {}
        virtual void visit(CastNumInst* inst) {}

        // Function call
        virtual void visit(FunCallInst* inst) {}
        virtual void visit(RetInst* inst) {}
        virtual void visit(DropInst* inst) {}

        // Conditionnal
        virtual void visit(Select2Inst* inst) {}
        virtual void visit(IfInst* inst) {}
        virtual void visit(SwitchInst* inst) {}

        // Loops
        virtual void visit(ForLoopInst* inst) {}
        virtual void visit(WhileLoopInst* inst) {}

        // Block
        virtual void visit(BlockInst* inst) {}

        // User interface
        virtual void visit(AddMetaDeclareInst* inst) {}
        virtual void visit(OpenboxInst* inst) {}
        virtual void visit(CloseboxInst* inst) {}
        virtual void visit(AddButtonInst* inst) {}
        virtual void visit(AddSliderInst* inst) {}
        virtual void visit(AddBargraphInst* inst) {}

        virtual void visit(LabelInst* inst) {}

};

class CloneVisitor {

    public:

        CloneVisitor()
        {}
        virtual ~CloneVisitor()
        {}

        virtual ValueInst* visit(NullInst* inst) = 0;

        // Declarations
        virtual StatementInst* visit(DeclareVarInst* inst) = 0;
        virtual StatementInst* visit(DeclareFunInst* inst) = 0;
        virtual StatementInst* visit(DeclareTypeInst* inst) = 0;

        // Memory
        virtual ValueInst* visit(LoadVarInst* inst) = 0;
        virtual ValueInst* visit(LoadVarAddressInst* inst) = 0;
        virtual StatementInst* visit(StoreVarInst* inst) = 0;

        // Addresses
        virtual Address* visit(NamedAddress* address) = 0;
        virtual Address* visit(IndexedAddress* address) = 0;
        virtual Address* visit(CastAddress* address) = 0;

        // Primitives : numbers
        virtual ValueInst* visit(FloatNumInst* inst) = 0;
        virtual ValueInst* visit(IntNumInst* inst) = 0;
        virtual ValueInst* visit(BoolNumInst* inst) = 0;
        virtual ValueInst* visit(DoubleNumInst* inst) = 0;

        // Numerical computation
        virtual ValueInst* visit(BinopInst* inst) = 0;
        virtual ValueInst* visit(CastNumInst* inst) = 0;

        // Function call
        virtual ValueInst* visit(FunCallInst* inst) = 0;
        virtual StatementInst* visit(RetInst* inst) = 0;
        virtual StatementInst* visit(DropInst* inst) = 0;

        // Conditionnal
        virtual ValueInst* visit(Select2Inst* inst) = 0;
        virtual StatementInst* visit(IfInst* inst) = 0;
        virtual StatementInst* visit(SwitchInst* inst) = 0;

        // Loops
        virtual StatementInst* visit(ForLoopInst* inst) = 0;
        virtual StatementInst* visit(WhileLoopInst* inst) = 0;

        // Block
        virtual StatementInst* visit(BlockInst* inst) = 0;

        // User interface
        virtual StatementInst* visit(AddMetaDeclareInst* inst) = 0;
        virtual StatementInst* visit(OpenboxInst* inst) = 0;
        virtual StatementInst* visit(CloseboxInst* inst) = 0;
        virtual StatementInst* visit(AddButtonInst* inst) = 0;
        virtual StatementInst* visit(AddSliderInst* inst) = 0;
        virtual StatementInst* visit(AddBargraphInst* inst) = 0;
        virtual StatementInst* visit(LabelInst* inst) = 0;

        // Types
        virtual Typed* visit(BasicTyped* type) = 0;
        virtual Typed* visit(NamedTyped* type) = 0;
        virtual Typed* visit(FunTyped* type) = 0;
        virtual Typed* visit(ArrayTyped* type) = 0;
};

// ============================
// Base class for instructions
// ============================

struct Printable
{
    static std::ostream* fOut;

    int fTab;

    Printable()
    {}
    virtual ~Printable()
    {}

};

struct Vectorizable
{
    int fSize;

    Vectorizable(int size = 1)
        :fSize(size)
    {}
    virtual ~Vectorizable()
    {}

};

// Added in compilation environment
struct StatementInst : public Printable
{
    virtual void accept(InstVisitor* visitor) = 0;

    virtual StatementInst* clone(CloneVisitor* cloner) = 0;
};

// Results from the compilation
struct ValueInst : public Printable, public Vectorizable
{
    virtual void accept(InstVisitor* visitor) = 0;

    virtual ValueInst* clone(CloneVisitor* cloner) = 0;

    ValueInst(int size = 1):Vectorizable(size)
    {}
};

// ==================
// Null instruction
// ==================

struct NullInst : public ValueInst
{
    NullInst()
    {}

    virtual void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

// ==========================
//  Instruction with a type
// ==========================

struct Typed : public Printable
{
    enum VarType {kFloatMacro, kFloatMacro_ptr, kFloat, kFloat_ptr, kFloat_vec, kFloat_vec_ptr,
                kInt, kInt_ptr, kInt_vec, kInt_vec_ptr,
                kDouble, kDouble_ptr, kDouble_vec, kDouble_vec_ptr,
                kQuad, kQuad_ptr,
                kBool, kBool_ptr, kBool_vec, kBool_vec_ptr,
                kVoid, kVoid_ptr, kVoid_ptr_ptr, kObj, kObj_ptr};

    Typed()
    {}

    virtual VarType getType() const = 0;

    // Returns the pointer type version of a primitive type
    static VarType getPtrFromType(VarType type)
    {
        switch (type) {
            case kFloatMacro:
                return kFloatMacro_ptr;
            case kFloat:
                return kFloat_ptr;
            case kFloat_vec:
                return kFloat_vec_ptr;
            case kInt:
                return kInt_ptr;
            case kInt_vec:
                return kInt_vec_ptr;
            case kDouble:
                return kDouble_ptr;
            case kDouble_vec:
                return kDouble_vec_ptr;
            case kQuad:
                return kQuad_ptr;
            case kBool:
                return kBool_ptr;
            case kBool_vec:
                return kBool_vec_ptr;
            case kVoid:
                return kVoid_ptr;
            case kVoid_ptr:
                return kVoid_ptr_ptr;
            default:
                // Not supposed to happen
                cerr << "getPtrFromType " << type << endl;
                assert(false);
                return kVoid;
        }
    }

    // Returns the vector type version of a primitive type
    static VarType getVecFromType(VarType type)
    {
        switch (type) {
            case kFloat:
                return kFloat_vec;
            case kInt:
                return kInt_vec;
            case kDouble:
                return kDouble_vec;
            case kBool:
                return kBool_vec;
            default:
                // Not supposed to happen
                cerr << "getVecFromType " << type << endl;
                assert(false);
                return kVoid;
        }
    }

    // Returns the type version from pointer on a primitive type
    static VarType getTypeFromPtr(VarType type)
    {
        switch (type) {
            case kFloatMacro_ptr:
                return kFloatMacro;
            case kFloat_ptr:
                return kFloat;
            case kFloat_vec_ptr:
                return kFloat_vec;
            case kInt_ptr:
                return kInt;
            case kInt_vec_ptr:
                return kInt_vec;
            case kDouble_ptr:
                return kDouble;
            case kQuad_ptr:
                return kQuad;
            case kDouble_vec_ptr:
                return kDouble_vec;
            case kBool_ptr:
                return kBool;
            case kBool_vec_ptr:
                return kBool_vec;
            case kVoid_ptr:
                return kVoid;
            case kVoid_ptr_ptr:
                return kVoid_ptr;
            default:
                // Not supposed to happen
                cerr << "getTypeFromPtr " << type << endl;
                assert(false);
                return kVoid;
        }
    }

    // Returns the type version from vector on a primitive type
    static VarType getTypeFromVec(VarType type)
    {
        switch (type) {
            case kFloat_vec:
                return kFloat;
            case kInt_vec:
                return kInt;
            case kDouble_vec:
                return kDouble;
            case kBool_vec:
                return kBool;
            default:
                // Not supposed to happen
                cerr << "getTypeFromVec " << type << endl;
                assert(false);
                return kVoid;
        }
    }

    virtual Typed* clone(CloneVisitor* cloner) = 0;

    virtual std::vector<int> dimensions() const = 0;
};

struct BasicTyped : public Typed {

    VarType fType;

    static map<VarType, BasicTyped*> gTypeTable;

    BasicTyped(VarType type)
        :fType(type)
    {}

    VarType getType() const { return fType; }

    Typed* clone(CloneVisitor* cloner) { return cloner->visit(this); }

    std::vector<int> dimensions() const
    {
        return std::vector<int>(); // empty vector
    }
};

struct NamedTyped : public Typed {

    string fName;
    Typed* fType;

    NamedTyped(const string& name, Typed* type)
        :fName(name), fType(type)
    {}

    VarType getType() const { return fType->getType(); }

    Typed* clone(CloneVisitor* cloner) { return cloner->visit(this); }

    std::vector<int> dimensions() const
    {
        return fType->dimensions();
    }
};

struct FunTyped : public Typed {

    enum FunAttribute {
        kDefault = 0x1,
        kLocal = 0x2,
        kVirtual = 0x4
    };

    list<NamedTyped*> fArgsTypes;
    BasicTyped* fResult;
    FunAttribute fAttribute;

    FunTyped(const list<NamedTyped*>& args, BasicTyped* result, FunAttribute attribute = kDefault)
        :fArgsTypes(args), fResult(result), fAttribute(attribute)
    {}

    VarType getType() const { assert(false); return fResult->getType(); }

    Typed* clone(CloneVisitor* cloner) { return cloner->visit(this); }

    std::vector<int> dimensions() const
    {
        throw std::logic_error("FunTyped doesn't have the notion of \"dimensions\"");
        return std::vector<int>();
    }
};

struct ArrayTyped : public Typed {

    Typed* fType;
    int fSize;

    ArrayTyped(Typed* type, int size)
        :fType(type), fSize(size)
    {}

    VarType getType() const { return getPtrFromType(fType->getType()); }

    Typed* clone(CloneVisitor* cloner) { return cloner->visit(this); }

    std::vector<int> dimensions() const
    {
        std::vector<int> base = fType->dimensions();
        base.push_back(fSize);
        return base;
    }
};

inline ArrayTyped * isArrayTyped(Typed * t)
{
    return dynamic_cast<ArrayTyped*>(t);
}

// ============
//  Addresses
// ============

struct Address : public Printable {

    enum AccessType {
        kStruct = 0x1,
        kStaticStruct = 0x2,
        kFunArgs = 0x4,
        kStack = 0x8,
        kGlobal = 0x10,
        kLink = 0x20,
        kLoop = 0x40,
        kVolatile = 0x80
    };

    Address()
    {}

    virtual void setAccess(Address::AccessType type) = 0;
    virtual Address::AccessType getAccess() = 0;

    virtual void setName(const string& name) = 0;
    virtual string getName() = 0;

    static void dump(AccessType access)
    {
        if (access & kStruct) {
            *fOut << "kStruct";
        } else if (access & kStaticStruct) {
            *fOut << "kStaticStruct";
        } else if (access & kFunArgs) {
            *fOut << "kFunArgs";
        } else if (access & kStack) {
            *fOut << "kStack";
        } else if (access & kGlobal) {
            *fOut << "kGlobal";
        } else if (access & kLink) {
            *fOut << "kLink";
        } else if (access & kLoop) {
            *fOut << "kLoop";
        } else if (access & kVolatile) {
            *fOut << "kVolatile";
        }
    }

    virtual Address* clone(CloneVisitor* cloner) = 0;

    virtual void accept(InstVisitor* visitor) = 0;

    virtual std::vector<int> dimensions() const = 0;
};

struct NamedAddress : public Address {

    string fName;
    AccessType fAccess;
    Typed * fTyped;

    NamedAddress(const string& name, AccessType access, Typed * typed)
        :fName(name), fAccess(access), fTyped(typed)
    {}

    NamedAddress(const string& name, AccessType access)
#ifdef __GNUC__
        __attribute__ ((deprecated))
#endif
        :fName(name), fAccess(access)
    {}

    void setAccess(Address::AccessType type) { fAccess = type; }
    Address::AccessType getAccess() { return fAccess; }

    void setName(const string& name) { fName = name; }
    string getName() {return fName; }

    Address* clone(CloneVisitor* cloner) { return cloner->visit(this); }
    void accept(InstVisitor* visitor) { visitor->visit(this); }

    virtual std::vector<int> dimensions() const
    {
        if (!fTyped)
            // FIXME: for now assume a scalar
            return std::vector<int>();
        return fTyped->dimensions();
    }
};

struct IndexedAddress : public Address {

    Address* fAddress;
    ValueInst* fIndex;

    IndexedAddress(Address* address, ValueInst* index)
        :fAddress(address), fIndex(index)
    {}

    void setAccess(Address::AccessType type) { fAddress->setAccess(type); }
    Address::AccessType getAccess() { return fAddress->getAccess(); }

    void setName(const string& name) { fAddress->setName(name); }
    string getName() {return fAddress->getName(); }

    Address* clone(CloneVisitor* cloner) { return cloner->visit(this); }
    void accept(InstVisitor* visitor) { visitor->visit(this); }

    vector<int> dimensions(void) const
    {
        vector<int> base = fAddress->dimensions();
        base.pop_back();
        return base;
    }
};

struct CastAddress: public Address
{
    Address * fAddress;
    Typed * fType;

    CastAddress (Address * address, Typed * type):
        fAddress(address), fType(type)
    {}
    void setAccess(Address::AccessType type) { fAddress->setAccess(type); }
    Address::AccessType getAccess() { return fAddress->getAccess(); }

    void setName(const string& name) { fAddress->setName(name); }
    string getName() {return fAddress->getName(); }

    Address* clone(CloneVisitor* cloner) { return cloner->visit(this); }
    void accept(InstVisitor* visitor) { visitor->visit(this); }

    vector<int> dimensions(void) const
    {
        return fType->dimensions();
    }
};


// ===============
// User interface
// ===============

struct AddMetaDeclareInst : public StatementInst
{
    string fZone;
    string fKey;
    string fValue;

    AddMetaDeclareInst(const string& zone, const string& key, const string& value)
        :fZone(zone), fKey(key), fValue(value)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct OpenboxInst : public StatementInst
{
    int fOrient;
    string fName;

    OpenboxInst(int orient, const string& name)
        :fOrient(orient), fName(name)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct CloseboxInst : public StatementInst
{
    CloseboxInst()
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct AddButtonInst : public StatementInst
{
    enum ButtonType {kDefaultButton, kCheckbutton};

    string fLabel;
    string fZone;
    ButtonType fType;

    AddButtonInst(const string& label, const string& zone, ButtonType type)
        :fLabel(label), fZone(zone), fType(type)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct AddSliderInst : public StatementInst
{
    enum SliderType {kHorizontal, kVertical, kNumEntry};

    string fLabel;
    string fZone;
    double fInit;
    double fMin;
    double fMax;
    double fStep;
    SliderType fType;

    AddSliderInst(const string& label, const string& zone, double init, double min, double max, double step, SliderType type)
        :fLabel(label), fZone(zone), fInit(init), fMin(min), fMax(max), fStep(step), fType(type)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct AddBargraphInst : public StatementInst
{
     enum BargraphType {kHorizontal, kVertical};

    string fLabel;
    string fZone;
    double fMin;
    double fMax;
    BargraphType fType;

    AddBargraphInst(const string& label, const string& zone, double min, double max, BargraphType type)
        :fLabel(label), fZone(zone), fMin(min), fMax(max), fType(type)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct LabelInst : public StatementInst
{
    string fLabel;

    LabelInst(const string& label)
        :fLabel(label)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

// =============
// Declarations
// =============

struct DeclareVarInst : public StatementInst
{
    Address* fAddress;
    Typed* fTyped;
    ValueInst* fValue;

    DeclareVarInst(Address* address, Typed* typed, ValueInst* value)
        :fAddress(address), fTyped(typed), fValue(value)
    {
        assert(value ? dynamic_cast<NullInst*>(value) == 0 : true);

        if (gVarTable.find(fAddress->getName()) == gVarTable.end()) {
            gVarTable[fAddress->getName()] = typed;
        }
    }

    virtual ~DeclareVarInst()
    {
        gVarTable.erase(fAddress->getName());
        delete fAddress;
    }

    void setAccess(Address::AccessType type) { fAddress->setAccess(type); }
    Address::AccessType getAccess() { return fAddress->getAccess(); }

    void setName(const string& name) { fAddress->setName(name); }
    string getName() {return fAddress->getName(); }

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }

    Address * getAddress(void) { return fAddress; }
    struct StoreVarInst * store (ValueInst * val);
    struct LoadVarInst * load ();
};

// ==============
// Memory access
// ==============

struct DropInst : public StatementInst
{
    ValueInst* fResult;

    DropInst(ValueInst* result = NULL)
        :fResult(result)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct LoadVarInst : public ValueInst
{
    Address* fAddress;

    LoadVarInst(Address* address, int size = 1)
        :ValueInst(size), fAddress(address)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct LoadVarAddressInst : public ValueInst
{
    Address* fAddress;

    LoadVarAddressInst(Address* address, int size = 1)
        :ValueInst(size), fAddress(address)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct StoreVarInst : public StatementInst
{
    Address* fAddress;
    ValueInst* fValue;

    StoreVarInst(Address* address, ValueInst* value)
        :fAddress(address), fValue(value)
    {
        assert(dynamic_cast<NullInst*>(value) == 0);
    }

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

// ========
// Numbers
// ========

struct FloatNumInst : public ValueInst
{
    float fNum;

    FloatNumInst(float num, int size = 1)
        :ValueInst(size), fNum(num)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct DoubleNumInst : public ValueInst
{
    double fNum;

    DoubleNumInst(double num, int size = 1)
        :ValueInst(size), fNum(num)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct IntNumInst : public ValueInst
{
    int fNum;

    IntNumInst(int num, int size = 1)
        :ValueInst(size), fNum(num)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct BoolNumInst : public ValueInst
{
    int fNum;

    BoolNumInst(bool num, int size = 1)
        :ValueInst(size), fNum(num)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct BinopInst : public ValueInst
{
    int fOpcode;
    ValueInst* fInst1;
    ValueInst* fInst2;

    BinopInst(int opcode, ValueInst* inst1, ValueInst* inst2, int size = 1)
        :ValueInst(size), fOpcode(opcode), fInst1(inst1), fInst2(inst2)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct CastNumInst : public ValueInst
{
    Typed* fTyped;
    ValueInst* fInst;

    CastNumInst(ValueInst* inst, Typed* typed, int size = 1)
        :ValueInst(size), fTyped(typed), fInst(inst)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

// ==============
// Control flow
// ===============

struct BlockInst : public StatementInst
{
    list<StatementInst*> fCode;
    bool fIndent;

    BlockInst(list<StatementInst*> code)
        :fCode(code), fIndent(false)
    {}

    BlockInst()
        :fIndent(false)
    {}

    void setIndent(bool indent) { fIndent = indent; }
    bool getIndent() { return fIndent; }

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }

    void pushFrontInst(StatementInst* inst)
    {
        fCode.push_front(inst);
    }

    void pushBackInst(StatementInst* inst)
    {
        fCode.push_back(inst);
    }

    bool hasReturn();

};

struct Select2Inst : public ValueInst
{
    ValueInst* fCond;
    ValueInst* fThen;
    ValueInst* fElse;

    Select2Inst(ValueInst* cond_inst, ValueInst* then_inst, ValueInst* else_inst, int size = 1)
        :ValueInst(size), fCond(cond_inst), fThen(then_inst), fElse(else_inst)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct IfInst : public StatementInst
{
    ValueInst* fCond;
    BlockInst* fThen;
    BlockInst* fElse;

    IfInst(ValueInst* cond_inst, BlockInst* then_inst, BlockInst* else_inst)
        :fCond(cond_inst), fThen(then_inst), fElse(else_inst)
    {}

    IfInst(ValueInst* cond_inst, BlockInst* then_inst)
        :fCond(cond_inst), fThen(then_inst), fElse(new BlockInst())
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct SwitchInst : public StatementInst
{
    ValueInst* fCond;
    list<pair<int, BlockInst*> > fCode;

    SwitchInst(ValueInst* cond, const list<pair<int, BlockInst*> >& code)
        :fCond(cond), fCode(code)
    {}

    SwitchInst(ValueInst* cond)
        :fCond(cond)
    {}

    void addCase(int value, BlockInst* block) { fCode.push_back(make_pair(value, block)); }

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct RetInst : public StatementInst
{
    ValueInst* fResult;

    RetInst(ValueInst* result = NULL)
        :fResult(result)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct FunCallInst : public ValueInst
{
    string fName;
    list<ValueInst*> fArgs;   // List of arguments
    bool fMethod;

    FunCallInst(const string& name, const list<ValueInst*>& args, bool method, int size = 1)
        :ValueInst(size), fName(name), fArgs(args), fMethod(method)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    ValueInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct DeclareFunInst : public StatementInst
{
    string fName;
    FunTyped* fType;
    BlockInst* fCode;    // Code is a list of StatementInst*

    DeclareFunInst(const string& name, FunTyped* type, BlockInst* code)
        :fName(name), fType(type), fCode(code)
    {}
    DeclareFunInst(const string& name, FunTyped* type)
        :fName(name), fType(type), fCode(new BlockInst())
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

struct DeclareTypeInst : public StatementInst
{
    /*
    NamedTyped* fType;

    DeclareTypeInst(const string& name, Typed* type)
        :fType(new NamedTyped(name, type))
    {}
    DeclareTypeInst(NamedTyped* type)
        :fType(type)
    {}
    */

    Typed* fType;

    DeclareTypeInst(Typed* type)
        :fType(type)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

// ======
// Loops
// ======

struct ForLoopInst : public StatementInst
{
    StatementInst* fInit;
    ValueInst* fEnd;
    StatementInst* fIncrement;
    BlockInst* fCode;

    ForLoopInst(StatementInst* init, ValueInst* end, StatementInst* increment, BlockInst* code)
        :fInit(init), fEnd(end), fIncrement(increment), fCode(code)
    {}
    ForLoopInst(StatementInst* init, ValueInst* end, StatementInst* increment)
        :fInit(init), fEnd(end), fIncrement(increment), fCode(new BlockInst())
    {}

    void pushFrontInst(StatementInst* inst)
    {
        fCode->pushFrontInst(inst);
    }

    void pushBackInst(StatementInst* inst)
    {
        fCode->pushBackInst(inst);
    }

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }

    ValueInst * loadDeclaration(void)
    {
        DeclareVarInst * declaration = dynamic_cast<DeclareVarInst*>(fInit);
        assert(declaration);
        return declaration->load();
    }
};

struct WhileLoopInst : public StatementInst
{
    ValueInst* fCond;
    BlockInst* fCode;

    WhileLoopInst(ValueInst* cond, BlockInst* code)
        :fCond(cond), fCode(code)
    {}

    void accept(InstVisitor* visitor) { visitor->visit(this); }

    StatementInst* clone(CloneVisitor* cloner) { return cloner->visit(this); }
};

// =====================
// Basic clone visitor
// =====================

class BasicCloneVisitor : public CloneVisitor {

    public:

        BasicCloneVisitor()
        {}

        virtual NullInst* visit(NullInst* inst) { return new NullInst(); }

        // Declarations
        virtual StatementInst* visit(DeclareVarInst* inst)
        {
            return (inst->fValue)
                ? new DeclareVarInst(inst->fAddress->clone(this), inst->fTyped->clone(this), inst->fValue->clone(this))
                : new DeclareVarInst(inst->fAddress->clone(this), inst->fTyped->clone(this), NULL);
        }
        virtual StatementInst* visit(DeclareFunInst* inst)
        {
            return new DeclareFunInst(inst->fName, dynamic_cast<FunTyped*>(inst->fType->clone(this)), dynamic_cast<BlockInst*>(inst->fCode->clone(this)));
        }
        virtual StatementInst* visit(DeclareTypeInst* inst)
        {
            return new DeclareTypeInst(inst->fType->clone(this));
        }

        // Memory
        virtual ValueInst* visit(LoadVarInst* inst) { return new LoadVarInst(inst->fAddress->clone(this), inst->fSize); }
        virtual ValueInst* visit(LoadVarAddressInst* inst) { return new LoadVarAddressInst(inst->fAddress->clone(this), inst->fSize); }
        virtual StatementInst* visit(StoreVarInst* inst) { return new StoreVarInst(inst->fAddress->clone(this), inst->fValue->clone(this)); }

        // Addresses
        virtual Address* visit(NamedAddress* address) { return new NamedAddress(address->fName, address->fAccess, address->fTyped); }
        virtual Address* visit(IndexedAddress* address) { return new IndexedAddress(address->fAddress->clone(this), address->fIndex->clone(this)); }
        virtual Address* visit(CastAddress* address) { return new CastAddress(address->fAddress->clone(this), address->fType->clone(this)); }

        // Primitives : numbers and string
        virtual ValueInst* visit(FloatNumInst* inst) { return new FloatNumInst(inst->fNum, inst->fSize); }
        virtual ValueInst* visit(IntNumInst* inst) { return new IntNumInst(inst->fNum, inst->fSize); }
        virtual ValueInst* visit(BoolNumInst* inst) { return new BoolNumInst(inst->fNum, inst->fSize); }
        virtual ValueInst* visit(DoubleNumInst* inst) { return new DoubleNumInst(inst->fNum, inst->fSize); }

        // Numerical computation
        virtual ValueInst* visit(BinopInst* inst)
        {
            return new BinopInst(inst->fOpcode, inst->fInst1->clone(this), inst->fInst2->clone(this), inst->fSize);
        }

        virtual ValueInst* visit(CastNumInst* inst) { return new CastNumInst(inst->fInst->clone(this), inst->fTyped->clone(this), inst->fSize); }

        // Function call
        virtual ValueInst* visit(FunCallInst* inst)
        {
            list<ValueInst*> cloned_args;

            for (list<ValueInst*>::const_iterator it = inst->fArgs.begin(); it != inst->fArgs.end(); it++) {
                cloned_args.push_back((*it)->clone(this));
            }

            return new FunCallInst(inst->fName, cloned_args, inst->fMethod, inst->fSize);
        }
        virtual StatementInst* visit(RetInst* inst) { return new RetInst((inst->fResult) ? inst->fResult->clone(this) : NULL); }
        virtual StatementInst* visit(DropInst* inst) { return new DropInst((inst->fResult) ? inst->fResult->clone(this) : NULL); }

        // Conditionnal
        virtual ValueInst* visit(Select2Inst* inst) { return new Select2Inst(inst->fCond->clone(this), inst->fThen->clone(this), inst->fElse->clone(this), inst->fSize); }
        virtual StatementInst* visit(IfInst* inst)
        {
            return new IfInst(inst->fCond->clone(this), dynamic_cast<BlockInst*>(inst->fThen->clone(this)), dynamic_cast<BlockInst*>(inst->fElse->clone(this)));
        }
        virtual StatementInst* visit(SwitchInst* inst)
        {
            SwitchInst* cloned = new SwitchInst(inst->fCond->clone(this));
            list<pair <int, BlockInst*> >::const_iterator it;
            for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
                cloned->addCase((*it).first, dynamic_cast<BlockInst*>(((*it).second)->clone(this)));
            }
            return cloned;
        }

        // Loop
        virtual StatementInst* visit(ForLoopInst* inst)
        {
            return new ForLoopInst(inst->fInit->clone(this), inst->fEnd->clone(this), inst->fIncrement->clone(this), dynamic_cast<BlockInst*>(inst->fCode->clone(this)));
        }

        virtual StatementInst* visit(WhileLoopInst* inst)
        {
            return new WhileLoopInst(inst->fCond->clone(this), dynamic_cast<BlockInst*>(inst->fCode->clone(this)));
        }

        // Block
        virtual StatementInst* visit(BlockInst* inst)
         {
            list<StatementInst*> cloned;
            list<StatementInst*>::const_iterator it;
            for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
                cloned.push_back((*it)->clone(this));
            }
            return new BlockInst(cloned);
        }

        // User interface
        virtual StatementInst* visit(AddMetaDeclareInst* inst) {return new AddMetaDeclareInst(inst->fZone, inst->fKey, inst->fValue); }
        virtual StatementInst* visit(OpenboxInst* inst) { return new OpenboxInst(inst->fOrient, inst->fName); }
        virtual StatementInst* visit(CloseboxInst* inst) { return new CloseboxInst(); }
        virtual StatementInst* visit(AddButtonInst* inst) { return new AddButtonInst(inst->fLabel, inst->fZone, inst->fType); }
        virtual StatementInst* visit(AddSliderInst* inst) { return new AddSliderInst(inst->fLabel, inst->fZone, inst->fInit, inst->fMin, inst->fMax, inst->fStep, inst->fType); }
        virtual StatementInst* visit(AddBargraphInst* inst) { return new AddBargraphInst(inst->fLabel, inst->fZone, inst->fMin, inst->fMax, inst->fType); }
        virtual StatementInst* visit(LabelInst* inst) { return new LabelInst(inst->fLabel); }

        // Typed
        virtual Typed* visit(BasicTyped* typed) { return typed->gTypeTable[typed->fType]; }
        virtual Typed* visit(NamedTyped* typed) { return new NamedTyped(typed->fName, typed->fType); }
        virtual Typed* visit(FunTyped* typed)
        {
            list<NamedTyped*> cloned;
            list<NamedTyped*>::const_iterator it;
            for (it = typed->fArgsTypes.begin(); it != typed->fArgsTypes.end(); it++) {
                cloned.push_back(dynamic_cast<NamedTyped*>((*it)->clone(this)));
            }
            return new FunTyped(cloned, dynamic_cast<BasicTyped*>(typed->fResult->clone(this)), typed->fAttribute);
        }
        virtual Typed* visit(ArrayTyped* typed) { return new ArrayTyped(typed->fType->clone(this), typed->fSize); }
};

// ========================
// Basic dispatch visitor
// ========================

struct DispatchVisitor : public InstVisitor {

    virtual void visit(DeclareVarInst* inst)
    {
        if (inst->fValue)
            inst->fValue->accept(this);
    }

    virtual void visit(LoadVarInst* inst) { inst->fAddress->accept(this); }
    virtual void visit(LoadVarAddressInst* inst) { inst->fAddress->accept(this); }
    virtual void visit(StoreVarInst* inst)
    {
        inst->fAddress->accept(this);
        inst->fValue->accept(this);
    }

    virtual void visit(IndexedAddress* address)
    {
        address->fAddress->accept(this);
        address->fIndex->accept(this);
    }

    virtual void visit(BinopInst* inst)
    {
        inst->fInst1->accept(this);
        inst->fInst2->accept(this);
    }

    virtual void visit(CastNumInst* inst) { inst->fInst->accept(this); }

    virtual void visit(FunCallInst* inst)
    {
        list<ValueInst*>::const_iterator it;
        for (it = inst->fArgs.begin(); it != inst->fArgs.end(); it++) {
            (*it)->accept(this);
        }
    }

    virtual void visit(RetInst* inst)
    {
        if (inst->fResult)
            inst->fResult->accept(this);
    }

    virtual void visit(DropInst* inst)
    {
        if (inst->fResult)
            inst->fResult->accept(this);
    }

    virtual void visit(Select2Inst* inst)
    {
        inst->fCond->accept(this);
        inst->fThen->accept(this);
        inst->fElse->accept(this);
    }

    virtual void visit(IfInst* inst)
    {
        inst->fCond->accept(this);
        inst->fThen->accept(this);
        inst->fElse->accept(this);
    }

    virtual void visit(ForLoopInst* inst)
    {
        inst->fInit->accept(this);
        inst->fEnd->accept(this);
        inst->fIncrement->accept(this);
        inst->fCode->accept(this);
    }

     virtual void visit(WhileLoopInst* inst)
    {
        inst->fCond->accept(this);
        inst->fCode->accept(this);
    }

    virtual void visit(SwitchInst* inst)
    {
        inst->fCond->accept(this);
        list<pair <int, BlockInst*> >::const_iterator it;
        for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
            ((*it).second)->accept(this);
        }
    }

    virtual void visit(BlockInst* inst)
    {
        list<StatementInst*>::const_iterator it;
        for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
            (*it)->accept(this);
        }
    }
};

struct VariableScopeVisitor : public DispatchVisitor {

    stack<list<DeclareVarInst*> > fBlockVarTable;

    VariableScopeVisitor()
    {}

    virtual void visit(DeclareVarInst* inst)
    {
        fBlockVarTable.top().push_back(inst);
        DispatchVisitor::visit(inst);
    }

    virtual void visit(BlockInst* inst)
    {
        list<DeclareVarInst*> variable_block;
        fBlockVarTable.push(variable_block);
        DispatchVisitor::visit(inst);
        fBlockVarTable.pop();
    }
};

class ScalVecDispatcherVisitor : public DispatchVisitor {

    protected:

        InstVisitor* fScalarVisitor;
        InstVisitor* fVectorVisitor;

        void Dispatch2Visitor(ValueInst* inst)
        {
            if (inst->fSize == 1) {
                fScalarVisitor->visit(inst);
            } else {
                fVectorVisitor->visit(inst);
            }
        }

    public:

        ScalVecDispatcherVisitor(InstVisitor* scal, InstVisitor* vec)
            :fScalarVisitor(scal), fVectorVisitor(vec)
        {}

        ~ScalVecDispatcherVisitor()
        {
            delete fScalarVisitor;
            delete fVectorVisitor;
        }

        virtual void visit(LoadVarInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(LoadVarAddressInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(StoreVarInst* inst)
        {
            if (inst->fValue->fSize == 1) {
                fScalarVisitor->visit(inst);
            } else {
                fVectorVisitor->visit(inst);
            }
        }

        virtual void visit(FloatNumInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(IntNumInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(BoolNumInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(DoubleNumInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(BinopInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(CastNumInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(FunCallInst* inst)
        {
            Dispatch2Visitor(inst);
        }

        virtual void visit(Select2Inst* inst)
        {
            Dispatch2Visitor(inst);
        }

};

// ========================
// Combining visitors
// ========================

class CombinerVisitor : public DispatchVisitor
{
    protected:

        InstVisitor* fVisitor1;
        InstVisitor* fVisitor2;
        InstVisitor* fCurVisitor;

    public:

        CombinerVisitor(InstVisitor* v1, InstVisitor* v2)
            :fVisitor1(v1), fVisitor2(v2)
        {
            fCurVisitor = v1;
        }

        virtual ~CombinerVisitor()
        {
            delete fVisitor1;
            delete fVisitor2;
        }

};

// =======================
// Instruction generator
// =======================

struct InstBuilder
{

    static map<string, int> fIDCounters;

    static string getFreshID(const string& prefix)
    {
        if (fIDCounters.find(prefix) == fIDCounters.end()) {
            fIDCounters[prefix] = 0;
        }
        int n = fIDCounters[prefix];
        fIDCounters[prefix] = n+1;
        return subst("$0$1", prefix, T(n));
    }

    // User interface
    static AddMetaDeclareInst* genAddMetaDeclareInst(const string& zone, const string& key, const string& value)
        { return new AddMetaDeclareInst(zone, key, value); }

    static OpenboxInst* genOpenboxInst(int orient, const string& name)
        { return new OpenboxInst(orient, name); }

    static CloseboxInst* genCloseboxInst()
        { return new CloseboxInst(); }

    static AddButtonInst* genAddButtonInst(const string& label, const string& zone)
        { return new AddButtonInst(label, zone, AddButtonInst::kDefaultButton); }

    static AddButtonInst* genAddCheckbuttonInst(const string& label, const string& zone)
        { return new AddButtonInst(label, zone, AddButtonInst::kCheckbutton); }

    static AddSliderInst* genAddHorizontalSliderInst(const string& label, const string& zone, double init, double min, double max, double step)
        { return new AddSliderInst(label, zone, init, min, max, step, AddSliderInst::kHorizontal); }

    static AddSliderInst* genAddVerticalSliderInst(const string& label, const string& zone, double init, double min, double max, double step)
        { return new AddSliderInst(label, zone, init, min, max, step, AddSliderInst::kVertical); }

    static AddSliderInst* genAddNumEntryInst(const string& label, const string& zone, double init, double min, double max, double step)
        { return new AddSliderInst(label, zone, init, min, max, step, AddSliderInst::kNumEntry); }

    static AddBargraphInst* genAddHorizontalBargraphInst(const string& label, const string& zone, double min, double max)
        { return new AddBargraphInst(label, zone, min, max, AddBargraphInst::kHorizontal); }

    static AddBargraphInst* genAddVerticalBargraphInst(const string& label, const string& zone, double min, double max)
        { return new AddBargraphInst(label, zone, min, max, AddBargraphInst::kVertical); }

     static LabelInst* genLabelInst(const string& label)
        { return new LabelInst(label); }

    // Null instruction
    static NullInst* genNullInst() { return new NullInst(); }

    // Declarations
    static DeclareVarInst* genDeclareVarInst(Address* address, Typed* typed, ValueInst* value = NULL)
        { return new DeclareVarInst(address, typed, value); }

    static DeclareFunInst* genDeclareFunInst(const string& name, FunTyped* typed, BlockInst* code)
        {return new DeclareFunInst(name, typed, code);}
    static DeclareFunInst* genDeclareFunInst(const string& name, FunTyped* typed)
        {return new DeclareFunInst(name, typed);}

    static DeclareTypeInst* genDeclareTypeInst(Typed* type);

    // Memory
    static LoadVarInst* genLoadVarInst(Address* address, int size = 1) { return new LoadVarInst(address, size); }
    static LoadVarAddressInst* genLoadVarAddressInst(Address* address, int size = 1) { return new LoadVarAddressInst(address, size); }
    static StoreVarInst* genStoreVarInst(Address* address, ValueInst* value) {return new StoreVarInst(address, value); }

    // Numbers
    static FloatNumInst* genFloatNumInst(float num, int size = 1) { return new FloatNumInst(num, size);}
    static DoubleNumInst* genDoubleNumInst(double num, int size = 1) { return new DoubleNumInst(num, size); }
    static DoubleNumInst* genQuadNumInst(double num, int size = 1) { return new DoubleNumInst(num, size); }  // Use DoubleNumInst

    static ValueInst* genRealNumInst(Typed::VarType ctype, double num)
    {
        if (ctype == Typed::kFloat) {
            return new FloatNumInst(num);
        } else if (ctype == Typed::kFloatMacro) {
            return genCastNumInst(new DoubleNumInst(num), genBasicTyped(Typed::kFloatMacro));
        } else if (ctype == Typed::kDouble) {
            return new DoubleNumInst(num);
        } else if (ctype == Typed::kQuad) {
            return new DoubleNumInst(num);  // Use DoubleNumInst
        } else {
            assert(false);
        }
    }

    static IntNumInst* genIntNumInst(int num, int size = 1) { return new IntNumInst(num); }
    static BoolNumInst* genBoolNumInst(bool num, int size = 1) { return new BoolNumInst(num); }

    // Numerical computation
    static BinopInst* genBinopInst(int opcode, ValueInst* inst1, ValueInst* inst2, int size = 1) { return new BinopInst(opcode, inst1, inst2, size); }

    static ValueInst* genCastNumInst(ValueInst* inst, Typed* typed_ext, int size = 1)
    {
        IntNumInst* int_num = dynamic_cast<IntNumInst*>(inst);
        FloatNumInst* float_num = dynamic_cast<FloatNumInst*>(inst);
        DoubleNumInst* double_num = dynamic_cast<DoubleNumInst*>(inst);

        BasicTyped* typed = dynamic_cast<BasicTyped*>(typed_ext);

        if (!typed) {
            // Default case
            return new CastNumInst(inst, typed_ext, size);
        } else if (typed->getType() == Typed::kFloat) {
            if (int_num) {
                // Simple float cast of integer
                return genFloatNumInst(float(int_num->fNum));
            } else if (float_num) {
                // No cast needed
                return inst;
            } else if (double_num) {
                return genFloatNumInst(float(double_num->fNum));
            } else {
                // Default case
                return new CastNumInst(inst, typed, size);
            }
        } else if (typed->getType() == Typed::kDouble || typed->getType() == Typed::kQuad) {
            if (int_num) {
                // Simple double cast of integer
                return genDoubleNumInst(double(int_num->fNum));
            } else if (float_num) {
                return genDoubleNumInst(double(float_num->fNum));
            } else if (double_num) {
                // No cast needed
                return inst;
            } else {
                // Default case
                return new CastNumInst(inst, typed, size);
            }
        } else if (typed->getType() == Typed::kInt) {
             if (int_num) {
                // No cast needed
                return inst;
            } else if (float_num) {
                // Simple int cast of float
                return genIntNumInst(int(float_num->fNum));
            } else if (double_num) {
                // Simple int cast of double
                return genIntNumInst(int(double_num->fNum));
            } else {
                // Default case
                return new CastNumInst(inst, typed, size);
            }
        } else {
            // Default case
            return new CastNumInst(inst, typed, size);
        }
    }

    // Control flow
    static RetInst* genRetInst(ValueInst* result = NULL) { return new RetInst(result); }
    static DropInst* genDropInst(ValueInst* result) { return new DropInst(result); }

    // Conditionnal
    static Select2Inst* genSelect2Inst(ValueInst* cond_inst, ValueInst* then_inst, ValueInst* else_inst) { return new Select2Inst(cond_inst, then_inst, else_inst); }
    static IfInst* genIfInst(ValueInst* cond_inst, BlockInst* then_inst, BlockInst* else_inst) { return new IfInst(cond_inst, then_inst, else_inst); }
    static IfInst* genIfInst(ValueInst* cond_inst, BlockInst* then_inst) { return new IfInst(cond_inst, then_inst); }
    static SwitchInst* genSwitchInst(ValueInst* cond) { return new SwitchInst(cond); }

    // Function management
    static FunCallInst* genFunCallInst(const string& name, const list<ValueInst*>& args)
        { return new FunCallInst(name, args, false); }
    static FunCallInst* genFunCallInst(const string& name, const list<ValueInst*>& args, bool method, int size = 1)
        { return new FunCallInst(name, args, method, size); }

    // Loop
    static ForLoopInst* genForLoopInst(StatementInst* init, ValueInst* end, StatementInst* increment, BlockInst* code)
        { return new ForLoopInst(init, end, increment, code); }
    static ForLoopInst* genForLoopInst(StatementInst* init, ValueInst* end, StatementInst* increment)
        { return new ForLoopInst(init, end, increment); }

    static WhileLoopInst* genWhileLoopInst(ValueInst* cond, BlockInst* code)
        { return new WhileLoopInst(cond, code); }

    static BlockInst* genBlockInst(const list<StatementInst*>& code)
        { return new BlockInst(code); }
    static BlockInst* genBlockInst()
        { return new BlockInst(); }

    // Types
    static BasicTyped* genBasicTyped(Typed::VarType type)
    {
        BasicTyped* result;
        if (BasicTyped::gTypeTable.find(type) != BasicTyped::gTypeTable.end()) {
            result = BasicTyped::gTypeTable[type]; // Already allocated
        } else {
            result = new BasicTyped(type);
            BasicTyped::gTypeTable[type] = result;
        }
        return result;
    }

    static NamedTyped* genNamedTyped(const string& name, Typed* type) { return new NamedTyped(name, type); }
    static NamedTyped* genNamedTyped(const string& name, Typed::VarType  type) { return new NamedTyped(name, new BasicTyped(type)); }

    static FunTyped* genFunTyped(const list<NamedTyped*>& args, BasicTyped* result, FunTyped::FunAttribute attribute = FunTyped::kDefault) { return new FunTyped(args, result, attribute); }
    static ArrayTyped* genArrayTyped(Typed* type, int size);

    // Addresses
    static NamedAddress* genNamedAddress(const string& name, Address::AccessType access)
#ifdef __GNUC__
//         __attribute__ ((deprecated))
#endif
        { return new NamedAddress(name, access, NULL); }
    static NamedAddress* genNamedAddress(const string& name, Address::AccessType access, Typed * typed) { return new NamedAddress(name, access, typed); }

    static IndexedAddress* genIndexedAddress(Address* address, ValueInst* index) { return new IndexedAddress(address, index); }
    static CastAddress* genCastAddress(Address * address, Typed * typed) { return new CastAddress(address, typed); }
    static NamedAddress* genVectorAddress(string name, Typed * typed, int size, Address::AccessType access)
    {
        return new NamedAddress(name, access, genArrayTyped(typed, size));
    }

    // Helper build methods

    static DeclareVarInst* genDecVar(string vname, Address::AccessType var_access, Typed* type, ValueInst* exp = NULL)
    {
        return genDeclareVarInst(genNamedAddress(vname, var_access), type, exp);
    }

    static LoadVarInst* genLoadVar(string vname, Address::AccessType var_access)
    {
        return genLoadVarInst(genNamedAddress(vname, var_access));
    }

    static LoadVarInst* genLoadArrayVar(string vname, Address::AccessType var_access, ValueInst* index)
    {
        return genLoadVarInst(genIndexedAddress(genNamedAddress(vname, var_access), index));
    }

    static StoreVarInst* genStoreVar(string vname, Address::AccessType var_access, ValueInst* exp)
    {
        return genStoreVarInst(genNamedAddress(vname, var_access), exp);
    }

    static StoreVarInst* genStoreArrayVar(string vname, Address::AccessType var_access, ValueInst* index, ValueInst* exp)
    {
        return genStoreVarInst(genIndexedAddress(genNamedAddress(vname, var_access), index), exp);
    }

    // Struct variable
    static DeclareVarInst* genDecStructVar(string vname, Typed* type, ValueInst* exp = NULL)
    {
        return genDeclareVarInst(genNamedAddress(vname, Address::kStruct, type), type, exp);
    }

    static LoadVarInst* genLoadStructVar(string vname)
    {
        return genLoadVarInst(genNamedAddress(vname, Address::kStruct));
    }

    template <typename Iterator>
    static LoadVarInst* genLoadArrayStructVar(string vname, Iterator indexBegin, Iterator indexEnd)
    {
        typedef reverse_iterator<Iterator> Rit;
        Rit rbegin (indexEnd);
        Rit rend (indexBegin);

        Address * address = genNamedAddress(vname, Address::kStruct);
        for (Rit it = rbegin; it != rend; ++it)
            address = genIndexedAddress(address, *it);

        return genLoadVarInst(address);
    }

    static LoadVarInst* genLoadArrayStructVar(string vname, ValueInst* index)
    {
        vector<ValueInst*> indices;
        indices.push_back(index);
        return genLoadArrayStructVar(vname, indices.begin(), indices.end());
    }

    static LoadVarInst* genLoadArrayStructVar(string vname)
    {
        return genLoadVarInst(genNamedAddress(vname, Address::kStruct));
    }

    static LoadVarAddressInst* genLoadArrayStructAddressVar(string vname, ValueInst* index)
    {
        return genLoadVarAddressInst(genIndexedAddress(genNamedAddress(vname, Address::kStruct), index));
    }

    static StoreVarInst* genStoreStructVar(string vname, ValueInst* exp)
    {
        return genStoreVarInst(genNamedAddress(vname, Address::kStruct), exp);
    }

    template <typename Iterator>
    static StoreVarInst* genStoreArrayStructVar(string vname, ValueInst* exp, Iterator indexBegin, Iterator indexEnd)
    {
        typedef reverse_iterator<Iterator> Rit;
        Rit rbegin (indexEnd);
        Rit rend (indexBegin);

        Address * address = genNamedAddress(vname, Address::kStruct);
        for (Rit it = rbegin; it != rend; ++it)
            address = genIndexedAddress(address, *it);

        return genStoreVarInst(address, exp);
    }

    static StoreVarInst* genStoreArrayStructVar(string vname, ValueInst* index, ValueInst* exp)
    {
        vector<ValueInst*> indices;
        indices.push_back(index);
        return genStoreArrayStructVar(vname, exp, indices.begin(), indices.end());
    }

    static StoreVarInst* genStoreArrayStructVar(string vname, ValueInst* exp)
    {
        return genStoreVarInst(genNamedAddress(vname, Address::kStruct), exp);
    }

    // static struct variable
    static DeclareVarInst* genDecStaticStructVar(string vname, Typed* type, ValueInst* exp = NULL)
    {
        return genDeclareVarInst(genNamedAddress(vname, Address::kStaticStruct), type, exp);
    }

    static LoadVarInst* genLoadStaticStructVar(string vname)
    {
        return genLoadVarInst(genNamedAddress(vname, Address::kStaticStruct));
    }

    static StoreVarInst* genStoreStaticStructVar(string vname, ValueInst* exp)
    {
        return genStoreVarInst(genNamedAddress(vname, Address::kStaticStruct), exp);
    }

    // Stack variable
    static DeclareVarInst* genDecStackVar(string vname, Typed* type, ValueInst* exp = NULL)
    {
        return genDeclareVarInst(genNamedAddress(vname, Address::kStack), type, exp);
    }

    static LoadVarInst* genLoadStackVar(string vname)
    {
        return genLoadVarInst(genNamedAddress(vname, Address::kStack));
    }

    static LoadVarInst* genLoadArrayStackVar(string vname, ValueInst* index)
    {
        return genLoadVarInst(genIndexedAddress(genNamedAddress(vname, Address::kStack), index));
    }

    static LoadVarAddressInst* genLoadArrayStackAddressVar(string vname, ValueInst* index)
    {
        return genLoadVarAddressInst(genIndexedAddress(genNamedAddress(vname, Address::kStack), index));
    }

    static StoreVarInst* genStoreStackVar(string vname, ValueInst* exp)
    {
        return genStoreVarInst(genNamedAddress(vname, Address::kStack), exp);
    }

    static StoreVarInst* genStoreArrayStackVar(string vname, ValueInst* index, ValueInst* exp)
    {
        return genStoreVarInst(genIndexedAddress(genNamedAddress(vname, Address::kStack), index), exp);
    }

    // Loop variable
    static DeclareVarInst* genDecLoopVar(string vname, Typed* type, ValueInst* exp = NULL)
    {
        return genDeclareVarInst(genNamedAddress(vname, Address::kLoop), type, exp);
    }

    static LoadVarInst* genLoadLoopVar(string vname)
    {
        return genLoadVarInst(genNamedAddress(vname, Address::kLoop));
    }

    static StoreVarInst* genStoreLoopVar(string vname, ValueInst* exp)
    {
        return genStoreVarInst(genNamedAddress(vname, Address::kLoop), exp);
    }

    // FunArgs variable
    static LoadVarInst* genLoadFunArgsVar(string vname)
    {
        return genLoadVarInst(genNamedAddress(vname, Address::kFunArgs));
    }

    static LoadVarInst* genLoadArrayFunArgsVar(string vname, ValueInst* index)
    {
        return genLoadVarInst(genIndexedAddress(genNamedAddress(vname, Address::kFunArgs), index));
    }

    static StoreVarInst* genStoreArrayFunArgsVar(string vname, ValueInst* index, ValueInst* exp)
    {
        return genStoreVarInst(genIndexedAddress(genNamedAddress(vname, Address::kFunArgs), index), exp);
    }

    // Global variable
    static DeclareVarInst* genDecGlobalVar(string vname, Typed* type, ValueInst* exp = NULL)
    {
        return genDeclareVarInst(genNamedAddress(vname, Address::kGlobal), type, exp);
    }

    static LoadVarInst* genLoadGlobalVar(string vname)
    {
        return genLoadVarInst(genNamedAddress(vname, Address::kGlobal));
    }

    static StoreVarInst* genStoreGlobalVar(string vname, ValueInst* exp)
    {
        return genStoreVarInst(genNamedAddress(vname, Address::kGlobal), exp);
    }

    static BinopInst* genAdd(ValueInst* a1, ValueInst* a2)
    {
        return genBinopInst(kAdd, a1, a2);
    }

    static BinopInst* genAdd(ValueInst* a1, int value)
    {
        return genBinopInst(kAdd, a1, genIntNumInst(value));
    }

    static BinopInst* genSub(ValueInst* a1, ValueInst* a2)
    {
        return genBinopInst(kSub, a1, a2);
    }

    static ValueInst* genMul(ValueInst* a1, ValueInst* a2)
    {
        IntNumInst * intArg = dynamic_cast<IntNumInst*>(a1);
        if (intArg && intArg->fNum == 1)
            return a2;

        intArg = dynamic_cast<IntNumInst*>(a2);
        if (intArg && intArg->fNum == 1)
            return a1;

        return genBinopInst(kMul, a1, a2);
    }

    static ValueInst * genDiv(ValueInst* a1, ValueInst* a2)
    {
        IntNumInst * intArg = dynamic_cast<IntNumInst*>(a2);
        if (intArg && intArg->fNum == 1)
            return a1;
        return genBinopInst(kDiv, a1, a2);
    }

    static BinopInst* genAnd(ValueInst* a1, ValueInst* a2)
    {
        return genBinopInst(kAND, a1, a2);
    }

    static BinopInst* genLessThan(ValueInst* a1, ValueInst* a2)
    {
        return genBinopInst(kLT, a1, a2);
    }

    static BinopInst* genGreaterThan(ValueInst* a1, ValueInst* a2)
    {
        return genBinopInst(kGT, a1, a2);
    }

    static DeclareFunInst* genVoidFunction(const string& name);
    static DeclareFunInst* genFunction0(const string& name, Typed::VarType res);
    static DeclareFunInst* genFunction1(const string& name, Typed::VarType res,
                    const string& arg1, Typed::VarType arg1_ty);
    static DeclareFunInst* genFunction2(const string& name, Typed::VarType res,
                    const string& arg1, Typed::VarType arg1_ty,
                    const string& arg2, Typed::VarType arg2_ty);
    static DeclareFunInst* genFunction3(const string& name, Typed::VarType res,
                    const string& arg1, Typed::VarType arg1_ty,
                    const string& arg2, Typed::VarType arg2_ty,
                    const string& arg3, Typed::VarType arg3_ty);
    static DeclareFunInst* genFunction4(const string& name, Typed::VarType res,
                    const string& arg1, Typed::VarType arg1_ty,
                    const string& arg2, Typed::VarType arg2_ty,
                    const string& arg3, Typed::VarType arg3_ty,
                    const string& arg4, Typed::VarType arg4_ty);

    // Convert a signal type in a Fir type by using an intermediate Tree based implementation to assure type creation unicity.
    static DeclareTypeInst* genType(AudioType* type);
};

struct FIRIndex
{
    explicit FIRIndex(ValueInst * inst):
        data(inst)
    {}

    FIRIndex(FIRIndex const & rhs):
        data(rhs.data)
    {}

    operator ValueInst* (void) const
    {
        return data;
    }

    friend FIRIndex operator+ (FIRIndex const & lhs, ValueInst * rhs)
    {
        return FIRIndex(InstBuilder::genAdd(lhs.data, rhs));
    }

    friend FIRIndex operator+ (FIRIndex const & lhs, FIRIndex const & rhs)
    {
        return operator+(lhs, rhs.data);
    }

    friend FIRIndex operator+ (FIRIndex const & lhs, int rhs)
    {
        return operator+(lhs, InstBuilder::genIntNumInst(rhs));
    }

    friend FIRIndex operator- (FIRIndex const & lhs, ValueInst * rhs)
    {
        return FIRIndex(InstBuilder::genSub(lhs.data, rhs));
    }

    friend FIRIndex operator- (FIRIndex const & lhs, FIRIndex const & rhs)
    {
        return operator+(lhs, rhs.data);
    }

    friend FIRIndex operator- (FIRIndex const & lhs, int rhs)
    {
        return operator+(lhs, InstBuilder::genIntNumInst(rhs));
    }

    friend FIRIndex operator* (FIRIndex const & lhs, ValueInst * rhs)
    {
        return FIRIndex(InstBuilder::genMul(lhs.data, rhs));
    }

    friend FIRIndex operator* (FIRIndex const & lhs, FIRIndex const & rhs)
    {
        return operator*(lhs, rhs.data);
    }

    friend FIRIndex operator* (FIRIndex const & lhs, int rhs)
    {
        return operator*(lhs, InstBuilder::genIntNumInst(rhs));
    }


    friend FIRIndex operator/ (FIRIndex const & lhs, ValueInst * rhs)
    {
        return FIRIndex(InstBuilder::genDiv(lhs.data, rhs));
    }

    friend FIRIndex operator/ (FIRIndex const & lhs, FIRIndex const & rhs)
    {
        return operator/(lhs, rhs.data);
    }

    friend FIRIndex operator/ (FIRIndex const & lhs, int rhs)
    {
        return operator/(lhs, InstBuilder::genIntNumInst(rhs));
    }

private:
    ValueInst * data;
};

#endif

/*

Name := sequence of char

Size := digits

Opcode := + | - | * | / |...etc...

Access := kGlobal | kStruct | kStaticStruct | kFunArgs | kStack | kLoop

Type := kFloat | kInt | kDouble | kVoid | Type* --> Type | Vector (Type, Size) | Array (Type, Size)  si size = 0, then equivalent to a pointer on the type

Address := Access name | Address index

Statement   := DeclareVar (Address, Type, Value)
            | ForLoop (Statement, Value, Statement, Block)
            | WhileLoop (Value, Block)
            | StoreVar (Address, Value)
            | DeclareFun (Name, Type, Statement*)
            | Drop Value
            | Return Value
            | BlockInst (Statement*)
            | If (Value1, BlockInst, BlockInst)
            | Switch (Value1, <int, BlockInst> *)

Value       := LoadVar (Address)
            | Float | Int | Double

Code rewritting :

For WSS:

1) change access of variable of type kStack in kStruct


Loop to function rewritting (faster compilation ?):

2 methods

I)

1) change kStack variables in kStruct
2) transform Loop (Name, Value, Statement*) in DeclareFun (Name, Type, Statement*) : function type kVoid --> kVoid
3) in Compute, replace each loop with a call to the created function


II)

1) in each loop, transform in put vector access from kStack in kFunArgs
2) transform Loop (Name, Value, Statement*) in DeclareFun (Name, Type, Statement*) : all input variables become function parameters
3) in Compute, replace each loop with a call to the created function giving it the good parameters


Scalarisation (some ideas, possibly not correct or not complete...):

1) transform all vectors *without delay* on the stack (used in loops) in scalar
2) in each loop, transform input vector accessin scalar access (Load/Store)
3) regroup all  "postcode" of all loop at the end
4) rename loop variable into the Compute variable name
5) extract code and put it in Compute, suppress Loop statements

Vision des boucles (count, liste de vecteurs d'entrée, liste de vecteurs de sorties) différente du prototype externe compute(count, float**, float**) ou veut homogenéiser

D'ou:

DAG de boucles au format (count, liste de vecteurs d'entrée, liste de vecteurs de sorties)
compute(count, float**, float**)

1) générer le header qui prépare les tableaux d'entrée et de sortie séparés
2) compiler les boucles

Comment différencier les vecteurs sans retard (qu'on peut transformer en scalaire) des vecteurs avec retard? Avec un nommage spécifique?

TODO : gestion des indices de boucles:


TODO : gestion des indices de boucles:

 - dans IndexedAddress, mettre un ValueInst à la place de fIndex, mettre à jour les visiteurs

 - dans InstructionsCompiler, generer des accès avec "LoadVar (loop-index)

 - Dans ForLoopInst, fName devient un "DeclareVarInst" (permet de nommer et d'initialiser l'indice), ajout d'une expression test, ajout de ValueInst fNext, calcul qui utilise fName.

 - nouveau type d'accès  kLoop pour les variables de loop

 - lors des transformations sur les loops, Loop2FunctionBuider, SeqLoopBuilderVisitor, "désactiver" les statements qui manipulent les indices de la boucle ??
 (pas besoin, ils n'apparaissent pas dans le corps de le boucle, par contre l'indice de la boucle est utilisée dans le corps de la boucle, il faut le faire correspondre
 au nouvel indice de boucle, renommage nécessaire ?)

 - utiliser le *même* nom d'index dans ForLoopInst est dans le code interne de la loop

*/



