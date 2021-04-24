/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mcr/extras/linux/p_extras.h"
#include "mcr/extras/extras.h"

#include <algorithm>
#include <functional>
#include <map>
#include <vector>

#define priv _private

namespace mcr
{
class LibmacroPlatformPrivate
{
	friend class LibmacroPlatform;
public:
	struct key_element_less : std::binary_function<mcr_MapElement, mcr_MapElement, bool>
	{
		bool operator()(const mcr_MapElement &lhs, const mcr_MapElement &rhs) const
		{
			return lhs.first.integer < rhs.first.integer;
		}
	};

	std::vector<input_event> echoKeys;
	std::vector<mcr_MapElement> keyEchoMaps[2];
	std::vector<std::string> grabs;
	std::vector<const char *> grabPaths;
};
static void localAddKeys(Libmacro *ctx);

LibmacroPlatform::LibmacroPlatform(Libmacro &context)
	: _context(&context), priv(new LibmacroPlatformPrivate)
{
}

LibmacroPlatform::LibmacroPlatform(const LibmacroPlatform &copytron)
	: _context(copytron._context), priv(new LibmacroPlatformPrivate)
{
	*priv = *copytron.priv;
}

LibmacroPlatform::~LibmacroPlatform()
{
	delete priv;
}

LibmacroPlatform &LibmacroPlatform::operator=(const LibmacroPlatform &copytron)
{
	if (&copytron != this) {
		*priv = *copytron.priv;
		reset();
	}
	return *this;
}

size_t LibmacroPlatform::echo(const mcr_Key &val) const
{
	mcr_MapElement *element;
	if (!valid(val.apply) || priv->keyEchoMaps[val.apply].empty())
		return (size_t)~0;
	auto &map = priv->keyEchoMaps[val.apply];
	element = (mcr_MapElement *)bsearch(&val.key, &map.front(), map.size(), sizeof(mcr_MapElement), mcr_int_compare);
	return element ? element->second.index : MCR_HIDECHO_ANY;
}

mcr_Key LibmacroPlatform::echoKey(size_t echo) const
{
	mcr_Key ret = { 0, MCR_SET };
	if (echo >= priv->echoKeys.size())
		return ret;
	auto found = priv->echoKeys.begin() + echo;
	ret.key = found->code;
	ret.apply = found->value ? MCR_SET : MCR_UNSET;
	return ret;
}

size_t LibmacroPlatform::setEcho(const mcr_Key &val, bool updateFlag)
{
	input_event insert;
	mcr_MapElement insertKey;
	size_t current = echo(val);
	if (current != MCR_HIDECHO_ANY)
		return current;
	mcr_throwif(!valid(val.apply), EINVAL);

	current = priv->echoKeys.size();
	ZERO(insert);
	insert.code = val.key;
	insert.value = val.apply == MCR_SET ? 1 : 0;
	priv->echoKeys.insert(priv->echoKeys.begin() + current, insert);

	insertKey.first.integer = val.key;
	insertKey.second.index = current;
	auto &map = priv->keyEchoMaps[val.apply];
	auto found = std::lower_bound(map.begin(), map.end(), insertKey, LibmacroPlatformPrivate::key_element_less());
	if (found != map.end() && found->first.integer == val.key) {
		found->second.index = current;
	} else {
		map.insert(found, insertKey);
	}
	if (updateFlag)
		updateEchos();
	return current;
}

size_t LibmacroPlatform::echoCount() const
{
	return priv->echoKeys.size();
}

void LibmacroPlatform::removeEcho(size_t code)
{
	mcr_throwif(code == (size_t)~0, EINVAL);
	for (int i = 0; i < 2; i++) {
		auto &map = priv->keyEchoMaps[i];
		for (auto iter = map.rbegin(); iter != map.rend(); iter++) {
			if (iter->second.index == code)
				map.erase(iter.base());
		}
	}
	if (code < priv->echoKeys.size())
		priv->echoKeys.erase(priv->echoKeys.begin() + code);
	updateEchos();
}

void LibmacroPlatform::updateEchos()
{
	input_event *events = priv->echoKeys.empty() ? nullptr : &priv->echoKeys.front();
	mcr_standard_set_echo_events(&**_context, events, priv->echoKeys.size());
	auto &map = priv->keyEchoMaps[MCR_SET];
	mcr_intercept_set_key_echo_map(&**_context, map.empty() ? nullptr : &map.front(), map.size(), MCR_SET);
	map = priv->keyEchoMaps[MCR_UNSET];
	mcr_intercept_set_key_echo_map(&**_context, map.empty() ? nullptr : &map.front(), map.size(), MCR_UNSET);
}

size_t LibmacroPlatform::grabCount() const
{
	return priv->grabs.size();
}

void LibmacroPlatform::setGrabCount(size_t count, bool updateFlag)
{
	priv->grabs.resize(count);
	priv->grabPaths.resize(count);
	if (updateFlag)
		updateGrabs();
}

String LibmacroPlatform::grab(size_t index) const
{
	mcr_throwif(index >= priv->grabs.size(), EINVAL);
	return priv->grabs[index];
}

void LibmacroPlatform::setGrab(size_t index, const String &value, bool updateFlag)
{
	mcr_throwif(index >= priv->grabs.size(), EINVAL);
	priv->grabs[index] = *value;
	priv->grabPaths[index] = priv->grabs[index].c_str();
	if (updateFlag)
		updateGrabs();
}

void LibmacroPlatform::updateGrabs()
{
	if (priv->grabPaths.size()) {
		if (mcr_intercept_set_grabs(&**_context, &priv->grabPaths.front(), priv->grabPaths.size()))
			throwError(MCR_LINE, mcr_read_err());
	} else {
		if (mcr_intercept_set_grabs(&**_context, nullptr, 0))
			throwError(MCR_LINE, mcr_read_err());
	}
}

void LibmacroPlatform::clear()
{
	mcr_standard_set_echo_events(&**_context, nullptr, 0);
	mcr_intercept_set_key_echo_map(&**_context, nullptr, 0, MCR_SET);
	mcr_intercept_set_key_echo_map(&**_context, nullptr, 0, MCR_UNSET);
	priv->echoKeys.clear();
	priv->keyEchoMaps[0].clear();
	priv->keyEchoMaps[1].clear();
}

void LibmacroPlatform::initialize()
{
	size_t i;

	priv->echoKeys.resize(MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT);
	for (i = 0; i < MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT; i++) {
		priv->echoKeys[i] = mcr_standard_echo_event_defaults[i];
	}

	priv->keyEchoMaps[0].resize(MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT);
	priv->keyEchoMaps[1].resize(MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT);
	for (i = 0; i < MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT; i++) {
		/* Map insert ordered, already ordered by default. */
		priv->keyEchoMaps[0][i] = mcr_intercept_key_echo_defaults[0][i];
		priv->keyEchoMaps[1][i] = mcr_intercept_key_echo_defaults[1][i];
	}
	reset();
}

void LibmacroPlatform::reset()
{
	updateEchos();
	updateGrabs();
}

void Libmacro::initialize()
{
	localAddKeys(this);
}

void Libmacro::deinitialize()
{

}

static inline void add(Libmacro *ctx, int character, bool shiftFlag,
					   const int keyVals[])
{
	if (character <= 0x7E)
		ctx->setCharacterKey(character, keyVals[character], 20, shiftFlag);
}

static inline void arange(Libmacro *ctx, int charMin, int charMax,
						  bool shiftFlag, const int keyVals[])
{
	for (int i = charMin; i <= charMax; i++) {
		add(ctx, i, shiftFlag, keyVals);
	}
}

static void localAddKeys(Libmacro *ctx)
{
	const int keyVals[] = {
		0,		/* Null character */
		0,		/*01    SOH             ^ A             Start of Header */
		0,		/*02    STX             ^ B             Start of Text */
		0,		/*03    ETX             ^ C             End of Text */
		0,		/*04    EOT             ^ D             End of Transmission */
		0,		/*05    ENQ             ^ E             Enquiry */
		0,		/*06    ACK             ^ F             Acknowledgment */
		0,		/*07    BEL             ^ G     \a      Bell */
		KEY_BACKSPACE,	/*08    BS              ^ H     \b      Backspace [ d ][ e ] */
		KEY_TAB,	/*09    HT              ^ I     \t      Horizontal Tab [ f ] */
		KEY_ENTER,	/*0A    LF              ^ J     \n      Line feed */
		0,		/*0B    VT              ^ K     \v      Vertical Tab */
		0,		/*0C    FF              ^ L     \f      Form feed */
		0,		/*0D    CR              ^ M     \r      Carriage return [ g ] */
		0,		/*0E    SO              ^ N             Shift Out */
		0,		/*0F    SI              ^ O             Shift In */
		0,		/*10    DLE             ^ P             Data Link Escape */
		0,		/*11    DC1             ^ Q             Device Control 1 ( oft. XON ) */
		0,		/*12    DC2             ^ R             Device Control 2 */
		0,		/*13    DC3             ^ S             Device Control 3 ( oft. XOFF ) */
		0,		/*14    DC4             ^ T             Device Control 4 */
		0,		/*15    NAK             ^ U             Negative Acknowledgement */
		0,		/*16    SYN             ^ V             Synchronous idle */
		0,		/*17    ETB             ^ W             End of Transmission Block */
		0,		/*18    CAN             ^ X             Cancel */
		0,		/*19    EM              ^ Y             End of Medium */
		0,		/*1A    SUB             ^ Z             Substitute */
		KEY_ESC,	/*1B    ESC             ^ [     \e [ h ]        Escape [ i ] */
		0,		/*1C    FS              ^\              File Separator */
		0,		/*1D    GS              ^ ]             Group Separator */
		0,		/*1E    RS              ^^ [ j ]                Record Separator */
		0,		/*1F    US              ^ _             Unit Separator */
		KEY_SPACE,	/*20    Space */
		KEY_1,		/*21    ! */
		KEY_APOSTROPHE,	/*22    " */
		KEY_3,		/*23    # */
		KEY_4,		/*24    $ */
		KEY_5,		/*25    % */
		KEY_7,		/*26    & */
		KEY_APOSTROPHE,	/*27    ' */
		KEY_9,		/*28    ( */
		KEY_0,		/*29    ) */
		KEY_8,		/*2A    * */
		KEY_EQUAL,	/*2B    + */
		KEY_COMMA,	/*2C    , */
		KEY_MINUS,	/*2D    - */
		KEY_DOT,	/*2E    . */
		KEY_SLASH,	/*2F    / */
		KEY_0,		/*30    0 */
		KEY_1,		/*31    1 */
		KEY_2,		/*32    2 */
		KEY_3,		/*33    3 */
		KEY_4,		/*34    4 */
		KEY_5,		/*35    5 */
		KEY_6,		/*36    6 */
		KEY_7,		/*37    7 */
		KEY_8,		/*38    8 */
		KEY_9,		/*39    9 */
		KEY_SEMICOLON,	/*3A    : */
		KEY_SEMICOLON,	/*3B    ; */
		KEY_COMMA,	/*3C    < */
		KEY_EQUAL,	/*3D    = */
		KEY_DOT,	/*3E    > */
		KEY_SLASH,	/*3F    ? */
		KEY_2,		/*40    @ */
		KEY_A,		/*41    A */
		KEY_B,		/*42    B */
		KEY_C,		/*43    C */
		KEY_D,		/*44    D */
		KEY_E,		/*45    E */
		KEY_F,		/*46    F */
		KEY_G,		/*47    G */
		KEY_H,		/*48    H */
		KEY_I,		/*49    I */
		KEY_J,		/*4A    J */
		KEY_K,		/*4B    K */
		KEY_L,		/*4C    L */
		KEY_M,		/*4D    M */
		KEY_N,		/*4E    N */
		KEY_O,		/*4F    O */
		KEY_P,		/*50    P */
		KEY_Q,		/*51    Q */
		KEY_R,		/*52    R */
		KEY_S,		/*53    S */
		KEY_T,		/*54    T */
		KEY_U,		/*55    U */
		KEY_V,		/*56    V */
		KEY_W,		/*57    W */
		KEY_X,		/*58    X */
		KEY_Y,		/*59    Y */
		KEY_Z,		/*5A    Z */
		KEY_LEFTBRACE,	/*5B    [ */
		KEY_BACKSLASH,	/*5C    '\' */
		KEY_RIGHTBRACE,	/*5D    ] */
		KEY_6,		/*5E    ^ */
		KEY_MINUS,	/*5F    _ */
		KEY_GRAVE,	/*60    ` */
		KEY_A,		/*61    a */
		KEY_B,		/*62    b */
		KEY_C,		/*63    c */
		KEY_D,		/*64    d */
		KEY_E,		/*65    e */
		KEY_F,		/*66    f */
		KEY_G,		/*67    g */
		KEY_H,		/*68    h */
		KEY_I,		/*69    i */
		KEY_J,		/*6A    j */
		KEY_K,		/*6B    k */
		KEY_L,		/*6C    l */
		KEY_M,		/*6D    m */
		KEY_N,		/*6E    n */
		KEY_O,		/*6F    o */
		KEY_P,		/*70    p */
		KEY_Q,		/*71    q */
		KEY_R,		/*72    r */
		KEY_S,		/*73    s */
		KEY_T,		/*74    t */
		KEY_U,		/*75    u */
		KEY_V,		/*76    v */
		KEY_W,		/*77    w */
		KEY_X,		/*78    x */
		KEY_Y,		/*79    y */
		KEY_Z,		/*7A    z */
		KEY_LEFTBRACE,	/*7B    { */
		KEY_BACKSLASH,	/*7C    | */
		KEY_RIGHTBRACE,	/*7D    } */
		KEY_MINUS	/*7E    ~ */
	};

	/* \b - \n */
	arange(ctx, 0x00, 0x20, false, keyVals);
	/* ! - & */
	arange(ctx, 0x21, 0x26, true, keyVals);
	/* ' */
	add(ctx, 0x27, false, keyVals);
	/* ( - + */
	arange(ctx, 0x28, 0x2B, true, keyVals);
	/*, - 9 */
	arange(ctx, 0x2C, 0x39, false, keyVals);
	/* : */
	add(ctx, 0x3A, true, keyVals);
	/*; */
	add(ctx, 0x3B, false, keyVals);
	/* < - Z */
	arange(ctx, 0x3C, 0x5A, true, keyVals);
	/* [ - ] */
	arange(ctx, 0x5B, 0x5D, false, keyVals);
	/* ^ */
	add(ctx, 0x5E, true, keyVals);
	/* _ */
	add(ctx, 0x5F, true, keyVals);
	/* ` - z */
	arange(ctx, 0x60, 0x7A, false, keyVals);
	/* { - ~ */
	arange(ctx, 0x7B, 0x7E, true, keyVals);
}
}
