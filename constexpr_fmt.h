#ifndef CONSTEXPR_IF_H__
#define CONSTEXPR_IF_H__

#include <string>
#include <iostream>
#include <array>
#include <cstring>
#include <cassert>
#include <charconv>
#include <tuple>

#include "Portability.h"

//#define	to_char(n)	((n) + '0')
#define	PADSIZE		16

//static constexpr const char* BLANKS = "                ";
//static constexpr const char* ZEROS = "0000000000000000";


using u_short = unsigned short;
static constexpr const /*u_*/short digit_pairs[100] = {
	0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930,
	0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931,
	0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932,
	0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933,
	0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934,
	0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935,
	0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936,
	0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937,
	0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
	0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939
};

//extern const char* const ZEROS;
//extern const char* const BLANKS;
//extern const short digit_pairs[100];

// Converts value in the range [0, 1000) to a string.
//constexpr inline const char* digits3(size_t value) {
//	// GCC generates slightly better code when value is pointer-size.
//	return &"000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232233234235236237238239240241242243244245246247248249250251252253254255256257258259260261262263264265266267268269270271272273274275276277278279280281282283284285286287288289290291292293294295296297298299300301302303304305306307308309310311312313314315316317318319320321322323324325326327328329330331332333334335336337338339340341342343344345346347348349350351352353354355356357358359360361362363364365366367368369370371372373374375376377378379380381382383384385386387388389390391392393394395396397398399400401402403404405406407408409410411412413414415416417418419420421422423424425426427428429430431432433434435436437438439440441442443444445446447448449450451452453454455456457458459460461462463464465466467468469470471472473474475476477478479480481482483484485486487488489490491492493494495496497498499500501502503504505506507508509510511512513514515516517518519520521522523524525526527528529530531532533534535536537538539540541542543544545546547548549550551552553554555556557558559560561562563564565566567568569570571572573574575576577578579580581582583584585586587588589590591592593594595596597598599600601602603604605606607608609610611612613614615616617618619620621622623624625626627628629630631632633634635636637638639640641642643644645646647648649650651652653654655656657658659660661662663664665666667668669670671672673674675676677678679680681682683684685686687688689690691692693694695696697698699700701702703704705706707708709710711712713714715716717718719720721722723724725726727728729730731732733734735736737738739740741742743744745746747748749750751752753754755756757758759760761762763764765766767768769770771772773774775776777778779780781782783784785786787788789790791792793794795796797798799800801802803804805806807808809810811812813814815816817818819820821822823824825826827828829830831832833834835836837838839840841842843844845846847848849850851852853854855856857858859860861862863864865866867868869870871872873874875876877878879880881882883884885886887888889890891892893894895896897898899900901902903904905906907908909910911912913914915916917918919920921922923924925926927928929930931932933934935936937938939940941942943944945946947948949950951952953954955956957958959960961962963964965966967968969970971972973974975976977978979980981982983984985986987988989990991992993994995996997998999"[value * 3];
//}


