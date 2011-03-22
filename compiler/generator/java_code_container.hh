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

#ifndef _JAVA_CODE_CONTAINER_H
#define _JAVA_CODE_CONTAINER_H

/**********************************************************************
			- code_gen.h : generic code generator (projet FAUST) -


		Historique :
		-----------

***********************************************************************/

#include "code_container.hh"
#include "java_instructions.hh"

#include "vec_code_container.hh"
#include "omp_code_container.hh"
#include "wss_code_container.hh"

using namespace std;

class JAVACodeContainer : public virtual CodeContainer {

    protected:

        JAVAInstVisitor fCodeProducer;
        std::ostream* fOut;
        string fKlassName;
        string fSuperKlassName;

    public:

        JAVACodeContainer(const string& name, const string& super, int numInputs, int numOutputs, std::ostream* out)
            :fCodeProducer(out), fOut(out), fKlassName(name), fSuperKlassName(super)
        {
            initializeCodeContainer(numInputs, numOutputs);
        }
        virtual ~JAVACodeContainer()
        {}

        virtual void produceClass();
        virtual void generateCompute(int tab) = 0;
        void produceInternal();

        void printIncludeFile(ostream& fout);

        CodeContainer* createInternalContainer(const string& name, int sub_container_type);

         static CodeContainer* createContainer(int numInputs, int numOutputs, ostream* dst);
};

class JAVAScalarCodeContainer : public JAVACodeContainer {

    protected:

    public:

        JAVAScalarCodeContainer(const string& name, const string& super, int numInputs, int numOutputs, std::ostream* out, int sub_container_type);
        virtual ~JAVAScalarCodeContainer();

        void generateCompute(int tab);

};

class JAVAVectorCodeContainer : public VectorCodeContainer, public JAVACodeContainer {

    protected:

    public:

        JAVAVectorCodeContainer(const string& name, const string& super, int numInputs, int numOutputs, std::ostream* out);
        virtual ~JAVAVectorCodeContainer();

        void generateCompute(int tab);

};

class JAVAOpenMPCodeContainer : public OpenMPCodeContainer, public JAVACodeContainer {

    protected:

    public:

        JAVAOpenMPCodeContainer(const string& name, const string& super, int numInputs, int numOutputs, std::ostream* out);
        virtual ~JAVAOpenMPCodeContainer();

        void generateCompute(int tab) {}

};

class JAVAWorkStealingCodeContainer : public JAVACodeContainer {

    protected:


    public:

        JAVAWorkStealingCodeContainer(const string& name, const string& super, int numInputs, int numOutputs, std::ostream* out);
        virtual ~JAVAWorkStealingCodeContainer();

        void generateCompute(int tab) {}

};

#endif
