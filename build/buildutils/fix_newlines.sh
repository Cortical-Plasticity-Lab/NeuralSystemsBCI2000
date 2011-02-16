#!/bin/sh
g++ build/buildutils/fix_newlines.cpp -o build/buildutils/fix_newlines

for i in `find . -iname "*.cpp"` `find . -iname "*.h"`;
  do echo "$i";
  mv "$i" "$i.bak" && ( build/buildutils/fix_newlines --style-PC --tab-4 < "$i.bak" > "$i" )
done;

for i in `find . -iname "*.m"`;
  do echo "$i";
  mv "$i" "$i.bak" && ( build/buildutils/fix_newlines --style-PC < "$i.bak" > "$i" )
done;

for i in `find . -iname "Makefile"`;
  do echo "$i";
  mv "$i" "$i.bak" && ( build/buildutils/fix_newlines --style-PC < "$i.bak" > "$i" )
done;

for i in `find . -iname "*.cmd"` `find . -iname "*.bat"`;
  do echo "$i";
  mv "$i" "$i.bak" && ( build/buildutils/fix_newlines --style-PC < "$i.bak" > "$i" )
done;