//static constexpr const /*u_*/int digit_pairs_large[1000] = {
//	0X30303000, 0X31303000, 0X32303000, 0X33303000, 0X34303000, 0X35303000, 0X36303000, 0X37303000, 0X38303000, 0X39303000, 
//	0X30313000, 0X31313000, 0X32313000, 0X33313000, 0X34313000, 0X35313000, 0X36313000, 0X37313000, 0X38313000, 0X39313000, 
//	0X30323000, 0X31323000, 0X32323000, 0X33323000, 0X34323000, 0X35323000, 0X36323000, 0X37323000, 0X38323000, 0X39323000, 
//	0X30333000, 0X31333000, 0X32333000, 0X33333000, 0X34333000, 0X35333000, 0X36333000, 0X37333000, 0X38333000, 0X39333000, 
//	0X30343000, 0X31343000, 0X32343000, 0X33343000, 0X34343000, 0X35343000, 0X36343000, 0X37343000, 0X38343000, 0X39343000, 
//	0X30353000, 0X31353000, 0X32353000, 0X33353000, 0X34353000, 0X35353000, 0X36353000, 0X37353000, 0X38353000, 0X39353000, 
//	0X30363000, 0X31363000, 0X32363000, 0X33363000, 0X34363000, 0X35363000, 0X36363000, 0X37363000, 0X38363000, 0X39363000, 
//	0X30373000, 0X31373000, 0X32373000, 0X33373000, 0X34373000, 0X35373000, 0X36373000, 0X37373000, 0X38373000, 0X39373000, 
//	0X30383000, 0X31383000, 0X32383000, 0X33383000, 0X34383000, 0X35383000, 0X36383000, 0X37383000, 0X38383000, 0X39383000, 
//	0X30393000, 0X31393000, 0X32393000, 0X33393000, 0X34393000, 0X35393000, 0X36393000, 0X37393000, 0X38393000, 0X39393000, 
//	0X30303100, 0X31303100, 0X32303100, 0X33303100, 0X34303100, 0X35303100, 0X36303100, 0X37303100, 0X38303100, 0X39303100, 
//	0X30313100, 0X31313100, 0X32313100, 0X33313100, 0X34313100, 0X35313100, 0X36313100, 0X37313100, 0X38313100, 0X39313100, 
//	0X30323100, 0X31323100, 0X32323100, 0X33323100, 0X34323100, 0X35323100, 0X36323100, 0X37323100, 0X38323100, 0X39323100, 
//	0X30333100, 0X31333100, 0X32333100, 0X33333100, 0X34333100, 0X35333100, 0X36333100, 0X37333100, 0X38333100, 0X39333100,
//	0X30343100, 0X31343100, 0X32343100, 0X33343100, 0X34343100, 0X35343100, 0X36343100, 0X37343100, 0X38343100, 0X39343100,
//	0X30353100, 0X31353100, 0X32353100, 0X33353100, 0X34353100, 0X35353100, 0X36353100, 0X37353100, 0X38353100, 0X39353100,
//	0X30363100, 0X31363100, 0X32363100, 0X33363100, 0X34363100, 0X35363100, 0X36363100, 0X37363100, 0X38363100, 0X39363100,
//	0X30373100, 0X31373100, 0X32373100, 0X33373100, 0X34373100, 0X35373100, 0X36373100, 0X37373100, 0X38373100, 0X39373100,
//	0X30383100, 0X31383100, 0X32383100, 0X33383100, 0X34383100, 0X35383100, 0X36383100, 0X37383100, 0X38383100, 0X39383100,
//	0X30393100, 0X31393100, 0X32393100, 0X33393100, 0X34393100, 0X35393100, 0X36393100, 0X37393100, 0X38393100, 0X39393100,
//	0X30303200, 0X31303200, 0X32303200, 0X33303200, 0X34303200, 0X35303200, 0X36303200, 0X37303200, 0X38303200, 0X39303200,
//	0X30313200, 0X31313200, 0X32313200, 0X33313200, 0X34313200, 0X35313200, 0X36313200, 0X37313200, 0X38313200, 0X39313200,
//	0X30323200, 0X31323200, 0X32323200, 0X33323200, 0X34323200, 0X35323200, 0X36323200, 0X37323200, 0X38323200, 0X39323200,
//	0X30333200, 0X31333200, 0X32333200, 0X33333200, 0X34333200, 0X35333200, 0X36333200, 0X37333200, 0X38333200, 0X39333200,
//	0X30343200, 0X31343200, 0X32343200, 0X33343200, 0X34343200, 0X35343200, 0X36343200, 0X37343200, 0X38343200, 0X39343200,
//	0X30353200, 0X31353200, 0X32353200, 0X33353200, 0X34353200, 0X35353200, 0X36353200, 0X37353200, 0X38353200, 0X39353200,
//	0X30363200, 0X31363200, 0X32363200, 0X33363200, 0X34363200, 0X35363200, 0X36363200, 0X37363200, 0X38363200, 0X39363200,
//	0X30373200, 0X31373200, 0X32373200, 0X33373200, 0X34373200, 0X35373200, 0X36373200, 0X37373200, 0X38373200, 0X39373200,
//	0X30383200, 0X31383200, 0X32383200, 0X33383200, 0X34383200, 0X35383200, 0X36383200, 0X37383200, 0X38383200, 0X39383200,
//	0X30393200, 0X31393200, 0X32393200, 0X33393200, 0X34393200, 0X35393200, 0X36393200, 0X37393200, 0X38393200, 0X39393200,
//	0X30303300, 0X31303300, 0X32303300, 0X33303300, 0X34303300, 0X35303300, 0X36303300, 0X37303300, 0X38303300, 0X39303300,
//	0X30313300, 0X31313300, 0X32313300, 0X33313300, 0X34313300, 0X35313300, 0X36313300, 0X37313300, 0X38313300, 0X39313300,
//	0X30323300, 0X31323300, 0X32323300, 0X33323300, 0X34323300, 0X35323300, 0X36323300, 0X37323300, 0X38323300, 0X39323300,
//	0X30333300, 0X31333300, 0X32333300, 0X33333300, 0X34333300, 0X35333300, 0X36333300, 0X37333300, 0X38333300, 0X39333300,
//	0X30343300, 0X31343300, 0X32343300, 0X33343300, 0X34343300, 0X35343300, 0X36343300, 0X37343300, 0X38343300, 0X39343300,
//	0X30353300, 0X31353300, 0X32353300, 0X33353300, 0X34353300, 0X35353300, 0X36353300, 0X37353300, 0X38353300, 0X39353300,
//	0X30363300, 0X31363300, 0X32363300, 0X33363300, 0X34363300, 0X35363300, 0X36363300, 0X37363300, 0X38363300, 0X39363300,
//	0X30373300, 0X31373300, 0X32373300, 0X33373300, 0X34373300, 0X35373300, 0X36373300, 0X37373300, 0X38373300, 0X39373300,
//	0X30383300, 0X31383300, 0X32383300, 0X33383300, 0X34383300, 0X35383300, 0X36383300, 0X37383300, 0X38383300, 0X39383300,
//	0X30393300, 0X31393300, 0X32393300, 0X33393300, 0X34393300, 0X35393300, 0X36393300, 0X37393300, 0X38393300, 0X39393300,
//	0X30303400, 0X31303400, 0X32303400, 0X33303400, 0X34303400, 0X35303400, 0X36303400, 0X37303400, 0X38303400, 0X39303400,
//	0X30313400, 0X31313400, 0X32313400, 0X33313400, 0X34313400, 0X35313400, 0X36313400, 0X37313400, 0X38313400, 0X39313400,
//	0X30323400, 0X31323400, 0X32323400, 0X33323400, 0X34323400, 0X35323400, 0X36323400, 0X37323400, 0X38323400, 0X39323400,
//	0X30333400, 0X31333400, 0X32333400, 0X33333400, 0X34333400, 0X35333400, 0X36333400, 0X37333400, 0X38333400, 0X39333400,
//	0X30343400, 0X31343400, 0X32343400, 0X33343400, 0X34343400, 0X35343400, 0X36343400, 0X37343400, 0X38343400, 0X39343400,
//	0X30353400, 0X31353400, 0X32353400, 0X33353400, 0X34353400, 0X35353400, 0X36353400, 0X37353400, 0X38353400, 0X39353400,
//	0X30363400, 0X31363400, 0X32363400, 0X33363400, 0X34363400, 0X35363400, 0X36363400, 0X37363400, 0X38363400, 0X39363400,
//	0X30373400, 0X31373400, 0X32373400, 0X33373400, 0X34373400, 0X35373400, 0X36373400, 0X37373400, 0X38373400, 0X39373400,
//	0X30383400, 0X31383400, 0X32383400, 0X33383400, 0X34383400, 0X35383400, 0X36383400, 0X37383400, 0X38383400, 0X39383400,
//	0X30393400, 0X31393400, 0X32393400, 0X33393400, 0X34393400, 0X35393400, 0X36393400, 0X37393400, 0X38393400, 0X39393400,
//	0X30303500, 0X31303500, 0X32303500, 0X33303500, 0X34303500, 0X35303500, 0X36303500, 0X37303500, 0X38303500, 0X39303500,
//	0X30313500, 0X31313500, 0X32313500, 0X33313500, 0X34313500, 0X35313500, 0X36313500, 0X37313500, 0X38313500, 0X39313500,
//	0X30323500, 0X31323500, 0X32323500, 0X33323500, 0X34323500, 0X35323500, 0X36323500, 0X37323500, 0X38323500, 0X39323500,
//	0X30333500, 0X31333500, 0X32333500, 0X33333500, 0X34333500, 0X35333500, 0X36333500, 0X37333500, 0X38333500, 0X39333500,
//	0X30343500, 0X31343500, 0X32343500, 0X33343500, 0X34343500, 0X35343500, 0X36343500, 0X37343500, 0X38343500, 0X39343500,
//	0X30353500, 0X31353500, 0X32353500, 0X33353500, 0X34353500, 0X35353500, 0X36353500, 0X37353500, 0X38353500, 0X39353500,
//	0X30363500, 0X31363500, 0X32363500, 0X33363500, 0X34363500, 0X35363500, 0X36363500, 0X37363500, 0X38363500, 0X39363500,
//	0X30373500, 0X31373500, 0X32373500, 0X33373500, 0X34373500, 0X35373500, 0X36373500, 0X37373500, 0X38373500, 0X39373500,
//	0X30383500, 0X31383500, 0X32383500, 0X33383500, 0X34383500, 0X35383500, 0X36383500, 0X37383500, 0X38383500, 0X39383500,
//	0X30393500, 0X31393500, 0X32393500, 0X33393500, 0X34393500, 0X35393500, 0X36393500, 0X37393500, 0X38393500, 0X39393500,
//	0X30303600, 0X31303600, 0X32303600, 0X33303600, 0X34303600, 0X35303600, 0X36303600, 0X37303600, 0X38303600, 0X39303600,
//	0X30313600, 0X31313600, 0X32313600, 0X33313600, 0X34313600, 0X35313600, 0X36313600, 0X37313600, 0X38313600, 0X39313600,
//	0X30323600, 0X31323600, 0X32323600, 0X33323600, 0X34323600, 0X35323600, 0X36323600, 0X37323600, 0X38323600, 0X39323600,
//	0X30333600, 0X31333600, 0X32333600, 0X33333600, 0X34333600, 0X35333600, 0X36333600, 0X37333600, 0X38333600, 0X39333600,
//	0X30343600, 0X31343600, 0X32343600, 0X33343600, 0X34343600, 0X35343600, 0X36343600, 0X37343600, 0X38343600, 0X39343600,
//	0X30353600, 0X31353600, 0X32353600, 0X33353600, 0X34353600, 0X35353600, 0X36353600, 0X37353600, 0X38353600, 0X39353600,
//	0X30363600, 0X31363600, 0X32363600, 0X33363600, 0X34363600, 0X35363600, 0X36363600, 0X37363600, 0X38363600, 0X39363600,
//	0X30373600, 0X31373600, 0X32373600, 0X33373600, 0X34373600, 0X35373600, 0X36373600, 0X37373600, 0X38373600, 0X39373600,
//	0X30383600, 0X31383600, 0X32383600, 0X33383600, 0X34383600, 0X35383600, 0X36383600, 0X37383600, 0X38383600, 0X39383600,
//	0X30393600, 0X31393600, 0X32393600, 0X33393600, 0X34393600, 0X35393600, 0X36393600, 0X37393600, 0X38393600, 0X39393600,
//	0X30303700, 0X31303700, 0X32303700, 0X33303700, 0X34303700, 0X35303700, 0X36303700, 0X37303700, 0X38303700, 0X39303700,
//	0X30313700, 0X31313700, 0X32313700, 0X33313700, 0X34313700, 0X35313700, 0X36313700, 0X37313700, 0X38313700, 0X39313700,
//	0X30323700, 0X31323700, 0X32323700, 0X33323700, 0X34323700, 0X35323700, 0X36323700, 0X37323700, 0X38323700, 0X39323700,
//	0X30333700, 0X31333700, 0X32333700, 0X33333700, 0X34333700, 0X35333700, 0X36333700, 0X37333700, 0X38333700, 0X39333700,
//	0X30343700, 0X31343700, 0X32343700, 0X33343700, 0X34343700, 0X35343700, 0X36343700, 0X37343700, 0X38343700, 0X39343700,
//	0X30353700, 0X31353700, 0X32353700, 0X33353700, 0X34353700, 0X35353700, 0X36353700, 0X37353700, 0X38353700, 0X39353700,
//	0X30363700, 0X31363700, 0X32363700, 0X33363700, 0X34363700, 0X35363700, 0X36363700, 0X37363700, 0X38363700, 0X39363700,
//	0X30373700, 0X31373700, 0X32373700, 0X33373700, 0X34373700, 0X35373700, 0X36373700, 0X37373700, 0X38373700, 0X39373700,
//	0X30383700, 0X31383700, 0X32383700, 0X33383700, 0X34383700, 0X35383700, 0X36383700, 0X37383700, 0X38383700, 0X39383700,
//	0X30393700, 0X31393700, 0X32393700, 0X33393700, 0X34393700, 0X35393700, 0X36393700, 0X37393700, 0X38393700, 0X39393700,
//	0X30303800, 0X31303800, 0X32303800, 0X33303800, 0X34303800, 0X35303800, 0X36303800, 0X37303800, 0X38303800, 0X39303800,
//	0X30313800, 0X31313800, 0X32313800, 0X33313800, 0X34313800, 0X35313800, 0X36313800, 0X37313800, 0X38313800, 0X39313800,
//	0X30323800, 0X31323800, 0X32323800, 0X33323800, 0X34323800, 0X35323800, 0X36323800, 0X37323800, 0X38323800, 0X39323800,
//	0X30333800, 0X31333800, 0X32333800, 0X33333800, 0X34333800, 0X35333800, 0X36333800, 0X37333800, 0X38333800, 0X39333800,
//	0X30343800, 0X31343800, 0X32343800, 0X33343800, 0X34343800, 0X35343800, 0X36343800, 0X37343800, 0X38343800, 0X39343800,
//	0X30353800, 0X31353800, 0X32353800, 0X33353800, 0X34353800, 0X35353800, 0X36353800, 0X37353800, 0X38353800, 0X39353800,
//	0X30363800, 0X31363800, 0X32363800, 0X33363800, 0X34363800, 0X35363800, 0X36363800, 0X37363800, 0X38363800, 0X39363800,
//	0X30373800, 0X31373800, 0X32373800, 0X33373800, 0X34373800, 0X35373800, 0X36373800, 0X37373800, 0X38373800, 0X39373800,
//	0X30383800, 0X31383800, 0X32383800, 0X33383800, 0X34383800, 0X35383800, 0X36383800, 0X37383800, 0X38383800, 0X39383800,
//	0X30393800, 0X31393800, 0X32393800, 0X33393800, 0X34393800, 0X35393800, 0X36393800, 0X37393800, 0X38393800, 0X39393800, 
//	0X30303900, 0X31303900, 0X32303900, 0X33303900, 0X34303900, 0X35303900, 0X36303900, 0X37303900, 0X38303900, 0X39303900,
//	0X30313900, 0X31313900, 0X32313900, 0X33313900, 0X34313900, 0X35313900, 0X36313900, 0X37313900, 0X38313900, 0X39313900,
//	0X30323900, 0X31323900, 0X32323900, 0X33323900, 0X34323900, 0X35323900, 0X36323900, 0X37323900, 0X38323900, 0X39323900,
//	0X30333900, 0X31333900, 0X32333900, 0X33333900, 0X34333900, 0X35333900, 0X36333900, 0X37333900, 0X38333900, 0X39333900,
//	0X30343900, 0X31343900, 0X32343900, 0X33343900, 0X34343900, 0X35343900, 0X36343900, 0X37343900, 0X38343900, 0X39343900,
//	0X30353900, 0X31353900, 0X32353900, 0X33353900, 0X34353900, 0X35353900, 0X36353900, 0X37353900, 0X38353900, 0X39353900, 
//	0X30363900, 0X31363900, 0X32363900, 0X33363900, 0X34363900, 0X35363900, 0X36363900, 0X37363900, 0X38363900, 0X39363900,
//	0X30373900, 0X31373900, 0X32373900, 0X33373900, 0X34373900, 0X35373900, 0X36373900, 0X37373900, 0X38373900, 0X39373900, 
//	0X30383900, 0X31383900, 0X32383900, 0X33383900, 0X34383900, 0X35383900, 0X36383900, 0X37383900, 0X38383900, 0X39383900, 
//	0X30393900, 0X31393900, 0X32393900, 0X33393900, 0X34393900, 0X35393900, 0X36393900, 0X37393900, 0X38393900, 0X39393900
//};


