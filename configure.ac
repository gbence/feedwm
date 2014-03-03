AC_PREREQ([2.67])
AC_INIT([feedwm], [0.9.1], [b@blorb.io], [], [https://github.com/gbence/feedwm])
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
AC_CHECK_TOOL([STRIP], [strip])

#---------------------------------------------------------------------
# Check for GNU make 3.80 or above
#---------------------------------------------------------------------
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

#---------------------------------------------------------------------
# Check for C env and required basic functions
#---------------------------------------------------------------------
AC_C_INLINE
AC_HEADER_STDC
AC_FUNC_MALLOC
AC_CHECK_HEADERS([limits.h signal.h time.h stdio.h stdlib.h unistd.h])
AC_CHECK_FUNCS([access realpath time localtime sigaction usleep])

#---------------------------------------------------------------------
# Check for required X11 functions
#---------------------------------------------------------------------
AC_CHECK_LIB([X11], [XStoreName])

#---------------------------------------------------------------------
# Check for ruby
# TODO it is fetched from somewhere, and requires a huge cleanup!
#---------------------------------------------------------------------

AC_CHECK_PROGS(RUBY, ruby)

AC_ARG_WITH(ruby-arch-install-dir-variable, [ --with-ruby-arch-install-dir-variable=NAME Set Ruby arch install directory config variable (default 'archdir')], ruby_arch_install_dir_variable="$withval", ruby_arch_install_dir_variable="archdir")

dnl Setting this overrides ruby-arch-install-dir-variable above
AC_ARG_WITH(ruby-arch-install-dir, [ --with-ruby-arch-install-dir=NAME Set Ruby install directory (default 'auto')], ruby_arch_install_dir="$withval", ruby_arch_install_dir="auto")

AC_ARG_WITH(ruby-install-dir-variable, [ --with-ruby-install-dir-variable=NAME Set Ruby install directory config variable (default 'rubylibdir')], ruby_install_dir_variable="$withval", ruby_install_dir_variable="rubylibdir")

dnl Setting this overrides ruby-install-dir-variable above
AC_ARG_WITH(ruby-install-dir, [ --with-ruby-install-dir=NAME Set Ruby install directory (default 'auto')], ruby_install_dir="$withval", ruby_install_dir="auto")

RUBY_CFLAGS=
RUBY_LDFLAGS=
RUBY_INCLUDES=
RUBY_SO_SUFFIX=
RUBY_INSTALLDIR=
RUBY_ARCH_INSTALLDIR=
if test "X$RUBY" != X; then
  dnl need to change quotes to allow square brackets
  changequote(<<, >>)dnl
  RUBY_ARCHDIR=`$RUBY -rrbconfig -e "print RbConfig::CONFIG['archdir']"`
  ruby_install_dir_config=`$RUBY -rrbconfig -e "puts RbConfig::CONFIG['$ruby_install_dir_variable']"`
  ruby_header_dir_config=`$RUBY -rrbconfig -e "puts RbConfig::CONFIG['rubyhdrdir']"`
  ruby_arch_config=`$RUBY -rrbconfig -e "puts RbConfig::CONFIG['arch']"`
  ruby_arch_install_dir_config=`$RUBY -rrbconfig -e "puts RbConfig::CONFIG['$ruby_arch_install_dir_variable']"`
  librubyarg_shared=`$RUBY -rrbconfig -e "puts RbConfig::CONFIG['LIBRUBYARG_SHARED']"`
  ruby_ccdlflags=`$RUBY -rrbconfig -e "puts RbConfig::CONFIG['CCDLFLAGS']"`
  changequote([, ])dnl

  AC_MSG_CHECKING(Ruby installation dir)
  if test "X$ruby_install_dir" != "Xauto"; then
    RUBY_INSTALLDIR=$ruby_install_dir
    AC_MSG_RESULT($RUBY_INSTALLDIR from user)
  else
    RUBY_INSTALLDIR=$ruby_install_dir_config
    AC_MSG_RESULT($RUBY_INSTALLDIR from RbConfig::CONFIG $ruby_install_dir_variable)
  fi

  AC_MSG_CHECKING(Ruby arch installation dir)
  if test "X$ruby_arch_install_dir" != "Xauto"; then
    RUBY_ARCH_INSTALLDIR=$ruby_arch_install_dir
    AC_MSG_RESULT($RUBY_ARCH_INSTALLDIR from user)
  else
    RUBY_ARCH_INSTALLDIR=$ruby_arch_install_dir_config
    AC_MSG_RESULT($RUBY_ARCH_INSTALLDIR from RbConfig::CONFIG $ruby_arch_install_dir_variable)
  fi

  AC_MSG_CHECKING(Ruby headers dir)
  if test "X$ruby_header_dir_config" != "Xnil" -a "X$ruby_header_dir_config" != "X"; then
    RUBY_INCLUDES="$RUBY_INCLUDES -I$ruby_header_dir_config"
    if test "X$ruby_arch_config" != "Xnil" -a "X$ruby_arch_config" != "X"; then
      ruby_arch_hdrdir="$ruby_header_dir_config/$ruby_arch_config"
      if test -d "$ruby_arch_hdrdir"; then
        RUBY_INCLUDES="$RUBY_INCLUDES -I$ruby_arch_hdrdir"
      fi
    fi
    AC_MSG_RESULT($RUBY_INCLUDES from RbConfig::CONFIG rubyhdrdir)
  else
    if test -r "$RUBY_ARCHDIR/ruby.h"; then
      RUBY_INCLUDES="$RUBY_INCLUDES -I$RUBY_ARCHDIR"
      AC_MSG_RESULT($RUBY_INCLUDES from arch dir)
    else
      AC_MSG_RESULT(ruby.h not found - unable to compile Redland ruby bindings)
    fi
  fi

  ## OSX/Darwin default is bundle
  #if test `uname` = Darwin; then
  #  # FIXME - how to decide between bundle and dylib?
  #  ruby_linking=bundle
  #else
  #  ruby_linking=so
  #fi
  #AC_ARG_WITH(ruby-linking, [ --with-ruby-linking=TYPE Set Ruby module linking type (so, dylib, bundle)], ruby_linking="$withval")
  #case $ruby_linking in
  #  so)
  #    RUBY_LDFLAGS="$RUBY_LDFLAGS -shared $librubyarg_shared"
  #    RUBY_CFLAGS="$RUBY_CFLAGS $ruby_ccdlflags"
  #    ;;
  #  dylib)
  #    RUBY_LDFLAGS="$RUBY_LDFLAGS -flat_namespace -undefined suppress $librubyarg_shared"
  #    RUBY_CFLAGS="$RUBY_CFLAGS -fno-common"
  #    ;;
  #  bundle)
  #    RUBY_LDFLAGS="$RUBY_LDFLAGS -bundle -flat_namespace -undefined suppress $librubyarg_shared"
  #    RUBY_CFLAGS="$RUBY_CFLAGS -fno-common"
  #    ;;
  #  *)
  #    AC_MSG_ERROR(No such RUBY linking type $ruby_linking)
  #    ;;
  #esac

  RUBY_LDFLAGS="$RUBY_LDFLAGS $librubyarg_shared"
  RUBY_CFLGAS="$RUBY_CFLAGS $ruby_ccdlflags"

  AC_MSG_CHECKING(Ruby Linking)
  AC_MSG_RESULT(compile '$RUBY_CFLAGS' link '$RUBY_LDFLAGS')
else
  # This allows 'make clean' in the ruby directory to work when
  # ruby isn't available
  RUBY=:
fi
AC_SUBST(RUBY_CFLAGS)
AC_SUBST(RUBY_LDFLAGS)
AC_SUBST(RUBY_INCLUDES)
AC_SUBST(RUBY_INSTALLDIR)
AC_SUBST(RUBY_ARCH_INSTALLDIR)


#---------------------------------------------------------------------
# Process output
#---------------------------------------------------------------------
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

