use strict;
use warnings;
use autodie;
use Test::More;

BEGIN { use_ok('Text::Upmake'); }

my $vars = read_files_list(*DATA);
is_deeply($vars->{VAR1}, [qw(file1 file2)], 'VAR1 has expected value');
is_deeply($vars->{VAR2}, [qw(file3 file4)], 'VAR2 has expected value');

done_testing()

__DATA__
# Some comments

VAR1 =
    file1
    # comment between the files
    file2
VAR2 =
    file3
    file4 # comment
    # another comment
