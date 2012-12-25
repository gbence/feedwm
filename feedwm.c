#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

char *program_name;
Display *dpy;
int screen;
Window root;

static void
usage(void)
{
    fprintf(stderr, "usage: %s [options]\n", program_name);
    fprintf(stderr, "  where options are:\n");
    fprintf(stderr, "  --display <display>  or  -d <display>\n");
    exit(1);
}

int
main(int argc, char**argv)
{
  char *display_name = NULL;
  time_t *now;
  char *name;
  struct tm *tm_now;

  int i;

  program_name=argv[0];

  for (i = 1; i < argc; i++) {
    if (!strcmp ("--display", argv[i]) || !strcmp ("-d", argv[i])) {
      if (++i>=argc) usage();
      display_name = argv[i];
      continue;
    }
    usage();
  }

  dpy = XOpenDisplay(display_name);
  if (!dpy) {
    fprintf(stderr, "%s:  unable to open display '%s'\n",
        program_name, XDisplayName(display_name));
    exit (2);
  }
  screen = DefaultScreen(dpy);
  root = RootWindow(dpy, screen);
  now = malloc(sizeof(time_t));
  name = malloc(sizeof(char)*100);

  for(;;)
  {

    time(now);
    tm_now = localtime(now);
    sprintf(name, "%02d-%02d %02d:%02d:%02d", tm_now->tm_mon, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    XStoreName(dpy, root, name);
    XFetchName(dpy, root, &name);

    usleep(100000);
  }

  free(now);
  XCloseDisplay(dpy);
  return 0;
}
