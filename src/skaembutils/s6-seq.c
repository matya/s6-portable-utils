/* ISC license. */

#include <skalibs/uint.h>
#include <skalibs/sgetopt.h>
#include <skalibs/buffer.h>
#include <skalibs/strerr2.h>

#define USAGE "s6-seq [ -w ] [ -s separator ] limits"
#define dieusage() strerr_dieusage(100, USAGE)

int main (int argc, char const *const *argv)
{
  char const *sep = "\n" ;
  unsigned int fixed = 0, seplen = 1, i = 1, increment = 1 ;
  unsigned int last ;
  char fmt[UINT_FMT] ;
  PROG = "s6-seq" ;
  {
    subgetopt_t l = SUBGETOPT_ZERO ;
    for (;;)
    {
      register int opt = subgetopt_r(argc, argv, "ws:", &l) ;
      if (opt == -1) break ;
      switch (opt)
      {
        case 'w': fixed = 1 ; break ;
        case 's': sep = l.arg ; seplen = str_len(sep) ; break ;
        default : dieusage() ;
      }
    }
    argc -= l.ind ; argv += l.ind ;
  }
  switch (argc)
  {
    case 1 :
      if (!uint0_scan(argv[0], &last)) dieusage() ;
      break ;
    case 2 :
      if (!uint0_scan(argv[0], &i)
       || !uint0_scan(argv[1], &last)) dieusage() ;
      break ;
    case 3 :
      if (!uint0_scan(argv[0], &i)
       || !uint0_scan(argv[1], &increment)
       || !uint0_scan(argv[2], &last)) dieusage() ;
      break ;
    default : dieusage() ;
  }
  if (!seplen) seplen = 1 ;
  if (fixed) fixed = uint_fmt(0, i + increment * ((last - i) / increment)) ;
  for (; i <= last ; i += increment)
  {
    if (buffer_put(buffer_1, fmt, fixed ? (uint0_fmt(fmt, i, fixed), fixed) : uint_fmt(fmt, i)) < 0) goto err ;
    if (buffer_put(buffer_1, sep, seplen) < 0) goto err ;
  }
  if (!buffer_flush(buffer_1)) goto err ;
  return 0 ;

err:
  strerr_diefu1sys(111, "write to stdout") ;
}
