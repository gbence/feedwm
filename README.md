# feedwm

feedwm is a dwm status bar application, very similar to
[dwmstatus](http://dwm.suckless.org/dwmstatus/).  Its primary purpose is to be
a bridge between dwm's xsetroot-based status bar and my elderly evolving Ruby
script, hence the Ruby bindings.

## Installation

Clone, autoconf, configure, make, install, run.

Check the [`examples`](https://github.com/gbence/feedwm/tree/master/examples)
directory for loadable Ruby scripts.

## Usage

There is a help:

```
$ ./feedwm -h
Usage: ./feedwm [options]
  -d <display>, --display <display>
               # sets display (eg. :0 or localhost:0.0)
  -h, --help   # shows this help
  -i <milliseconds>, --interval <milliseconds>
               # sets interval, time between samplings
  -l <ruby-program>, --load <ruby-program>
               # sets loaded ruby program (must export a TOPLEVEL_BINDING#feedwm method!)
  --version    # shows version
```

## Contribution

Contributions are mostly welcome (in the following way):

* Fork this repository on github,
* Make your changes,
* Send me a pull request,
* If I like them I'll merge them.

If you find a bug or any inconsistencies please do not hesitate to file a
[new](https://github.com/gbence/feedwm/issues/new) ticket.

## License

Copyright (c) 2014, GOLDA Bence <b@blorb.io>

feedwm is released under the [MIT License](https://github.com/gbence/feedwm/blob/master/LICENSE.txt).
