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

/**********************************************************************
			- code_gen.h : generic code generator (projet FAUST) -


		Historique :
		-----------

***********************************************************************/
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>

#include "instructions.hh"
#include "instructions_compiler.hh"
#include "sigprint.hh"
#include "floats.hh"

#include "ppsig.hh"
#include "Text.hh"
#include "sigtyperules.hh"
#include "timing.hh"
#include "recursivness.hh"
#include "privatise.hh"
#include "simplify.hh"
#include "xtended.hh"
#include "prim2.hh"

#include "ensure.hh"
#include "sigrateinference.hh"

using namespace std;

// globals

extern int gMaxCopyDelay;
extern bool gVectorSwitch;
extern int gVecSize;
extern bool gOpenCLSwitch;
extern bool gCUDASwitch;


CodeContainer* InstructionsCompiler::signal2Container(const string& name, Tree sig)
{
	Type t = getSigType(sig);

	CodeContainer* container = fContainer->createInternalContainer(name, t->nature());
    InstructionsCompiler C(container);
    C.compileSingleSignal(sig);
    return container;
}

/*****************************************************************************
						    compileMultiSignal
*****************************************************************************/

void InstructionsCompiler::compileMultiSignal(Tree L)
{
	L = prepare(L);		// Optimize, share and annotate expression

    Typed* type;
    /*
    if (gVectorSwitch) {
        type = InstBuilder::genVectorTyped(InstBuilder::genBasicTyped(Typed::kFloatMacro), gVecSize);
    } else {
        type = InstBuilder::genBasicTyped(Typed::kFloatMacro);
    }
    */
    type = InstBuilder::genBasicTyped(Typed::kFloatMacro);

    if (!gOpenCLSwitch && !gCUDASwitch) { // HACK

        // "input" and "inputs" used as a name convention
        for (int index = 0; index < fContainer->inputs(); index++) {
            string name = subst("input$0", T(index));
            pushComputeBlockMethod(InstBuilder::genDecStackVar(name, InstBuilder::genArrayTyped(type, 0),
                InstBuilder::genLoadArrayFunArgsVar("inputs", InstBuilder::genIntNumInst(index))));
        }

        // "output" and "outputs" used as a name convention
        for (int index = 0; index < fContainer->outputs(); index++) {
            string name = subst("output$0", T(index));
            pushComputeBlockMethod(InstBuilder::genDecStackVar(name, InstBuilder::genArrayTyped(type, 0),
                InstBuilder::genLoadArrayFunArgsVar("outputs", InstBuilder::genIntNumInst(index))));
        }
    }

	for (int index = 0; isList(L); L = tl(L), index++) {
		Tree sig = hd(L);
        string name = subst("output$0", T(index));

        // Cast to external float
        ValueInst* res = InstBuilder::genCastNumInst(CS(sig), InstBuilder::genBasicTyped(Typed::kFloatMacro));
        pushComputeDSPMethod(InstBuilder::genStoreArrayStackVar(name, fContainer->getCurLoop()->getLoopIndex(), res));

        int rate = getSigRate(sig);
        fContainer->setOutputRate(index, rate);
    }

	generateUserInterfaceTree(prepareUserInterfaceTree(fUIRoot));
	generateMacroInterfaceTree("", prepareUserInterfaceTree(fUIRoot));
	if (fDescription) {
		fDescription->ui(prepareUserInterfaceTree(fUIRoot));
	}

    fContainer->processFIR();
}

/*****************************************************************************
						    compileSingleSignal
*****************************************************************************/

void InstructionsCompiler::compileSingleSignal(Tree sig)
{
  	sig = prepare2(sig);		// Optimize and annotate expression
    string name = "output";

    pushComputeDSPMethod(InstBuilder::genStoreArrayFunArgsVar(name, fContainer->getCurLoop()->getLoopIndex(), CS(sig)));

	generateUserInterfaceTree(prepareUserInterfaceTree(fUIRoot));
	generateMacroInterfaceTree("", prepareUserInterfaceTree(fUIRoot));
	if (fDescription) {
		fDescription->ui(prepareUserInterfaceTree(fUIRoot));
	}
}