//template <size_t N/*, class T*/>
//std::tuple<char*, size_t> formatDec(char(&buf)[N], uintmax_t d) {
//	char* it = &buf[N - 3];
//	intmax_t div = d / 1000;
//
//	while (div) {
//		memcpy(it, digits3(d - div * 1000), 3);
//
//		d = div;
//		it -= 3;
//		div = d / 1000;
//		// d > 0
//	}
//	// d < 100
//	memcpy(it, digits3(d), 3);
//
//	if (d < 10) {
//		it += 2;
//	}
//	else if (d < 100) {
//		it++;
//	}
//
//	return std::make_tuple(it, &buf[N] - it);
//
//}

template <size_t N/*, class T*/>
std::tuple<char*, size_t> formatDec(char(&buf)[N], uintmax_t d) {
	char* it = &buf[N - 2];
	intmax_t div = d / 100;

	while (div) {
		*reinterpret_cast</*u_*/short*>(it) = digit_pairs[d - div * 100];

		d = div;
		it -= 2;
		div = d / 100;
		// d > 0
	}
	// d < 100

	*reinterpret_cast</*u_*/short*>(it) = digit_pairs[d];

	if (d < 10) {
		it++;
	}

	return std::make_tuple(it, &buf[N] - it);

}


//template <bool Upper, size_t Divisor, class T, size_t N>
//std::tuple<const char*, size_t> format(char(&buf)[N], T d) {
//
//	if constexpr (Divisor == 10) {
//		/*static constexpr const char digit_pairs[201] = { "00010203040506070809"
//														"10111213141516171819"
//														"20212223242526272829"
//														"30313233343536373839"
//														"40414243444546474849"
//														"50515253545556575859"
//														"60616263646566676869"
//														"70717273747576777879"
//														"80818283848586878889"
//														"90919293949596979899" };*/
//
//		char* it = &buf[N - 2];
//		if constexpr (std::is_signed<T>::value) {
//			if (d >= 0) {
//				int div = d / 100;
//				while (div) {
//					std::memcpy(it, &digit_pairs[2 * (d - div * 100)], 2);
//					d = div;
//					it -= 2;
//					div = d / 100;
//				}
//
//				std::memcpy(it, &digit_pairs[2 * d], 2);
//
//				if (d < 10) {
//					it++;
//				}
//			}
//			else {
//				int div = d / 100;
//				while (div) {
//					std::memcpy(it, &digit_pairs[-2 * (d - div * 100)], 2);
//					d = div;
//					it -= 2;
//					div = d / 100;
//				}
//
//				std::memcpy(it, &digit_pairs[-2 * d], 2);
//
//				if (d <= -10) {
//					it--;
//				}
//
//				*it = '-';
//			}
//		}
//		else {
//			if (d >= 0) {
//				int div = d / 100;
//				while (div) {
//					std::memcpy(it, &digit_pairs[2 * (d - div * 100)], 2);
//					d = div;
//					it -= 2;
//					div = d / 100;
//				}
//
//				std::memcpy(it, &digit_pairs[2 * d], 2);
//
//				if (d < 10) {
//					it++;
//				}
//			}
//		}
//
//		return std::make_tuple(it, &buf[N] - it);
//	}
//	else if constexpr (Divisor == 16) {
//		[[maybe_unused]] static constexpr const char xdigit_pairs_l[513] = { "000102030405060708090a0b0c0d0e0f"
//																			"101112131415161718191a1b1c1d1e1f"
//																			"202122232425262728292a2b2c2d2e2f"
//																			"303132333435363738393a3b3c3d3e3f"
//																			"404142434445464748494a4b4c4d4e4f"
//																			"505152535455565758595a5b5c5d5e5f"
//																			"606162636465666768696a6b6c6d6e6f"
//																			"707172737475767778797a7b7c7d7e7f"
//																			"808182838485868788898a8b8c8d8e8f"
//																			"909192939495969798999a9b9c9d9e9f"
//																			"a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
//																			"b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
//																			"c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
//																			"d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
//																			"e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
//																			"f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff" };
//
//		[[maybe_unused]] static constexpr const char xdigit_pairs_u[513] = { "000102030405060708090A0B0C0D0E0F"
//																			"101112131415161718191A1B1C1D1E1F"
//																			"202122232425262728292A2B2C2D2E2F"
//																			"303132333435363738393A3B3C3D3E3F"
//																			"404142434445464748494A4B4C4D4E4F"
//																			"505152535455565758595A5B5C5D5E5F"
//																			"606162636465666768696A6B6C6D6E6F"
//																			"707172737475767778797A7B7C7D7E7F"
//																			"808182838485868788898A8B8C8D8E8F"
//																			"909192939495969798999A9B9C9D9E9F"
//																			"A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
//																			"B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
//																			"C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
//																			"D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
//																			"E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
//																			"F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF" };
//
//		// NOTE(eteran): we include the x/X, here as an easy way to put the
//		//               upper/lower case prefix for hex numbers
//		[[maybe_unused]] static constexpr const char alphabet_l[] = "0123456789abcdefx";
//		[[maybe_unused]] static constexpr const char alphabet_u[] = "0123456789ABCDEFX";
//
//		typename std::make_unsigned<T>::type ud = d;
//
//		char* p = buf + N;
//
//		if (ud >= 0) {
//			while (ud > 16) {
//				p -= 2;
//				if constexpr (Upper) {
//					std::memcpy(p, &xdigit_pairs_u[2 * (ud & 0xff)], 2);
//				}
//				else {
//					std::memcpy(p, &xdigit_pairs_l[2 * (ud & 0xff)], 2);
//				}
//				ud /= 256;
//			}
//
//			while (ud > 0) {
//				p -= 1;
//				if constexpr (Upper) {
//					std::memcpy(p, &xdigit_pairs_u[2 * (ud & 0x0f) + 1], 1);
//				}
//				else {
//					std::memcpy(p, &xdigit_pairs_l[2 * (ud & 0x0f) + 1], 1);
//				}
//				ud /= 16;
//			}
//
//		}
//
//		return std::make_tuple(p, (buf + N) - p);
//	}
//	else if constexpr (Divisor == 8) {
//		static constexpr const char digit_pairs[129] = { "0001020304050607"
//														"1011121314151617"
//														"2021222324252627"
//														"3031323334353637"
//														"4041424344454647"
//														"5051525354555657"
//														"6061626364656667"
//														"7071727374757677" };
//		typename std::make_unsigned<T>::type ud = d;
//
//		char* p = buf + N;
//
//		if (ud >= 0) {
//			while (ud > 64) {
//				p -= 2;
//				std::memcpy(p, &digit_pairs[2 * (ud & 077)], 2);
//				ud /= 64;
//			}
//
//			while (ud > 0) {
//				p -= 1;
//				std::memcpy(p, &digit_pairs[2 * (ud & 007) + 1], 1);
//				ud /= 8;
//			}
//		}
//
//		return std::make_tuple(p, (buf + N) - p);
//	}
//	if constexpr (Divisor == 2) {
//		static constexpr const char digit_pairs[9] = { "00011011" };
//
//		typename std::make_unsigned<T>::type ud = d;
//
//		char* p = buf + N;
//
//		if (ud >= 0) {
//			while (ud > 4) {
//				p -= 2;
//				std::memcpy(p, &digit_pairs[2 * (ud & 0x03)], 2);
//				ud /= 4;
//			}
//
//			while (ud > 0) {
//				p -= 1;
//				std::memcpy(p, &digit_pairs[2 * (ud & 0x01) + 1], 1);
//				ud /= 2;
//			}
//		}
//
//		return std::make_tuple(p, (buf + N) - p);
//	}
//}







