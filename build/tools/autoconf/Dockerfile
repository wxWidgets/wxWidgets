FROM alpine:3.13

RUN apk add autoconf

# wxWidgets sources must be mounted here.
VOLUME /wx

WORKDIR /wx

CMD ["autoconf"]