ValueInst* InstructionsCompiler::generateCode(Tree sig)
{
#if 0
	fprintf(stderr, "CALL generateCode(");
    printSignal(sig, stderr);
	fprintf(stderr, ")\n");
#endif

    ValueInst* code;
    if (getCompiledExpression(sig, code)) {
        return code;
    }

	int 	i;
	double	r;
	Tree 	c, sel, x, y, z, label, id, ff, largs, type, name, file;

	//printf("compilation of %p : ", sig); print(sig); printf("\n");

		 if ( getUserData(sig) ) 					{ return generateXtended(sig); }
	else if ( isSigInt(sig, &i) ) 					{ return generateIntNumber(sig, i); }
	else if ( isSigReal(sig, &r) ) 					{ return generateRealNumber(sig, r); }
	else if ( isSigInput(sig, &i) ) 				{ return generateInput(sig, i); }

	else if ( isSigFixDelay(sig, x, y) ) 			{ return generateFixDelay(sig, x, y); }
	else if ( isSigPrefix(sig, x, y) ) 				{ return generatePrefix(sig, x, y); }
	else if ( isSigIota(sig, x) ) 					{ return generateIota(sig, x); }

	else if ( isSigBinOp(sig, &i, x, y) )			{ return generateBinOp(sig, i, x, y); }
	else if ( isSigFFun(sig, ff, largs) )			{ return generateFFun(sig, ff, largs); }
    else if ( isSigFConst(sig, type, name, file) )  { return generateFConst(sig, type, tree2str(file), tree2str(name)); }
    else if ( isSigFVar(sig, type, name, file) )    { return generateFVar(sig, type, tree2str(file), tree2str(name)); }

	else if ( isSigTable(sig, id, x, y) ) 			{ return generateTable(sig, x, y); }
	else if ( isSigWRTbl(sig, id, x, y, z) )		{ return generateWRTbl(sig, x, y, z); }
	else if ( isSigRDTbl(sig, x, y) ) 				{ return generateRDTbl(sig, x, y); }

	else if ( isSigSelect2(sig, sel, x, y) ) 		{ return generateSelect2(sig, sel, x, y); }
	else if ( isSigSelect3(sig, sel, x, y, z) ) 	{ return generateSelect3(sig, sel, x, y, z); }

	else if ( isSigGen(sig, x) ) 					{ return generateSigGen(sig, x); }

    else if ( isProj(sig, &i, x) )                  { return generateRecProj(sig, x, i); }

	else if ( isSigIntCast(sig, x) ) 				{ return generateIntCast(sig, x); }
	else if ( isSigFloatCast(sig, x) ) 				{ return generateFloatCast(sig, x); }

	else if ( isSigButton(sig, label) ) 			{ return generateButton(sig, label); }
	else if ( isSigCheckbox(sig, label) ) 			{ return generateCheckbox(sig, label); }
	else if ( isSigVSlider(sig, label,c,x,y,z) )	{ return generateVSlider(sig, label, c, x, y, z); }
	else if ( isSigHSlider(sig, label,c,x,y,z) )	{ return generateHSlider(sig, label, c, x, y, z); }
	else if ( isSigNumEntry(sig, label,c,x,y,z) )	{ return generateNumEntry(sig, label, c, x, y, z); }


	else if ( isSigVBargraph(sig, label,x,y,z) )	{ return generateVBargraph(sig, label, x, y, CS(z)); }
	else if ( isSigHBargraph(sig, label,x,y,z) )	{ return generateHBargraph(sig, label, x, y, CS(z)); }
	else if ( isSigAttach(sig, x, y) )				{ CS(y); return generateCacheCode(sig, CS(x)); }
    else if (isSigVectorize(sig, x, y)) {
        printf("vectorize not implemented\n");
        exit (0);
    }
    else if (isSigSerialize(sig, x)) {
        printf("serialize not implemented\n");
        exit (0);
    }
    else if (isSigConcat(sig, x, y)) {
        printf("concatenation not implemented\n");
        exit (0);
    }
    else if (isSigVectorAt(sig, x, y)) {
        printf("vector at not implemented\n");
        exit (0);
    }
	else {
		printf("Error in compiling signal, unrecognized signal : ");
		print(sig);
		printf("\n");
		exit(1);
	}
	return InstBuilder::genNullInst();
}

ValueInst* InstructionsCompiler::generateCacheCode(Tree sig, ValueInst* exp)
{
    ValueInst* code;

    // Check reentrance
    if (getCompiledExpression(sig, code)) {
        return code;
    }

    string vname;
    Typed::VarType ctype;
    int sharing = getSharingCount(sig);
	Occurences* o = fOccMarkup.retrieve(sig);

    // Check for expression occuring in delays
	if (o->getMaxDelay() > 0) {

        getTypedNames(getSigType(sig), "Vec", ctype, vname);
        if (sharing > 1) {
            return generateDelayVec(sig, generateVariableStore(sig, exp), ctype, vname, o->getMaxDelay());
        } else {
		    return generateDelayVec(sig, exp, ctype, vname, o->getMaxDelay());
        }

	} else if (sharing == 1) {

        return exp;

	} else if (sharing > 1) {

        return generateVariableStore(sig, exp);

	} else {
        cerr << "Error in sharing count (" << sharing << ") for " << *sig << endl;
		exit(1);
	}
}

ValueInst* InstructionsCompiler::CS(Tree sig)
{
    ValueInst* code;

    if (!getCompiledExpression(sig, code)) {
        code = generateCode(sig);
        setCompiledExpression(sig, code);
    }

    return code;
}

ValueInst* InstructionsCompiler::generateVariableStore(Tree sig, ValueInst* exp)
{
    string vname;
    Typed::VarType ctype;
    ::Type t = getSigType(sig);

    Typed* type;

    switch (t->variability()) {

        case kKonst:
            getTypedNames(t, "Const", ctype, vname);
            /*
            if (gVectorSwitch) {
                type = InstBuilder::genVectorTyped(InstBuilder::genBasicTyped(ctype), gVecSize);
            } else {
                type = InstBuilder::genBasicTyped(ctype);
            }
            */
            type = InstBuilder::genBasicTyped(ctype);
            pushDeclare(InstBuilder::genDecStructVar(vname, type));
            pushInitMethod(InstBuilder::genStoreStructVar(vname, exp));
            return InstBuilder::genLoadStructVar(vname);

        case kBlock:
            getTypedNames(t, "Slow", ctype, vname);
             /*
            if (gVectorSwitch) {
                type = InstBuilder::genVectorTyped(InstBuilder::genBasicTyped(ctype), gVecSize);
            } else {
                type = InstBuilder::genBasicTyped(ctype);
            }
            */
            type = InstBuilder::genBasicTyped(ctype);
            pushComputeBlockMethod(InstBuilder::genDecStackVar(vname, type, exp));
            return InstBuilder::genLoadStackVar(vname);

        case kSamp:
            getTypedNames(t, "Temp", ctype, vname);
             /*
            if (gVectorSwitch) {
                type = InstBuilder::genVectorTyped(InstBuilder::genBasicTyped(ctype), gVecSize);
            } else {
                type = InstBuilder::genBasicTyped(ctype);
            }
            */
            type = InstBuilder::genBasicTyped(ctype);
            pushComputeDSPMethod(InstBuilder::genDecStackVar(vname, type, exp));
            return InstBuilder::genLoadStackVar(vname);

        default:
            return InstBuilder::genNullInst();
    }
}