// A non-type template - parameter shall have one of the following(optionally cv-qualified) types:
// integral or enumeration type,
// pointer to object or pointer to function,
// lvalue reference to object or lvalue reference to function,
// pointer to member,
// std::nullptr_t

/*
 * Flags used during conversion.
 */

#define	__FLAG_ALT         0x008  /* show prefixes 0x or 0 and show the decimal point even if no fractional digits present */
#define __FLAG_ZEROPAD     0x002  /* zero (as opposed to blank) pad */
#define	__FLAG_LADJUST     0x004  /* left adjustment */

#define	__FLAG_LONGDBL     0x010  /* long double */
#define	__FLAG_LONGINT     0x020  /* long integer */
#define	__FLAG_LLONGINT    0x040  /* long long integer */
#define	__FLAG_SHORTINT    0x080  /* short integer */
 /* C99 additional size modifiers: */
#define	__FLAG_SIZET       0x100  /* size_t */
#define	__FLAG_PTRDIFFT	   0x200  /* ptrdiff_t */
#define	__FLAG_INTMAXT     0x400  /* intmax_t */
#define	__FLAG_CHARINT     0x800  /* print char using int format */

//#define	__FLAG_INTEGER_SIZE (__FLAG_LONGINT|__FLAG_LLONGINT|__FLAG_SHORTINT|__FLAG_SIZET|__FLAG_PTRDIFFT|__FLAG_INTMAXT|__FLAG_CHARINT)

 // #define	FPT		    0x100     /* Floating point number */
 // #define	GROUPING	0x200     /* use grouping ("'" flag) */

#define DYNAMIC_WIDTH      0x80000000
#define DYNAMIC_PRECISION  0x80000000

#define BUF 400

// A macro to disallow the copy constructor and operator= functions
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete;


/** used by CFMT_STR */
struct OutbufArg {

	//static const char* BLANKS;
	//static const char* ZEROS;

	template <size_t N>
	OutbufArg(char(&buffer)[N]) 
		: pBuf_(buffer), pBufEnd_(buffer + N), written_(0) {
	}

	OutbufArg(char* buffer, size_t size) 
		: pBuf_(buffer), pBufEnd_(buffer + size), written_(0) {
	}

	size_t getWrittenNum() {
		return written_;
	}

	void write(char ch) noexcept {
		++written_;

		// check if sufficient free space remains in the buffer
		// the last byte position is reserved for the terminating '\0' 
		size_t remaining = static_cast<size_t>(pBufEnd_ - pBuf_ - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) <= 0)
			return;

		else {
			*pBuf_++ = ch;
		}
	}

	/**
	 * Copies [n] bytes from source p to destination pBuf_, leaving pBuf_
	 * pointing at byte following block copied.
	 * If [n] exceeds the number of bytes available in pBuf_, only the number
	 * of bytes that fit within the buffer are copied. In this case [n] is
	 * still added to written_ (although no further copying will be performed)
	 * so that written_ can record the number of characters that would have
	 * been copied if the supplied buffer was of sufficient size.
	 */
	void write(const char* p, size_t n) {
		written_ += n;

		// check if sufficient free space remains in the buffer
		// the last byte position is reserved for the terminating '\0' 
		size_t remaining = static_cast<size_t>(pBufEnd_ - pBuf_ - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) <= 0)
			return;

		else if (n > remaining)
			n = remaining;

		std::memcpy(pBuf_, p, n);
		pBuf_ += n;
	}

	template </*const char* const**/ char padding>
	void writePaddings(int n/*, const char padding = ' '*/) noexcept {
		if (n <= 0) return;

		written_ += n;

		// check if sufficient free space remains in the buffer
		// the last byte position is reserved for the terminating '\0' 
		size_t remaining = static_cast<size_t>(pBufEnd_ - pBuf_ - 1);

		// fail if at the end of buffer, recall need a single byte for null
		if (static_cast<std::make_signed<size_t>::type>(remaining) <= 0)
			return;

		else if (n > remaining)
			n = static_cast<int>(remaining);

		//while (n > PADSIZE) {
		//	std::memcpy(pBuf_, *padding, PADSIZE);
		//	n -= PADSIZE;
		//	pBuf_ += PADSIZE;
		//}
		//std::memcpy(pBuf_, *padding, n);
		//pBuf_ += n;

		while (n-- > 0) {
			*pBuf_++ = padding;
		}
	}

	void done() noexcept {
		if (static_cast<size_t>(pBufEnd_ - pBuf_) > 0) 
			*pBuf_ = '\0';
	}

private:

	// running pointer to the next char
	char* pBuf_/*{ nullptr }*/;
	// const pointer to buffer end
	char* const pBufEnd_;

	// pointer to buffer end
	// char* pBufEnd_{ nullptr };

	// writing buffer size
	// size_t size_/*{ 0 }*/;

	// record the number of characters that would have been written if writing
	// buffer had been sufficiently large, not counting the terminating null 
	// character. 
	size_t written_/*{ 0 }*/;

	DISALLOW_COPY_AND_ASSIGN(OutbufArg);
};


//#define PAD(howmany, with) { \
//    int n; \
//    if ((n = (howmany)) > 0) { \
//	    while (n > PADSIZE) { \
//            outbuf.write(with, PADSIZE); \
//	        n -= PADSIZE; \
//	    } \
//        outbuf.write(with, static_cast<size_t>(n)); \
//	} \
//}

//inline void PAD(int howmany, const char* with, OutbufArg& outbuf) {
//	if (howmany > 0) {
//		while (howmany > PADSIZE) {
//			outbuf.write(with, PADSIZE);
//			howmany -= PADSIZE;
//		}
//		outbuf.write(with, static_cast<size_t>(howmany));
//	}
//}


using flags_t = int;
using width_t = int;
using precision_t = int;
using sign_t = char;
using terminal_t = char;
using begin_t = unsigned;
using end_t = unsigned;
using width_first_t = bool;
/**
 * Stores the static format information associated with a specifier in a format
 * string (i.e. %<flags><width>.<precision><length><terminal>).
 */
struct SpecInfo {
	begin_t begin_ = 0;
	end_t end_ = 0;
	flags_t flags_ = 0;
	width_t width_ = 0;
	precision_t prec_ = -1;
	sign_t sign_ = '\0';
	terminal_t terminal_ = '\0';
	width_first_t wFirst_ = true; // indicate which one was extracted first in the 
								  // argument list when width and precision are both
								  // dynamically resolved
};
constexpr int LEN = sizeof(SpecInfo);

