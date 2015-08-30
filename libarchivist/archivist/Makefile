.include <bsd.own.mk>
INCS=	 locator.h record.h hash.h list.h cache.h uuid.h table.h
INCSDIR= ${INCLUDEDIR}/archivist

beforeinstall :
	${INSTALL} -d -o ${BINOWN} -g ${BINGRP} -m ${BINMODE} ${INCSDIR}

.include <bsd.lib.mk>