ValueInst* InstructionsCompiler::generateXtended(Tree sig)
{
    xtended* p = (xtended*)getUserData(sig);
	list<ValueInst*> args;
    vector< ::Type> arg_types;
    ::Type result_type = getSigType(sig);

	for (int i = 0; i < sig->arity(); i++) {
		args.push_back(CS(sig->branch(i)));
		arg_types.push_back(getSigType(sig->branch(i)));
	}

	if (p->needCache()) {
		return generateCacheCode(sig, p->generateCode(fContainer, args, result_type, arg_types));
	} else {
		return p->generateCode(fContainer, args, result_type, arg_types);
	}
}

ValueInst* InstructionsCompiler::generateFixDelay(Tree sig, Tree exp, Tree delay)
{
    int mxd;
	string vname;

    CS(exp); // Ensure exp is compiled to have a vector name, result of CS is not needed, only side effect is important
    mxd = fOccMarkup.retrieve(exp)->getMaxDelay();

	if (!getVectorNameProperty(exp, vname)) {
        cerr << "No vector name for : " << ppsig(exp) << endl;
        assert(0);
    }

    if (mxd == 0) {
        return InstBuilder::genLoadStackVar(vname);
	} else if (mxd < gMaxCopyDelay) {
		return InstBuilder::genLoadArrayStructVar(vname, CS(delay));
	} else {
		// Long delay : we use a ring buffer of size 2^x
		int N = pow2limit(mxd + 1);

        ValueInst* value2 = InstBuilder::genSub(InstBuilder::genLoadStructVar("IOTA"), CS(delay));
        ValueInst* value3 = InstBuilder::genAnd(value2, InstBuilder::genIntNumInst(N - 1));
        return InstBuilder::genLoadArrayStructVar(vname, value3);
    }
}

ValueInst* InstructionsCompiler::generatePrefix(Tree sig, Tree x, Tree e)
{
    ::Type te = getSigType(sig);

	string vperm = getFreshID("M");
	string vtemp = getFreshID("T");

    Typed::VarType type = ctType(te);

    // Table declaration
    pushDeclare(InstBuilder::genDecStructVar(vperm, InstBuilder::genBasicTyped(type)));

    // Init
    pushInitMethod(InstBuilder::genStoreStructVar(vperm, CS(x)));

    // Exec
    pushComputeDSPMethod(InstBuilder::genDecStructVar(vtemp, InstBuilder::genBasicTyped(type), InstBuilder::genLoadStructVar(vperm)));
    pushComputeDSPMethod(InstBuilder::genStoreStructVar(vperm, CS(e)));

    return InstBuilder::genLoadStackVar(vtemp);
}

ValueInst* InstructionsCompiler::generateIota(Tree sig, Tree arg) { return InstBuilder::genNullInst(); }  // Result not used

ValueInst* InstructionsCompiler::generateBinOp(Tree sig, int opcode, Tree arg1, Tree arg2)
{
    int t1 = getSigType(arg1)->nature();
    int t2 = getSigType(arg2)->nature();
    int t3 = getSigType(sig)->nature();

    ValueInst* res = NULL;
    ValueInst* val1 = CS(arg1);
    ValueInst* val2 = CS(arg2);

    // Arguments and expected result type analysis, add the required "cast" when needed
    if (t1 == kReal) {
        if (t2 == kReal) {
            res = InstBuilder::genBinopInst(opcode, val1, val2);
            if (t3 == kReal) {
                // Nothing
            } else {
                res = InstBuilder::genCastNumInst(res, InstBuilder::genBasicTyped(Typed::kInt));
            }
        } else {
            res = InstBuilder::genBinopInst(opcode, val1, InstBuilder::genCastNumInst(val2, InstBuilder::genBasicTyped(itfloat())));
            if (t3 == kReal) {
                // Nothing
            } else {
                res = InstBuilder::genCastNumInst(res, InstBuilder::genBasicTyped(Typed::kInt));
            }
        }
    } else if (t2 == kReal) {
        res = InstBuilder::genBinopInst(opcode, InstBuilder::genCastNumInst(val1, InstBuilder::genBasicTyped(itfloat())), val2);
        if (t3 == kReal) {
            // Nothing
        } else {
            res = InstBuilder::genCastNumInst(res, InstBuilder::genBasicTyped(Typed::kInt));
        }
    } else {
        res = InstBuilder::genBinopInst(opcode, val1, val2);
        if (t3 == kReal) {
            res = InstBuilder::genCastNumInst(res, InstBuilder::genBasicTyped(itfloat()));
        } else {
            // Nothing
        }
    }

    assert(res);
    return generateCacheCode(sig, res);
}