/**
 * Stores the static format information associated with a CFMT_STR invocation site.
 */
 //struct StaticFmtInfo {
 //	// With constexpr constructors, objects of user-defined types can be
 //	// included in valid constant expressions.
 //	// Definitions of constexpr constructors must satisfy the following 
 //	// requirements:
 //	// - The containing class must not have any virtual base classes.
 //	// - Each of the parameter types is a literal type. 
 //	//   (all reference types are literal types)
 //	// - Its function body is = delete or = default; otherwise, it must satisfy
 //	//   the following constraints:
 //	//   1) It is not a function try block.
 //	//   2) The compound statement in it must contain only the following statements:
 //	//      null statements, static_assert declarations,
 //	//      typedef declarations that do not define classes or enumerations,
 //	//      using directives, using declarations
 //	// - Each nonstatic data member and base class subobject is initialized.
 //	// - Each constructor that is used for initializing nonstatic data members
 //	//   and base class subobjects is a constexpr constructor.
 //	// - Initializers for all nonstatic data members that are not named by a 
 //	//   member initializer identifier are constant expressions.
 //	// - When initializing data members, all implicit conversions that are 
 //	//   involved in the following context must be valid in a constant expression:
 //	//   Calling any constructors, Converting any expressions to data member types
 //	constexpr StaticFmtInfo(const char* fmtString, const int numSpecs,
 //		const int numVarArgs, const SpecInfo* fmtInfos)
 //		: /*filename_(filename)
 //		, lineNum_(lineNum)
 //		, severity_(severity)
 //		,*/ formatString_(fmtString)
 //		, numSpecs_(numSpecs)
 //		, numVarArgs_(numVarArgs)
 //		, fmtInfos_(fmtInfos)
 //	{ }
 //
 //
 //	//// File where the log invocation is invoked
 //	//const char* filename_;
 //
 //	//// Line number in the file for the invocation
 //	//const uint32_t lineNum_;
 //
 //	//// LogLevel severity associated with the log invocation
 //	//const uint8_t severity_;
 //
 //	// printf format string associated with the log invocation
 //	const char* formatString_;
 //
 //	// Number of fmt specifiers fetched from format string
 //	const int numSpecs_;
 //
 //	// Number of variadic arguments required for CFMT_STR invocation
 //	const int numVarArgs_;
 //
 //	// Mapping of detailed infos of fmt specifiers as inferred from CFMT_STR invocation.
 //	const SpecInfo* fmtInfos_;
 //};


/**
 * Checks whether a character is with the terminal set of format specifier
 * characters according to the printf specification:
 * http://www.cplusplus.com/reference/cstdio/printf/
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is in the set, indicating the end of the specifier
 */
constexpr inline bool
isTerminal(char c) {
	return c == 'd' || c == 'i'
		|| c == 'u' || c == 'o'
		|| c == 'x' || c == 'X'
		|| c == 'f' || c == 'F'
		|| c == 'e' || c == 'E'
		|| c == 'g' || c == 'G'
		|| c == 'a' || c == 'A'
		|| c == 'c' || c == 'p'
		|| c == 's' || c == 'n';
}

/**
 * Checks whether a character is in the set of characters that specifies
 * a flag according to the printf specification:
 * http://www.cplusplus.com/reference/cstdio/printf/
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is in the set
 */
constexpr inline bool
isFlag(char c) {
	return c == '-' || c == '+' || c == ' ' || c == '#' || c == '0';
}

/**
 * Checks whether a character is in the set of characters that specifies
 * a length field according to the printf specification:
 * http://www.cplusplus.com/reference/cstdio/printf/
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is in the set
 */
constexpr inline bool
isLength(char c) {
	return c == 'h' || c == 'l' || c == 'j'
		|| c == 'z' || c == 't' || c == 'L';
}

/**
 * Checks whether a character is a digit (0-9) or not.
 *
 * \param c
 *      character to check
 * \return
 *      true if the character is a digit
 */
constexpr inline bool
isDigit(char c) {
	return (c >= '0' && c <= '9');
}

constexpr inline int
toDigit(char c) {
	return static_cast<int>(c - '0');
}


/**
 * Analyzes a static printf style format string and extracts info of the num-th format specifier
 * would be used in a corresponding CFMT_STR() invocation.
 *
 * \tparam N
 *      Length of the static format string (automatically deduced)
 * \param fmt
 *      Format c style string to parse
 * \param num
 *      select the num-th format specifier to analyze (starts from zero)
 * \return
 *      Returns an SpecInfo object describing the detailed information of num-th format specifier
 */
//template<int N>
constexpr inline SpecInfo
getOneSepc(/*const char(&fmt)[N]*/const char* fmt, int num = 0) {
	begin_t begin = 0;
	end_t end = 0;
	flags_t flags = 0;
	width_t width = 0;
	precision_t prec = -1;
	sign_t sign = '\0';
	terminal_t terminal = '\0';
	width_first_t widthFirst = true;

	int intWidth = 0;
	int pos = 0;
	int specStartPos = 0;
	int sizeOfInvalidSpecs = 0;
	bool exit = false;

	//const char* fmt = *pFmt;
	//int num = N;

	while (num > 0) {
		// consume the current non-format string until reaching the next '%'
		// if num is greater than 0, fmt[pos] can never be '\0'
		for (; /*(fmt[pos] != '\0') && */(fmt[pos] != '%'); pos++);

		specStartPos = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			if (--num <= 0) begin = pos + 1 - sizeOfInvalidSpecs;
		}
		else { // isTerminal(fmt[pos]) == false
			// pos - specStartPos is the size of this invlaid specifier
			sizeOfInvalidSpecs += pos - specStartPos;
		}

		pos++;   // skips over '%', terminal, or non-terminal
	}

	while (true) {
		// consume the current non-format string until reaching the next '%' or the final '\0'
		for (; (fmt[pos] != '\0') && (fmt[pos] != '%'); pos++);

		if (fmt[pos] == '\0') {
			end = pos - sizeOfInvalidSpecs;
			exit = true;
			break;
		}

		specStartPos = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			end = specStartPos - sizeOfInvalidSpecs;
			break;
		}
		else { // isTerminal(fmt[pos]) == false
			// pos - specStartPos is the size of this invlaid specifier
			sizeOfInvalidSpecs += pos - specStartPos;
		}

		if (fmt[pos] == '\0') {
			end = pos - sizeOfInvalidSpecs;
			exit = true;
			break;
		}

		pos++;   // skips over '%' or non-terminal
	}

	pos = specStartPos + 1;  // fmt[specStartPos] == '%', so specStartPos + 1 over '%'

	while (!exit) {

		switch (fmt[pos]) {
		case ' ':
			/*-
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 *	-- ANSI X3J11
			 */
			if (!sign)
				sign = ' ';
			break;
		case '#':
			flags |= __FLAG_ALT;
			break;
		case '*':
			/*-
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			width = DYNAMIC_WIDTH;
			if (prec == DYNAMIC_PRECISION)
				widthFirst = false;
			break;
		case '-':
			flags |= __FLAG_LADJUST;
			break;
		case '+':
			sign = '+';
			break;
		case '.':
			pos++;
			if (fmt[pos] == '*') {
				prec = DYNAMIC_PRECISION;
				if (width == DYNAMIC_WIDTH)
					widthFirst = true;
				break;
			}
			prec = 0;
			while (isDigit(fmt[pos])) {
				prec = 10 * prec + toDigit(fmt[pos]);
				pos++;
			}
			pos--;
			break;
		case '0':
			/*-
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= __FLAG_ZEROPAD;
			break;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			width = 0;
			do {
				width = 10 * width + toDigit(fmt[pos]);
				pos++;
			} while (isDigit(fmt[pos]));

			pos--;
			break;

		case 'L':
			flags |= __FLAG_LONGDBL;
			break;

		case 'h':
			pos++;
			if (fmt[pos] == 'h') {
				intWidth = __FLAG_CHARINT;
				break;
			}
			else {
				intWidth = __FLAG_SHORTINT;
				pos--;
				break;
			}
		case 'l':
			pos++;
			if (fmt[pos] == 'l') {
				intWidth = __FLAG_LLONGINT;
				break;
			}
			else {
				intWidth = __FLAG_LONGINT;
				pos--;
				break;
			}
		case 'j':
			intWidth = __FLAG_INTMAXT;
			break;
		case 't':
			intWidth = __FLAG_PTRDIFFT;
			break;
		case 'z':
			intWidth = __FLAG_SIZET;
			break;

		case 'c': case 'd': case 'i': case 'a': case 'A': case 'e': case 'E':
		case 'f': case 'F': case 'g': case 'G': case 'n': case 'o': case 'p':
		case 's': case 'u': case 'X': case 'x':
			terminal = fmt[pos];
			// end = pos + 1;
			exit = true;
			break;

		default: /* should never happen */
			abort();
			break;
		}

		pos++;
	}

	flags |= intWidth;
	//begin -= sizeOfInvalidSpecs;
	//end -= sizeOfInvalidSpecs;
	return { begin, end, flags, width, prec, sign, terminal, widthFirst };
}


