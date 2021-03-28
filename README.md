The only major caveat with my code is that when you run the software with arguments such that all data should be 
imported, at the end of the output the program will complain that a vector has reached maximum size. I am not sure why 
this is happening or if it can be fixed. 

For some reason, my code fails test 12 on CS Autograder, but it passes on my laptop. Moreover, 
tests for outputs 4,5,6 and 7 also fail on Autograder, but when ran on my laptop the output is the correct one, and I 
have used a string comparison web site to conform my program gives the correct output. For some reason, on Autograder 
the code puts the authority code instead of the measure code, when on my laptop it works correctly.

The following is a list of pieces of code which I think you should pay attention to, as I believe the solution 
I found is clever and would give me more marks:

- The code in bethyw.cpp for parsing command line arguments, in the methods you specified and the private methods I added.
  
- The code in areas.cpp in the populateFromWelshStatsJSON and populateFromAuthorityByYearCSV methods and the private 
  static methods I added which are used in those two methods.
    
- The way I implemented object to json conversion, using the to_json method in areas.cpp, area.cpp and measure.cpp .

- The way I implemented the << operators in areas.cpp, area.cpp and measure.cpp, including the private static methods 
  used for that in measure.cpp .
  
- The way I implemented the "insert or merge" behaviour you described for setArea and setMeasure, using the overloaded 
  copy assigment operators in the Area and Measure classes.