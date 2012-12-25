AC_PREREQ([2.67])
AC_INIT([feedwm], [0.9], [golda@bitandpixel.hu])
AC_CONFIG_AUX_DIR([vendor/scripts])

AC_PATH_PROGS([RM], [rm])
AS_IF([test -z "$RM"], [AC_MSG_ERROR([missing required tool: "rm"])])

# Check for required tool
AC_DEFUN(
    [ACX_CHECK_TOOL_REQ],
    [AC_CHECK_TOOLS([$1], [$2])
     AS_IF(
        [test -z "$$1"],
        [AC_MSG_ERROR([missing required tool: $2])])
    ])

# Check for required tool, set variable to full pathname
AC_DEFUN(
    [ACX_PATH_TOOL_REQ],
    [ACX_CHECK_TOOL_REQ([$1], [$2])
     AS_CASE(
        [$$1],
        [/*],,
        [?*],[AC_MSG_CHECKING([for absolute path to $$1])
              $1=$(which $$1)
              AC_MSG_RESULT([$$1])])])

# Check for required program
AC_DEFUN(
    [ACX_CHECK_PROGS_REQ],
    [AC_CHECK_PROGS([$1], [$2])
     AS_IF(
        [test -z "$$1"],
        [AC_MSG_ERROR([missing required tool: $2])])
    ])

#--------------------------------------------------------------------
# Allow dummy --{en,dis}able-{static,shared}
AC_ARG_ENABLE(
    [local],
    [AS_HELP_STRING(
        [--enable-local],
        ["don't install, and use current directory"])])
AC_SUBST([enable_local], [${enable_local:-no}])
AC_ARG_ENABLE(
    [shared],
    [AS_HELP_STRING(
        [--enable-shared],
        [build shared libraries (default=yes) (ignored)])])
AC_ARG_ENABLE(
    [static],
    [AS_HELP_STRING(
        [--enable-static],
        [build static libraries (default=yes) (ignored)])])

#---------------------------------------------------------------------
# Check for --build and --host...
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
# ... but refuse --target
AS_IF([test -n "$target_alias"], AC_MSG_ERROR([--target is not allowed]))

# Allow program name transformation (--program-{prefix,suffix,transform-name})
AC_ARG_PROGRAM

#---------------------------------------------------------------------
# Initial checks that are usually done first (I don't know why, that's
# just what I seem to experience...)
#---------------------------------------------------------------------
AC_ARG_WITH([install],
    AS_HELP_STRING([--with-install=PATH],
                   [Specify the full PATH to a BSD-compatible install]),
    [INSTALL=$withval])
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_CC
AS_IF([test -z "$CC"], [AC_MSG_ERROR([no suitable compiler found])])
AC_PROG_CPP
AC_PROG_EGREP

#----------------------------------------
# Check for GNU make 3.80 or above
AC_CACHE_VAL([ac_cv_path_MAKE],
    [AC_ARG_WITH([make],
        AS_HELP_STRING([--with-make=PATH],
                       [Specify the full PATH to GNU make >= 3.80]),
        [ac_cv_path_MAKE=$withval])])
AC_CACHE_CHECK([for GNU make >= 3.80], [ac_cv_path_MAKE],
    [AC_PATH_PROGS_FEATURE_CHECK([MAKE], [make gmake],
        [[MAKE_ver=$($ac_path_MAKE --version 2>&1 \
                     |$EGREP '^GNU Make (3.[89][[:digit:]]|[4-9])')
          test -n "$MAKE_ver" && ac_cv_path_MAKE=$ac_path_MAKE ac_path_MAKE_found=:]],
        [AC_MSG_RESULT([no])
         AC_MSG_ERROR([could not find GNU make >= 3.80])])])
AC_SUBST([MAKE], [$ac_cv_path_MAKE])
AC_PROG_MAKE_SET

AC_C_INLINE
AC_HEADER_STDC
AC_FUNC_MALLOC
AC_CHECK_FUNCS([time localtime usleep])
AC_CHECK_LIB([X11], [XStoreName])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