/**
 * Helper to analyzeFormatString. This level of indirection is needed to
 * unpack the index_sequence generated in analyzeFormatString and
 * use the sequence as indices for calling getParamInfo.
 *
 * \tparam N
 *      Length of the format string (automatically deduced)
 * \tparam Indices
 *      An index sequence from [0, Num) where Num is the number of fmt
 *      specifiers in the format string (automatically deduced)
 *
 * \param fmt
 *      printf format string to analyze
 *
 * \return
 *      An std::array of format specifiers that describes the detailed
 *      info (position of the head '%', position of the end of specifier,
 *      width, precision, sign and terminal) of each specifier.
 */
 // std::integer_sequence Defined in header <utility>
 // template< class T, T... Ints >
 // class integer_sequence;
 // T: The type of the values; must be an integral type : bool, char, char16_t, 
 //    char32_t, wchar_t, or signed or unsigned integer types.
 // Ints: A non-type parameter pack that represents a sequence of values of
 //       integral type T.
 // The class template std::integer_sequence represents a compile-time sequence
 // of integers. When used as an argument to a function template, the parameter 
 // pack Ints can be deduced and used in pack expansion.
 // std::integer_sequence::size
 // static constexpr std::size_t size() noexcept;
 // Returns the number of elements in Ints. Equivalent to sizeof...(Ints)
 // A helper alias template std::index_sequence is defined for the common case 
 // where T is std::size_t:
 // template<std::size_t... Ints>
 // using index_sequence = std::integer_sequence<std::size_t, Ints...>;
 // Helper alias templates std::make_integer_sequence and std::make_index_sequence
 // are defined to simplify creation of std::integer_sequence and 
 // std::index_sequence types, respectively, with 0, 1, 2, ..., N-1 as Ints:
 // template<class T, T N>
 // using make_integer_sequence = std::integer_sequence<T, /* a sequence 0, 1, 2, ..., N-1 */ >;
 // template<std::size_t N>
 // using make_index_sequence = std::make_integer_sequence<std::size_t, N>;
 // The program is ill-formed if N is negative. If N is zero, the indicated type is integer_sequence<T>.
template<int N, std::size_t... Indices>
constexpr /*auto*/std::array<SpecInfo, sizeof...(Indices)> // Returns the number of elements in pack Indices
analyzeFormatStringHelper(const char(&fmt)[N]/*const char* fmt*/, std::index_sequence<Indices...>) {
	return { { getOneSepc(fmt, Indices)... } };
	//return std::make_tuple(getOneSepc(fmt, Indices)...);
}


/**
 * Computes a fmt_info_t array describing format specifiers (subsequences
 * beginning with %) that would be used with the provided printf style format
 * string. The indices of the array correspond with one of the specifiers in
 * format string.
 *
 * \template NParams
 *      The number of the format specifiers that follow the format
 *      string in a printf-like function.
 *      For example printf("%*.*ddsdf%%, %ll-+ K", 9, 8, 7)
 *      would have NParams = 3
 * \template N
 *      length of the printf style format string (automatically deduced)
 *
 * \param fmt
 *      Format string to generate the array for
 *
 * \return
 *      An std::array of format specifiers that describes the detailed
 *      info (position of the head '%', position of the end of specifier,
 *      width, precision, sign and terminal) of each specifier.
 */
template<int NParams, size_t N>
constexpr std::array<SpecInfo, NParams>/*auto*/
analyzeFormatString(const char(&fmt)[N]/*const char* fmt*/) {
	return analyzeFormatStringHelper(fmt, std::make_index_sequence<NParams>{});
}


template<int N>
constexpr inline char
storeOneChar(const char(&fmt)[N], int& offset, int num = 0) {
	int saveOffset = offset;

	// "sd%%sf%%%fes%h-+ 01233lzhhjt *.***k12.dsdsss%h-+ 01233lzhhjt *.***k23.\n"
	//  num    0 1 2 3 4 5 6 7 8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25  26 ...                
	//  offset 0 0 1 1 1 2 2 2 2  2  24 24 24 24 24 24 24 24 24 24 46 46 46 46 46 46  46 ...        
	//  index  0 1 3 4 5 7 8 9 10 11 34 35 36 37 38 39 40 41 42 43 66 67 68 69 70 71  72 ...
	//  char   s d % s f % % f e  s  k  1  2  .  d  s  d  s  s  s  k  2  3  .  \n \0  \0 ...            
	if (num + offset < N) {
		if (fmt[num + offset] == '%') {
			offset++;   // fmt[num + offset] == '%', so offset++ skips over '%'

			while (isFlag(fmt[num + offset]) || isDigit(fmt[num + offset])
				|| isLength(fmt[num + offset]) || fmt[num + offset] == '.'
				|| fmt[num + offset] == '*') {
				offset++;
			}

			if (isTerminal(fmt[num + offset])) {
				offset = saveOffset;
			}
		}

		return fmt[num + offset];

	}
	else
		return '\0';
}

template<int N, std::size_t... Indices>
constexpr std::array<char, sizeof...(Indices)> // Returns the number of elements in pack Indices
preprocessInvalidSpecsHelper(const char(&fmt)[N], std::index_sequence<Indices...>) {
	int offset = 0;
	return { { storeOneChar(fmt, offset, Indices)... } };
}

template<int Len, size_t N>
constexpr std::enable_if_t < Len < N, std::array<char, Len>>
	preprocessInvalidSpecs(const char(&fmt)[N]) {
	return preprocessInvalidSpecsHelper(fmt, std::make_index_sequence<Len>{});
}

template<int Len, size_t N>
constexpr std::enable_if_t< Len == N, std::array<char, 0>>
preprocessInvalidSpecs(const char(&fmt)[N]) {
	return std::array<char, 0>();
}

/*
 * Count the number of valid specifiers in the format string at compile time
 */
template<int N>
constexpr inline int
countValidSpecInfos(const char(&fmt)[N]) {
	int pos = 0;
	int count = 0;
	// A C string is usually declared as an array of char. However, an array 
	// of char is NOT by itself a C string. A valid C string requires the 
	// presence of a terminating "null character" (a character with ASCII
	// value 0, usually represented by the character literal '\0').
	for (;;) {
		// consume the current non-format string until reaching the next '%' 
		for (; (fmt[pos] != '\0') && (fmt[pos] != '%'); pos++);

		if (fmt[pos] == '\0')
			return count;

		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (isTerminal(fmt[pos])) {
			count++;
		}

		if (fmt[pos] == '\0')
			return count;

		pos++;   // skips over '%', terminal, or non-terminal
	}
}


/*
 * Count the minimum number of arguments required by valid specifiers in the format string
 */
template<SpecInfo... SIs>
constexpr inline int
countArgsRequired() {
	int count = 0;
	for (auto& SI : { SIs... }) {
		if (isTerminal(SI.terminal_)) count++;
		if (SI.width_ == DYNAMIC_WIDTH) count++;
		if (SI.prec_ == DYNAMIC_PRECISION) count++;
	}

	return count;
}


/*
 * return the size of the format string without fault specififers or format
 * escape sequence (e.g., "%-+ #0ltz%", "%-+ K", "%%")
 */
template<int N>
constexpr inline int
squeezeSoundSize(const char(&fmt)[N]) {
	int pos = 0;
	int specStartPos = 0;
	int size = N;

	for (;;) {
		// consume the current non-format string until reaching the next '%' 
		for (; (fmt[pos] != '\0') && (fmt[pos] != '%'); pos++);

		if (fmt[pos] == '\0')
			return size;

		specStartPos = pos; // record the position of '%' as it appears
		pos++;   // fmt[pos] == '%', so pos++ skips over '%'

		while (isFlag(fmt[pos]) || isDigit(fmt[pos]) || isLength(fmt[pos])
			|| fmt[pos] == '.' || fmt[pos] == '*') {
			pos++;
		}

		if (!isTerminal(fmt[pos])) {
			// pos - specStartPos is the size of this invlaid specifier
			size -= pos - specStartPos;
		}

		if (fmt[pos] == '\0')
			return size;

		pos++;   // skips over '%', terminal, or non-terminal
	}
}


/**
 * No-Op function that triggers the preprocessor's format checker for
 * printf format strings and argument parameters.
 *
 * \param format
 *      printf format string
 * \param ...
 *      format parameters
 */
//static void
//CFMT_PRINTF_FORMAT_ATTR(1, 2)
//checkFormat(CFMT_PRINTF_FORMAT const char*, ...) {}

//template<size_t N, size_t L>
//constexpr inline const char*
//getRTFmtStr(const char(&fmt)[N], const std::array<char, L>& fmtArr) {
//	if constexpr (0 == L) {
//		// return { fmt, /*static_cast<size_t>(N)*/ N };
//
//		return fmt;
//	}
//	else 
//		// return { fmtArr.data(), static_cast<int>(L) };
//		return /*fmtArr.data()*/const_cast<const char*>(&fmtArr[0]);
//}

