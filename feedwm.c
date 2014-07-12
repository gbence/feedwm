/*
 * feedwm.c
 *
 * Released under MIT licence, copyright (c) 2014, GOLDA Bence <b@blorb.io>
 *
 * TODO:
 * - restructure code
 * - error handling during require()s
 * - TOPLEVEL#feedwm check before calling
 * - proper cleanup after CTRL-C-s while running Ruby code (that goes almost straight into handle/0)
 * - code reloading
 */
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <ruby/ruby.h>

#define DEFAULT_INTERVAL 990000
#define MAX_TITLE 1000

char *program_name;
Display *dpy;
int screen;
Window root;
int running = 1;

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define ERROR_MSG(message, ...) fprintf(stderr, "feedwm error: "message"\n", ##__VA_ARGS__)

static void
usage(void)
{
  fprintf(stderr, "Usage: %s [options]\n", program_name);
  fprintf(stderr, "  -d <display>, --display <display>\n               # sets display (eg. :0 or localhost:0.0)\n");
  fprintf(stderr, "  -h, --help   # shows this help\n");
  fprintf(stderr, "  -i <milliseconds>, --interval <milliseconds>\n               # sets interval, time between samplings\n");
  fprintf(stderr, "  -l <ruby-program>, --load <ruby-program>\n               # sets loaded ruby program (must export a TOPLEVEL_BINDING#feedwm method!)\n");
  fprintf(stderr, "  --version    # shows version\n");
  exit(1);
}

static void
version(void)
{
  fprintf(stderr, "feedwm version " STR(VERSION) "\n(c) 2014, GOLDA Bence <b@blorb.io>\n");
  exit(0);
}

static void
handle(int signum)
{
  fprintf(stderr, "Received SIGTERM, exiting...\n");
  running = 0;
}

static VALUE
callback(VALUE args)
{
  VALUE rb_topLevel     = rb_const_get(rb_cObject, rb_intern("TOPLEVEL_BINDING"));
  VALUE rb_resultObject = rb_funcall(rb_topLevel, rb_intern("feedwm"), 0);
  VALUE rb_resultString = rb_funcall(rb_resultObject, rb_intern("to_s"), 0);
  return(rb_resultString);
}

static VALUE
rescue(VALUE args, VALUE exception_object)
{
  char message[MAX_TITLE+1];
  const char *exception_message = RSTRING_PTR(rb_funcall(exception_object, rb_intern("message"), 0));
  const char *exception_class   = rb_obj_classname(exception_object);
  VALUE rb_resultString;

  snprintf((char*)&message, MAX_TITLE, "%s! (%s)", exception_message, exception_class);
  rb_resultString = rb_str_new2((char*)&message);

  return(rb_resultString);
}

int
main(int argc, char**argv)
{
  char *display_name = NULL;
  time_t *now;
  char *title;
  struct tm *tm_now;

  long int interval = DEFAULT_INTERVAL;

  char *relative_ruby_program_name = NULL;
  char *ruby_program_name          = NULL;

  struct sigaction action;
  int i;

  VALUE result;

  program_name=argv[0];

  for (i = 1; i < argc; i++) {
    if (!strcmp ("--display", argv[i]) || !strcmp ("-d", argv[i])) {
      if (++i>=argc) usage();
      display_name = argv[i];
      continue;
    }
    if (!strcmp ("--help", argv[i]) || !strcmp ("-h", argv[i])) {
      usage();
    }
    if (!strcmp ("--interval", argv[i]) || !strcmp ("-i", argv[i])) {
      if (++i>=argc) usage();
      interval = strtol(argv[i], (char **)NULL, 10);
      if (interval == LONG_MAX && errno == ERANGE) usage();
      if (interval == 0) usage();
      if (interval < 100)
        fprintf(stderr, "You should set an interval longer than 100ms (it is %ldms now).\nIt will eat your machine! :)\n", interval);
      interval *= 1000; // it is in microseconds...
      continue;
    }
    if (!strcmp ("--load", argv[i]) || !strcmp ("-l", argv[i])) {
      if (++i>=argc) usage();
      relative_ruby_program_name = argv[i];
      continue;
    }
    if (!strcmp ("--version", argv[i]) || !strcmp ("-v", argv[i])) {
      version();
    }
    usage();
  }

  if (relative_ruby_program_name != NULL) {
    ruby_program_name = (char *) malloc(PATH_MAX * sizeof(char));

    if (realpath(relative_ruby_program_name, ruby_program_name) == NULL) {
      ERROR_MSG("%s! `%s'", strerror(errno), relative_ruby_program_name);
      exit(1);
    }

    if (access( ruby_program_name, F_OK ) == -1) {
      ERROR_MSG("file does not exists! `%s'", ruby_program_name);
      exit(1);
    }
  }


  dpy = XOpenDisplay(display_name);
  if (!dpy) {
    ERROR_MSG("%s: unable to open display `%s'",
        program_name, XDisplayName(display_name));
    exit (2);
  }
  screen = DefaultScreen(dpy);
  root = RootWindow(dpy, screen);
  now = malloc(sizeof(time_t));

  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = handle;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGINT, &action, NULL);

  // Load ruby program if given.
  if (ruby_program_name)
  {
    ruby_init();
    ruby_init_loadpath();
    rb_require(ruby_program_name);
  }

  while (running)
  {
    // reserve title
    title = malloc(sizeof(char)*(MAX_TITLE+1));

    if (ruby_program_name)
    {
      result = rb_rescue(callback, Qnil, rescue, Qnil);
      snprintf(title, MAX_TITLE, "%s", RSTRING_PTR(result));
    }
    else
    {
      time(now);
      tm_now = localtime(now);
      snprintf(title, MAX_TITLE, " %02d-%02d %02d:%02d:%02d", tm_now->tm_mon+1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    }

    // drawing name
    XStoreName(dpy, root, title);

    // release title
    free(title);

    // we'll use title for other purposes too, and then release
    XFetchName(dpy, root, &title);
    XFree(title);

    usleep(interval);
  }

  if (ruby_program_name) {
    ruby_finalize();
    free(ruby_program_name);
  }

  free(now);
  XCloseDisplay(dpy);
  return 0;
}
