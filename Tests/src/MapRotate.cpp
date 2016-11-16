/*
  ThreadedDatabase - a test program for libosmscout
  Copyright (C) 2016  Lukas Karas

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <osmscout/MapPainterNoOp.h>

static const double ringCoords[] = {
  50.08336917, 14.41309835,
  50.08305669, 14.41285159,
  50.08217558, 14.4127148,
  50.08204281, 14.4129106,
  50.08159086, 14.41285159,
  50.0812006, 14.41289719,
  50.08119524, 14.41286768,
  50.08106247, 14.41290523,
  50.08105978, 14.41292669,
  50.08098066, 14.41295083,
  50.08059174, 14.41313054,
  50.07988766, 14.41361066,
  50.07960603, 14.41376354,
  50.07915542, 14.41391106,
  50.07838428, 14.41403981,
  50.07774994, 14.41411759,
  50.07756487, 14.41411223,
  50.07711694, 14.4140559,
  50.07683396, 14.41397007,
  50.07644504, 14.41383328,
  50.07604942, 14.41364552,
  50.07555589, 14.41349264,
  50.07358581, 14.41355969,
  50.07241905, 14.41367771,
  50.07241771, 14.41373404,
  50.07198319, 14.41380377,
  50.07197112, 14.41375013,
  50.07043019, 14.41400226,
  50.06853789, 14.41435363,
  50.06699026, 14.41469159,
  50.06598175, 14.41494908,
  50.0665316, 14.41225882,
  50.06691784, 14.4121113,
  50.06704927, 14.41183771,
  50.06738052, 14.41168215,
  50.06786868, 14.41147562,
  50.06805778, 14.41153999,
  50.06822273, 14.41180285,
  50.06829917, 14.41178407,
  50.06875515, 14.41170897,
  50.06975427, 14.41144611,
  50.07001713, 14.41138442,
  50.07043153, 14.41129323,
  50.07043555, 14.41149171,
  50.07125229, 14.41129591,
  50.07124424, 14.41108938,
  50.07124826, 14.41100087,
  50.07193893, 14.41083457,
  50.07249013, 14.41073801,
  50.07341817, 14.41046979,
  50.07355765, 14.41043492,
  50.07411555, 14.41018011,
  50.07548884, 14.40945323,
  50.0762479, 14.40906431,
  50.07644638, 14.40898116,
  50.07655501, 14.40897043,
  50.07665023, 14.40898384,
  50.0766623, 14.4090053,
  50.07667437, 14.40907772,
  50.07669047, 14.40907504,
  50.07670254, 14.40911259,
  50.0767763, 14.40906968,
  50.07689834, 14.40902408,
  50.0769037, 14.40892752,
  50.07731006, 14.40872099,
  50.07741466, 14.40868344,
  50.07743478, 14.40866734,
  50.07744014, 14.4086432,
  50.07743746, 14.40856542,
  50.07744014, 14.40856542,
  50.07743612, 14.40840985,
  50.07743344, 14.40840985,
  50.0774321, 14.40833475,
  50.07865518, 14.40825697,
  50.0786753, 14.40852519,
  50.0787437, 14.40851982,
  50.07880405, 14.40851178,
  50.07880539, 14.40843936,
  50.07971063, 14.40836962,
  50.08039996, 14.40834011,
  50.08045361, 14.40833475,
  50.08045361, 14.40832939,
  50.08048445, 14.40831597,
  50.08049786, 14.40830525,
  50.08050591, 14.40829452,
  50.08052066, 14.40826233,
  50.08055687, 14.40825697,
  50.08055687, 14.40825965,
  50.08056894, 14.40825697,
  50.0805676, 14.40824355,
  50.08198649, 14.40813358,
  50.08201465, 14.40807458,
  50.08250549, 14.40815772,
  50.08257121, 14.40826501,
  50.08299366, 14.40840449,
  50.08349791, 14.40861638,
  50.08351803, 14.40867807,
  50.08353412, 14.40871294,
  50.08357167, 14.4087639,
  50.08359045, 14.40878,
  50.08382917, 14.40889265,
  50.08382917, 14.40889801,
  50.08385062, 14.40890874,
  50.08385331, 14.40889533,
  50.08394182, 14.40893825,
  50.08394048, 14.40894898,
  50.08402228, 14.40898116,
  50.08402765, 14.40896775,
  50.08402497, 14.40896239,
  50.08405179, 14.40881755,
  50.08448094, 14.4088551,
  50.08527353, 14.40911527,
  50.08565441, 14.40920915,
  50.0866884, 14.40944787,
  50.08719534, 14.4096088,
  50.08723825, 14.40970804,
  50.08743808, 14.40982069,
  50.08750379, 14.40976437,
  50.0875105, 14.40974023,
  50.08750916, 14.40972413,
  50.08757755, 14.40980996,
  50.08758158, 14.40991725,
  50.08759633, 14.40994676,
  50.08759633, 14.40997626,
  50.08757353, 14.40998967,
  50.08754, 14.41003795,
  50.08754805, 14.41011037,
  50.08759365, 14.41015597,
  50.08764595, 14.41031154,
  50.08775458, 14.4105127,
  50.08785382, 14.41075142,
  50.08790881, 14.41083457,
  50.0880228, 14.4108909,
  50.08810729, 14.41095259,
  50.08815557, 14.41099818,
  50.08819848, 14.41102501,
  50.08824274, 14.41100087,
  50.08844927, 14.41107865,
  50.08866653, 14.41111084,
  50.0888221, 14.41125568,
  50.08921236, 14.41141124,
  50.08959994, 14.4115534,
  50.08998752, 14.41174116,
  50.09027988, 14.41204693,
  50.09104297, 14.41268261,
  50.09241089, 14.41404249,
  50.09246454, 14.41415246,
  50.09266302, 14.41445823,
  50.09322494, 14.41550429,
  50.09330005, 14.41566523,
  50.09340331, 14.41586908,
  50.0934194, 14.41601928,
  50.09361789, 14.41639479,
  50.09380832, 14.41691782,
  50.09395048, 14.41772785,
  50.09402692, 14.4181114,
  50.09416103, 14.41875245,
  50.09439975, 14.4201928,
  50.09456873, 14.42161705,
  50.09476721, 14.42362871,
  50.0947766, 14.42392107,
  50.09475648, 14.42399349,
  50.09479672, 14.42409809,
  50.09483963, 14.42413833,
  50.09505689, 14.42519243,
  50.09513468, 14.42637261,
  50.09521648, 14.42714776,
  50.09555042, 14.4294196,
  50.09574086, 14.43037983,
  50.09595946, 14.43145807,
  50.09628534, 14.43265702,
  50.09646237, 14.43319615,
  50.09693444, 14.43497981,
  50.09718523, 14.43602319,
  50.09733141, 14.43670984,
  50.09761036, 14.43786319,
  50.09785712, 14.43895217,
  50.09802744, 14.4400626,
  50.09816423, 14.44134738,
  50.09808377, 14.44516416,
  50.09810254, 14.44640603,
  50.09820446, 14.44767203,
  50.09838954, 14.44887902,
  50.0988321, 14.45074316,
  50.0993216, 14.45240076,
  50.09979501, 14.45383306,
  50.10006994, 14.45460554,
  50.10028318, 14.45507224,
  50.10082766, 14.4561183,
  50.1014325, 14.45701953,
  50.10205075, 14.45780541,
  50.10270521, 14.45844646,
  50.10276154, 14.45857521,
  50.10313436, 14.45879247,
  50.10317996, 14.45869322,
  50.10350317, 14.45888098,
  50.10351256, 14.4589829,
  50.10413215, 14.45933964,
  50.10575354, 14.45955421,
  50.10637313, 14.45938792,
  50.10765925, 14.45885952,
  50.10851488, 14.45833649,
  50.1093021, 14.45772763,
  50.11008397, 14.45685591,
  50.11045143, 14.45638921,
  50.11083767, 14.45579107,
  50.11130706, 14.45499714,
  50.11190385, 14.4538116,
  50.11237726, 14.45237394,
  50.11263073, 14.45133056,
  50.11267096, 14.45104625,
  50.11263073, 14.45100333,
  50.11259317, 14.45108112,
  50.11231691, 14.4519126,
  50.11185423, 14.45304181,
  50.11138886, 14.45387866,
  50.11093691, 14.45455994,
  50.11056811, 14.45491936,
  50.10753453, 14.45752915,
  50.10703966, 14.45787783,
  50.10657162, 14.45812996,
  50.10605395, 14.45827748,
  50.1057294, 14.45827748,
  50.10564089, 14.45697125,
  50.1059708, 14.45688542,
  50.10657564, 14.45662256,
  50.10696188, 14.45639189,
  50.11064991, 14.45323225,
  50.11113539, 14.45277359,
  50.11149481, 14.45235248,
  50.11187568, 14.45178117,
  50.11185557, 14.45171143,
  50.11217743, 14.45108916,
  50.11235178, 14.45063587,
  50.11230216, 14.45052322,
  50.11245102, 14.45017453,
  50.11253819, 14.44968637,
  50.11261731, 14.44875564,
  50.11268303, 14.44775786,
  50.11270315, 14.44650795,
  50.11253953, 14.44275554,
  50.11242956, 14.4420984,
  50.11194408, 14.43843986,
  50.11148006, 14.43618949,
  50.11121586, 14.43467136,
  50.1111193, 14.43387206,
  50.11095569, 14.43244244,
  50.11084169, 14.43062927,
  50.1110214, 14.43007405,
  50.11115819, 14.42967709,
  50.11118099, 14.42965295,
  50.11121318, 14.4290119,
  50.11122927, 14.42852374,
  50.11123464, 14.42741062,
  50.11135936, 14.42743208,
  50.11132985, 14.42773249,
  50.11138484, 14.42775126,
  50.11141837, 14.42746695,
  50.11139289, 14.42745354,
  50.11143178, 14.42701634,
  50.11141569, 14.42700561,
  50.11138484, 14.42708607,
  50.11127219, 14.42706193,
  50.11129901, 14.42689564,
  50.1113312, 14.42676421,
  50.1114358, 14.42655768,
  50.11160076, 14.42544188,
  50.11181131, 14.42446019,
  50.11199907, 14.42380573,
  50.1123853, 14.42275967,
  50.11291504, 14.42167069,
  50.11345148, 14.42047443,
  50.11384577, 14.41939082,
  50.11408046, 14.41822942,
  50.1142025, 14.41705461,
  50.114173, 14.41685345,
  50.1141797, 14.41649939,
  50.11410862, 14.41634651,
  50.11402547, 14.41641088,
  50.11395305, 14.41632237,
  50.11382565, 14.41406395,
  50.11359766, 14.41158559,
  50.1132825, 14.40916355,
  50.11320338, 14.40839644,
  50.11317521, 14.40761055,
  50.11316985, 14.40651353,
  50.11320472, 14.40607901,
  50.11332274, 14.40490957,
  50.11326775, 14.40452333,
  50.11336833, 14.40403517,
  50.1132597, 14.40408613,
  50.11323959, 14.40401908,
  50.11339516, 14.40387155,
  50.11350244, 14.40377768,
  50.11373982, 14.40344508,
  50.11443988, 14.40170433,
  50.11533037, 14.40000917,
  50.11567772, 14.39933594,
  50.11613503, 14.39871367,
  50.11717841, 14.39750131,
  50.11719719, 14.39745035,
  50.11719048, 14.39738597,
  50.11723474, 14.39733501,
  50.11727095, 14.39736452,
  50.11732325, 14.39735915,
  50.11793479, 14.39682807,
  50.11852086, 14.39641501,
  50.11925712, 14.3959349,
  50.12002021, 14.39554061,
  50.1205875, 14.39539846,
  50.12124732, 14.39530726,
  50.12129828, 14.39519461,
  50.1212889, 14.39505245,
  50.12104079, 14.3947735,
  50.12071893, 14.3949398,
  50.1202777, 14.39469035,
  50.11880517, 14.39386691,
  50.11857182, 14.39384546,
  50.11802733, 14.39396079,
  50.11800319, 14.39381595,
  50.11798173, 14.39353968,
  50.1173608, 14.39367916,
  50.11735141, 14.39360674,
  50.11732325, 14.39332511,
  50.11824727, 14.39313199,
  50.1190573, 14.39315345,
  50.11984453, 14.39336534,
  50.12006715, 14.39345385,
  50.11979893, 14.39316149,
  50.11982575, 14.39310248,
  50.12017712, 14.39349945,
  50.12023613, 14.39348336,
  50.12042388, 14.39359065,
  50.12059018, 14.39373817,
  50.12116283, 14.39413514,
  50.12171939, 14.39457234,
  50.12218878, 14.39488884,
  50.12225047, 14.39505245,
  50.12232557, 14.39509537,
  50.12234837, 14.39511146,
  50.12246236, 14.39528044,
  50.12265012, 14.39543064,
  50.1227212, 14.39528044,
  50.12284458, 14.39533408,
  50.12292102, 14.39541455,
  50.12293309, 14.39552988,
  50.12291968, 14.39555939,
  50.12288883, 14.39553525,
  50.12285128, 14.39555671,
  50.1228486, 14.3956023,
  50.12288615, 14.39565863,
  50.1231356, 14.39589466,
  50.12318924, 14.39592685,
  50.123263, 14.39583566,
  50.12360633, 14.39616557,
  50.12376592, 14.39644988,
  50.12416154, 14.39699973,
  50.1245679, 14.3974423,
  50.12496218, 14.39777758,
  50.12522504, 14.39812894,
  50.12526795, 14.39842399,
  50.1256381, 14.39883973,
  50.12632877, 14.39963098,
  50.12696982, 14.40020229,
  50.12722865, 14.40037664,
  50.12724206, 14.40034981,
  50.12719512, 14.40028008,
  50.12708515, 14.4001862,
  50.12707576, 14.40009769,
  50.127116, 14.39995553,
  50.12718842, 14.39987774,
  50.12732655, 14.39989652,
  50.12735874, 14.39997699,
  50.1276806, 14.40005745,
  50.12769535, 14.40000649,
  50.12756527, 14.39990188,
  50.12758672, 14.39984556,
  50.12760282, 14.39971949,
  50.12786165, 14.39976509,
  50.12800649, 14.39990993,
  50.12833774, 14.39998772,
  50.12853623, 14.40036322,
  50.1286529, 14.40046515,
  50.12874812, 14.4005322,
  50.12884334, 14.4005027,
  50.12901232, 14.40050806,
  50.12919337, 14.40040078,
  50.12943074, 14.40032567,
  50.12963727, 14.40018352,
  50.13006106, 14.40000649,
  50.1303494, 14.40059121,
  50.13061091, 14.40112765,
  50.12959302, 14.40172847,
  50.129361, 14.40187331,
  50.1285778, 14.40190818,
  50.12793675, 14.40176066,
  50.12776643, 14.4016936,
  50.12711734, 14.40140124,
  50.12682363, 14.40124567,
  50.12655944, 14.40108206,
  50.12632474, 14.40092381,
  50.12592778, 14.40063681,
  50.12565017, 14.40041687,
  50.12542084, 14.40022375,
  50.12447, 14.39937081,
  50.12358084, 14.39859297,
  50.12326568, 14.39830597,
  50.12323484, 14.39833279,
  50.12320668, 14.39830865,
  50.1232107, 14.39825233,
  50.1229237, 14.39804043,
  50.12252674, 14.39786609,
  50.12241811, 14.3978339,
  50.12240335, 14.39782586,
  50.12229875, 14.39773734,
  50.12202785, 14.39763542,
  50.12168318, 14.39760055,
  50.12159333, 14.3976086,
  50.12148738, 14.39757105,
  50.12133584, 14.39752813,
  50.12109578, 14.39750935,
  50.12103677, 14.39761128,
  50.12086243, 14.39761128,
  50.12068674, 14.39768102,
  50.12056738, 14.39772125,
  50.12049094, 14.39783122,
  50.12044132, 14.39783658,
  50.12036219, 14.39768102,
  50.12023747, 14.39766224,
  50.12004837, 14.39771052,
  50.11999473, 14.39782586,
  50.119638, 14.39793583,
  50.11935502, 14.39807798,
  50.11921153, 14.39818259,
  50.11905998, 14.3982416,
  50.11902377, 14.39822819,
  50.11904389, 14.39815308,
  50.11898354, 14.39816381,
  50.11893928, 14.39825233,
  50.11887089, 14.39833279,
  50.11883199, 14.39835157,
  50.11874616, 14.39844813,
  50.11868849, 14.39854737,
  50.11866167, 14.39850714,
  50.11858657, 14.39856614,
  50.1184015, 14.39874585,
  50.11838541, 14.39883168,
  50.11820167, 14.39907845,
  50.11722401, 14.40014597,
  50.11637107, 14.40110083,
  50.11542022, 14.40217103,
  50.11522442, 14.40249022,
  50.11505678, 14.40296765,
  50.11492536, 14.40324124,
  50.11488915, 14.40355237,
  50.11484355, 14.40370794,
  50.1148127, 14.40371062,
  50.1147376, 14.40399494,
  50.11462092, 14.40452065,
  50.11445999, 14.40495785,
  50.11422798, 14.40573301,
  50.11404023, 14.40599586,
  50.11390746, 14.40636333,
  50.11388466, 14.4068354,
  50.11398658, 14.4068944,
  50.11403352, 14.40709557,
  50.11396781, 14.40718945,
  50.11394233, 14.40789219,
  50.11397585, 14.40823551,
  50.11394367, 14.40854933,
  50.11387125, 14.40859492,
  50.11386186, 14.40879073,
  50.11400804, 14.40975632,
  50.11398122, 14.40989311,
  50.11409253, 14.41134687,
  50.1144962, 14.41414173,
  50.11461288, 14.41518779,
  50.11467993, 14.41621776,
  50.1147081, 14.41629823,
  50.11471748, 14.41635187,
  50.1147148, 14.41642697,
  50.1147832, 14.41652085,
  50.11481673, 14.41678639,
  50.11468932, 14.41706534,
  50.1146169, 14.41793974,
  50.11445329, 14.41899117,
  50.11404559, 14.42065145,
  50.11365935, 14.42205425,
  50.11357084, 14.42234393,
  50.1134032, 14.42219909,
  50.11334553, 14.42225541,
  50.11331603, 14.42240294,
  50.11326239, 14.42246999,
  50.11320606, 14.42275162,
  50.11318326, 14.42289915,
  50.11296332, 14.42402299,
  50.11275277, 14.42501273,
  50.11262536, 14.42588981,
  50.11255965, 14.42651476,
  50.11248455, 14.42778077,
  50.11246711, 14.42800875,
  50.11248455, 14.42804362,
  50.11262134, 14.42806776,
  50.11255562, 14.42882683,
  50.11247516, 14.42996945,
  50.11244029, 14.43092431,
  50.11247784, 14.43190064,
  50.11253819, 14.43254437,
  50.11261463, 14.43306204,
  50.11277691, 14.43407055,
  50.11308536, 14.43610098,
  50.11313096, 14.43634774,
  50.11349171, 14.43854179,
  50.11391282, 14.44130983,
  50.11402145, 14.44242831,
  50.11407912, 14.44342609,
  50.11413142, 14.44486107,
  50.11413142, 14.44549676,
  50.11407912, 14.44614585,
  50.11393562, 14.44751914,
  50.11370495, 14.44891657,
  50.11360303, 14.44978561,
  50.11359766, 14.44998141,
  50.11365935, 14.45010211,
  50.11363655, 14.450255,
  50.11359632, 14.4503301,
  50.11351183, 14.45054467,
  50.11353061, 14.45158537,
  50.11356279, 14.45392426,
  50.1135762, 14.45521976,
  50.1135695, 14.45609685,
  50.11354938, 14.45702757,
  50.11348233, 14.45763375,
  50.11338577, 14.45813264,
  50.11326909, 14.45862349,
  50.1130867, 14.4591492,
  50.11290297, 14.45955421,
  50.11271387, 14.45992436,
  50.11245504, 14.46032401,
  50.11144921, 14.46178045,
  50.11125878, 14.46207281,
  50.11119306, 14.46206744,
  50.11096642, 14.46248319,
  50.11075988, 14.46279969,
  50.10993242, 14.46384575,
  50.10897756, 14.46512248,
  50.1086852, 14.46550604,
  50.10802537, 14.46634557,
  50.10786712, 14.46661379,
  50.107788, 14.46680691,
  50.10771155, 14.46707781,
  50.10763377, 14.46703221,
  50.10767937, 14.46683373,
  50.10764182, 14.46678813,
  50.10760829, 14.46636434,
  50.10755196, 14.46615245,
  50.10742724, 14.46626778,
  50.10746613, 14.46705367,
  50.10740712, 14.46724947,
  50.10696858, 14.46906264,
  50.10682643, 14.46925576,
  50.10503739, 14.46952398,
  50.10501593, 14.46891512,
  50.10674194, 14.46825262,
  50.10685995, 14.4681802,
  50.1069203, 14.46810778,
  50.10696054, 14.46802999,
  50.10699272, 14.46792271,
  50.10706246, 14.46746405,
  50.10705978, 14.46737017,
  50.10702893, 14.46725484,
  50.10700077, 14.46721997,
  50.10503739, 14.46797367,
  50.10499716, 14.46779128,
  50.10499045, 14.46745868,
  50.10500789, 14.46734871,
  50.10501862, 14.46704026,
  50.10582596, 14.46675058,
  50.10639861, 14.46644749,
  50.10667086, 14.46627047,
  50.10712549, 14.46604516,
  50.10731593, 14.46608271,
  50.10739908, 14.46605857,
  50.10741249, 14.46608271,
  50.10746077, 14.46603175,
  50.10761902, 14.46587618,
  50.10771021, 14.46560528,
  50.10778263, 14.46529682,
  50.10795564, 14.46513321,
  50.10809243, 14.46497764,
  50.1086691, 14.4642749,
  50.10972053, 14.46289088,
  50.11047289, 14.46184214,
  50.11091143, 14.4610187,
  50.11088863, 14.46096237,
  50.11052117, 14.46112062,
  50.11020199, 14.46142103,
  50.10972858, 14.46182336,
  50.10949656, 14.46221765,
  50.1094161, 14.46229007,
  50.10916263, 14.46243222,
  50.10875762, 14.46259584,
  50.10814071, 14.46269508,
  50.10788456, 14.46271386,
  50.10705575, 14.46291234,
  50.1064308, 14.46324225,
  50.10547862, 14.46362581,
  50.10439769, 14.46386989,
  50.10438696, 14.4626468,
  50.10464579, 14.46257706,
  50.1054129, 14.46260389,
  50.10588497, 14.46261461,
  50.10625511, 14.46253683,
  50.10672718, 14.46248587,
  50.10790065, 14.4620594,
  50.10863289, 14.46172144,
  50.11126414, 14.459632,
  50.11164904, 14.45919212,
  50.11181265, 14.45892658,
  50.11240542, 14.45794757,
  50.11260927, 14.45759888,
  50.11270046, 14.45740308,
  50.11282921, 14.45700075,
  50.11289626, 14.45675399,
  50.11299148, 14.45632215,
  50.11306658, 14.45588227,
  50.11311889, 14.45542361,
  50.11312559, 14.45491936,
  50.11311889, 14.45487376,
  50.11310682, 14.45482816,
  50.11308536, 14.45480402,
  50.11306658, 14.45478793,
  50.11304513, 14.4547772,
  50.11302501, 14.4547772,
  50.11300221, 14.45478793,
  50.11298075, 14.45480402,
  50.11296064, 14.45483353,
  50.11292577, 14.45489253,
  50.1128909, 14.45496764,
  50.11274204, 14.45537801,
  50.11241213, 14.45614513,
  50.11217341, 14.4566467,
  50.11216536, 14.45663597,
  50.11213183, 14.45670571,
  50.11214122, 14.45671375,
  50.11191726, 14.45713486,
  50.1116249, 14.45762034,
  50.11128426, 14.45812191,
  50.11125743, 14.458079,
  50.11085108, 14.45860739,
  50.1104863, 14.45904191,
  50.11003703, 14.45950862,
  50.10956764, 14.45992972,
  50.1091747, 14.46024354,
  50.10858595, 14.46064856,
  50.10812998, 14.46091946,
  50.10770217, 14.46112331,
  50.10733202, 14.4612896,
  50.1069954, 14.46140226,
  50.10666549, 14.4615015,
  50.10640264, 14.46156855,
  50.10604858, 14.46160879,
  50.10576561, 14.4616222,
  50.10537937, 14.46163024,
  50.10478392, 14.46155514,
  50.10382637, 14.46127083,
  50.10299757, 14.46090068,
  50.10256708, 14.46064856,
  50.10206282, 14.46028646,
  50.10158539, 14.45989754,
  50.10076866, 14.45903118,
  50.10015443, 14.45820506,
  50.09979636, 14.45763643,
  50.0994423, 14.45700612,
  50.09898096, 14.4559976,
  50.09868592, 14.45529487,
  50.09827688, 14.45425685,
  50.09799123, 14.45344682,
  50.09772167, 14.45257511,
  50.09724021, 14.45089604,
  50.09670511, 14.449048,
  50.0964503, 14.44825675,
  50.09609222, 14.44716777,
  50.09581462, 14.4463792,
  50.09546325, 14.44548066,
  50.09509847, 14.44458749,
  50.09486109, 14.44373723,
  50.09467736, 14.44288965,
  50.09452984, 14.4417819,
  50.09446144, 14.4416156,
  50.09437427, 14.44052394,
  50.09425625, 14.43918284,
  50.0941436, 14.43806704,
  50.09404972, 14.43684127,
  50.0938043, 14.43373795,
  50.09360448, 14.43087872,
  50.09352535, 14.42980583,
  50.09345427, 14.42784246,
  50.09341672, 14.42620631,
  50.09337381, 14.42362066,
  50.09327054, 14.42201133,
  50.09315252, 14.42097064,
  50.09295002, 14.41972073,
  50.09271935, 14.41853787,
  50.09244978, 14.4177198,
  50.09223118, 14.41715654,
  50.09193346, 14.4165745,
  50.09168536, 14.41613193,
  50.09140909, 14.41573765,
  50.09036571, 14.41460307,
  50.08944839, 14.4140264,
  50.08881942, 14.41377695,
  50.08845061, 14.41371526,
  50.08784577, 14.41368844,
  50.08711487, 14.41389229,
  50.08702368, 14.41395398,
  50.08660123, 14.41390838,
  50.08662939, 14.41375549,
  50.08703977, 14.41369649,
  50.08704513, 14.41363748,
  50.0868547, 14.41356238,
  50.08664012, 14.41356774,
  50.08643493, 14.41353555,
  50.08639336, 14.41355969,
  50.08623779, 14.41350873,
  50.08613452, 14.4135141,
  50.08604065, 14.41355165,
  50.08610904, 14.41365893,
  50.08611307, 14.41366162,
  50.08607954, 14.4138896,
  50.0859414, 14.41384401,
  50.08579657, 14.41385742,
  50.08565173, 14.41386547,
  50.0856638, 14.41377963,
  50.08575365, 14.41377695,
  50.0857684, 14.41311713,
  50.08577377, 14.4129857,
  50.08625254, 14.41281672,
  50.08624718, 14.41281404,
  50.08618683, 14.41272284,
  50.08581802, 14.41282745,
  50.08542374, 14.41285695,
  50.08538216, 14.41285695,
  50.08535132, 14.41287037,
  50.0853084, 14.41284623,
  50.08527219, 14.412865,
  50.08525342, 14.41291328,
  50.0852561, 14.41296961,
  50.08527756, 14.41301252,
  50.08532316, 14.41303934,
  50.08531109, 14.41373404,
  50.08501738, 14.41360529,
  50.08446217, 14.41331561,
  50.08427173, 14.41320564,
  0.0
};

class TestPainter : public osmscout::MapPainterNoOp{
public:
  TestPainter(const osmscout::StyleConfigRef& styleConfig);
  inline ~TestPainter(){};
  bool IsVisibleAreaPublic(const osmscout::Projection& projection,
                           const std::vector<osmscout::Point>& nodes,
                           double pixelOffset) const;
};

TestPainter::TestPainter(const osmscout::StyleConfigRef& styleConfig):
  MapPainterNoOp(styleConfig)
{

}

bool TestPainter::IsVisibleAreaPublic(const osmscout::Projection& projection,
                                      const std::vector<osmscout::Point>& nodes,
                                      double pixelOffset) const
{
  return IsVisibleArea(projection, nodes, pixelOffset);
}

int main(int /*argc*/, char** /*argv*/)
{
  osmscout::TypeConfigRef typeConfig = std::make_shared<osmscout::TypeConfig>();
  osmscout::StyleConfigRef styleConfig=std::make_shared<osmscout::StyleConfig>(typeConfig);

  TestPainter painter(styleConfig);
  std::vector<osmscout::Point> ring;

  for (int i=0;;i+=2){
    double lat = ringCoords[i];
    if (lat == 0.0)
      break;
    double lon = ringCoords[i+1];
    ring.push_back(osmscout::Point(0, osmscout::GeoCoord(lat, lon)));
  }

  osmscout::Magnification mag;
  mag.SetLevel(15);

  double dpi = 132.78;
  size_t width = 1358;
  size_t height = 809;

  osmscout::GeoCoord center(50.107252570499767, 14.459053009732296);

  int problems = 0;
  osmscout::MercatorProjection  projection;
  for (double angle=0; angle<2*M_PI; angle+= 2*M_PI/32.0){
    projection.Set(center, angle, mag,
                   dpi, width, height
                   );
    double angleDeg = (360*(angle/(2*M_PI)));

    bool visible = painter.IsVisibleAreaPublic(projection, ring, 0.0);
    if (!visible){
      problems+=1;
      std::cerr << "angle " << angle << " (" << angleDeg << "°) => " << visible << std::endl;
    }else{
      std::cout << "angle " << angle << " (" << angleDeg << "°) => " << visible << std::endl;
    }
  }

  return problems;
}

