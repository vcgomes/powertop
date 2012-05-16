#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_LANG([C++])
AC_INIT([powertop], [2.0], [power@bughost.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign ])
AC_CONFIG_FILES([Makefile src/Makefile pevent/Makefile po/Makefile.in])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
GETTEXT_PACKAGE=powertop
AC_SUBST(GETTEXT_PACKAGE)
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
# Checks for programs.
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

# Checks for libraries.
# Checks for header files.
AC_CHECK_HEADERS([fcntl.h libintl.h limits.h locale.h malloc.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/statfs.h sys/time.h termios.h unistd.h ncurses.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([fdatasync getpagesize gettimeofday memmove memset mkdir munmap pow realpath regcomp select setlocale socket sqrt strcasecmp strchr strdup strerror strncasecmp strstr strtoul strtoull])

AC_SEARCH_LIBS([delwin], [ncursesw ncurses], [], AC_MSG_ERROR([ncurses is required but was not found]), [])

PKG_CHECK_MODULES([PCIUTILS], [libpci],[],[
	AC_SEARCH_LIBS([pci_get_dev], [pci], [], AC_MSG_ERROR([libpci is required but was not found]), [])
]) 
PKG_CHECK_MODULES([LIBZ], [zlib],[],[
	AC_SEARCH_LIBS([deflate], [z], [], AC_MSG_ERROR([zlib is required but was not found]), [])
]) 

PKG_CHECK_MODULES(LIBNL3, libnl-3.0 libnl-genl-3.0, [
	NETLINK_CFLAGS=${LIBNL3_CFLAGS}
	NETLINK_LIBS=${LIBNL3_LIBS}
], [
	PKG_CHECK_MODULES(LIBNL2, libnl-2.0, [
		NETLINK_CFLAGS=${LIBNL2_CFLAGS}
		NETLINK_LIBS=${LIBNL2_LIBS}
	], [
		PKG_CHECK_MODULES(LIBNL1, libnl-1, dummy=yes,
			AC_MSG_ERROR(Netlink library is required))
		AC_DEFINE(NEED_LIBNL_COMPAT, 1,
			[Define to 1 if you need libnl-1 compat functions.])
		NETLINK_CFLAGS=${LIBNL1_CFLAGS}
		NETLINK_LIBS=${LIBNL1_LIBS}
	])
])
AC_SUBST(NETLINK_CFLAGS)
AC_SUBST(NETLINK_LIBS)

AC_SEARCH_LIBS([pthread_create], [pthread], [], AC_MSG_ERROR([libpthread is required but was not found]), [])
AC_SEARCH_LIBS([inet_aton], [resolv], [], AC_MSG_ERROR([libresolv is required but was not found]), [])

AC_OUTPUT