// TODO
ValueInst* InstructionsCompiler::generateFFun(Tree sig, Tree ff, Tree largs)
{
    fContainer->addIncludeFile(ffincfile(ff));
	fContainer->addLibrary(fflibfile(ff));

    string funname = ffname(ff);
    list<ValueInst*> args_value;
    list<NamedTyped*> args_types;
    FunTyped* fun_type;

    for (int i = 0; i< ffarity(ff); i++) {
        stringstream num; num << i;
        Tree parameter = nth(largs, i);
        ::Type t1 = getSigType(parameter);
        args_types.push_back(InstBuilder::genNamedTyped("dummy" + num.str(), InstBuilder::genBasicTyped((t1->nature() == kInt) ? Typed::kInt : itfloat())));
        args_value.push_back(CS(parameter));
    }

    // Add function declaration
    fun_type = InstBuilder::genFunTyped(args_types, InstBuilder::genBasicTyped((ffrestype(ff) == kInt) ? Typed::kInt : itfloat()));
    pushExtGlobalDeclare(InstBuilder::genDeclareFunInst(funname, fun_type));

    return generateCacheCode(sig, InstBuilder::genFunCallInst(funname, args_value));
}

ValueInst* InstructionsCompiler::generateInput(Tree sig, int idx)
{
    int rate = getSigRate(sig);
    fContainer->setInputRate(idx, rate);

    string name = subst("input$0", T(idx));
    ValueInst* res = InstBuilder::genLoadArrayStackVar(name, fContainer->getCurLoop()->getLoopIndex());

    ValueInst* castedToFloat = InstBuilder::genCastNumInst(res, InstBuilder::genBasicTyped(itfloat()));
    return generateCacheCode(sig, castedToFloat);
}

ValueInst* InstructionsCompiler::generateTable(Tree sig, Tree tsize, Tree content)
{
    ValueInst* 	generator = CS(content);
    Typed::VarType ctype;
    Tree        g;
	string		vname;
	int 		size;

    // already compiled but check if we need to add declarations
    assert(isSigGen(content, g));
    pair<string, string> kvnames;
    if (!fInstanceInitProperty.get(g, kvnames)) {
        // not declared here, we add a declaration
        bool b = fStaticInitProperty.get(g, kvnames);
        assert(b);
        const list<ValueInst*> args;
        ValueInst* obj = InstBuilder::genFunCallInst("new" + kvnames.first, args);
        pushInitMethod(InstBuilder::genDecStackVar(kvnames.second, InstBuilder::genNamedTyped(kvnames.first, InstBuilder::genBasicTyped(Typed::kObj_ptr)), obj));

        // Delete object
        list<ValueInst*> args3;
        args3.push_back(generator);
        pushPostInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("delete" + kvnames.first, args3)));
    }

	if (!isSigInt(tsize, &size)) {
		cerr << "error in ScalarCompiler::generateTable() : "
			 << *tsize
			 << " is not an integer expression "
			 << endl;
        exit(1);
	}

	// definition du nom et du type de la table
	// A REVOIR !!!!!!!!!
	Type t = getSigType(content);//, tEnv);
	if (t->nature() == kInt) {
		vname = getFreshID("itbl");
		ctype = Typed::kInt;
	} else {
		vname = getFreshID("ftbl");
		ctype = itfloat();
	}

    // Table declaration
    pushDeclare(InstBuilder::genDecStructVar(vname, InstBuilder::genArrayTyped(InstBuilder::genBasicTyped(ctype), size)));

    string tablename;
    getTableNameProperty(content, tablename);

    // Init content generator
    list<ValueInst*> args1;
    args1.push_back(generator);
    args1.push_back(InstBuilder::genLoadFunArgsVar("samplingFreq"));
    pushInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("instanceInit" + tablename, args1, true)));

    // Fill the table
    list<ValueInst*> args2;
    args2.push_back(generator);
    args2.push_back(InstBuilder::genIntNumInst(size));
    args2.push_back(InstBuilder::genLoadStructVar(vname));
    pushInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("fill" + tablename, args2, true)));

    // Return table access
    return InstBuilder::genLoadStructVar(vname);
}

ValueInst* InstructionsCompiler::generateStaticTable(Tree sig, Tree tsize, Tree content)
{
    Tree        g;
	ValueInst* 	cexp;
    Typed::VarType ctype;
	string      vname;
	int         size;

	ensure(isSigGen(content, g));

	if (!getCompiledExpression(content, cexp)) {
		cexp = setCompiledExpression(content, generateStaticSigGen(content, g));
	} else {
        // already compiled but check if we need to add declarations
        pair<string, string> kvnames;
        if (!fStaticInitProperty.get(g, kvnames)) {
            // not declared here, we add a declaration
            bool b = fInstanceInitProperty.get(g, kvnames);
            assert(b);
            const list<ValueInst*> args;
            ValueInst* obj = InstBuilder::genFunCallInst("new" + kvnames.first, args);
            pushInitMethod(InstBuilder::genDecStackVar(kvnames.second, InstBuilder::genNamedTyped(kvnames.first, InstBuilder::genBasicTyped(Typed::kObj_ptr)), obj));

            // Delete object
            list<ValueInst*> args3;
            args3.push_back(cexp);
            pushPostInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("delete" + kvnames.first, args3)));
        }
    }

    if (!isSigInt(tsize, &size)) {
		//fprintf(stderr, "error in ScalarCompiler::generateTable()\n"); exit(1);
		cerr << "error in ScalarCompiler::generateTable() : "
			 << *tsize
			 << " is not an integer expression "
			 << endl;
        exit(1);
	}
	// definition du nom et du type de la table
	// A REVOIR !!!!!!!!!
	Type t = getSigType(content);//, tEnv);
	if (t->nature() == kInt) {
		vname = getFreshID("itbl");
		ctype = Typed::kInt;
	} else {
		vname = getFreshID("ftbl");
		ctype = itfloat();
	}

    string tablename;
    getTableNameProperty(content, tablename);

    vname += tablename;

    // Table declaration
    pushGlobalDeclare(InstBuilder::genDecStaticStructVar(vname, InstBuilder::genArrayTyped(InstBuilder::genBasicTyped(ctype), size)));

    // Init content generator
    list<ValueInst*> args1;
    args1.push_back(cexp);
    args1.push_back(InstBuilder::genLoadFunArgsVar("samplingFreq"));
    pushStaticInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("instanceInit" + tablename, args1, true)));

    // Fill the table
    list<ValueInst*> args2;
    args2.push_back(cexp);
    args2.push_back(InstBuilder::genIntNumInst(size));
    args2.push_back(InstBuilder::genLoadStaticStructVar(vname));
    pushStaticInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("fill" + tablename, args2, true)));

    // Return table access
    return InstBuilder::genLoadStaticStructVar(vname);
}

