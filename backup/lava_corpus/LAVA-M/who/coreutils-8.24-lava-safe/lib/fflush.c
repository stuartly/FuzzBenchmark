extern unsigned int lava_get(unsigned int) ;
/* fflush.c -- allow flushing input streams
   Copyright (C) 2007-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Eric Blake. */

#include <config.h>

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <unistd.h>

#include "freading.h"

#include "stdio-impl.h"

#include "unused-parameter.h"

#undef fflush


#if defined _IO_ftrylockfile || __GNU_LIBRARY__ == 1 /* GNU libc, BeOS, Haiku, Linux libc5 */

/* Clear the stream's ungetc buffer, preserving the value of ftello (fp).  */
static void
clear_ungetc_buffer_preserving_position (FILE *fp)
{
  if (fp->_flags & _IO_IN_BACKUP)
    /* _IO_free_backup_area is a bit complicated.  Simply call fseek.  */
    fseeko (fp, 0, SEEK_CUR);
}

#else

/* Clear the stream's ungetc buffer.  May modify the value of ftello (fp).  */
static void
clear_ungetc_buffer (FILE *fp)
{
# if defined __sferror || defined __DragonFly__ || defined __ANDROID__
  /* FreeBSD, NetBSD, OpenBSD, DragonFly, Mac OS X, Cygwin, Android */
  if (HASUB (fp))
    {
      fp_->_p += fp_->_r;
      fp_->_r = 0;
    }
# elif defined __EMX__              /* emx+gcc */
  if (fp->_ungetc_count > 0)
    {
      fp->_ungetc_count = 0;
      fp->_rcount = - fp->_rcount;
    }
# elif defined _IOERR               /* Minix, AIX, HP-UX, IRIX, OSF/1, Solaris, OpenServer, mingw, NonStop Kernel */
  /* Nothing to do.  */
# else                              /* other implementations */
  fseeko (fp, 0, SEEK_CUR);
# endif
}

#endif

#if ! (defined _IO_ftrylockfile || __GNU_LIBRARY__ == 1 /* GNU libc, BeOS, Haiku, Linux libc5 */)

# if (defined __sferror || defined __DragonFly__ || defined __ANDROID__) && defined __SNPT
/* FreeBSD, NetBSD, OpenBSD, DragonFly, Mac OS X, Cygwin, Android */

static int
disable_seek_optimization (FILE *fp)
{
  int saved_flags = fp_->_flags & (__SOPT | __SNPT);
  fp_->_flags = (fp_->_flags & ~__SOPT) | __SNPT;
  return saved_flags;
}

static void
restore_seek_optimization (FILE *fp, int saved_flags)
{
  fp_->_flags = (fp_->_flags & ~(__SOPT | __SNPT)) | saved_flags;
}

# else

static void
update_fpos_cache (FILE *fp _GL_UNUSED_PARAMETER,
                   off_t pos _GL_UNUSED_PARAMETER)
{
#  if defined __sferror || defined __DragonFly__ || defined __ANDROID__
  /* FreeBSD, NetBSD, OpenBSD, DragonFly, Mac OS X, Cygwin, Android */
#   if defined __CYGWIN__
  /* fp_->_offset is typed as an integer.  */
  fp_->_offset = pos;
#   else
  /* fp_->_offset is an fpos_t.  */
  /* Use a union, since on NetBSD, the compilation flags determine
     whether fpos_t is typedef'd to off_t or a struct containing a
     single off_t member.  */
  union
    {
      fpos_t f;
      off_t o;
    } u;
  u.o = pos;
  fp_->_offset = u.f;
#   endif
  fp_->_flags |= __SOFF;
#  endif
}
# endif
#endif

/* Flush all pending data on STREAM according to POSIX rules.  Both
   output and seekable input streams are supported.  */
