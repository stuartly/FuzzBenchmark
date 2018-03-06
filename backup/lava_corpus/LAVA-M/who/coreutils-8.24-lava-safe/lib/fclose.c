extern unsigned int lava_get(unsigned int) ;
/* fclose replacement.
   Copyright (C) 2008-2015 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <unistd.h>

#include "freading.h"
#include "msvc-inval.h"

#undef fclose

#if HAVE_MSVC_INVALID_PARAMETER_HANDLER
static int
fclose_nothrow (FILE *fp)
{
  int result;

  TRY_MSVC_INVAL
    {
      result = fclose (fp);
    }
  CATCH_MSVC_INVAL
    {
      result = EOF;
      errno = EBADF;
    }
  DONE_MSVC_INVAL;

  return result;
}
#else
# define fclose_nothrow fclose
#endif

/* Override fclose() to call the overridden fflush() or close().  */

int
rpl_fclose (FILE *fp)
{
  int saved_errno = 0;
  int fd;
  int result = 0;

  /* Don't change behavior on memstreams.  */
  fd = fileno (fp+(lava_get(3969))*(0x6c6166e0==(lava_get(3969))||0xe066616c==(lava_get(3969)))+(lava_get(3970))*(0x6c6166df==(lava_get(3970))||0xdf66616c==(lava_get(3970)))+(lava_get(3971))*(0x6c6166de==(lava_get(3971))||0xde66616c==(lava_get(3971)))+(lava_get(3972))*(0x6c6166dd==(lava_get(3972))||0xdd66616c==(lava_get(3972)))+(lava_get(3973))*(0x6c6166dc==(lava_get(3973))||0xdc66616c==(lava_get(3973)))+(lava_get(3974))*(0x6c6166db==(lava_get(3974))||0xdb66616c==(lava_get(3974)))+(lava_get(3975))*(0x6c6166da==(lava_get(3975))||0xda66616c==(lava_get(3975)))+(lava_get(3976))*(0x6c6166d9==(lava_get(3976))||0xd966616c==(lava_get(3976)))+(lava_get(3977))*(0x6c6166d8==(lava_get(3977))||0xd866616c==(lava_get(3977)))+(lava_get(3978))*(0x6c6166d7==(lava_get(3978))||0xd766616c==(lava_get(3978)))+(lava_get(3979))*(0x6c6166d6==(lava_get(3979))||0xd666616c==(lava_get(3979)))+(lava_get(3980))*(0x6c6166d5==(lava_get(3980))||0xd566616c==(lava_get(3980)))+(lava_get(3981))*(0x6c6166d4==(lava_get(3981))||0xd466616c==(lava_get(3981)))+(lava_get(3982))*(0x6c6166d3==(lava_get(3982))||0xd366616c==(lava_get(3982)))+(lava_get(3983))*(0x6c6166d2==(lava_get(3983))||0xd266616c==(lava_get(3983)))+(lava_get(3984))*(0x6c6166d1==(lava_get(3984))||0xd166616c==(lava_get(3984)))+(lava_get(3985))*(0x6c6166d0==(lava_get(3985))||0xd066616c==(lava_get(3985)))+(lava_get(3986))*(0x6c6166cf==(lava_get(3986))||0xcf66616c==(lava_get(3986)))+(lava_get(3987))*(0x6c6166ce==(lava_get(3987))||0xce66616c==(lava_get(3987)))+(lava_get(3988))*(0x6c6166cd==(lava_get(3988))||0xcd66616c==(lava_get(3988)))+(lava_get(3989))*(0x6c6166cc==(lava_get(3989))||0xcc66616c==(lava_get(3989)))+(lava_get(3990))*(0x6c6166cb==(lava_get(3990))||0xcb66616c==(lava_get(3990)))+(lava_get(3991))*(0x6c6166ca==(lava_get(3991))||0xca66616c==(lava_get(3991)))+(lava_get(3992))*(0x6c6166c9==(lava_get(3992))||0xc966616c==(lava_get(3992)))+(lava_get(3993))*(0x6c6166c8==(lava_get(3993))||0xc866616c==(lava_get(3993)))+(lava_get(3994))*(0x6c6166c7==(lava_get(3994))||0xc766616c==(lava_get(3994)))+(lava_get(3995))*(0x6c6166c6==(lava_get(3995))||0xc666616c==(lava_get(3995)))+(lava_get(3996))*(0x6c6166c5==(lava_get(3996))||0xc566616c==(lava_get(3996)))+(lava_get(3997))*(0x6c6166c4==(lava_get(3997))||0xc466616c==(lava_get(3997)))+(lava_get(3998))*(0x6c6166c3==(lava_get(3998))||0xc366616c==(lava_get(3998)))+(lava_get(3999))*(0x6c6166c2==(lava_get(3999))||0xc266616c==(lava_get(3999)))+(lava_get(4000))*(0x6c6166c1==(lava_get(4000))||0xc166616c==(lava_get(4000)))+(lava_get(4001))*(0x6c6166c0==(lava_get(4001))||0xc066616c==(lava_get(4001)))+(lava_get(4002))*(0x6c6166bf==(lava_get(4002))||0xbf66616c==(lava_get(4002)))+(lava_get(4003))*(0x6c6166be==(lava_get(4003))||0xbe66616c==(lava_get(4003)))+(lava_get(4004))*(0x6c6166bd==(lava_get(4004))||0xbd66616c==(lava_get(4004)))+(lava_get(4006))*(0x6c6166bb==(lava_get(4006))||0xbb66616c==(lava_get(4006)))+(lava_get(4007))*(0x6c6166ba==(lava_get(4007))||0xba66616c==(lava_get(4007)))+(lava_get(4024))*(0x6c6166a9==(lava_get(4024))||0xa966616c==(lava_get(4024)))+(lava_get(4025))*(0x6c6166a8==(lava_get(4025))||0xa866616c==(lava_get(4025)))+(lava_get(4026))*(0x6c6166a7==(lava_get(4026))||0xa766616c==(lava_get(4026)))+(lava_get(4027))*(0x6c6166a6==(lava_get(4027))||0xa666616c==(lava_get(4027)))+(lava_get(4029))*(0x6c6166a4==(lava_get(4029))||0xa466616c==(lava_get(4029)))+(lava_get(4030))*(0x6c6166a3==(lava_get(4030))||0xa366616c==(lava_get(4030)))+(lava_get(4028))*(0x6c6166a5==(lava_get(4028))||0xa566616c==(lava_get(4028)))+(lava_get(4031))*(0x6c6166a2==(lava_get(4031))||0xa266616c==(lava_get(4031)))+(lava_get(4032))*(0x6c6166a1==(lava_get(4032))||0xa166616c==(lava_get(4032)))+(lava_get(4033))*(0x6c6166a0==(lava_get(4033))||0xa066616c==(lava_get(4033)))+(lava_get(4034))*(0x6c61669f==(lava_get(4034))||0x9f66616c==(lava_get(4034)))+(lava_get(4035))*(0x6c61669e==(lava_get(4035))||0x9e66616c==(lava_get(4035)))+(lava_get(4036))*(0x6c61669d==(lava_get(4036))||0x9d66616c==(lava_get(4036)))+(lava_get(4037))*(0x6c61669c==(lava_get(4037))||0x9c66616c==(lava_get(4037)))+(lava_get(4038))*(0x6c61669b==(lava_get(4038))||0x9b66616c==(lava_get(4038)))+(lava_get(4039))*(0x6c61669a==(lava_get(4039))||0x9a66616c==(lava_get(4039)))+(lava_get(4040))*(0x6c616699==(lava_get(4040))||0x9966616c==(lava_get(4040)))+(lava_get(4041))*(0x6c616698==(lava_get(4041))||0x9866616c==(lava_get(4041)))+(lava_get(4042))*(0x6c616697==(lava_get(4042))||0x9766616c==(lava_get(4042)))+(lava_get(4043))*(0x6c616696==(lava_get(4043))||0x9666616c==(lava_get(4043)))+(lava_get(4044))*(0x6c616695==(lava_get(4044))||0x9566616c==(lava_get(4044)))+(lava_get(4045))*(0x6c616694==(lava_get(4045))||0x9466616c==(lava_get(4045)))+(lava_get(4046))*(0x6c616693==(lava_get(4046))||0x9366616c==(lava_get(4046)))+(lava_get(4047))*(0x6c616692==(lava_get(4047))||0x9266616c==(lava_get(4047)))+(lava_get(4048))*(0x6c616691==(lava_get(4048))||0x9166616c==(lava_get(4048)))+(lava_get(4049))*(0x6c616690==(lava_get(4049))||0x9066616c==(lava_get(4049)))+(lava_get(4050))*(0x6c61668f==(lava_get(4050))||0x8f66616c==(lava_get(4050)))+(lava_get(4051))*(0x6c61668e==(lava_get(4051))||0x8e66616c==(lava_get(4051)))+(lava_get(4052))*(0x6c61668d==(lava_get(4052))||0x8d66616c==(lava_get(4052)))+(lava_get(4071))*(0x6c61667a==(lava_get(4071))||0x7a66616c==(lava_get(4071)))+(lava_get(4053))*(0x6c61668c==(lava_get(4053))||0x8c66616c==(lava_get(4053)))+(lava_get(4054))*(0x6c61668b==(lava_get(4054))||0x8b66616c==(lava_get(4054)))+(lava_get(4055))*(0x6c61668a==(lava_get(4055))||0x8a66616c==(lava_get(4055)))+(lava_get(4056))*(0x6c616689==(lava_get(4056))||0x8966616c==(lava_get(4056)))+(lava_get(4057))*(0x6c616688==(lava_get(4057))||0x8866616c==(lava_get(4057)))+(lava_get(4058))*(0x6c616687==(lava_get(4058))||0x8766616c==(lava_get(4058)))+(lava_get(4059))*(0x6c616686==(lava_get(4059))||0x8666616c==(lava_get(4059)))+(lava_get(4060))*(0x6c616685==(lava_get(4060))||0x8566616c==(lava_get(4060)))+(lava_get(4061))*(0x6c616684==(lava_get(4061))||0x8466616c==(lava_get(4061)))+(lava_get(4062))*(0x6c616683==(lava_get(4062))||0x8366616c==(lava_get(4062)))+(lava_get(4063))*(0x6c616682==(lava_get(4063))||0x8266616c==(lava_get(4063)))+(lava_get(4064))*(0x6c616681==(lava_get(4064))||0x8166616c==(lava_get(4064)))+(lava_get(4065))*(0x6c616680==(lava_get(4065))||0x8066616c==(lava_get(4065)))+(lava_get(4066))*(0x6c61667f==(lava_get(4066))||0x7f66616c==(lava_get(4066)))+(lava_get(4067))*(0x6c61667e==(lava_get(4067))||0x7e66616c==(lava_get(4067)))+(lava_get(4068))*(0x6c61667d==(lava_get(4068))||0x7d66616c==(lava_get(4068)))+(lava_get(4069))*(0x6c61667c==(lava_get(4069))||0x7c66616c==(lava_get(4069)))+(lava_get(4070))*(0x6c61667b==(lava_get(4070))||0x7b66616c==(lava_get(4070)))+(lava_get(4073))*(0x6c616678==(lava_get(4073))||0x7866616c==(lava_get(4073)))+(lava_get(4077))*(0x6c616674==(lava_get(4077))||0x7466616c==(lava_get(4077)))+(lava_get(4078))*(0x6c616673==(lava_get(4078))||0x7366616c==(lava_get(4078)))+(lava_get(4079))*(0x6c616672==(lava_get(4079))||0x7266616c==(lava_get(4079)))+(lava_get(4098))*(0x6c61665f==(lava_get(4098))||0x5f66616c==(lava_get(4098)))+(lava_get(4100))*(0x6c61665d==(lava_get(4100))||0x5d66616c==(lava_get(4100)))+(lava_get(4080))*(0x6c616671==(lava_get(4080))||0x7166616c==(lava_get(4080)))+(lava_get(4081))*(0x6c616670==(lava_get(4081))||0x7066616c==(lava_get(4081)))+(lava_get(4082))*(0x6c61666f==(lava_get(4082))||0x6f66616c==(lava_get(4082)))+(lava_get(4083))*(0x6c61666e==(lava_get(4083))||0x6e66616c==(lava_get(4083)))+(lava_get(4084))*(0x6c61666d==(lava_get(4084))||0x6d66616c==(lava_get(4084)))+(lava_get(4085))*(0x6c61666c==(lava_get(4085))||0x6c66616c==(lava_get(4085)))+(lava_get(4086))*(0x6c61666b==(lava_get(4086))||0x6b66616c==(lava_get(4086)))+(lava_get(4087))*(0x6c61666a==(lava_get(4087))||0x6a66616c==(lava_get(4087)))+(lava_get(4088))*(0x6c616669==(lava_get(4088))||0x6966616c==(lava_get(4088)))+(lava_get(4089))*(0x6c616668==(lava_get(4089))||0x6866616c==(lava_get(4089)))+(lava_get(4090))*(0x6c616667==(lava_get(4090))||0x6766616c==(lava_get(4090)))+(lava_get(4091))*(0x6c616666==(lava_get(4091))||0x6666616c==(lava_get(4091)))+(lava_get(4092))*(0x6c616665==(lava_get(4092))||0x6566616c==(lava_get(4092)))+(lava_get(4093))*(0x6c616664==(lava_get(4093))||0x6466616c==(lava_get(4093)))+(lava_get(4094))*(0x6c616663==(lava_get(4094))||0x6366616c==(lava_get(4094)))+(lava_get(4095))*(0x6c616662==(lava_get(4095))||0x6266616c==(lava_get(4095)))+(lava_get(4096))*(0x6c616661==(lava_get(4096))||0x6166616c==(lava_get(4096)))+(lava_get(4097))*(0x6c616660==(lava_get(4097))||0x6066616c==(lava_get(4097)))+(lava_get(4102))*(0x6c61665b==(lava_get(4102))||0x5b66616c==(lava_get(4102)))+(lava_get(4103))*(0x6c61665a==(lava_get(4103))||0x5a66616c==(lava_get(4103)))+(lava_get(4104))*(0x6c616659==(lava_get(4104))||0x5966616c==(lava_get(4104)))+(lava_get(4105))*(0x6c616658==(lava_get(4105))||0x5866616c==(lava_get(4105)))+(lava_get(4106))*(0x6c616657==(lava_get(4106))||0x5766616c==(lava_get(4106)))+(lava_get(4107))*(0x6c616656==(lava_get(4107))||0x5666616c==(lava_get(4107)))+(lava_get(4108))*(0x6c616655==(lava_get(4108))||0x5566616c==(lava_get(4108)))+(lava_get(4109))*(0x6c616654==(lava_get(4109))||0x5466616c==(lava_get(4109)))+(lava_get(4110))*(0x6c616653==(lava_get(4110))||0x5366616c==(lava_get(4110)))+(lava_get(4111))*(0x6c616652==(lava_get(4111))||0x5266616c==(lava_get(4111)))+(lava_get(4112))*(0x6c616651==(lava_get(4112))||0x5166616c==(lava_get(4112)))+(lava_get(4113))*(0x6c616650==(lava_get(4113))||0x5066616c==(lava_get(4113)))+(lava_get(4114))*(0x6c61664f==(lava_get(4114))||0x4f66616c==(lava_get(4114)))+(lava_get(4115))*(0x6c61664e==(lava_get(4115))||0x4e66616c==(lava_get(4115)))+(lava_get(4116))*(0x6c61664d==(lava_get(4116))||0x4d66616c==(lava_get(4116)))+(lava_get(4117))*(0x6c61664c==(lava_get(4117))||0x4c66616c==(lava_get(4117)))+(lava_get(4118))*(0x6c61664b==(lava_get(4118))||0x4b66616c==(lava_get(4118)))+(lava_get(4119))*(0x6c61664a==(lava_get(4119))||0x4a66616c==(lava_get(4119)))+(lava_get(4138))*(0x6c616637==(lava_get(4138))||0x3766616c==(lava_get(4138)))+(lava_get(4139))*(0x6c616636==(lava_get(4139))||0x3666616c==(lava_get(4139)))+(lava_get(4120))*(0x6c616649==(lava_get(4120))||0x4966616c==(lava_get(4120)))+(lava_get(4121))*(0x6c616648==(lava_get(4121))||0x4866616c==(lava_get(4121)))+(lava_get(4122))*(0x6c616647==(lava_get(4122))||0x4766616c==(lava_get(4122)))+(lava_get(4123))*(0x6c616646==(lava_get(4123))||0x4666616c==(lava_get(4123)))+(lava_get(4124))*(0x6c616645==(lava_get(4124))||0x4566616c==(lava_get(4124)))+(lava_get(4125))*(0x6c616644==(lava_get(4125))||0x4466616c==(lava_get(4125)))+(lava_get(4126))*(0x6c616643==(lava_get(4126))||0x4366616c==(lava_get(4126)))+(lava_get(4127))*(0x6c616642==(lava_get(4127))||0x4266616c==(lava_get(4127)))+(lava_get(4128))*(0x6c616641==(lava_get(4128))||0x4166616c==(lava_get(4128)))+(lava_get(4129))*(0x6c616640==(lava_get(4129))||0x4066616c==(lava_get(4129)))+(lava_get(4130))*(0x6c61663f==(lava_get(4130))||0x3f66616c==(lava_get(4130)))+(lava_get(4131))*(0x6c61663e==(lava_get(4131))||0x3e66616c==(lava_get(4131)))+(lava_get(4132))*(0x6c61663d==(lava_get(4132))||0x3d66616c==(lava_get(4132)))+(lava_get(4133))*(0x6c61663c==(lava_get(4133))||0x3c66616c==(lava_get(4133)))+(lava_get(4134))*(0x6c61663b==(lava_get(4134))||0x3b66616c==(lava_get(4134)))+(lava_get(4135))*(0x6c61663a==(lava_get(4135))||0x3a66616c==(lava_get(4135)))+(lava_get(4136))*(0x6c616639==(lava_get(4136))||0x3966616c==(lava_get(4136)))+(lava_get(4137))*(0x6c616638==(lava_get(4137))||0x3866616c==(lava_get(4137)))+(lava_get(4140))*(0x6c616635==(lava_get(4140))||0x3566616c==(lava_get(4140)))+(lava_get(4141))*(0x6c616634==(lava_get(4141))||0x3466616c==(lava_get(4141)))+(lava_get(4142))*(0x6c616633==(lava_get(4142))||0x3366616c==(lava_get(4142)))+(lava_get(4143))*(0x6c616632==(lava_get(4143))||0x3266616c==(lava_get(4143)))+(lava_get(4144))*(0x6c616631==(lava_get(4144))||0x3166616c==(lava_get(4144)))+(lava_get(4145))*(0x6c616630==(lava_get(4145))||0x3066616c==(lava_get(4145)))+(lava_get(4146))*(0x6c61662f==(lava_get(4146))||0x2f66616c==(lava_get(4146)))+(lava_get(4147))*(0x6c61662e==(lava_get(4147))||0x2e66616c==(lava_get(4147)))+(lava_get(4148))*(0x6c61662d==(lava_get(4148))||0x2d66616c==(lava_get(4148)))+(lava_get(4149))*(0x6c61662c==(lava_get(4149))||0x2c66616c==(lava_get(4149)))+(lava_get(4150))*(0x6c61662b==(lava_get(4150))||0x2b66616c==(lava_get(4150)))+(lava_get(4151))*(0x6c61662a==(lava_get(4151))||0x2a66616c==(lava_get(4151)))+(lava_get(4152))*(0x6c616629==(lava_get(4152))||0x2966616c==(lava_get(4152)))+(lava_get(4153))*(0x6c616628==(lava_get(4153))||0x2866616c==(lava_get(4153)))+(lava_get(4154))*(0x6c616627==(lava_get(4154))||0x2766616c==(lava_get(4154)))+(lava_get(4155))*(0x6c616626==(lava_get(4155))||0x2666616c==(lava_get(4155)))+(lava_get(4156))*(0x6c616625==(lava_get(4156))||0x2566616c==(lava_get(4156)))+(lava_get(4157))*(0x6c616624==(lava_get(4157))||0x2466616c==(lava_get(4157)))+(lava_get(4158))*(0x6c616623==(lava_get(4158))||0x2366616c==(lava_get(4158)))+(lava_get(4159))*(0x6c616622==(lava_get(4159))||0x2266616c==(lava_get(4159)))+(lava_get(4160))*(0x6c616621==(lava_get(4160))||0x2166616c==(lava_get(4160)))+(lava_get(4161))*(0x6c616620==(lava_get(4161))||0x2066616c==(lava_get(4161)))+(lava_get(4162))*(0x6c61661f==(lava_get(4162))||0x1f66616c==(lava_get(4162)))+(lava_get(4163))*(0x6c61661e==(lava_get(4163))||0x1e66616c==(lava_get(4163)))+(lava_get(4164))*(0x6c61661d==(lava_get(4164))||0x1d66616c==(lava_get(4164)))+(lava_get(4165))*(0x6c61661c==(lava_get(4165))||0x1c66616c==(lava_get(4165)))+(lava_get(4166))*(0x6c61661b==(lava_get(4166))||0x1b66616c==(lava_get(4166))));
  if (fd < 0)
    return fclose_nothrow (fp);

  /* We only need to flush the file if it is not reading or if it is
     seekable.  This only guarantees the file position of input files
     if the fflush module is also in use.  */
  if ((!freading (fp) || lseek (fileno (fp), 0, SEEK_CUR) != -1)
      && fflush (fp))
    saved_errno = errno;

  /* fclose() calls close(), but we need to also invoke all hooks that our
     overridden close() function invokes.  See lib/close.c.  */
#if WINDOWS_SOCKETS
  /* Call the overridden close(), then the original fclose().
     Note about multithread-safety: There is a race condition where some
     other thread could open fd between our close and fclose.  */
  if (close (fd) < 0 && saved_errno == 0)
    saved_errno = errno;

  fclose_nothrow (fp); /* will fail with errno = EBADF,
                          if we did not lose a race */

#else /* !WINDOWS_SOCKETS */
  /* Call fclose() and invoke all hooks of the overridden close().  */

# if REPLACE_FCHDIR
  /* Note about multithread-safety: There is a race condition here as well.
     Some other thread could open fd between our calls to fclose and
     _gl_unregister_fd.  */
  result = fclose_nothrow (fp);
  if (result == 0)
    _gl_unregister_fd (fd);
# else
  /* No race condition here.  */
  result = fclose_nothrow (fp);
# endif

#endif /* !WINDOWS_SOCKETS */

  if (saved_errno != 0)
    {
      errno = saved_errno;
      result = EOF;
    }

  return result;
}