ValueInst* InstructionsCompiler::generateWRTbl(Tree sig, Tree tbl, Tree idx, Tree data)
{
    ValueInst* tblname = CS(tbl);
    LoadVarInst* load_value = dynamic_cast<LoadVarInst*>(tblname);
    assert(load_value);

    pushComputeDSPMethod(InstBuilder::genStoreArrayStructVar(load_value->fAddress->getName(), CS(idx), CS(data)));

    // Return table access
    return InstBuilder::genLoadStructVar(load_value->fAddress->getName());
}

ValueInst* InstructionsCompiler::generateRDTbl(Tree sig, Tree tbl, Tree idx)
{
    // Test the special case of a read only table that can be compiled as a static member
	Tree id, size, content;
    ValueInst* tblname;
    Address::AccessType access;

	if (isSigTable(tbl, id, size, content)) {
        access = Address::kStaticStruct;
		if (!getCompiledExpression(tbl, tblname)) {
			tblname = setCompiledExpression(tbl, generateStaticTable(tbl, size, content));
        }
	} else {
		tblname = CS(tbl);
        access = Address::kStruct;
	}

    LoadVarInst* load_value1 = dynamic_cast<LoadVarInst*>(tblname);
    assert(load_value1);

    LoadVarInst* load_value2 = InstBuilder::genLoadVarInst(InstBuilder::genIndexedAddress(
        InstBuilder::genNamedAddress(load_value1->fAddress->getName(), access), CS(idx)));

    return generateCacheCode(sig, load_value2);
}

ValueInst* InstructionsCompiler::generateSigGen(Tree sig, Tree content)
{
    string cname = getFreshID("SIG");
	string signame = getFreshID("sig");

    CodeContainer* subcontainer = signal2Container(cname, content);
	fContainer->addSubContainer(subcontainer);

    // We must allocate an object of type "cname"
    const list<ValueInst*> args;
    ValueInst* obj = InstBuilder::genFunCallInst("new" + cname, args);
    pushInitMethod(InstBuilder::genDecStackVar(signame, InstBuilder::genNamedTyped(cname, InstBuilder::genBasicTyped(Typed::kObj_ptr)), obj));

    // Delete object
    list<ValueInst*> args3;
    args3.push_back(InstBuilder::genLoadStackVar(signame));
    pushPostInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("delete" + cname, args3)));

    setTableNameProperty(sig, cname);
    fInstanceInitProperty.set(content, pair<string, string>(cname, signame));

    return InstBuilder::genLoadStackVar(signame);
}

ValueInst* InstructionsCompiler::generateStaticSigGen(Tree sig, Tree content)
{
    string cname = getFreshID("SIG");
	string signame = getFreshID("sig");

	CodeContainer* subcontainer = signal2Container(cname, content);
	fContainer->addSubContainer(subcontainer);

   // We must allocate an object of type "cname"
    const list<ValueInst*> args;
    ValueInst* obj = InstBuilder::genFunCallInst("new" + cname, args);
    pushStaticInitMethod(InstBuilder::genDecStackVar(signame, InstBuilder::genNamedTyped(cname, InstBuilder::genBasicTyped(Typed::kObj_ptr)), obj));

    // Delete object
    list<ValueInst*> args3;
    args3.push_back(InstBuilder::genLoadStackVar(signame));
    pushPostStaticInitMethod(InstBuilder::genDropInst(InstBuilder::genFunCallInst("delete" + cname, args3)));

    setTableNameProperty(sig, cname);
    fStaticInitProperty.set(content, pair<string,string>(cname, signame));

    return InstBuilder::genLoadStackVar(signame);
}

ValueInst* InstructionsCompiler::generateSelect2(Tree sig, Tree sel, Tree s1, Tree s2)
{
    int t1 = getSigType(s1)->nature();
    int t2 = getSigType(s2)->nature();

    ValueInst* cond = CS(sel);
    ValueInst* val1 = CS(s1);
    ValueInst* val2 = CS(s2);

    if (t1 == kReal) {
        if (t2 == kReal) {
            return generateCacheCode(sig, InstBuilder::genSelect2Inst(cond, val2, val1));
        } else {
            return generateCacheCode(sig, InstBuilder::genSelect2Inst(cond, InstBuilder::genCastNumInst(val2, InstBuilder::genBasicTyped(itfloat())), val1));
        }
    } else if (t2 == kReal) {
        return generateCacheCode(sig, InstBuilder::genSelect2Inst(cond, val2, InstBuilder::genCastNumInst(val1, InstBuilder::genBasicTyped(itfloat()))));
    } else {
        return generateCacheCode(sig, InstBuilder::genSelect2Inst(cond, val2, val1));
    }
}

