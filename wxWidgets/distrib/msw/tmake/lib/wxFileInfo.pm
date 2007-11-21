package wxFileInfo;

=head1 NAME

wxFileInfo

=head1 SYNOPSIS

  use wxFileInfo;

  my $info = new wxFileInfo( $filename, $filetype, $fileflags );
  my $info2 = new wxFileInfo( 'mdig.cpp', 'Generic',
                              'NotWin32,NotGTK,NotMac' );

  $f = $info->filename;
  $t = $info->filetype;
  $flags = $info->fileflags;
  $bool = $info->is_header;
  $bool = $info->is_source;
  $file = $info->object_file;
  $file = $info->source_file;
  $bool = $info->has_flag( 'NotX' );

=cut

use strict;

sub new {
    my $ref = shift;
    my $class = ref( $ref ) || $ref;
    my $self = bless {}, $class;

    my( $filename, $filetype, $fileflags ) = @_;
    $fileflags =~ tr/ \t//d;

    @{$self}{'filename', 'filetype'} = ( $filename, $filetype );
    $self->{fileflags} = [ split /,/, $fileflags ];

    return $self;
}

sub filename { $_[0]->{filename} }
sub filetype { $_[0]->{filetype} }
sub fileflags { $_[0]->{fileflags} }
sub is_header { scalar( $_[0]->{filename} =~ m/\.h$/i ) }
sub is_source { !scalar( $_[0]->{filename} =~ m/\.h$/i ) }

sub object_file {
    my $self = shift;
    my $obj = $self->{filename};

    $obj =~ s/cp?p?$/o/i; # PORTABILITY

    return $obj;
}

my %src_prefix = ( Common  => 'common/',
                   Generic => 'generic/',
                   MSW     => 'msw/',
                   Mac     => 'mac/',
                   Motif   => 'motif/',
                   GTK     => 'gtk/',
                   Univ    => 'univ/',
                   X11     => 'x11/',
                   HTML    => 'html/',
                   Unix    => 'unix/',
                   WXH     => '',
                   ProtoH  => 'protocol/',
                   HtmlH   => 'html/',
                   MotifH  => 'motif/',
                   X11H    => 'x11/',
                   GenericH => 'generic/',
                   UnixH   => 'unix/',
                 );

sub source_file {
    my $self = shift;
    my $type = $self->filetype;

    die "Unknown file type '$type'" unless exists $src_prefix{$type};
    return $src_prefix{$type} . $self->filename; # PORTABILITY
}

sub has_flag {
    my( $self, $flag ) = @_;
    $flag = lc( $flag );
    return grep { lc( $_ ) eq $flag } @{$self->{fileflags}};
}

1;
