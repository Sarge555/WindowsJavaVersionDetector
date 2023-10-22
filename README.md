Useful for batch files or install checks.

This C program will set the cmd errorlevel based on the passed parms.

This program basically runs "Java -version" and works with the results. It is obviously path dependent.

If java is not installed that is reported also.

********************************************************************************

-q (optional) cmd line displays only (No window popups)

-r x.x.x.x minimum required version. Can be 1-4 numbers separated by periods.

-R x.x.x.x Same as above but installed java version must exactly match.

********************************************************************************
Windows only.

CMD file snippet:

 etc\DetectJavaVersion -q -r 17.0
 
 if errorlevel 1 ( explorer https://www.oracle.com/java/technologies/downloads/ && exit)
