-------------
README
-------------
included:
bwtsearch.cpp
makefile
this README

------------
Command Line Arguments
------------
-n:  it can output the total number of matching patterns
-r:  it can output the number of the non-duplicated offsets
-a:  it can output all uniqe offsets in the form of '[offset]'

------------
Algorithm Analysis and some method's specification
------------
(1) Read from the bwt file
    In order to suit the memory space, it read the file segment by segment. I set the buffer size to 10000, which means every time it reads 10000 characters.(this size is also bucket size, see(3))

(2) Create Count table
    Create a two-dimensional array(index: 0-128). When reading the bwt file, we update this array.   

(3) Rank table
    Every bucket has a rank table (a rank table is also a two-dimensional table, index: 0-128). It is used to count how many time a character appears in a buffer size (or bucket size) length string.

(4) How to count the rank;
    first, we should get the character's positon in the bwt file. Then, we can know which bucket the position in, and get the character's appearing times in the previous buckets quickly. Then we can count the remain times in the bucket, which the character's position in. Lastly, we sum them up to get the rank.

(5) -n
    using backward search to get the First and Last, and return Last-First+1.

(6) -r, -a
    calculate First and Last first (we can call backward search). Then using LF function to do backward search until we reach the '[', and get the offset between '[' and ']'. 
    Notice: if LF function's result is between and First and Last, it indicates that the start point is duplicated. So we do not need to calculate it, to save the time. 

------------
Design issues and summary
------------
The key poit of the assignment is to improve the efficiency of counting rank/occurrence. Using bucket could calculate the rank more faster when dealing with large data.

