use strict;
use warnings;
use autodie;
use Test::More;

use Text::Upmake;
BEGIN { use_ok('Text::Upmake::MSBuild'); }

my $sources = [qw(file1.cpp file2.cpp fileNew.cpp)];
my $headers = [qw(file1.h file2.h fileNew.h)];

open my $out, '>', \my $outstr;
update_msbuild(*DATA, $out, $sources, $headers);

note("Result: $outstr");

like($outstr, qr/file1\.cpp/, 'existing source file was preserved');
like($outstr, qr/fileNew\.cpp/m, 'new source file was added');
unlike($outstr, qr/fileOld\.cpp/, 'old source file was removed');
unlike($outstr, qr/file3\.h/, 'old header was removed');
like($outstr, qr/fileNew\.h/, 'new header was added');

done_testing()

__DATA__
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <ItemGroup>
    <ClCompile Include="file1.cpp" />
    <ClCompile Include="file2.cpp" />
    <ClCompile Include="fileOld.cpp" />
    <ClCompile Include="file3.cpp" >
      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='DLL Debug|Win32'">Create</PrecompiledHeader>
    </ClCompile>
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="file1.h" />
    <ClInclude Include="file2.h" />
    <ClInclude Include="file3.h" />
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
</Project>
