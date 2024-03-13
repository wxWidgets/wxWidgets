# Docker container for running autoconf

## Rationale

We store generated `configure` in Git to make it simpler for people to build
wxWidgets without having to run autoconf themselves first, but this means that
generating it on another system often results in insignificant changes just due
to a different version of autoconf being used. By using this container, we can
use the same version everywhere.


## Using Pre-Made Image

To use this container, you can pull it from Docker registry and run it
directly by executing the following command from the top-level wx directory:

```shell
$ podman run -v `pwd`:/wx --rm docker.io/vadz/autoconf-for-wx
```

or, if you prefer to use podman:

```shell
$ docker run -v `pwd`:/wx --rm vadz/autoconf-for-wx
```


## Build

You can also build the container yourself easily using the Dockerfile in this
directory, just run

```shell
$ docker build -t autoconf-for-wx build/tools/autoconf
```

This should (quickly) finish with
```
Successfully tagged autoconf-for-wx:latest
```


## Updating autoconf version

When we decide to switch to a newer autoconf version, we should update the
`Dockerfile` here to use a later version of Alpine Linux: the currently used
version is specifically chosen because it has the package with the latest
version of autoconf 2.69.
