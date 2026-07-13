<table border="0">
  <tr>
    <td>
      <!-- VERSION -->v6.07.10<br>
      <!-- DATE -->12-Jul-2026<br>
      Win & MacOS<br>
      <a href="https://landenlabs.com">Home</a>
    </td>
    <td>
      <a href="https://landenlabs.com">
        <img src="screens/landen_labs_300.webp" width="300" alt="LanDen Labs">
      </a>
    </td>
  </tr>
</table>

# llmerge

# LanDen Labs - llmerge 
llmerge [Home website](https://landenlabs.com)  
  
### Simple C++ Text file comparison and/or merge command line utility.
Simple text file row by row comparison or merge.

  [![Build status](https://travis-ci.org/landenlabs/llmerge.svg?branch=master)](https://travis-ci.org/landenlabs/llmerge)
  [![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE.txt)
  ![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
  ![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows-lightgrey.svg)
  

## Key features   
1. Simple C++ code to read and compare text file row by row.
2. Output side-by-side or merged
3. Optional regular expression to replace text prior to comparison
4. Optional regular expression to extract comparison text
5. Optional regular expression to extract merge text

***  
  
[Home webpage ](https://landenlabs.com/index.html) 

Related implementations:

* [WinMerge](https://github.com/WinMerge/winmerge)
  
---  
<a name="llmerge"></a>  
### llmerge
  
<pre>
Merge (compare) text files
  Build: <!-- VERSION -->v6.07.10  <!-- DATE -->12-Jul-2026
  By: Dennis Lang  landenlabs.com

Command:
  llmerge [options] <file1> <file2>

  ; Compare text files side-by-side:
     llmerge ([-c=<regEx>] |[-c1=<regEx1>] [-c2=<regEx2>])  <file1> <file2>
  Example:
     llmerge file1.xml file2.xml
     llmerge '-c=.*tag=([^ ]+).*' file1.xml file2.xml
     llmerge '-c=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml

  ; Merge text files line by line:
      llmerge [-c1=<regEx1>] [-c2=<regEx2>] (-m|-m1=<regMergeEx1> -m2=<regMergeEx2>) <file1> <file2>
  Example:
      llmerge -m file1.xml file2.xml
      llmerge '-m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml
      llmerge '-c=.*tag1=([^ ]+).*' '-m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml
      llmerge '-c=.*tag1=([^ ]+).*tag2=([^ ]+).*'  file1.xml file2.xml

Where:
  -c Sets the optional compare extraction regular expression which must have one or more capture groups
     and must match entire row, so add .* at front or end of pattern.
  -m Sets the optional merge extraction regular expression which must have one or more capture groups.

   Regular expression must include a group to extract, such as '.*Something([^ ]+).*'

  During file load optionally replace text using regular expression groups:
      -r=<regex1>;replace1  -r=<regex2>;replace2 ...
  Example:
      '-r=.*(OldWord).*;NewWord'

  -N# No line output from file '#' when merging and both files have identical matching keys
  Example:
      llmerge '-c=.*tag=([^ ]+).*' -m file1.xml file2.xml
      llmerge -N0 '-c=.*tag=([^ ]+).*' -m file1.xml file2.xml
      llmerge -N1 '-c=.*tag=([^ ]+).*' -m file1.xml file2.xml

  -D Set divider displayed in side-by-side compare, defaults to '||'
  -L Set LEFT side when no match available. Useful if generating fixed column output such as CSV
  -R Set RIGHT side when no match available. Useful if generating fixed column output such as CSV
  Example:
      llmerge '-c=([A-Z]+[0-9]+),.*' '-L=,,,,,,' -D=, and.srt ios.srt > compare-android-ios.csv

Links:
  https://www.regular-expressions.info/refcapture.html
</pre>

### License

```
Copyright 2026 Dennis Lang

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
See [LICENSE.txt](LICENSE.txt) for the full license text.
 
  
[To Top](#llmerge)  
<br>[Home website](https://landenlabs.com)
