package wxFileList;

=head1 NAME

wxFileList

=head1 SYNOPSIS

  use wxFileList qw(grep_filetype grep_fileflag grep_not_fileflag
                    grep_source grep_header sort_files make_arrays);

  # shorthand for
  # @wxGeneric = sort_files grep_filetype 'Generic', @wxALL;
  # @wxGenericInclude = sort_files grep_filetype 'GenericH', @wxALL;
  make_arrays( 'wxGeneric', 'wxGenericInclude' );

=head1 METHODS

=cut

use strict;

# alias wxALL from main
use vars qw(@wxALL);
*wxALL = \@main::wxALL;

use base 'Exporter';
use vars qw(@EXPORT_OK %EXPORT_TAGS);

@EXPORT_OK = qw(grep_filetype grep_fileflag grep_not_fileflag
                grep_source grep_header sort_files make_arrays);

%EXPORT_TAGS = ( 'default' => [ qw(grep_filetype grep_fileflag grep_source
                                   grep_not_fileflag grep_header
                                   sort_files) ],
                );

my %type_2_array = (
                    Common    => "wxCommon",
                    Generic   => "wxGeneric",
                    GenericH  => "wxGenericInclude",
                    HTML      => "wxHtml",
                    HtmlH     => "wxHtmlInclude",
                    Motif     => "wxMotif",
                    MotifH    => "wxMotifInclude",
                    ProtoH    => "wxProtocolInclude",
                    Unix      => "wxUnix",
                    UnixH     => "wxUnixInclude",
                    WXH       => "wxWxInclude",
                   );
# inverse mapping
my %array_2_type = map { ( $type_2_array{$_}, $_ ) } keys %type_2_array;

sub _sort {
    sort { $a->filename cmp $b->filename } @_;
}

=head2 grep_filetype

  my @files = grep_filetype 'Type', @all_files;

Returns files in C<@all_files> whose file type matches C<'Type'>.

=cut

sub grep_filetype {
    my $filetype = lc( shift );

    return _sort grep { $filetype eq lc( $_->filetype ) } @_;
}

=head2 grep_fileflag

=head2 grep_not_fileflag

  my @files  = grep_fileflag 'NotX', @all_files;
  my @files2 = grep_not_fileflag 'NotX', @all_files;

Return files in C<@all_files> [not] having the given file flag.

=cut

sub grep_fileflag {
    my $fileflag = shift;

    return _sort grep { $_->has_flag( $fileflag ) } @_;
}

sub grep_not_fileflag {
    my $fileflag = shift;

    return _sort grep { !( $_->has_flag( $fileflag ) ) } @_;
}

=head2 grep_header

=head2 grep_source

  my @headers = grep_header @all_files;
  my @sources = grep_source @all_files;

Return header/source files contained in C<@all_files>.

=cut

sub grep_header {
    return _sort grep { $_->is_header } @_;
}

sub grep_source {
    return _sort grep { $_->is_source } @_;
}

=head2 sort_files

  my @sorted_files = sort_files @files;

Sorts files by file name.

=cut

sub sort_files {
    return sort { $a->{filename} cmp $b->{filename} } @_;
}

=head2 make_arrays

See SYNOPSIS.

=cut

sub make_arrays {
    my( $package ) = caller;

    foreach my $array ( @_ ) {
        my $type = $array_2_type{$array};

        unless( $type ) {
            require Carp;
            croak( "Invalid array name '$array'" );
        }

        no strict 'refs';
        @{"${package}::${array}"} = sort_files grep_filetype $type, @wxALL;
    }
}

1;
