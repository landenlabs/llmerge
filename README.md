
# LanDen Labs - llmerge 
llmerge [Home website](http://landenlabs.com)  
19-Jan-2020  
  
### Simple C++ Text file comparison and/or merge command line utility.
Simple text file row by row comparison or merge.

  [![Build status](https://travis-ci.org/landenlabs/llmerge.svg?branch=master)](https://travis-ci.org/landenlabs/llmerge)
  

## Key features   
1. Simple C++ code to read and compare text file row by row.
2. Output side-by-side or merged
3. Optional regular expression to replace text prior to comparison
4. Optional regular expression to extract comparison text
5. Optional regular expression to extract merge text

***  
  
[Home webpage ](http://landenlabs.com/index.html) 

Related implementations:

* [WinMerge](https://github.com/WinMerge/winmerge)
  
---  
<a name="llmerge"></a>  
### llmerge
  
<pre>
Merge (compare) text files
  Build: Jan 19 2020
  By: Dennis Lang  landenlabs.com

Command:
  llmerge [options] <file1> <file2>

  ; Compare text files line-by-line:
     llmerge ([-c=<regEx>] |[-c1=<regEx1>] [-c2=<regEx2>])  <file1> <file2>
  Example:
     llmerge file1.xml file2.xml
     llmerge llmerge '-c=.*tag=([^ ]+).*' file1.xml file2.xml
     llmerge llmerge '-c=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml

  ; Merge text files:
      llmerge [-c1=<regEx1>] [-c2=<regEx2>] (-m|-m1=<regMergeEx1> -m2=<regMergeEx2>) <file1> <file2>
  Example:
      llmerge -m file1.xml file2.xml
      llmerge '-m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml
      llmerge '-c=.*tag1=([^ ]+).*' '-m=.*tag1=([^ ]+).*tag2=([^ ]+).*' file1.xml file2.xml
      llmerge '-c=.*tag1=([^ ]+).*tag2=([^ ]+).*'  file1.xml file2.xml

Where:
  -c Sets the optional compare extraction regular expression which must have one or more capture groups.
  -m Sets the optional merge extraction regular expression which must have one or more capture groups.

   Regular expression must include a group to extract, such as '.*Something([^ ]+).*'

  During file load optionally replace text using regular expression groups:
      -r=<regex1>;replace1  -r=<regex2>;replace2 ...
  Example:
      '-r=.*(OldWord).*;NewWord'
Links:
  https://www.regular-expressions.info/refcapture.html
</pre>
 
  
[To Top](#llmerge)  
<br>[Home website](http://landenlabs.com)