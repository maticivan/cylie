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


#ifndef __INCL_HSC_CPP
#define __INCL_HSC_CPP 
namespace HSC{ 
std::string helpString(){
    std::string paper=GL_secondPaperName;
    while(paper.size()>0&&(paper[0]=='\n'||paper[0]==' ')){paper.erase(0,1);}
    while(paper.size()>0&&(paper[paper.size()-1]=='\n'||paper[paper.size()-1]==' ')){paper.erase(paper.size()-1);}
    std::string res;
    res+="Usage: ./cylie -i certificate.txt\n";
    res+="       ./cylie -i constructions\n";
    res+="\n";
    res+="The second form verifies the constructions of Part 1 of the paper\n";
    res+="\""+paper+"\" (the pinwheel inequalities and the files cWindows/cyl_window_N*.txt),\n";
    res+="and ends with "+GL_PASS_CONSTRUCTIONS+".\n";
    res+="\n";
    res+="CONTRACT for the first form. A certificate file contains, among other records,\n";
    res+="    targetBound          a positive integer N\n";
    res+="    tauSqNum, tauSqDen   positive integers with tauSqNum < 4*tauSqDen.\n";
    res+="Let tau = sqrt(tauSqNum/tauSqDen) and omega = tau/(2-tau). If the last line printed is\n";
    res+="    "+GL_PASS+"\n";
    res+="then the following statement is a theorem, in the notation of the paper:\n";
    res+="    (N,\\omega)\\in\\Pcyl.\n";
    res+="That is, no more than N pairwise non-overlapping\n";
    res+="infinite circular cylinders of radius omega can simultaneously touch the unit ball.\n";
    res+="Everything else in the file is a machine-generated witness. No modification of it can\n";
    res+="make the program print\n";
    res+="    "+GL_PASS+"\n";
    res+="for a false statement. Tampering only makes the proof fail. The theorem is unconditional\n";
    res+="when skipMultVerification and skipOrthVerification are both \"no\". If either is \"yes\",\n";
    res+="the corresponding identities of the bundled tables are assumed rather than verified.\n";
    res+="Text outside the [key]...[/key] [value]...[/value] records is ignored.\n"; 
    return res;
} 
}

#endif
