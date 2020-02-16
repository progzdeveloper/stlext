// Copyright (c) 2010-2016, Michael Polukarov (Russia).
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer listed
//   in this license in the documentation and/or other materials
//   provided with the distribution.
//
// - Neither the name of the copyright holders nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include "../platform/common.h"

#if 0
/*! e */
#define E            2.7182818284590452353602874713526624977572470937000
/*! log[2](e) */
#define LOG2E        1.4426950408889634073599246810018921374266459541530
/*! log[10](e) */
#define LOG10E       0.43429448190325182765112891891660508229439700580366
/*! log[e](2) */
#define LN2          0.69314718055994530941723212145817656807550013436026
/*! log[e](10) */
#define LN10         2.3025850929940456840179914546843642076011014886288
/* log[e](pi) */
#define LN_PI        1.1447298858494001741434273513530587116472948129153

/*! 1/e   */
#define INV_E        0.36787944117144232159552377016146086744581113103176
/*! sqrt(e) */
#define SQRT_E       1.6487212707001281468486507878141635716537761007101

#define SQRT_3		 1.7320508075688772935274463415059
/*! sqrt(2) */
#define SQRT_2       1.4142135623730950488016887242096980785696718753769
/*! sqrt(1/2)  1/sqrt(2) = sqrt(2)/2 */
#define SQRT_1_2     0.70710678118654752440084436210484903928483593768845
/*! sqrt(3)/2 */
#define HALF_SQRT_3  0.86602540378443864676372317075293618347140262690520

/*! pi */
#define PI           3.1415926535897932384626433832795028841971693993751
/*! pi/2 */
#define PI_2         1.5707963267948966192313216916397514420985846996876
/*! pi/4 */
#define PI_4         0.78539816339744830961566084581987572104929234984378
/*! sqrt(pi)*/
#define SQRT_PI      1.7724538509055160272981674833411451827975494561224
/*! sqrt(2pi)*/
#define SQRT_2PI     2.5066282746310005024157652848110452530069867406099
/*! 1/pi */
#define INV_PI           0.31830988618379067153776752674502872406891929148091
/*! 2/pi */
#define TWO_INV_PI       0.63661977236758134307553505349005744813783858296182
/*! 1/sqrt(pi) */
#define INV_SQRT_PI      0.56418958354775628694807945156077258584405062932899
/*! 1/sqrt(2pi) */
#define INV_SQRT_2PI     0.39894228040143267793994605993438186847585863116492
/*! 2/sqrt(pi) */
#define TWO_INV_SQRT_PI  1.1283791670955125738961589031215451716881012586580

/*! (pi)/180 */
#define DEGREE           0.017453292519943295769236907684886127134428718885417
/*! (pi)/200 */
#define GRAD             0.015707963267948966192313216916397514420985846996876

/*! Catalan constant: Sum(k=0 -> inf){ (-1)^k/(2*k + 1)2 } */
#define CATALAN            0.9159655941772190150546035149323841107741493742816721342664981196217630197762547694794
/*! The Euler-Mascheroni constant: lim(n -> inf){ Sum(k=1 -> n) { 1/k - log(n) } }*/
#define EULER_GAMMA        0.5772156649015328606065120900824024310421593359399235988057672348849d;
/*! (1+sqrt(5))/2 */
#define GOLDEN_RATIO       1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072
/*! Glaisher Constant e^(1/12 - Zeta(-1)) */
#define GLAISHER           1.2824271291006226368753425688697917277676889273250011920637400217404063088588264611297
/*! Khinchin constant: prod(k=1 -> inf){1+1/(k*(k+2))^log(k,2)} */
#define KHINCHIN           2.6854520010653064453097148354817956938203822939944629530511523455572188595371520028011

/*! Chaos Theory constants */
#define FEIGENBAUM_ALPHA   2.502907875
#define FEIGENBAUM_DELTA   4.669201609

/*! Analizys constant */
#define BERNSTEIN         0.280169499
#endif

_STDX_BEGIN

// A class for math constants.
template<typename _RealType>
struct math_constants
{
	static_assert(std::is_floating_point<_RealType>::value,
				  "template argument not a floating point type");

	// Constant Euler's number @f$ e @f$.
	static constexpr _RealType e                = 2.7182818284590452353602874713526624977572470937000L;
	// Constant @f$ 1 / e @f$.
	static constexpr _RealType inv_e            = 0.36787944117144232159552377016146086744581113103176L;
	// Constant @f$ \sqrt(e) @f$ 
	static constexpr _RealType root_e           = 1.6487212707001281468486507878141635716537761007101L;
	// Constant @f$ \log_2(e) @f$.
	static constexpr _RealType log2_e           = 1.4426950408889634073599246810018921374266459541530L;
	// Constant @f$ \log_10(e) @f$.
	static constexpr _RealType log10_e          = 0.43429448190325182765112891891660508229439700580366L;
	// Constant @f$ \ln(2) @f$.
	static constexpr _RealType ln_2             = 0.69314718055994530941723212145817656807550013436026L;
	// Constant @f$ \ln(3) @f$.
	static constexpr _RealType ln_3             = 1.0986122886681096913952452369225257L;
	// Constant @f$ \ln(10) @f$.
	static constexpr _RealType ln_10            = 2.3025850929940456840179914546843642076011014886288L;
	// Constant @f$ \ln(pi) @f$.
	static constexpr _RealType ln_pi            = 1.1447298858494001741434273513530587116472948129153L;