ValueInst* InstructionsCompiler::generateSelect3(Tree sig, Tree sel, Tree s1, Tree s2, Tree s3)
{
    // TODO
    return InstBuilder::genNullInst();
}

ValueInst* InstructionsCompiler::generateRecProj(Tree sig, Tree r, int i)
{
    string vname;
    Tree var, le;
    ValueInst* res;

    if (!getVectorNameProperty(sig, vname)) {
        ensure(isRec(r, var, le));
        res = generateRec(r, var, le, i);
        ensure(getVectorNameProperty(sig, vname));
    } else {
        res = InstBuilder::genNullInst(); // Result not used
    }
    return res;
}

ValueInst* InstructionsCompiler::generateRec(Tree sig, Tree var, Tree le, int index)
{
    int N = len(le);

    ValueInst* res = NULL;
    vector<bool>    used(N);
    vector<int>     delay(N);
    vector<string>  vname(N);
    vector<Typed::VarType>  ctype(N);

    // Prepare each element of a recursive definition
    for (int i = 0; i < N; i++) {
        Tree e = sigProj(i, sig, unknown_box);     // recreate each recursive definition
        if (fOccMarkup.retrieve(e)) {
            // This projection is used
            used[i] = true;
            getTypedNames(getSigType(e), "Rec", ctype[i], vname[i]);
            setVectorNameProperty(e, vname[i]);
            delay[i] = fOccMarkup.retrieve(e)->getMaxDelay();
        } else {
            // This projection is not used therefore
            // we should not generate code for it
            used[i] = false;
        }
    }

    // Generate delayline for each element of a recursive definition
    for (int i = 0; i < N; i++) {
        if (used[i]) {
            Address::AccessType var_access;
            if (index == i) {
                res = generateDelayLine(CS(nth(le,i)), ctype[i], vname[i], delay[i], var_access);
            } else {
                generateDelayLine(CS(nth(le,i)), ctype[i], vname[i], delay[i], var_access);
            }
        }
    }

    return res;
}

ValueInst* InstructionsCompiler::generateIntCast(Tree sig, Tree x)
{
    return generateCacheCode(sig, InstBuilder::genCastNumInst(CS(x), InstBuilder::genBasicTyped(Typed::kInt)));
}

ValueInst* InstructionsCompiler::generateFloatCast(Tree sig, Tree x)
{
    return generateCacheCode(sig, InstBuilder::genCastNumInst(CS(x), InstBuilder::genBasicTyped(itfloat())));
}

ValueInst* InstructionsCompiler::generateButtonAux(Tree sig, Tree path, const string& name)
{
    string varname = getFreshID(name);
    Typed* type;
    /*
    if (gVectorSwitch) {
        type = InstBuilder::genVectorTyped(InstBuilder::genBasicTyped(yped::kFloatMacro), gVecSize);
    } else {
        type = InstBuilder::genBasicTyped(yped::kFloatMacro);
    }
    */
    type = InstBuilder::genBasicTyped(Typed::kFloatMacro);

    pushDeclare(InstBuilder::genDecStructVar(varname, type));
    pushInitMethod(InstBuilder::genStoreStructVar(varname, InstBuilder::genRealNumInst(Typed::kFloatMacro, 0)));
    addUIWidget(reverse(tl(path)), uiWidget(hd(path), tree(varname), sig));

    return generateCacheCode(sig, InstBuilder::genCastNumInst(InstBuilder::genLoadStructVar(varname), InstBuilder::genBasicTyped(itfloat())));
}

ValueInst* InstructionsCompiler::generateButton(Tree sig, Tree path)
{
    return generateButtonAux(sig, path, "fbutton");
}

ValueInst* InstructionsCompiler::generateCheckbox(Tree sig, Tree path)
{
    return generateButtonAux(sig, path, "fcheckbox");
}

ValueInst* InstructionsCompiler::generateSliderAux(Tree sig, Tree path, Tree cur, Tree min, Tree max, Tree step, const string& name)
{
    string varname = getFreshID(name);
    Typed* type;
    /*
    if (gVectorSwitch) {
        type = InstBuilder::genVectorTyped(InstBuilder::genBasicTyped(yped::kFloatMacro), gVecSize);
    } else {
        type = InstBuilder::genBasicTyped(yped::kFloatMacro);
    }
    */
    type = InstBuilder::genBasicTyped(Typed::kFloatMacro);

    pushDeclare(InstBuilder::genDecStructVar(varname, type));
    pushInitMethod(InstBuilder::genStoreStructVar(varname, InstBuilder::genRealNumInst(Typed::kFloatMacro, tree2float(cur))));
    addUIWidget(reverse(tl(path)), uiWidget(hd(path), tree(varname), sig));

    return generateCacheCode(sig, InstBuilder::genCastNumInst(InstBuilder::genLoadStructVar(varname), InstBuilder::genBasicTyped(itfloat())));
}

