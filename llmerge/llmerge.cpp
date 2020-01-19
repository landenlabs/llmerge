// Author:  Dennis Lang - 2020
// Website: http://landenlabs.com
//
// Description:  Text file line-by-line comparison or merge command line program.
//
// This file is part of llmerge project.
//
// ----- License ----
//
// Copyright (c) 2020 Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


const char* HELP =
"Merge (compare) text files\n"
"  Build: " __DATE__ "\n"
"  By: Dennis Lang  landenlabs.com\n"
"\n"
"Command:\n"
"  llmerge [options] <file1> <file2>\n"
"\n"
"  ; Compare text files line-by-line:\n"
"     llmerge ([-c=<regEx>] |[-c1=<regEx1>] [-c2=<regEx2>])  <file1> <file2> \n"
"  Example:\n"
"     llmerge file1.xml file2.xml\n"
"     llmerge llmerge '-c=.*tag=([^ ]+).*' file1.xml file2.xml \n"
"     llmerge llmerge '-c=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml \n"
"\n"
"  ; Merge text files:\n"
"      llmerge [-c1=<regEx1>] [-c2=<regEx2>] (-m|-m1=<regMergeEx1> -m2=<regMergeEx2>) <file1> <file2> \n"
"  Example:\n"
"      llmerge -m file1.xml file2.xml \n"
"      llmerge '-m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml \n"
"      llmerge '-c=.*tag1=([^ ]+).*' '-m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml \n"
"      llmerge '-c=.*tag1=([^ ]+).*tag2=([^ ]+).*'  file1.xml file2.xml \n"
"\n"
"Where:\n"
"  -c Sets the optional compare extraction regular expression which must have one or more capture groups.\n"
"  -m Sets the optional merge extraction regular expression which must have one or more capture groups.\n"
"\n"
"   Regular expression must include a group to extract, such as '.*Something([^ ]+).*'  \n"
"\n"
"  During file load optionally replace text using regular expression groups:\n"
"      -r=<regex1>;replace1  -r=<regex2>;replace2 ... \n"
"  Example:\n"
"      '-r=.*(OldWord).*;NewWord'   \n"
"Links:\n"
"  https://www.regular-expressions.info/refcapture.html \n"
"\n\n";


#include "lldiff.h"

using namespace std;

// ------------------------------------------------------------------------------------------------
// Display text rows side-by-side
void displayRowsSideBySide(
        const lldiff::Diff& diffInfo,
        lldiff::RowNum bRow0,   // begin row file 0
        lldiff::RowNum  eRow0,  //   end row file 0
        lldiff::RowNum  bRow1,  // begin row file 1
        lldiff::RowNum  eRow1)  //   end row file 1
{
    unsigned width = diffInfo.width;
    const lldiff::StrList& fileLines0 = diffInfo.fileLines0;
    const lldiff::StrList& fileLines1 = diffInfo.fileLines1;
    
    while (bRow0 < eRow0)   {
        std::cout
            << "[" << right << std::setw(3) << bRow0+1 << "]"
            << left << std::setw(width) << fileLines0[bRow0]
            << std::endl;
        bRow0++;
    }
    while (bRow1 < eRow1)   {
        std::cout
                << "[xxx]" << std::setw(width) << " " << " xx "
                << "[" << right << std::setw(3) << bRow1+1 << "]"
                << left << std::setw(width) << fileLines1[bRow1]
                << std::endl;
        bRow1++;
    }
    
    if (bRow0 == eRow0 && bRow1 == eRow1)  {
        std::cout
                << "[" << right << std::setw(3) << bRow0+1 << "]"
                << left << std::setw(width) << fileLines0[bRow0] << " || "
                << "[" << right << std::setw(3) << bRow1+1 << "]"
                << left << std::setw(width) << fileLines1[bRow1]
                << std::endl;
    }
}

// ------------------------------------------------------------------------------------------------
void compareTextFiles(const lldiff::Diff& diffInfo)
{
    unsigned width = diffInfo.width;
    std::cerr << "  Lines0=" << diffInfo.fileLines0.size() << std::endl;
    std::cerr << "  Lines1=" << diffInfo.fileLines1.size() << std::endl;
    std::cerr << "  MaxWidth=" << width << std::endl;
    
    lldiff::RowNum dsp0 = 0;
    lldiff::RowNum dsp1 = 0;
    lldiff::RowNum row0 = 0;
    lldiff::RowNum row1 = 0;
    bool more = true;
    lldiff::RowMatch rowMatch;
    
    while (more) {
        more = false;
        rowMatch.clear();
        const unsigned SCAN_ROWS = 5;
        for (unsigned idx =0; idx < SCAN_ROWS; idx++) {
            lldiff::RowMatch match0 = diffInfo.rowMatches0(idx, row0, row1); // TODO - use rowMatch instead of row0, row1
            lldiff::RowMatch match1 = diffInfo.rowMatches1(idx, row0, row1); // TODO - use rowMatch instead of row0, row1
            rowMatch = (match0.matchedRows >= match1.matchedRows) ? match0 : match1;
            if (rowMatch.matchedRows > diffInfo.minMatch) {
                displayRowsSideBySide(diffInfo, dsp0, rowMatch.row0, dsp1, rowMatch.row1);
                row0 = rowMatch.row0+1;   // TODO - use rowMatch instead of row0, row1
                row1 = rowMatch.row1+1;
                dsp0 = row0;
                dsp1 = row1;
                more = true;
                break;
            }
        }
    }
    
    // Display any remainning lines.
    displayRowsSideBySide(
              diffInfo,
              dsp0, (lldiff::RowNum)diffInfo.fileLines0.size()-1,
              dsp1, (lldiff::RowNum)diffInfo.fileLines1.size()-1);

    std::cout << "[end]\n";
}