	// Constant @f$ \pi @f$.
	static constexpr _RealType pi               = 3.1415926535897932384626433832795028841971693993751L;
	// Constant @f$ \pi / 2 @f$.
	static constexpr _RealType pi_half          = 1.5707963267948966192313216916397514420985846996876L;
	// Constant @f$ \pi / 3 @f$.
	static constexpr _RealType pi_third         = 1.0471975511965977461542144610931676L;
	// Constant @f$ \pi / 4 @f$.
	static constexpr _RealType pi_quarter       = 0.78539816339744830961566084581987572104929234984378L;
	// Constant @f$ 1 / \pi @f$.
	static constexpr _RealType inv_pi           = 0.31830988618379067153776752674502872406891929148091L;
	// Constant @f$ 2 / \pi @f$.
	static constexpr _RealType two_div_pi       = 0.63661977236758134307553505349005744813783858296182L;
	// Constant @f$ 2 / \sqrt(\pi) @f$.
	static constexpr _RealType two_div_root_pi  = 1.1283791670955125738961589031215451716881012586580L;

	// Constant @f$ \sqrt(\pi) @f$.
	static constexpr _RealType root_pi          = 1.7724538509055160272981674833411451827975494561224L;
	// Constant @f$ \sqrt(\pi / 2) @f$.
	static constexpr _RealType root_pi_div_2    = 1.2533141373155002512078826424055226L;
	// Constant @f$ \sqrt(2 * \pi) @f$
	static constexpr _RealType root_2pi         = 2.5066282746310005024157652848110452530069867406099L;

	// Constant @f$ 1/sqrt(pi) @f$ 
	static constexpr _RealType inv_root_pi      = 0.56418958354775628694807945156077258584405062932899L;
	// Constant @f$ 1/sqrt(2 * pi) @f$ 
	static constexpr _RealType inv_root_2pi     = 0.39894228040143267793994605993438186847585863116492L;

	/*! (pi)/180 */
	static constexpr _RealType degree           = 0.017453292519943295769236907684886127134428718885417L;
	/*! (pi)/200 */
	static constexpr _RealType grad             = 0.015707963267948966192313216916397514420985846996876L;


	// Constant @f$ \sqrt(2) @f$.
	static constexpr _RealType root_2           = 1.4142135623730950488016887242096981L;
	// Constant @f$ \sqrt(3) @f$.
	static constexpr _RealType root_3           = 1.7320508075688772935274463415058724L;
	// Constant @f$ \sqrt(5) @f$.
	static constexpr _RealType root_5           = 2.2360679774997896964091736687312762L;
	// Constant @f$ \sqrt(7) @f$.
	static constexpr _RealType root_7           = 2.6457513110645905905016157536392604L;
	// Constant @f$ 1 / \sqrt(2) @f$.   1/sqrt(2) = sqrt(2)/2
	static constexpr _RealType inv_root_2       = 0.70710678118654752440084436210484903928483593768845L;
	// Constant @f$ \sqrt(3) / 2 @f$ 
	static constexpr _RealType half_root_3      = 0.86602540378443864676372317075293618347140262690520L;


	// Constant Euler-Mascheroni @f$ \gamma_E @f$.
	// \def lim(n -> inf){ Sum(k=1 -> n) { 1/k - log(n) } }
	static constexpr _RealType gamma_e          = 0.5772156649015328606065120900824024310421593359399235988057672348849L;
	// Constant Golden Ratio @f$ \phi @f$.
	static constexpr _RealType phi              = 1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072L;

	// Catalan constant
	// \def Sum(k=0 -> inf){ (-1)^k/(2*k + 1)2 } 
	static constexpr _RealType catalan          = 0.9159655941772190150546035149323841107741493742816721342664981196217630197762547694794L;
	// Khinchin constant:
	// \def prod(k=1 -> inf){1+1/(k*(k+2))^log(k,2)} 
	static constexpr _RealType khinchin         = 2.6854520010653064453097148354817956938203822939944629530511523455572188595371520028011L;
	// Glaisher Constant 
	// \def e^(1/12 - Zeta(-1)) 
	static constexpr _RealType glaisher         = 1.2824271291006226368753425688697917277676889273250011920637400217404063088588264611297L;

	/*! Chaos Theory constants */
	static constexpr _RealType feigenbaum_alpha = 2.502907875;
	static constexpr _RealType feigenbaum_delta = 4.669201609;

	/*! Analizys constant */
	static constexpr _RealType bernstein        = 0.280169499;
};

// And the template definitions for the constants.
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::e;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::inv_e;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_e;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::log2_e;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::log10_e;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::ln_2;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::ln_3;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::ln_10;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::ln_pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::pi_half;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::pi_third;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::pi_quarter;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::inv_pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::two_div_pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::two_div_root_pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_pi_div_2;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_2pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::inv_root_pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::inv_root_2pi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::degree;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::grad;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_2;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_3;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_5;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::root_7;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::inv_root_2;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::half_root_3;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::gamma_e;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::phi;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::catalan;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::khinchin;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::glaisher;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::feigenbaum_alpha;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::feigenbaum_delta;
template<typename _RealType>
constexpr _RealType math_constants<_RealType>::bernstein;


_STDX_END