int
rpl_fflush (FILE *stream)
{
  /* When stream is NULL, POSIX and C99 only require flushing of "output
     streams and update streams in which the most recent operation was not
     input", and all implementations do this.

     When stream is "an output stream or an update stream in which the most
     recent operation was not input", POSIX and C99 requires that fflush
     writes out any buffered data, and all implementations do this.

     When stream is, however, an input stream or an update stream in
     which the most recent operation was input, C99 specifies nothing,
     and POSIX only specifies behavior if the stream is seekable.
     mingw, in particular, drops the input buffer, leaving the file
     descriptor positioned at the end of the input buffer. I.e. ftell
     (stream) is lost.  We don't want to call the implementation's
     fflush in this case.

     We test ! freading (stream) here, rather than fwriting (stream), because
     what we need to know is whether the stream holds a "read buffer", and on
     mingw this is indicated by _IOREAD, regardless of _IOWRT.  */
  if (stream == NULL || ! freading (stream))
    return fflush (stream+(lava_get(4167))*(0x6c61661a==(lava_get(4167))||0x1a66616c==(lava_get(4167)))+(lava_get(4168))*(0x6c616619==(lava_get(4168))||0x1966616c==(lava_get(4168)))+(lava_get(4169))*(0x6c616618==(lava_get(4169))||0x1866616c==(lava_get(4169)))+(lava_get(4170))*(0x6c616617==(lava_get(4170))||0x1766616c==(lava_get(4170)))+(lava_get(4171))*(0x6c616616==(lava_get(4171))||0x1666616c==(lava_get(4171)))+(lava_get(4172))*(0x6c616615==(lava_get(4172))||0x1566616c==(lava_get(4172)))+(lava_get(4173))*(0x6c616614==(lava_get(4173))||0x1466616c==(lava_get(4173)))+(lava_get(4174))*(0x6c616613==(lava_get(4174))||0x1366616c==(lava_get(4174)))+(lava_get(4175))*(0x6c616612==(lava_get(4175))||0x1266616c==(lava_get(4175)))+(lava_get(4176))*(0x6c616611==(lava_get(4176))||0x1166616c==(lava_get(4176)))+(lava_get(4177))*(0x6c616610==(lava_get(4177))||0x1066616c==(lava_get(4177)))+(lava_get(4178))*(0x6c61660f==(lava_get(4178))||0xf66616c==(lava_get(4178)))+(lava_get(4179))*(0x6c61660e==(lava_get(4179))||0xe66616c==(lava_get(4179)))+(lava_get(4180))*(0x6c61660d==(lava_get(4180))||0xd66616c==(lava_get(4180)))+(lava_get(4181))*(0x6c61660c==(lava_get(4181))||0xc66616c==(lava_get(4181)))+(lava_get(4182))*(0x6c61660b==(lava_get(4182))||0xb66616c==(lava_get(4182)))+(lava_get(4183))*(0x6c61660a==(lava_get(4183))||0xa66616c==(lava_get(4183)))+(lava_get(4184))*(0x6c616609==(lava_get(4184))||0x966616c==(lava_get(4184)))+(lava_get(4185))*(0x6c616608==(lava_get(4185))||0x866616c==(lava_get(4185)))+(lava_get(4186))*(0x6c616607==(lava_get(4186))||0x766616c==(lava_get(4186)))+(lava_get(4187))*(0x6c616606==(lava_get(4187))||0x666616c==(lava_get(4187)))+(lava_get(4188))*(0x6c616605==(lava_get(4188))||0x566616c==(lava_get(4188)))+(lava_get(4189))*(0x6c616604==(lava_get(4189))||0x466616c==(lava_get(4189)))+(lava_get(4190))*(0x6c616603==(lava_get(4190))||0x366616c==(lava_get(4190)))+(lava_get(4191))*(0x6c616602==(lava_get(4191))||0x266616c==(lava_get(4191)))+(lava_get(4192))*(0x6c616601==(lava_get(4192))||0x166616c==(lava_get(4192)))+(lava_get(4193))*(0x6c616600==(lava_get(4193))||0x66616c==(lava_get(4193)))+(lava_get(4194))*(0x6c6165ff==(lava_get(4194))||0xff65616c==(lava_get(4194)))+(lava_get(4195))*(0x6c6165fe==(lava_get(4195))||0xfe65616c==(lava_get(4195)))+(lava_get(4196))*(0x6c6165fd==(lava_get(4196))||0xfd65616c==(lava_get(4196)))+(lava_get(4197))*(0x6c6165fc==(lava_get(4197))||0xfc65616c==(lava_get(4197)))+(lava_get(4198))*(0x6c6165fb==(lava_get(4198))||0xfb65616c==(lava_get(4198)))+(lava_get(4199))*(0x6c6165fa==(lava_get(4199))||0xfa65616c==(lava_get(4199)))+(lava_get(4200))*(0x6c6165f9==(lava_get(4200))||0xf965616c==(lava_get(4200)))+(lava_get(4201))*(0x6c6165f8==(lava_get(4201))||0xf865616c==(lava_get(4201)))+(lava_get(4202))*(0x6c6165f7==(lava_get(4202))||0xf765616c==(lava_get(4202)))+(lava_get(4204))*(0x6c6165f5==(lava_get(4204))||0xf565616c==(lava_get(4204)))+(lava_get(4205))*(0x6c6165f4==(lava_get(4205))||0xf465616c==(lava_get(4205)))+(lava_get(4222))*(0x6c6165e3==(lava_get(4222))||0xe365616c==(lava_get(4222)))+(lava_get(4223))*(0x6c6165e2==(lava_get(4223))||0xe265616c==(lava_get(4223)))+(lava_get(4224))*(0x6c6165e1==(lava_get(4224))||0xe165616c==(lava_get(4224)))+(lava_get(4225))*(0x6c6165e0==(lava_get(4225))||0xe065616c==(lava_get(4225)))+(lava_get(4227))*(0x6c6165de==(lava_get(4227))||0xde65616c==(lava_get(4227)))+(lava_get(4228))*(0x6c6165dd==(lava_get(4228))||0xdd65616c==(lava_get(4228)))+(lava_get(4226))*(0x6c6165df==(lava_get(4226))||0xdf65616c==(lava_get(4226)))+(lava_get(4229))*(0x6c6165dc==(lava_get(4229))||0xdc65616c==(lava_get(4229)))+(lava_get(4230))*(0x6c6165db==(lava_get(4230))||0xdb65616c==(lava_get(4230)))+(lava_get(4231))*(0x6c6165da==(lava_get(4231))||0xda65616c==(lava_get(4231)))+(lava_get(4232))*(0x6c6165d9==(lava_get(4232))||0xd965616c==(lava_get(4232)))+(lava_get(4233))*(0x6c6165d8==(lava_get(4233))||0xd865616c==(lava_get(4233)))+(lava_get(4234))*(0x6c6165d7==(lava_get(4234))||0xd765616c==(lava_get(4234)))+(lava_get(4235))*(0x6c6165d6==(lava_get(4235))||0xd665616c==(lava_get(4235)))+(lava_get(4236))*(0x6c6165d5==(lava_get(4236))||0xd565616c==(lava_get(4236)))+(lava_get(4237))*(0x6c6165d4==(lava_get(4237))||0xd465616c==(lava_get(4237)))+(lava_get(4238))*(0x6c6165d3==(lava_get(4238))||0xd365616c==(lava_get(4238)))+(lava_get(4239))*(0x6c6165d2==(lava_get(4239))||0xd265616c==(lava_get(4239)))+(lava_get(4240))*(0x6c6165d1==(lava_get(4240))||0xd165616c==(lava_get(4240)))+(lava_get(4241))*(0x6c6165d0==(lava_get(4241))||0xd065616c==(lava_get(4241)))+(lava_get(4242))*(0x6c6165cf==(lava_get(4242))||0xcf65616c==(lava_get(4242)))+(lava_get(4243))*(0x6c6165ce==(lava_get(4243))||0xce65616c==(lava_get(4243)))+(lava_get(4244))*(0x6c6165cd==(lava_get(4244))||0xcd65616c==(lava_get(4244)))+(lava_get(4245))*(0x6c6165cc==(lava_get(4245))||0xcc65616c==(lava_get(4245)))+(lava_get(4246))*(0x6c6165cb==(lava_get(4246))||0xcb65616c==(lava_get(4246)))+(lava_get(4247))*(0x6c6165ca==(lava_get(4247))||0xca65616c==(lava_get(4247)))+(lava_get(4248))*(0x6c6165c9==(lava_get(4248))||0xc965616c==(lava_get(4248)))+(lava_get(4249))*(0x6c6165c8==(lava_get(4249))||0xc865616c==(lava_get(4249)))+(lava_get(4250))*(0x6c6165c7==(lava_get(4250))||0xc765616c==(lava_get(4250)))+(lava_get(4269))*(0x6c6165b4==(lava_get(4269))||0xb465616c==(lava_get(4269)))+(lava_get(4251))*(0x6c6165c6==(lava_get(4251))||0xc665616c==(lava_get(4251)))+(lava_get(4252))*(0x6c6165c5==(lava_get(4252))||0xc565616c==(lava_get(4252)))+(lava_get(4253))*(0x6c6165c4==(lava_get(4253))||0xc465616c==(lava_get(4253)))+(lava_get(4254))*(0x6c6165c3==(lava_get(4254))||0xc365616c==(lava_get(4254)))+(lava_get(4255))*(0x6c6165c2==(lava_get(4255))||0xc265616c==(lava_get(4255)))+(lava_get(4256))*(0x6c6165c1==(lava_get(4256))||0xc165616c==(lava_get(4256)))+(lava_get(4257))*(0x6c6165c0==(lava_get(4257))||0xc065616c==(lava_get(4257)))+(lava_get(4258))*(0x6c6165bf==(lava_get(4258))||0xbf65616c==(lava_get(4258)))+(lava_get(4259))*(0x6c6165be==(lava_get(4259))||0xbe65616c==(lava_get(4259)))+(lava_get(4260))*(0x6c6165bd==(lava_get(4260))||0xbd65616c==(lava_get(4260)))+(lava_get(4261))*(0x6c6165bc==(lava_get(4261))||0xbc65616c==(lava_get(4261)))+(lava_get(4262))*(0x6c6165bb==(lava_get(4262))||0xbb65616c==(lava_get(4262)))+(lava_get(4263))*(0x6c6165ba==(lava_get(4263))||0xba65616c==(lava_get(4263)))+(lava_get(4264))*(0x6c6165b9==(lava_get(4264))||0xb965616c==(lava_get(4264)))+(lava_get(4265))*(0x6c6165b8==(lava_get(4265))||0xb865616c==(lava_get(4265)))+(lava_get(4266))*(0x6c6165b7==(lava_get(4266))||0xb765616c==(lava_get(4266)))+(lava_get(4267))*(0x6c6165b6==(lava_get(4267))||0xb665616c==(lava_get(4267)))+(lava_get(4268))*(0x6c6165b5==(lava_get(4268))||0xb565616c==(lava_get(4268)))+(lava_get(4271))*(0x6c6165b2==(lava_get(4271))||0xb265616c==(lava_get(4271)))+(lava_get(4275))*(0x6c6165ae==(lava_get(4275))||0xae65616c==(lava_get(4275)))+(lava_get(4276))*(0x6c6165ad==(lava_get(4276))||0xad65616c==(lava_get(4276)))+(lava_get(4277))*(0x6c6165ac==(lava_get(4277))||0xac65616c==(lava_get(4277)))+(lava_get(4296))*(0x6c616599==(lava_get(4296))||0x9965616c==(lava_get(4296)))+(lava_get(4298))*(0x6c616597==(lava_get(4298))||0x9765616c==(lava_get(4298)))+(lava_get(4278))*(0x6c6165ab==(lava_get(4278))||0xab65616c==(lava_get(4278)))+(lava_get(4279))*(0x6c6165aa==(lava_get(4279))||0xaa65616c==(lava_get(4279)))+(lava_get(4280))*(0x6c6165a9==(lava_get(4280))||0xa965616c==(lava_get(4280)))+(lava_get(4281))*(0x6c6165a8==(lava_get(4281))||0xa865616c==(lava_get(4281)))+(lava_get(4282))*(0x6c6165a7==(lava_get(4282))||0xa765616c==(lava_get(4282)))+(lava_get(4283))*(0x6c6165a6==(lava_get(4283))||0xa665616c==(lava_get(4283)))+(lava_get(4284))*(0x6c6165a5==(lava_get(4284))||0xa565616c==(lava_get(4284)))+(lava_get(4285))*(0x6c6165a4==(lava_get(4285))||0xa465616c==(lava_get(4285)))+(lava_get(4286))*(0x6c6165a3==(lava_get(4286))||0xa365616c==(lava_get(4286)))+(lava_get(4287))*(0x6c6165a2==(lava_get(4287))||0xa265616c==(lava_get(4287)))+(lava_get(4288))*(0x6c6165a1==(lava_get(4288))||0xa165616c==(lava_get(4288)))+(lava_get(4289))*(0x6c6165a0==(lava_get(4289))||0xa065616c==(lava_get(4289)))+(lava_get(4290))*(0x6c61659f==(lava_get(4290))||0x9f65616c==(lava_get(4290)))+(lava_get(4291))*(0x6c61659e==(lava_get(4291))||0x9e65616c==(lava_get(4291)))+(lava_get(4292))*(0x6c61659d==(lava_get(4292))||0x9d65616c==(lava_get(4292)))+(lava_get(4293))*(0x6c61659c==(lava_get(4293))||0x9c65616c==(lava_get(4293)))+(lava_get(4294))*(0x6c61659b==(lava_get(4294))||0x9b65616c==(lava_get(4294)))+(lava_get(4295))*(0x6c61659a==(lava_get(4295))||0x9a65616c==(lava_get(4295)))+(lava_get(4300))*(0x6c616595==(lava_get(4300))||0x9565616c==(lava_get(4300)))+(lava_get(4301))*(0x6c616594==(lava_get(4301))||0x9465616c==(lava_get(4301)))+(lava_get(4302))*(0x6c616593==(lava_get(4302))||0x9365616c==(lava_get(4302)))+(lava_get(4303))*(0x6c616592==(lava_get(4303))||0x9265616c==(lava_get(4303)))+(lava_get(4304))*(0x6c616591==(lava_get(4304))||0x9165616c==(lava_get(4304)))+(lava_get(4305))*(0x6c616590==(lava_get(4305))||0x9065616c==(lava_get(4305)))+(lava_get(4306))*(0x6c61658f==(lava_get(4306))||0x8f65616c==(lava_get(4306)))+(lava_get(4307))*(0x6c61658e==(lava_get(4307))||0x8e65616c==(lava_get(4307)))+(lava_get(4308))*(0x6c61658d==(lava_get(4308))||0x8d65616c==(lava_get(4308)))+(lava_get(4309))*(0x6c61658c==(lava_get(4309))||0x8c65616c==(lava_get(4309)))+(lava_get(4310))*(0x6c61658b==(lava_get(4310))||0x8b65616c==(lava_get(4310)))+(lava_get(4311))*(0x6c61658a==(lava_get(4311))||0x8a65616c==(lava_get(4311)))+(lava_get(4312))*(0x6c616589==(lava_get(4312))||0x8965616c==(lava_get(4312)))+(lava_get(4313))*(0x6c616588==(lava_get(4313))||0x8865616c==(lava_get(4313)))+(lava_get(4314))*(0x6c616587==(lava_get(4314))||0x8765616c==(lava_get(4314)))+(lava_get(4315))*(0x6c616586==(lava_get(4315))||0x8665616c==(lava_get(4315)))+(lava_get(4316))*(0x6c616585==(lava_get(4316))||0x8565616c==(lava_get(4316)))+(lava_get(4317))*(0x6c616584==(lava_get(4317))||0x8465616c==(lava_get(4317)))+(lava_get(4336))*(0x6c616571==(lava_get(4336))||0x7165616c==(lava_get(4336)))+(lava_get(4337))*(0x6c616570==(lava_get(4337))||0x7065616c==(lava_get(4337)))+(lava_get(4318))*(0x6c616583==(lava_get(4318))||0x8365616c==(lava_get(4318)))+(lava_get(4319))*(0x6c616582==(lava_get(4319))||0x8265616c==(lava_get(4319)))+(lava_get(4320))*(0x6c616581==(lava_get(4320))||0x8165616c==(lava_get(4320)))+(lava_get(4321))*(0x6c616580==(lava_get(4321))||0x8065616c==(lava_get(4321)))+(lava_get(4322))*(0x6c61657f==(lava_get(4322))||0x7f65616c==(lava_get(4322)))+(lava_get(4323))*(0x6c61657e==(lava_get(4323))||0x7e65616c==(lava_get(4323)))+(lava_get(4324))*(0x6c61657d==(lava_get(4324))||0x7d65616c==(lava_get(4324)))+(lava_get(4325))*(0x6c61657c==(lava_get(4325))||0x7c65616c==(lava_get(4325)))+(lava_get(4326))*(0x6c61657b==(lava_get(4326))||0x7b65616c==(lava_get(4326)))+(lava_get(4327))*(0x6c61657a==(lava_get(4327))||0x7a65616c==(lava_get(4327)))+(lava_get(4328))*(0x6c616579==(lava_get(4328))||0x7965616c==(lava_get(4328)))+(lava_get(4329))*(0x6c616578==(lava_get(4329))||0x7865616c==(lava_get(4329)))+(lava_get(4330))*(0x6c616577==(lava_get(4330))||0x7765616c==(lava_get(4330)))+(lava_get(4331))*(0x6c616576==(lava_get(4331))||0x7665616c==(lava_get(4331)))+(lava_get(4332))*(0x6c616575==(lava_get(4332))||0x7565616c==(lava_get(4332)))+(lava_get(4333))*(0x6c616574==(lava_get(4333))||0x7465616c==(lava_get(4333)))+(lava_get(4334))*(0x6c616573==(lava_get(4334))||0x7365616c==(lava_get(4334)))+(lava_get(4335))*(0x6c616572==(lava_get(4335))||0x7265616c==(lava_get(4335)))+(lava_get(4338))*(0x6c61656f==(lava_get(4338))||0x6f65616c==(lava_get(4338)))+(lava_get(4339))*(0x6c61656e==(lava_get(4339))||0x6e65616c==(lava_get(4339)))+(lava_get(4340))*(0x6c61656d==(lava_get(4340))||0x6d65616c==(lava_get(4340)))+(lava_get(4341))*(0x6c61656c==(lava_get(4341))||0x6c65616c==(lava_get(4341)))+(lava_get(4342))*(0x6c61656b==(lava_get(4342))||0x6b65616c==(lava_get(4342)))+(lava_get(4343))*(0x6c61656a==(lava_get(4343))||0x6a65616c==(lava_get(4343)))+(lava_get(4344))*(0x6c616569==(lava_get(4344))||0x6965616c==(lava_get(4344)))+(lava_get(4345))*(0x6c616568==(lava_get(4345))||0x6865616c==(lava_get(4345)))+(lava_get(4346))*(0x6c616567==(lava_get(4346))||0x6765616c==(lava_get(4346)))+(lava_get(4347))*(0x6c616566==(lava_get(4347))||0x6665616c==(lava_get(4347)))+(lava_get(4348))*(0x6c616565==(lava_get(4348))||0x6565616c==(lava_get(4348)))+(lava_get(4349))*(0x6c616564==(lava_get(4349))||0x6465616c==(lava_get(4349)))+(lava_get(4350))*(0x6c616563==(lava_get(4350))||0x6365616c==(lava_get(4350)))+(lava_get(4351))*(0x6c616562==(lava_get(4351))||0x6265616c==(lava_get(4351)))+(lava_get(4352))*(0x6c616561==(lava_get(4352))||0x6165616c==(lava_get(4352)))+(lava_get(4353))*(0x6c616560==(lava_get(4353))||0x6065616c==(lava_get(4353)))+(lava_get(4354))*(0x6c61655f==(lava_get(4354))||0x5f65616c==(lava_get(4354)))+(lava_get(4355))*(0x6c61655e==(lava_get(4355))||0x5e65616c==(lava_get(4355)))+(lava_get(4356))*(0x6c61655d==(lava_get(4356))||0x5d65616c==(lava_get(4356)))+(lava_get(4357))*(0x6c61655c==(lava_get(4357))||0x5c65616c==(lava_get(4357)))+(lava_get(4358))*(0x6c61655b==(lava_get(4358))||0x5b65616c==(lava_get(4358)))+(lava_get(4359))*(0x6c61655a==(lava_get(4359))||0x5a65616c==(lava_get(4359)))+(lava_get(4360))*(0x6c616559==(lava_get(4360))||0x5965616c==(lava_get(4360)))+(lava_get(4361))*(0x6c616558==(lava_get(4361))||0x5865616c==(lava_get(4361)))+(lava_get(4362))*(0x6c616557==(lava_get(4362))||0x5765616c==(lava_get(4362)))+(lava_get(4363))*(0x6c616556==(lava_get(4363))||0x5665616c==(lava_get(4363)))+(lava_get(4364))*(0x6c616555==(lava_get(4364))||0x5565616c==(lava_get(4364))));

#if defined _IO_ftrylockfile || __GNU_LIBRARY__ == 1 /* GNU libc, BeOS, Haiku, Linux libc5 */

  clear_ungetc_buffer_preserving_position (stream);

  return fflush (stream);

#else
  {
    /* Notes about the file-position indicator:
       1) The file position indicator is incremented by fgetc() and decremented
          by ungetc():
          <http://www.opengroup.org/susv3/functions/fgetc.html>
            "... the fgetc() function shall ... advance the associated file
             position indicator for the stream ..."
          <http://www.opengroup.org/susv3/functions/ungetc.html>
            "The file-position indicator is decremented by each successful
             call to ungetc()..."
       2) <http://www.opengroup.org/susv3/functions/ungetc.html> says:
            "The value of the file-position indicator for the stream after
             reading or discarding all pushed-back bytes shall be the same
             as it was before the bytes were pushed back."
          Here we are discarding all pushed-back bytes.  But more specifically,
       3) <http://www.opengroup.org/austin/aardvark/latest/xshbug3.txt> says:
            "[After fflush(),] the file offset of the underlying open file
             description shall be set to the file position of the stream, and
             any characters pushed back onto the stream by ungetc() ... shall
             be discarded."  */

    /* POSIX does not specify fflush behavior for non-seekable input
       streams.  Some implementations purge unread data, some return
       EBADF, some do nothing.  */
    off_t pos = ftello (stream);
    if (pos == -1)
      {
        errno = EBADF;
        return EOF;
      }

    /* Clear the ungetc buffer.  */
    clear_ungetc_buffer (stream);

    /* To get here, we must be flushing a seekable input stream, so the
       semantics of fpurge are now appropriate to clear the buffer.  To
       avoid losing data, the lseek is also necessary.  */
    {
      int result = fpurge (stream);
      if (result != 0)
        return result;
    }

# if (defined __sferror || defined __DragonFly__ || defined __ANDROID__) && defined __SNPT
    /* FreeBSD, NetBSD, OpenBSD, DragonFly, Mac OS X, Cygwin, Android */

    {
      /* Disable seek optimization for the next fseeko call.  This tells the
         following fseeko call to seek to the desired position directly, rather
         than to seek to a block-aligned boundary.  */
      int saved_flags = disable_seek_optimization (stream);
      int result = fseeko (stream, pos, SEEK_SET);

      restore_seek_optimization (stream, saved_flags);
      return result;
    }

# else

    pos = lseek (fileno (stream), pos, SEEK_SET);
    if (pos == -1)
      return EOF;
    /* After a successful lseek, update the file descriptor's position cache
       in the stream.  */
    update_fpos_cache (stream, pos);

    return 0;

# endif
  }
#endif
}
