// Author:  Dennis Lang - 2020
// Website: http://landenlabs.com
//
// Description:  Text file line-by-line comparison.
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

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <regex>
#include <cstring>

namespace lldiff {

typedef std::vector<std::string> StrList;
typedef std::vector<size_t> HashList;
typedef std::pair<std::regex,const char*> RegReplace;
typedef std::vector<RegReplace> ReplaceList;
typedef unsigned int RowNum;

struct RowMatch {
    unsigned matchedRows;
    RowNum row0, row1;
    void clear() {
        matchedRows = 0;
        // row0 = row1 = 0;
    }
};

// Hold and manage Text file row-by-row difference comparison.
class Diff {
  public:
    const char* filenames[2];
    
    // Raw text lines
    StrList fileLines0;
    StrList fileLines1;
    
    // Hashed rows - raw text or extract comparison keys.
    HashList hashList0;
    HashList hashList1;
    
    unsigned widths[2];     // maximum file text row width.
    bool verbose;           // true show more output
    std::string divider;    // used when not verbose output to split diff output rows.
    std::string left;
    std::string right;
    
    bool mergeOut0;         // If merging, output from file #0
    bool mergeOut1;         // If merging, output from file #1
    
    // Optional regular expression to compare or merge files
    std::vector<std::regex*> cmpRxP;
    std::vector<std::regex*> mergeRxP;
    
    // Optional regular expression to replace text in rows.
    ReplaceList replaceList;
    
    unsigned scanRows;      // How far to scan for a match.
    unsigned minMatch;      // minimum number or rows to match during scan
    unsigned maxMatch;      // maximum number of rows to match during scan, smaller faster, longer better match
    unsigned width;         // side-by-side output padding, max of input files.

    mutable RowMatch rowMatch;
    mutable std::string tmpStr;
    
    Diff() :
        cmpRxP(2, NULL), mergeRxP(2, NULL),
        scanRows(100),
        minMatch(3), maxMatch(6),
        width(0),
        verbose(false),
        divider(" || "),
        replaceList(),
        mergeOut0(true),
        mergeOut1(true)
        {  }
    
    // Read file into vector, optionally replace fields and extract keys, hash code result.
    bool readFile(const char* filename, unsigned idx) {
        filenames[idx] = filename;
        StrList& vecLines = (idx == 0) ? fileLines0 : fileLines1;
        std::ifstream in;
        in.open(filename);
        widths[idx] = readFile(in, vecLines, replaceList);
        in.close();
        std::cerr << vecLines.size() << " lines, max width=" << widths[idx] << " from:" << filename << std::endl;
        
        if (idx == 0) {
            makeHash(fileLines0, hashList0, cmpRxP[0]);
        } else {
            makeHash(fileLines1, hashList1, cmpRxP[1]);
        }
        return vecLines.size() > 0;
    }

    // Get merged text row (optionally extract using regular expression).
    const std::string& getMerge(const std::string& str, std::regex* mergeRxP) const  {
        if (mergeRxP != nullptr)   {
            std::smatch match;
            tmpStr.clear();
            if (regex_match(str, match, *mergeRxP)) {
                for (unsigned idx = 1; idx < match.size(); idx++) {
                    tmpStr += match[idx].str();
                }
                return tmpStr;
            }
        }
        return str;
    }
    
    const RowMatch& rowMatches0(RowNum num, RowNum row0, RowNum row1) const {
        rowMatch.clear();
        for (RowNum off=0; off <= num; off++) {
            unsigned matchCnt = rowMatches(row0+num, row1+off);
            if (matchCnt > rowMatch.matchedRows) {
                rowMatch.matchedRows = matchCnt;
                rowMatch.row0 = row0+num;
                rowMatch.row1 = row1+off;
            }
        }
        return rowMatch;
    }
    
    const RowMatch& rowMatches1(RowNum num, RowNum row0, RowNum row1) const {
        rowMatch.clear();
        for (unsigned off=0; off < num; off++) {
            unsigned matchCnt = rowMatches(row0+off, row1+num);
            if (matchCnt > rowMatch.matchedRows) {
                rowMatch.matchedRows = matchCnt;
                rowMatch.row0 = row0+off;
                rowMatch.row1 = row1+num;
            }
        }
        return rowMatch;
    }
    
    
protected:
    unsigned rowMatches(RowNum row0, RowNum row1) const {
        unsigned cnt = 0;
        while (row0 < hashList0.size() && row1 < hashList1.size()) {
            if (hashList0[row0] == hashList1[row1]) {
                if (cnt++ > maxMatch)
                    break;
            } else {
                break;
            }
        }
        return cnt;
    }
    
    // Read file and optionally replace text using regular expression.
    unsigned readFile(std::ifstream &iFile, StrList& vecLines, const ReplaceList& replaceList) {
        unsigned maxWidth = 0;
        std::string str;
        std::smatch match;
        while (std::getline(iFile, str))
        {
            for (auto&& regReplace : replaceList ) {
                if (std::regex_match(str, match, regReplace.first)) {
                    str.replace(match.position(1),  match[1].length(), regReplace.second);
                }
            }
                           
            maxWidth = std::max(maxWidth, (unsigned)str.length());
            vecLines.push_back(str);
        }
        
        return maxWidth;
    }
    
    void makeHash(StrList& lines, HashList& hashes, std::regex* cmpRxP) {
        hashes.reserve(lines.size());
        if (cmpRxP != nullptr) {
            std::smatch match;
            for (RowNum idx = 0; idx < lines.size(); idx++) {
                const std::string& lineStr = lines[idx];
                const std::string* strP = &lineStr;
                std::string tmpStr;
                if (regex_match(lineStr, match, *cmpRxP)) {
                    // reg=".*Tag1=([^ ]+).*Tag2=([^ ]+).*"
                    // str="blah Tag1=Hello blah Tag2=World blah"
                    for (unsigned midx=1; midx < match.size(); midx++) {
                        tmpStr += match[midx].str();
                    }
                    strP = &tmpStr;
                }

                size_t hash = std::hash<std::string>{}(*strP);
                hashes.push_back(hash);
            }
        } else {
             for (RowNum idx = 0; idx < lines.size(); idx++) {
                 // todo - optionally trim lead/trail space, ignore spaces, etc.
                 hashes.push_back(std::hash<std::string>{}(lines[idx] ));
             }
        }
    }
    
};

} // namespace
