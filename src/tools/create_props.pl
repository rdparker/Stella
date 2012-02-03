#!/usr/bin/perl

use PropSet;

usage() if @ARGV != 2;

my %propset = PropSet::load_prop_set($ARGV[0]);
my $setsize = keys (%propset);
my $typesize = PropSet::num_prop_types();

printf "Valid properties found: $setsize\n";

# Construct the output file in C++ format
open(OUTFILE, ">$ARGV[1]");
print OUTFILE "//============================================================================\n";
print OUTFILE "//\n";
print OUTFILE "//   SSSS    tt          lll  lll\n";
print OUTFILE "//  SS  SS   tt           ll   ll\n";
print OUTFILE "//  SS     tttttt  eeee   ll   ll   aaaa\n";
print OUTFILE "//   SSSS    tt   ee  ee  ll   ll      aa\n";
print OUTFILE "//      SS   tt   eeeeee  ll   ll   aaaaa  --  \"An Atari 2600 VCS Emulator\"\n";
print OUTFILE "//  SS  SS   tt   ee      ll   ll  aa  aa\n";
print OUTFILE "//   SSSS     ttt  eeeee llll llll  aaaaa\n";
print OUTFILE "//\n";
print OUTFILE "// Copyright (c) 1995-2012 by Bradford W. Mott, Stephen Anthony\n";
print OUTFILE "// and the Stella Team\n";
print OUTFILE "//\n";
print OUTFILE "// See the file \"License.txt\" for information on usage and redistribution of\n";
print OUTFILE "// this file, and for a DISCLAIMER OF ALL WARRANTIES.\n";
print OUTFILE "//\n";
print OUTFILE "// \$Id\$\n";
print OUTFILE "//============================================================================\n";
print OUTFILE "\n";
print OUTFILE "#ifndef DEF_PROPS_HXX\n";
print OUTFILE "#define DEF_PROPS_HXX\n";
print OUTFILE "\n";
print OUTFILE "/**\n";
print OUTFILE "  This code is generated using the 'create_props.pl' script,\n";
print OUTFILE "  located in the src/tools directory.  All properties changes\n";
print OUTFILE "  should be made in stella.pro, and then this file should be\n";
print OUTFILE "  regenerated and the application recompiled.\n";
print OUTFILE "*/\n";
print OUTFILE "\n#define DEF_PROPS_SIZE " . $setsize;
print OUTFILE "\n\n";
print OUTFILE "static const char* DefProps[DEF_PROPS_SIZE][" . $typesize . "] = {\n";

# Walk the hash map and print each item in order of md5sum
my $idx = 0;
for my $key ( sort keys %propset )
{
	print OUTFILE PropSet::build_prop_string(@{ $propset{$key} });

	if ($idx+1 < $setsize) {
		print OUTFILE ", ";
	}
	print OUTFILE "\n";
	$idx++;
}

print OUTFILE "};\n";
print OUTFILE "\n";
print OUTFILE "#endif\n";

close(OUTFILE);


sub usage {
	print "create_props.pl <INPUT STELLA PROPS> <OUTPUT C++ header>\n";
	exit(0);
}
