# FileSort

Function / class that sorts lines in files

## Discription

This is my first project using cmake to create and build a programm on multiple platforms (Linux, tested on windows wsl) and Windows. For Windows I used the WinApi to create and read from Files and for Linux I used the respective Linux Api.

The goal was to create a simple programm that sorts lines in a Text File. The challenge was that these text files could have any size and therefore you couldn't load the whole file into memory and then sort it and replace the existing file. To Solve this I created smaller so segments that I later merged together again to create the completed and sorted file.