ValueInst* InstructionsCompiler::generateVSlider(Tree sig, Tree path, Tree cur, Tree min, Tree max, Tree step)
{
    return generateSliderAux(sig, path, cur, min, max, step, "fvslider");
}
ValueInst* InstructionsCompiler::generateHSlider(Tree sig, Tree path, Tree cur, Tree min, Tree max, Tree step)
{
    return generateSliderAux(sig, path, cur, min, max, step, "fhslider");
}

ValueInst* InstructionsCompiler::generateNumEntry(Tree sig, Tree path, Tree cur, Tree min, Tree max, Tree step)
{
    return generateSliderAux(sig, path, cur, min, max, step, "fentry");
}

ValueInst* InstructionsCompiler::generateBargraphAux(Tree sig, Tree path, Tree min, Tree max, ValueInst* exp, const string& name)
{
    string varname = getFreshID(name);
    pushDeclare(InstBuilder::genDecStructVar(varname, InstBuilder::genBasicTyped(Typed::kFloatMacro)));
 	addUIWidget(reverse(tl(path)), uiWidget(hd(path), tree(varname), sig));

	::Type t = getSigType(sig);
	switch (t->variability()) {

		case kKonst :
            pushInitMethod(InstBuilder::genStoreStructVar(varname, exp));
			break;

		case kBlock :
            pushComputeBlockMethod(InstBuilder::genStoreStructVar(varname, exp));
			break;

		case kSamp :
	        pushComputeDSPMethod(InstBuilder::genStoreStructVar(varname, exp));
			break;
	}

    return generateCacheCode(sig, InstBuilder::genCastNumInst(InstBuilder::genLoadStructVar(varname), InstBuilder::genBasicTyped(itfloat())));
}

ValueInst* InstructionsCompiler::generateVBargraph(Tree sig, Tree path, Tree min, Tree max, ValueInst* exp)
{
    return generateBargraphAux(sig, path, min, max, exp, "fvbargraph");
}

ValueInst* InstructionsCompiler::generateHBargraph(Tree sig, Tree path, Tree min, Tree max, ValueInst* exp)
{
    return generateBargraphAux(sig, path, min, max, exp, "fhbargraph");
}

ValueInst* InstructionsCompiler::generateIntNumber(Tree sig, int num)
{
    Typed::VarType ctype;
    string vname;
	Occurences* o = fOccMarkup.retrieve(sig);

	// Check for number occuring in delays
	if (o->getMaxDelay() > 0) {
		getTypedNames(getSigType(sig), "Vec", ctype, vname);
		generateDelayVec(sig, InstBuilder::genIntNumInst(num), ctype, vname, o->getMaxDelay());
	}

    // No cache for numbers
    return InstBuilder::genIntNumInst(num);
}

ValueInst* InstructionsCompiler::generateRealNumber(Tree sig, double num)
{
    Typed::VarType ctype = itfloat();
    string vname;
	Occurences* o = fOccMarkup.retrieve(sig);

	// Check for number occuring in delays
	if (o->getMaxDelay() > 0) {
		getTypedNames(getSigType(sig), "Vec", ctype, vname);
		generateDelayVec(sig, InstBuilder::genRealNumInst(ctype, num), ctype, vname, o->getMaxDelay());
	}

    // No cache for numbers
    return InstBuilder::genRealNumInst(ctype, num);
}

ValueInst* InstructionsCompiler::generateFConst(Tree sig, Tree type, const string& file, const string& name)
{
    Typed::VarType ctype;
    string vname;
	Occurences* o = fOccMarkup.retrieve(sig);

    fContainer->addIncludeFile(file);

	// Check for number occuring in delays
	if (o->getMaxDelay() > 0) {
		getTypedNames(getSigType(sig), "Vec", ctype, vname);
		generateDelayVec(sig, InstBuilder::genLoadGlobalVar(name), ctype, vname, o->getMaxDelay());
	}

    int sig_type = getSigType(sig)->nature();
    pushExtGlobalDeclare(InstBuilder::genDecGlobalVar(name,
        InstBuilder::genBasicTyped((sig_type == kInt) ? Typed::kInt : itfloat())));
    return InstBuilder::genLoadGlobalVar(name);
}

ValueInst* InstructionsCompiler::generateFVar(Tree sig, Tree type, const string& file, const string& name)
{
    fContainer->addIncludeFile(file);

    int sig_type = getSigType(sig)->nature();
    pushExtGlobalDeclare(InstBuilder::genDecGlobalVar(name,
        InstBuilder::genBasicTyped((sig_type == kInt) ? Typed::kInt : itfloat())));
    return generateCacheCode(sig, InstBuilder::genLoadGlobalVar(name));
}

ValueInst* InstructionsCompiler::generateDelayVec(Tree sig, ValueInst* exp, Typed::VarType ctype, const string& vname, int mxd)
{
    setVectorNameProperty(sig, vname);
    Address::AccessType var_access;
    return generateDelayLine(exp, ctype, vname, mxd, var_access);
}

