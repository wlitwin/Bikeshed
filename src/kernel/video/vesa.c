/*
** File:	vesa.c
**
** Author:	Sean Congden
**
** Description:	VESA support routines
*/

#define	__KERNEL__20113__

#include "types.h"
#include "lib/klib.h"
#include "c_io.h"
#include "bios.h"

#include "vesa.h"


//#define TRACE_VESA
#ifdef TRACE_VESA
#	define TRACE(...) c_printf(__VA_ARGS__)
#else
#	define TRACE(...) ;
#endif


/*
** PRIVATE FUNCTIONS
*/

static inline Int32 abs(Int32 value) {
	if (value < 0)
		value = -value;

	return value;
}


/*
** PUBLIC FUNCTIONS
*/


void _vesa_load_info(VesaControllerInfo *info) {
	Registers regs = {di: (Uint16)((Uint32)info), ax: GET_CONTROLLER_INFO};
	_bios_call(0x10, &regs);
	info->video_modes = SEGMENTED_TO_LINEAR(info->video_modes);

	/* Make sure the call succeeded */
	if (regs.ax != 0x4F) {
		_kpanic("_vesa_load_info",
			"Unable to load VESA controller info.  AX = %x",
			regs.ax);
	}
}


Uint16 _vesa_choose_mode(Uint16 *modes, int width, int height) {
	VesaModeInfo *mode = (VesaModeInfo *)VESA_MODE_ADDRESS;
	Uint16 best_mode = 0;
	Uint32 best_diff, diff = 0;
	Uint32 best_bpp = 0;

	TRACE("Searching for a decent video mode...\n");
	
	for ( int i = 0; modes[i] != 0xFFFF; ++i ) {
		// Load information about the mode
		_vesa_load_mode_info(modes[i], mode);

		// Make sure the mode has the required attributes
		if ((mode->attributes & REQUIRED_ATTRIBUTES) != 
			REQUIRED_ATTRIBUTES) continue;

		// Make sure the mode uses packed pixels or direct color
		if ((mode->memory_model != PACKED_PIXELS) &&
			(mode->memory_model != DIRECT_COLOR)) continue;

		// Make sure we support the color depth
		if (!SUPPORTED_DEPTH(mode->bits_per_pixel)) continue;

		// Only consider modes with a width <= to the desired width
		if (mode->x_resolution > width) continue;

#ifdef TRACE_VESA
		_vesa_print_mode_info_basic(modes[i], mode);

		if (i % 3 == 0)
			TRACE("\n");
#endif

		// Compare this mode to the previous best mode
		diff = abs((mode->x_resolution * mode->y_resolution)
			 - (width * height));

		if ((best_diff > diff) || ((best_diff == diff)
			&& (mode->bits_per_pixel > best_bpp)))
		{
			best_mode = modes[i];
			best_diff = diff;
		}
	}

	if (best_mode == 0)
		TRACE("No suitable video mode found.\n");

	return best_mode;
}


void _vesa_load_mode_info(Uint16 mode, VesaModeInfo *info) {
	Registers regs = {di: PTR_16(info), ax: GET_MODE_INFO, cx: mode};
	_bios_call(0x10, &regs);

	/* Make sure the call succeeded */
	if (regs.ax != 0x4F) {
		_kpanic("_vesa_load_mode_info",
			"Unable to load VESA mode info.  AX = %x",
			regs.ax);
	}
}


void _vesa_select_mode(Uint16 mode) {
	Registers regs = {ax: SET_MODE, bx: mode};
	_bios_call(0x10, &regs);

	/* Make sure the call succeeded */
	if (regs.ax != 0x4F) {
		_kpanic("_vesa_select_mode",
			"Unable to set VESA mode.  AX = %x",
			regs.ax);
	}
}


void _vesa_print_info(VesaControllerInfo *info) {
	c_printf("VESA Controller Information:\n");
	c_printf("    Header:  %s\n", &(info->signature));
	c_printf("    Version: %x\n", info->version);
	c_printf("    Video Memory: %d 64KB blocks\n", info->total_memory);

	c_puts("    Modes: ");
	Uint16* modes = (Uint16*)(info->video_modes);
	int count = 0;
	for( int i = 0; modes[i] != 0xFFFF; ++i ) {
		if (i % 10 == 0) TRACE("\n        ");
		c_printf("%d, ", modes[i]);

		count++;
	}
	c_printf("\n      %d total modes\n", count);
}


void _vesa_print_mode_info_basic(int mode_num, VesaModeInfo *info) {
	c_printf("%d: %dx%dx%d  ", mode_num, (Uint32)(info->x_resolution),
		(Uint32)(info->y_resolution), (Uint32)(info->bits_per_pixel));
}


void _vesa_print_mode_info(int mode_num, VesaModeInfo *mode) {
	c_printf("Mode Info (%d):\n", mode_num);
	c_printf("    Attributes: %d\n", (Uint32)(mode->attributes));
	c_printf("    Windows: %d  and %d\n", (Uint32)(mode->window_a),
		(Uint32)(mode->window_b));
	c_printf("        Granularity: %d   Size: %d   Segments:  %d and %d\n",
		(Uint32)(mode->window_granularity), (Uint32)(mode->window_size),
		(Uint32)(mode->window_a_segment), (Uint32)(mode->window_b_segment));
	c_printf("    Size: %dx%dx%d  Pitch: %d  Planes: %d\n",
		(Uint32)(mode->x_resolution), (Uint32)(mode->y_resolution),
		(Uint32)(mode->bits_per_pixel), (Uint32)(mode->pitch),
		(Uint32)(mode->num_planes));
	c_printf("    X Char size: %d  Y Char size: %d\n",
		(Uint32)(mode->x_char_size), (Uint32)(mode->y_char_size));
	c_printf("    Banks: %d  Memory Model: %d\n", (Uint32)(mode->num_planes),
		(Uint32)(mode->memory_model));
}
