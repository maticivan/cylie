//*************************************************************************************************
//*************************************************************************************************
//* The MIT License (MIT)                                                                         *
//* Copyright (C) 2026                                                                            *
//* Ivan Matic, Rados Radoicic, and Andreas Spomer                                                *
//*                                                                                               *
//* Permission is hereby granted, free of charge, to any person obtaining a copy of this          *
//* software and associated documentation files (the "Software"), to deal in the Software         *
//* without restriction, including without limitation the rights to use, copy, modify, merge,     *
//* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons    *
//* to whom the Software is furnished to do so, subject to the following conditions:              *
//*                                                                                               *
//* The above copyright notice and this permission notice shall be included in all copies or      *
//* substantial portions of the Software.                                                         *
//*                                                                                               *
//* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,           *
//* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR      *
//* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE     *
//* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR          *
//* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        *
//* DEALINGS IN THE SOFTWARE.                                                                     *
//*************************************************************************************************

#include "src/fundamental/SHF_standardHeadersMinimal.cpp"
#include <thread>
#include "src/fundamental/ssm.cpp"
#include "src/fundamental/fst.cpp" 
#include "src/fundamental/MFRF_massiveFindReplace.cpp"
#include "src/fundamental/BF_bijFunction.cpp"
#include "src/fundamental/TMF_timer.cpp"
#include "src/fundamental/SF_strings.cpp"
#include "src/fundamental/GF_globalInitsMinimal.cpp"
#include "src/fundamental/RNDF_randomCodesAndPermutations.cpp"
#include "src/fundamental/IOF_io.cpp"
std::string GL_PASS="THE THEOREM WAS PROVED";
std::string GL_PASS_CONSTRUCTIONS="ALL CONSTRUCTIONS VERIFIED";
std::string GL_FAIL_CONSTRUCTIONS="Failed to verify the constructions";
std::string GL_secondPaperName="\nInsertion and Expansion of Cylinders around Unit Ball\n";
#include "src/tools/CLAT_commandLineArguments.cpp"
#include "src/algebra/FA_fractions.cpp"
#include "src/algebra/PA_polynomials.h"
#include "src/algebra/PA_polynomials.cpp"
#include "src/cylinders/CPC_cPolynomials.cpp"
#include "src/cylinders/SOSC_sosCertificate.cpp"
#include "src/cylinders/CC_cylinders.cpp"
#include "src/cylinders/PC_pinwheelProofs.cpp"
#include "src/cylinders/RC_remainingCases.cpp"
#include "src/cylinders/HSC_helpString.cpp"
void printMessagesAndUpdateSuccessForOverflow(int& success){
    if((FA::GL_OVERFLOW)||(FA::GL_DIVBY0)){
        success=0;
        std::cout<<"WARNING: The program had a division by zero or an overflow in calculation. Results are not trustworthy. \n";
    }
    if(PA::GL_errorWithMonomials){
        success=0;
        std::cout<<"WARNING: The program attempted to initialize monomial incorrectly. Results are not trustworthy. \n";
    }
}
int main(int argc, char **argv){
    TMF::Timer tm;
    int success;
    CLAT::CommandLineArgs inputArgs=CLAT::getArguments(CLAT::argsInStrings(argc,argv));
    std::string sourceTxt; 
    if(inputArgs.inputFile=="notFound"){
        std::cout<<HSC::helpString()<<"\n";
        return 0;
    }
    if(inputArgs.inputFile=="constructions"){
        success=RC::checkCases<Int512>();
        printMessagesAndUpdateSuccessForOverflow(success);
        if(success==1){
            std::cout<<GL_PASS_CONSTRUCTIONS<<"\n";
        }
        else{
            std::cout<<GL_FAIL_CONSTRUCTIONS<<"\n";
        }
        return 0;    
    } 
    sourceTxt=IOF::fileToString(inputArgs.inputFile);
    if(sourceTxt=="fileNotFound"){
        std::cout<<"Input file not found\n";
        return 0;
    } 
    tm.start();
    success=CC::mainWork(sourceTxt,inputArgs.outputFile);
    tm.end();
    std::cout<<"Total time: "<<tm.getTimeSec()<<" seconds.\n";
    printMessagesAndUpdateSuccessForOverflow(success);
    if(success){
        std::cout<<GL_PASS<<"\n";
    }
    return 0;
}

