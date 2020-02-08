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


#include "lldiff.h"
#include "Colors.h"
using namespace std;

const char* HELP =
"Merge (compare) text files\n"
"  _P_Build:_X_ " __DATE__ "\n"
"  _P_By:_X_    Dennis Lang  landenlabs.com \n"
"\n"
"_P_Command:_X_\n"
"  llmerge [options] <file1> <file2>\n"
"\n"
"  ; Compare text files line-by-line:\n"
"     llmerge ([-_r_c=<regEx>] |[-_r_c1=<regEx1>] [-_r_c2=<regEx2>])  <file1> <file2> \n"
"  _B_Example:_X_\n"
"     llmerge file1.xml file2.xml\n"
"     llmerge llmerge '-_r_c=.*tag=([^ ]+).*' file1.xml file2.xml \n"
"     llmerge llmerge '-_r_c=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml \n"
"\n"
"  ; Merge text files:\n"
"      llmerge [-_r_c1=<regEx1>] [-_r_c2=<regEx2>] (-_r_m|-_r_m1=<regMergeEx1> -_r_m2=<regMergeEx2>) <file1> <file2> \n"
"  _B_Example:_X_\n"
"      llmerge -_r_m file1.xml file2.xml \n"
"      llmerge '-_r_m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml \n"
"      llmerge '-_r_c=.*tag1=([^ ]+).*' '-_r_m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml \n"
"      llmerge '-_r_c=.*tag1=([^ ]+).*tag2=([^ ]+).*'  file1.xml file2.xml \n"
"\n"
"_P_Where:_X_\n"
"  -_r_c Sets the optional compare extraction regular expression which must have one or more capture groups.\n"
"  -_r_m Sets the optional merge extraction regular expression which must have one or more capture groups.\n"
"   Regular expression must include a group to extract, such as '.*Something([^ ]+).*'  \n"
"\n"
"  During file load optionally replace text using regular expression groups:\n"
"      -_r_r=<regex1>;replace1  -r=<regex2>;replace2 ... \n"
"  _B_Example:_X_\n"
"      '-_r_r=.*(OldWord).*;NewWord'   \n"
"\n"
"  -_r_D Set divider displayed in side-by-side compare, defaults to ',' \n"
"  -_r_L Set LEFT side when no match available. Useful if generating fixed column output such as CSV \n"
"  -_r_R Set RIGHT side when no match available. Useful if generating fixed column output such as CSV \n"
"  _B_Example:_X_\n"
"      llmerge '-_r_c=([A-Z]+[0-9]+),.*' '-_r_L=,,,,,,' -_r_D=, and.srt ios.srt > compare-android-ios.csv \n"
"\n"
"_P_Links:_X_\n"
"  https://www.regular-expressions.info/refcapture.html \n"
"\n\n";



// Forward declaration
void init();


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
        if (diffInfo.verbose) {
            std::cout << "[" << right << std::setw(3) << bRow0+1 << "]" << left;
        }
        std::cout << std::setw(width) << fileLines0[bRow0];
        if (!diffInfo.verbose) {
            std::cout << diffInfo.divider << diffInfo.right;
        }
        std::cout << std::endl;
        bRow0++;
    }
    while (bRow1 < eRow1)   {
        if (diffInfo.verbose) {
            std::cout
                    << "[xxx]" << std::setw(width) << diffInfo.left << " xx "
                    << "[" << right << std::setw(3) << bRow1+1 << "]" << left ;
        } else {
            std::cout << std::setw(width) <<diffInfo.left << diffInfo.divider;
        }
        std::cout << std::setw(width) << fileLines1[bRow1]  << std::endl;
        bRow1++;
    }
    
    if (bRow0 == eRow0 && bRow1 == eRow1)  {
        if (diffInfo.verbose) {
            std::cout
                    << "[" << right << std::setw(3) << bRow0+1 << "]"
                    << left << std::setw(width) << fileLines0[bRow0] << diffInfo.divider
                    << "[" << right << std::setw(3) << bRow1+1 << "]"
                    << left << std::setw(width) << fileLines1[bRow1]
                    << std::endl;
        } else {
            std::cout
                   << left << std::setw(width) << fileLines0[bRow0]
                   << diffInfo.divider
                   << left << std::setw(width) << fileLines1[bRow1]
                   << std::endl;
        }
    }
}

// ------------------------------------------------------------------------------------------------
void compareTextFiles(const lldiff::Diff& diffInfo)
{
    unsigned width = diffInfo.width;
    
    lldiff::RowNum dsp0 = 0;
    lldiff::RowNum dsp1 = 0;
    lldiff::RowNum row0 = 0;
    lldiff::RowNum row1 = 0;
    bool more = true;
    lldiff::RowMatch rowMatch;
    
    while (more) {
        more = false;
        rowMatch.clear();
        for (unsigned idx =0; idx < diffInfo.scanRows; idx++) {
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
        for (unsigned idx =0; idx < diffInfo.scanRows; idx++) {
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
              case 'r': // Replace list of regex to execute on input lines before compare.
                diffInfo.replaceList.push_back(std::pair<regex,const char*>(*regP, value2));
                break;
              case 'v': // Verbose, defaults false
                diffInfo.verbose = true;
                break;
              case 'D': // Divider, defaults to ","
                diffInfo.divider = value1+1;
                break;
              case 'L': // Left when row is missing
                diffInfo.left = value1+1;
                break;
             case 'R': // Right when row is missing
                diffInfo.right = value1+1;
                break;
            }
            
            regP = NULL;
        } else {
            break;
        }
    }
    
    if (argIdx+2 <= argc) {
        // ---- Load files and hash lines
        bool got0 = diffInfo.readFile(argv[argIdx++], 0);
        bool got1 = diffInfo.readFile(argv[argIdx++], 1);
        diffInfo.width = max(diffInfo.widths[0], diffInfo.widths[1]);
        
        if (got0 && got1) {
            // ---- Merge or compare files.
            if (doMerge) {
                mergeTextDiles(diffInfo);
            } else {
                compareTextFiles(diffInfo);
            }
        } else {
            if (!got0)
                std::cerr << "Failed to load text rows from:" << diffInfo.filenames[0] << std::endl;
            if (!got1)
                std::cerr << "Failed to load text rows from:" << diffInfo.filenames[1] << std::endl;
        }
    } else {
        std::cerr << Colors::colorize(HELP);
    }
    
    return 0;
}


// ================================================================================================
void init() {
#ifdef HAVE_WIN
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        exit( GetLastError());
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        exit( GetLastError());
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        exit( GetLastError());
    }
#endif
}