// ============================================================================
// ============================================================================
//template <class R, class T>
//inline constexpr std::tuple<R, bool> formattedInteger([[maybe_unused]] T&& n) {
//	if constexpr (std::is_integral_v<std::remove_reference_t<T>>)
//		return { static_cast<R>(n), true };
//	else
//		return { static_cast<R>(0), false };
//}


template <class T>
inline constexpr int formattedPrec([[maybe_unused]] T&& n) {
	if constexpr (std::is_integral_v<std::remove_reference_t<T>>)
		return static_cast<int>(n);
	else
		return static_cast<int>(-1);
}


template <class T>
inline constexpr int formattedWidth([[maybe_unused]] T&& n) {
	if constexpr (std::is_integral_v<std::remove_reference_t<T>>)
		return static_cast<int>(n);
	else
		return static_cast<int>(0);
}

//#define	INTMAX_SIZE	(__FLAG_INTMAXT|__FLAG_SIZET|__FLAG_PTRDIFFT|__FLAG_LLONGINT)

/* template converter_impl declaration */
template<const char* const* fmt, const SpecInfo&... SIs, typename... Ts>
inline void converter_impl(OutbufArg& outbuf, Ts&&...args);

template<const char* const* fmt, const SpecInfo& SI, typename T>
inline void converter_single(OutbufArg& outbuf, T&& arg, width_t W = 0, 
    precision_t P = -1) {

	char buf[100];	// space for %c, %[diouxX], %[eEfgG]
	const char* cp = nullptr;
	int dprec = 0;	// a copy of prec if [diouxX], 0 otherwise
	int	fpprec = 0;	    // `extra' floating precision in [eEfgG]
	int	realsz = 0;	    // field size expanded by dprec
	size_t size = 0;
	int fieldsz = 0;	// field size expanded by sign, etc
	char ox[2] = { 0, 0 };	// space for 0x; ox[1] is either x, X, or \0
	std::to_chars_result ret;

	flags_t flags = SI.flags_;
	sign_t sign = SI.sign_;

	if constexpr (SI.width_ != DYNAMIC_WIDTH 
		&& SI.prec_ == DYNAMIC_PRECISION) {
		W = SI.width_;
		P = P < 0 ? -1 : P;
	}
	else if constexpr (SI.width_ == DYNAMIC_WIDTH 
		&& SI.prec_ != DYNAMIC_PRECISION) {
		if (W < 0) {
			W = -W;
			flags |= __FLAG_LADJUST;
		}
		P = SI.prec_;
	}
	else if constexpr (SI.width_ != DYNAMIC_WIDTH
		&& SI.prec_ != DYNAMIC_PRECISION) {
		W = SI.width_;
		P = SI.prec_;
	}
	else {
		P = P < 0 ? -1 : P;
		if (W < 0) {
			W = -W;
			flags |= __FLAG_LADJUST;
		}
	}

    if constexpr (SI.terminal_ == 'i' || SI.terminal_ == 'd' 
	    || SI.terminal_ == 'x' || SI.terminal_ == 'X' 
		|| SI.terminal_ == 'o' || SI.terminal_ == 'u') {

		if constexpr (std::is_integral_v<std::remove_reference_t<T>>) {
			if constexpr (SI.terminal_ == 'i' || SI.terminal_ == 'd') {
				intmax_t jval = 0;

				if constexpr ((SI.flags_ & __FLAG_LONGINT) != 0) {
					jval = static_cast<long int>(arg);
				}
				else if constexpr ((SI.flags_ & __FLAG_SHORTINT) != 0) {
					jval = static_cast<short int>(arg);
				}
				else if constexpr ((SI.flags_ & __FLAG_CHARINT) != 0) {
					jval = static_cast<signed char>(arg);
				}
				else if constexpr ((SI.flags_ & __FLAG_LLONGINT) != 0) {
					jval = static_cast<long long int>(arg);
				}
				else if constexpr ((SI.flags_ & __FLAG_SIZET) != 0) {
					jval = static_cast<std::make_signed<size_t>::type>(arg);
				}
				else if constexpr ((SI.flags_ & __FLAG_PTRDIFFT) != 0) {
					jval = static_cast<ptrdiff_t>(arg);
				}
				else if constexpr ((SI.flags_ & __FLAG_INTMAXT) != 0) {
					jval = static_cast<intmax_t>(arg);
				}
				else {
					jval = static_cast<int>(arg);
				}

				if (jval < 0) {
					jval = -jval;
					sign = '-';
				}

				std::tie(cp, size) = formatDec(buf, jval);
			}

			if constexpr (SI.terminal_ == 'x' || SI.terminal_ == 'X'
				|| SI.terminal_ == 'o' || SI.terminal_ == 'u') {

			}

			/*-
			 * ``... diouXx conversions ... if a precision is
			 * specified (P >= 0), the 0 flag will be ignored.''
			 *	-- ANSI X3J11 */
			if ((dprec = P) >= 0)
				flags &= ~__FLAG_ZEROPAD;
		}
		else {
			cp = buf;
			size = sizeof("(ER)") - 1;
			memcpy((void*)cp, "(ER)", size + 1);
		}
	}


	if constexpr (SI.end_ - SI.begin_ > 0) {
		outbuf.write(*fmt + SI.begin_, static_cast<size_t>(SI.end_ - SI.begin_));
	}

	if constexpr (SI.terminal_ != 'n') {
		// All reasonable formats wind up here. At this point, `cp' points to a
		// string which (if not flags&__FLAG_LADJUST) should be padded out to
		// `width' places. If flags&__FLAG_ZEROPAD, it should first be prefixed
		// by any sign or other prefix; otherwise, it should be blank padded
		// before the prefix is emitted. After any left-hand padding and 
		// prefixing, emit zeroes required by a decimal [diouxX] precision, 
		// then print the string properly, then emit zeroes required by any
		// leftover floating  precision; finally, if flags&_FLAG_LADJUST, pad
		// with blanks.

		// compute actual size, so we know how much to pad.
		// fieldsz excludes decimal prec; realsz includes it

		fieldsz = static_cast<int>(size + fpprec); // normally fpprec is 0
		realsz = (dprec > fieldsz) ? dprec : fieldsz;
		if (sign)
			realsz++;
		if (ox[1])
			realsz += 2;

		///////////////////////////// Right Adjust ////////////////////////////
		/* right-adjusting blank padding */
		if /*constexpr*/ ((/*SI.flags_*/flags & (/*__FLAG_ZEROPAD |*/ __FLAG_LADJUST)) == 0) {
			if ((flags & __FLAG_ZEROPAD) == 0)
				//PAD(W - realsz, BLANKS/*, outbuf*/);
				outbuf.writePaddings<' '>(W - realsz);
		}
		///////////////////////////// Right Adjust ////////////////////////////

		/* prefix */
		if (sign)
			outbuf.write(sign);
		if (ox[1]) {	/* ox[1] is either x, X, or \0 */
			ox[0] = '0';
			outbuf.write(ox, 2);
		}

		///////////////////////////// Right Adjust ////////////////////////////
		/* right-adjusting zero padding */
		if /*constexpr*/ ((/*SI.flags_*/ flags & (/*__FLAG_ZEROPAD |*/__FLAG_LADJUST)) == /*__FLAG_ZEROPAD*/0) {
			if ((flags & __FLAG_ZEROPAD) == __FLAG_ZEROPAD)
				//PAD(W - realsz, ZEROS/*, outbuf*/);
				outbuf.writePaddings<'0'>(W - realsz);
		}
		///////////////////////////// Right Adjust ////////////////////////////

		// [diouXx] leading zeroes from decimal precision
		// when dprec > fieldsz, realsz == dprec and zero padding size is dprec - fieldsz;
		// when dprec =< fieldsz, realsz == fieldsz and zero padding is skipped.
		//PAD(dprec - fieldsz, ZEROS/*, outbuf*/);
		outbuf.writePaddings<'0'>(dprec - fieldsz/*, '0'*/);

		// the string or number proper
		outbuf.write(cp, size);

		// trailing floating point zeroes
		//PAD(fpprec, ZEROS/*, outbuf*/);
		outbuf.writePaddings<'0'>(fpprec/*, '0'*/);

		////////////////////////////// Left Adjust ////////////////////////////
		/* left-adjusting padding (always blank) */
		if /*constexpr*/ ((/*SI.flags_*/ flags & __FLAG_LADJUST) == __FLAG_LADJUST)
			//PAD(W - realsz, BLANKS/*, outbuf*/);
		    outbuf.writePaddings<' '>(W - realsz);
		////////////////////////////// Left Adjust ////////////////////////////
	}
}


template<const char* const* fmt, const SpecInfo& SI, const SpecInfo&... SIs, typename T, typename... Ts>
inline void converter_args(OutbufArg& outbuf, T&& arg, Ts&&... rest) {
	converter_single<fmt, SI>(outbuf, std::forward<T>(arg));
	converter_impl<fmt, SIs...>(outbuf, std::forward<Ts>(rest)...);
}


