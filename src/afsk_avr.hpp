/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef __AFSK_AVR_H__
#define __AFSK_AVR_H__


/*

328P Variant (radio pin -> mcu pin):
TXD - > PD3 (OC2B)
EN  - > PD4

32u4 Variant (radio pin -> mcu pin):
TXD - > PB6 (OC4B)
EN  - > PD4


*/

#if defined(AVR)
	#define __PROG_TYPES_COMPAT__ 1

	#include <stdint.h>
	#include <avr/pgmspace.h>
	#include "config.hpp"

	// Exported functions
	void afsk_setup();
	void afsk_send(const uint8_t *buffer, int len);
	void afsk_start();
	bool afsk_flush();
	void afsk_isr();
	void afsk_timer_setup();
	void afsk_timer_start();
	void afsk_timer_stop();
	#ifdef DEBUG_MODEM
		void afsk_debug();
	#endif

	// Exported consts
	extern const uint32_t MODEM_CLOCK_RATE;
	extern const uint8_t  REST_DUTY;
	extern const uint16_t TABLE_SIZE;
	extern const uint32_t PLAYBACK_RATE;

	// Exported vars
	extern const unsigned char afsk_sine_table[];

	#if defined(__AVR_ATmega328P__)
		// in gcc with Arduino 1.6 prog_uchar is deprecated. Allow it:

			#define AFSK_ISR ISR(TIMER2_OVF_vect)



			inline uint8_t afsk_read_sample(int phase)
			{
				return pgm_read_byte_near(afsk_sine_table + phase);
			}

			inline void afsk_output_sample(uint8_t s)
			{
				OCR2B = s;
			}

			inline void afsk_clear_interrupt_flag()
			{
				// atmegas don't need this as opposed to pic32s.
			}

			#ifdef DEBUG_MODEM
				inline uint16_t afsk_timer_counter()
				{
					uint16_t t = TCNT2;

					if ((TIFR2 & _BV(TOV2)) && t < 128)
					{ t += 256; }

					return t;
				}

				inline int afsk_isr_overrun()
				{
					return (TIFR2 & _BV(TOV2));
				}
			#endif


	#endif  // __AVR_ATmega328P__

	#if defined(__AVR_ATmega32U4__)

			#define AFSK_ISR ISR(TIMER4_OVF_vect)


			inline uint8_t afsk_read_sample(int phase)
			{
				return pgm_read_byte_near(afsk_sine_table + phase);
			}

			inline void afsk_output_sample(uint8_t s)
			{
				OCR4B = s;
			}

			inline void afsk_clear_interrupt_flag()
			{
				// atmegas don't need this as opposed to pic32s.
			}

			#ifdef DEBUG_MODEM
				inline uint16_t afsk_timer_counter()
				{
					uint16_t t = TCNT4;

					if ((TIFR4 & _BV(TOV4)) && t < 128)
					{ t += 256; }

					return t;
				}

				inline int afsk_isr_overrun()
				{
					return (TIFR4 & _BV(TOV4));
				}
			#endif


	#endif  // __AVR_ATmega32U4__
#endif

#endif // AVR