// ------------------------------------------------------------------------------------------------
// Merge text rows into single output row.
void mergeRowsSideBySide(
        const lldiff::Diff& diffInfo,
        lldiff::RowNum bRow0,   // begin row file 0
        lldiff::RowNum  eRow0,  //   end row file 0
        lldiff::RowNum  bRow1,  // begin row file 1
        lldiff::RowNum  eRow1)  //   end row file 1
{
    const lldiff::StrList& fileLines0 = diffInfo.fileLines0;
    const lldiff::StrList& fileLines1 = diffInfo.fileLines1;
    
    while (bRow0 < eRow0)   {
        std::cout << diffInfo.getMerge(fileLines0[bRow0], diffInfo.mergeRxP[0]) << std::endl;
        bRow0++;
    }
    while (bRow1 < eRow1)   {
        std::cout << diffInfo.getMerge(fileLines1[bRow1], diffInfo.mergeRxP[1]) << std::endl;
        bRow1++;
    }
    
    if (bRow0 == eRow0 && bRow1 == eRow1)  {
        std::cout  << diffInfo.getMerge(fileLines0[bRow0], diffInfo.mergeRxP[0]) << std::endl;
    }
}

// ------------------------------------------------------------------------------------------------
void mergeTextDiles(lldiff::Diff& diffInfo)
{
    unsigned dsp0 = 0;
    unsigned dsp1 = 0;
    unsigned row0 = 0;
    unsigned row1 = 0;
    bool more = true;
    lldiff::RowMatch rowMatch;
    
    while (more) {
        more = false;
        rowMatch.clear();
        const unsigned SCAN_ROWS = 5;
        for (unsigned idx =0; idx < SCAN_ROWS; idx++) {
            lldiff::RowMatch match0 = diffInfo.rowMatches0(idx, row0, row1); // TODO - use rowMatch instead of row0, row1
            lldiff::RowMatch match1 = diffInfo.rowMatches1(idx, row0, row1); // TODO - use rowMatch instead of row0, row1
            rowMatch = (match0.matchedRows >= match1.matchedRows) ? match0 : match1;
            if (rowMatch.matchedRows > diffInfo.minMatch) {
                mergeRowsSideBySide(diffInfo, dsp0, rowMatch.row0, dsp1, rowMatch.row1);
                row0 = rowMatch.row0+1;   // TODO - use rowMatch instead of row0, row1
                row1 = rowMatch.row1+1;
                dsp0 = row0;
                dsp1 = row1;
                more = true;
                break;
            }
        }
    }
    
    // Display any remainning lines.
    mergeRowsSideBySide(
              diffInfo,
              dsp0, (lldiff::RowNum)diffInfo.fileLines0.size()-1,
              dsp1, (lldiff::RowNum)diffInfo.fileLines1.size()-1);
}
 

// ================================================================================================
int main(int argc, char* argv[]) {

    lldiff::Diff diffInfo;
    bool doMerge = false;
    const char* value1;
    char* value2;
    regex* regP = NULL;
    
    // ---- Parse runtime arguments (parameters)
    int argIdx = 1;
    for (; argIdx < argc; argIdx++) {
        if (argv[argIdx][0] == '-') {
            const char* arg = argv[argIdx]+1;
            if ((value1 = strchr(arg, '=')) != nullptr) {
                // -c1=regex
                // -r=regex;replace
                
                value2 = (char*)strrchr(value1, ';');
                if (value2 != nullptr) {
                    *value2++ = '\0';
                }
                // value = dequote((char*)value1+1);
                try {
                    regP = new regex(value1+1);
                } catch (exception ex) {
                    std::cerr << "Invalid regular expression " << value1+1
                        << "\n" <<  ex.what()
                        << std::endl;
                }
            }
            
            switch (arg[0]) {
                    
                    // Optional comparison extraction regular expression
              case 'c': //  ([-c=<regEx>] |[-c1=<regEx1>] [-c2=<regEx2>])
                if (isdigit(arg[1])) {
                    diffInfo.cmpRxP[arg[1] == '1' ? 0 : 1] = regP;
                } else {
                    diffInfo.cmpRxP[0] = diffInfo.cmpRxP[1] = regP;
                }
                break;
                    
                    // Any -m to start merge, optional regex to extract merge data
              case 'm':  //  (-m | [-m=<regEx>] | [-m1=<regEx1>] [-m2=<regEx2>])
                doMerge = true;
                if (isdigit(arg[1])) {
                    diffInfo.mergeRxP[arg[1] == '1' ? 0 : 1] = regP;
                } else {
                    diffInfo.mergeRxP[0] = diffInfo.mergeRxP[1] = regP;
                }
                break;
              case 'r':
                diffInfo.replaceList.push_back(std::pair<regex,const char*>(*regP, value2));
                break;
            }
            
            regP = NULL;
        } else {
            break;
        }
    }
    
    if (argIdx+2 <= argc) {
        // ---- Load files and hash lines
        diffInfo.readFile(argv[argIdx++], 0);
        diffInfo.readFile(argv[argIdx++], 1);
        diffInfo.width = max(diffInfo.widths[0], diffInfo.widths[1]);
        
        // ---- Merge or compare files.
        if (doMerge) {
            mergeTextDiles(diffInfo);
        } else {
            compareTextFiles(diffInfo);
        }
    } else {
        std::cerr << HELP;
    }
    
    return 0;
}