template<const char* const* fmt, const SpecInfo& SI, const SpecInfo&... SIs, typename D, typename T, typename... Ts>
inline void converter_D_args(OutbufArg& outbuf, D&& d, T&& arg, Ts&&... rest) {
	if constexpr (SI.width_ == DYNAMIC_WIDTH) {
		// test 
		//std::cout << "\nwidth: " << d << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, std::forward<T>(arg),
			formattedWidth(std::forward<D>(d)), -1);
	}
	else if constexpr (SI.prec_ == DYNAMIC_PRECISION) {
		// test 
		//std::cout << "\nprec: " << d << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, std::forward<T>(arg), 0,
			formattedPrec(std::forward<D>(d)));
	}
	else { /* should never happen */
		abort();
	}
	converter_impl<fmt, SIs...>(outbuf, std::forward<Ts>(rest)...);
}

template<const char* const* fmt, const SpecInfo& SI, const SpecInfo&... SIs, typename D1, typename D2, typename T, typename... Ts>
inline void converter_D_D_args(OutbufArg& outbuf, D1&& d1, D2&& d2, T&& arg, Ts&&... rest) {
	if constexpr (SI.wFirst_) {
		// test 
		//std::cout << "\nwidth: " << d1 << std::endl;
		//std::cout << "prec: " << d2 << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, std::forward<T>(arg),
			formattedWidth(std::forward<D1>(d1)),
			formattedPrec(std::forward<D2>(d2)));
	}
	else {
		// test 
		//std::cout << "\nwidth: " << d2 << std::endl;
		//std::cout << "prec: " << d1 << std::endl;
		//std::cout << "arg: " << arg << std::endl;
		//std::cout << std::endl;

		converter_single<fmt, SI>(outbuf, std::forward<T>(arg),
			formattedWidth(std::forward<D2>(d2)),
			formattedPrec(std::forward<D1>(d1)));
	}

	converter_impl<fmt, SIs...>(outbuf, /*fmt,*/ std::forward<Ts>(rest)...);
}

template<const char* const* fmt, const SpecInfo&... SIs, typename... Ts>
inline void converter_impl(OutbufArg& outbuf, Ts&&...args) {
	// According to CFMT_STR implementation, at least one argument exists in the
	// template parameter pack SpecInfo... SIs.
	constexpr auto& SI = std::get<0>(std::forward_as_tuple(SIs...));

	if constexpr (sizeof ...(SIs) > 1) {
		if constexpr (SI.width_ == DYNAMIC_WIDTH
			&& SI.prec_ == DYNAMIC_PRECISION) {
			converter_D_D_args<fmt, SIs...>(outbuf,  std::forward<Ts>(args)...);
		}
		else if constexpr (SI.width_ == DYNAMIC_WIDTH
			|| SI.prec_ == DYNAMIC_PRECISION) {
			converter_D_args<fmt, SIs...>(outbuf, std::forward<Ts>(args)...);
		}
		else {
			converter_args<fmt, SIs...>(outbuf, std::forward<Ts>(args)...);
		}
	}
	else {
		// Fetch tail SpecInfo to terminate the formatting process.
		outbuf.write(*fmt + SI.begin_, static_cast<size_t>(SI.end_ - SI.begin_));
	}
}

template</*const char* const* fmt, */SpecInfo... SIs>
struct Converter {
	constexpr Converter() {}

	template <const char* const* fmt, typename... Ts>
	void /*operator()*/convert(OutbufArg& outbuf, Ts&&... args) const {
		constexpr auto numArgsReuqired = countArgsRequired<SIs...>();
		if constexpr (static_cast<uint32_t>(numArgsReuqired) > 
			static_cast<uint32_t>(sizeof...(Ts))) {
			std::cerr << "CFMT: forced abort due to illegal number of variadic arguments"
				" passed to CFMT_STR for converting\n"
				"(Required: " << numArgsReuqired << " ---- " <<
				"Passed: " << (sizeof...(Ts)) << ")";
			abort();
		}
		else {
			converter_impl<fmt, SIs...>(outbuf, std::forward<Ts>(args)...);
		}
	}

};

//template<const auto tuple_like, template<auto...> typename Template>
//constexpr decltype(auto) unpack() {
//	constexpr auto size = std::tuple_size_v<decltype(tuple_like)>;
//	return []<std::size_t... Is>(std::index_sequence<Is...>) {
//		return Template<std::get<Is>(tuple_like)...>{};
//	} (std::make_index_sequence<size>{});
//}



template</*auto fmtStr, */int N, template<auto...> typename Template, auto fmt/*, size_t M*/>
constexpr decltype(auto) unpack(/*const char(&fmt)[M]*/) {
	//constexpr auto SIs = analyzeFormatString<N>(*fmt);
	//return[&]<std::size_t... Is>(std::index_sequence<Is...>) {
	//	return Template<std::get<Is>(SIs)...>{};
	//} (std::make_index_sequence<N>{});

	return[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		//static constexpr SpecInfo SI = getOneSepc(fmt, Is);
		return Template </*fmtStr, */getOneSepc(*fmt, Is)... > {};
	} (std::make_index_sequence<N>{});
}

// ============================================================================
// ============================================================================

/**
 * CFMT_STR macro used for caching format specifier infos at compile time.
 *
 * \param result
 *      The variable to store the number of characters that would have been
 *      written to buffer if count had been sufficiently large, not counting
 *      the terminating null character.
 *      Notice that only when this returned value is non-negative and less
 *      than count, the string has been completely written.
 * \param buffer
 *      buffer to write to formatted string
 * \param count
 *      max number of characters to store in buffer
 * \param format
 *      printf-like format string (must be literal)
 * \param ... (additional arguments)
 *      Depending on the format string, the function may expect a sequence of
 *      additional arguments, each containing a value to be used to replace a
 *      format specifier in the format string (or a pointer to a storage
 *      location, for n). There should be at least as many of these arguments
 *      as the number of values specified in the valid format specifiers.
 */
#define CFMT_STR(result, buffer, count, format, ...) do { \
constexpr int kNVSIs = countValidSpecInfos(format); \
constexpr int kSS = squeezeSoundSize(format); \
static constexpr auto fmtRawStr = format; \
/**
 * A static variable inside a scope or function is something different than
 * a global static variable. Since there can be as many scoped statics with
 * the same name as you like (provided they are all in different scopes),
 * the compiler might have to change their names internally (incorporating
 * the function's name or the line number or whatever), so that the linker
 * can tell them apart.
 * Adding the static keyword to the file scope version of variables doesn't
 * change their extent, but it does change its visibility with respect to
 * other translation units; the name is not exported to the linker, so it
 * cannot be accessed by name from another translation unit. */ \
/*static constexpr std::array<SpecInfo, kNVSIs + 1> kSIs*/ \
	/*= analyzeFormatString<kNVSIs + 1>(format);*/ \
static constexpr auto kfmtArr = preprocessInvalidSpecs<kSS>(format); \
static constexpr auto kRTStr /*= getRTFmtStr(format, kfmtArr);*/ \
   = [&](){ return (kSS < sizeof(format)? kfmtArr.data() : format); }(); \
/** 
 * use the address of the pointer to a string literal (&kRTStr) with static
 * storage duration and internal linkage instead of a raw string literal to
 * comply with c++ standard 14.3.2/1 */ \
static /*constexpr*/ auto kConverter = unpack<kNVSIs + 1, Converter, &fmtRawStr>(); \
/*static constexpr auto kConverter = unpack<kSIs, Converter>();*/ \
OutbufArg outbuf(buffer, count); \
kConverter.convert<&kRTStr>(outbuf, ##__VA_ARGS__); \
result = outbuf.getWrittenNum(); \
outbuf.done(); /* null - terminate the string */ \
} while (0);


/**
 * A template-argument for a non-type, non-template template-parameter shall
 * be one of:
 * 1) for a non-type template-parameter of integral or enumeration type, a
 *    converted constant expression(5.19) of the type of the 
 *    template-parameter; or
 * 2) the name of a non-type template-parameter; or
 * 3) a constant expression(5.19) that designates the address of an object
 *    with static storage duration and external or internal linkage or a 
 *    function with external or internal linkage, including function 
 *    templates and function template-ids but excluding non-static class
 *    members, expressed(ignoring parentheses) as & id-expression, except
 *    that the & may be omitted if the name refers to a function or an array
 *    and shall be omitted if the corresponding template-parameter is a 
 *    reference; or
 * 4) a constant expression that evaluates to a null pointer value(4.10); or
 * 5) a constant expression that evaluates to a null member pointer value
 *    (4.11); or
 * 6) a pointer to member expressed as described in 5.3.1; or
 * 7) an address constant expression of type std::nullptr_t. */

#endif