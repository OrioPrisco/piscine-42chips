##Description

My code from doing the 42chips piscine  

##Dependencies  
avr-gcc  
make  
avrdude  
avr-objcopy  

##Compiling
#day00 to day03
go to the folder and do `make`
#day04 to day09 and rush01
to compile do `make MAIN=PROGRAM_TO_COMPILE`
ex : `make MAIN=rush01/rush01.c`

why ? the earlier days use almost no library code. Even in day04, some exercice are pretty much empty as the goal of the day was to write the library code.  
you can still see the original code by checking out at the second commit (first one is not 100% up to date, but mostly).