StatementInst* InstructionsCompiler::generateInitArray(const string& vname, Typed::VarType ctype, int delay)
{
    ValueInst* init;
    BasicTyped* typed;
    string index = "i";

    if (ctype == Typed::kInt) {
        init = InstBuilder::genIntNumInst(0);
        typed = InstBuilder::genBasicTyped(Typed::kInt);
    } else {  // Real type
        init = InstBuilder::genRealNumInst(ctype, 0);
        typed = InstBuilder::genBasicTyped(ctype);
    }

    // Generates table declaration
    pushDeclare(InstBuilder::genDecStructVar(vname, InstBuilder::genArrayTyped(typed, delay)));

    // Generates init table loop
    DeclareVarInst* loop_decl = InstBuilder::genDecLoopVar(index, InstBuilder::genBasicTyped(Typed::kInt), InstBuilder::genIntNumInst(0));
    ValueInst* loop_end = InstBuilder::genLessThan(loop_decl->load(), InstBuilder::genIntNumInst(delay));
    StoreVarInst* loop_increment = loop_decl->store(InstBuilder::genAdd(loop_decl->load(), 1));

    ForLoopInst* loop = InstBuilder::genForLoopInst(loop_decl, loop_end, loop_increment);

    loop->pushFrontInst(InstBuilder::genStoreArrayStructVar(vname, loop_decl->load(), init));
    return loop;
}

StatementInst* InstructionsCompiler::generateShiftArray(const string& vname, int delay)
{
    string index = "j";

    // Generates init table loop
    DeclareVarInst* loop_decl = InstBuilder::genDecLoopVar(index, InstBuilder::genBasicTyped(Typed::kInt), InstBuilder::genIntNumInst(delay));
    ValueInst* loop_end = InstBuilder::genGreaterThan(loop_decl->load(), InstBuilder::genIntNumInst(0));
    StoreVarInst* loop_increment = loop_decl->store(InstBuilder::genSub(loop_decl->load(), InstBuilder::genIntNumInst(1)));

    ForLoopInst* loop = InstBuilder::genForLoopInst(loop_decl, loop_end, loop_increment);

    ValueInst* load_value2 = InstBuilder::genSub(loop_decl->load(), InstBuilder::genIntNumInst(1));
    ValueInst* load_value3 = InstBuilder::genLoadArrayStructVar(vname, load_value2);

    loop->pushFrontInst(InstBuilder::genStoreArrayStructVar(vname, loop_decl->load(), load_value3));
    return loop;
}

StatementInst* InstructionsCompiler::generateCopyArray(const string& vname, int index_from, int index_to)
{
    ValueInst* inst1 = InstBuilder::genLoadArrayStructVar(vname, InstBuilder::genIntNumInst(index_from));
    return InstBuilder::genStoreArrayStructVar(vname, InstBuilder::genIntNumInst(index_to), inst1);
}

StatementInst* InstructionsCompiler::generateCopyArray(const string& vname_to, const string& vname_from, int size)
{
    string index = "j";

    // Generates init table loop
    DeclareVarInst* loop_decl = InstBuilder::genDecLoopVar(index, InstBuilder::genBasicTyped(Typed::kInt), InstBuilder::genIntNumInst(0));
    ValueInst* loop_end = InstBuilder::genLessThan(loop_decl->load(), InstBuilder::genIntNumInst(size));
    StoreVarInst* loop_increment = loop_decl->store(InstBuilder::genAdd(loop_decl->load(), 1));

    ForLoopInst* loop = InstBuilder::genForLoopInst(loop_decl, loop_end, loop_increment);

    ValueInst* load_value = InstBuilder::genLoadArrayStructVar(vname_from, loop_decl->load());

    loop->pushFrontInst(InstBuilder::genStoreArrayStackVar(vname_to, loop_decl->load(), load_value));
    return loop;
}

ValueInst* InstructionsCompiler::generateDelayLine(ValueInst* exp, Typed::VarType ctype, const string& vname, int mxd, Address::AccessType& var_access)
{
    if (mxd == 0) {

        // Generate scalar use
        pushComputeDSPMethod(InstBuilder::genDecStackVar(vname, InstBuilder::genBasicTyped(ctype), exp));

   } else if (mxd < gMaxCopyDelay) {

        // Generates table init
        pushInitMethod(generateInitArray(vname, ctype, mxd + 1));

        // Generate table use
        pushComputeDSPMethod(InstBuilder::genStoreArrayStructVar(vname, InstBuilder::genIntNumInst(0), exp));

        // Generates post processing copy code to update delay values
        if (mxd == 1) {
            pushComputePostDSPMethod(generateCopyArray(vname, 0, 1));
        } else if (mxd == 2) {
            pushComputePostDSPMethod(generateCopyArray(vname, 1, 2));
            pushComputePostDSPMethod(generateCopyArray(vname, 0, 1));
        } else {
            pushComputePostDSPMethod(generateShiftArray(vname, mxd));
        }

    } else {
        int N = pow2limit(mxd + 1);

        ensureIotaCode();

        // Generates table init
        pushInitMethod(generateInitArray(vname, ctype, N));

        // Generate table use
        ValueInst* value1 = InstBuilder::genLoadStructVar("IOTA");
        ValueInst* value2 = InstBuilder::genAnd(value1, InstBuilder::genIntNumInst(N - 1));
        pushComputeDSPMethod(InstBuilder::genStoreArrayStructVar(vname, value2, exp));
    }

    return exp;
}

void InstructionsCompiler::ensureIotaCode()
{
    if (!fLoadedIota) {
        fLoadedIota = true;

        pushDeclare(InstBuilder::genDecStructVar("IOTA", InstBuilder::genBasicTyped(Typed::kInt)));
        pushInitMethod(InstBuilder::genStoreStructVar("IOTA", InstBuilder::genIntNumInst(0)));

        ValueInst* value = InstBuilder::genAdd(InstBuilder::genLoadStructVar("IOTA"), 1);
        pushComputePostDSPMethod(InstBuilder::genStoreStructVar("IOTA", value));
    }
}
