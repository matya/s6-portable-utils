/* ISC license. */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <skalibs/uint.h>
#include <skalibs/bytestr.h>
#include <skalibs/sgetopt.h>
#include <skalibs/strerr2.h>
#include <skalibs/djbunix.h>

#define USAGE "s6-dumpenv [ -m mode ] envdir"
#define dieusage() strerr_dieusage(100, USAGE)

int main (int argc, char const *const *argv, char const *const *envp)
{
  mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH ;
  unsigned int dirlen ;

  PROG = "s6-printenv" ;
  {
    subgetopt_t l = SUBGETOPT_ZERO ;
    for (;;)
    {
      register int opt = subgetopt_r(argc, argv, "m:", &l) ;
      if (opt == -1) break ;
      switch (opt)
      {
        case 'm' : if (!uint0_oscan(l.arg, &mode)) dieusage() ; break ;
        default : dieusage() ;
      }
    }
    argc -= l.ind ; argv += l.ind ;
  }
  if (!argc) dieusage() ;

  if (mkdir(argv[0], mode) < 0)
  {
    struct stat st ;
    if (errno != EEXIST) strerr_diefu2sys(111, "mkdir ", argv[0]) ;
    if (stat(argv[0], &st) < 0)
      strerr_diefu2sys(111, "stat ", argv[0]) ;
    if (!S_ISDIR(st.st_mode))
    {
      errno = ENOTDIR ;
      strerr_diefu2sys(111, "mkdir ", argv[0]) ;
    }
  }
  dirlen = str_len(argv[0]) ;
    
  for (; *envp ; envp++)
  {
    unsigned int varlen = str_chr(*envp, '=') ;
    char fn[dirlen + varlen + 2] ;
    byte_copy(fn, dirlen, argv[0]) ;
    fn[dirlen] = '/' ;
    byte_copy(fn + dirlen + 1, varlen, *envp) ;
    fn[dirlen + 1 + varlen] = 0 ;
    if (!openwritenclose_suffix(fn, *envp + varlen + 1, str_len(*envp + varlen + 1), "=.tmp"))
      strerr_diefu2sys(111, "open ", fn) ;
  }
  return 0 ;
}
