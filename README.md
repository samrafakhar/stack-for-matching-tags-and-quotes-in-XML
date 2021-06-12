# stack-for-matching-tags-and-quotes-in-XML
A C++ program that use a stack for matching tags and quotes in XML eXtensive Markup Language.

## How it works?
The program takes an XML code in an input file and it figures out if tags and quotes are properly matched or not using stack. In case the tags are not properly matched then program reports following
  i) the first error,
  ii) print the mismatched tag and
  iii) inform the line number where the starting tag occurred.

## XML Features
Program andles the following features of XML:
1. xml prolog (xml header)
2. xml tags (xml elements). The xml tags are case sensitive.
3. xml attribute
4. xml comments, start with < !-- and ends with -->

## How To run?
1. Download/Clone the cpp file
2. Run it in your preferred IDE for c++
3. provide the file containing xml code as command line argument 
