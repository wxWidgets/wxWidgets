use strict;
use warnings;
use autodie;
use Test::More;

use Text::Upmake;
BEGIN { use_ok('Text::Upmake::MSBuild'); }

my $sources = [qw(file1.cpp file2.cpp file4.cpp fileNew.cpp)];
my $headers = [qw(file1.h file2.h fileNew.h)];

sub filter_cb
{
    my ($file) = @_;

    return 'New Sources' if $file =~ /New\./;

    undef
}

open my $out, '>', \my $outstr;
update_msbuild_filters(*DATA, $out, $sources, $headers, \&filter_cb);

note("Result: $outstr");

like($outstr, qr/file1\.cpp/, 'existing source file was preserved');
like($outstr, qr/fileNew\.cpp/m, 'new source file was added');
unlike($outstr, qr/fileOld\.cpp/, 'old source file was removed');
unlike($outstr, qr/file3\.cpp/, 'another old source file was removed');
unlike($outstr, qr/file3\.h/, 'old header was removed');
like($outstr, qr/fileNew\.h/, 'new header was added');

done_testing()

__DATA__
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Common Sources">
      <UniqueIdentifier>{...}</UniqueIdentifier>
    </Filter>
    <Filter Include="Common Headers">
      <UniqueIdentifier>{...}</UniqueIdentifier>
    </Filter>
  </ItemGroup>
  <ItemGroup>
    <ClCompile Include="fileOld.cpp">
      <Filter>Common Sources</Filter>
    </ClCompile>
    <ClCompile Include="file1.cpp">
      <Filter>Common Sources</Filter>
    </ClCompile>
    <ClCompile Include="file2.cpp" />
    <ClCompile Include="file3.cpp" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="file1.h">
      <Filter>Common Headers</Filter>
    </ClInclude>
    <ClInclude Include="file2.h">
      <Filter>Common Headers</Filter>
    </ClInclude>
    <ClInclude Include="file3.h">
      <Filter>Common Headers</Filter>
    </ClInclude>
  </ItemGroup>
</Project>
