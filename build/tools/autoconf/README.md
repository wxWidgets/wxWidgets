# Docker container for running autoconf

## Rationale

We store generated `configure` in Git to make it simpler for people to build
wxWidgets without having to run autoconf themselves first, but this means that
generating it on another system often results in insignificant changes just due
to a different version of autoconf being used. By using this container, we can
use the same version everywhere.

To use it, first build the container, then run it, see below.


## Build

Just run

```shell
$ docker build -t autoconf-for-wx build/tools/autoconf
```

This should (quickly) finish with
```
Successfully tagged autoconf-for-wx:latest
```


## Run

Use the following command to update `configure` from `configure.in`:

```shell
$ docker run -v `pwd`:/wx --rm autoconf-for-wx
```

It should be run from the top-level wx directory.


## Updating autoconf version

When we decide to switch to a newer autoconf version, we should update the
`Dockerfile` here to use a later version of Alpine Linux: the currently used
version is specifically chosen because it has the package with the latest
version of autoconf 2.69.
