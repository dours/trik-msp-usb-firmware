print "// Generated by symbols.ld.to.addresses.h.pl from $ARGV[0]\n\n\n";

while (<>) { 
  if (/PROVIDE\((\w+)\s+=\s+([\da-fA-FXx]+)\);/) {
    print "#define $1 ($2)\n"
  } else { print }
}
