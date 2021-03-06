// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

        Pecom driver by Miodrag Milanovic

        08/11/2008 Preliminary driver.

****************************************************************************/

#include "emu.h"
#include "includes/pecom.h"

#include "sound/wave.h"
#include "speaker.h"


WRITE8_MEMBER(pecom_state::pecom_cdp1869_w)
{
	uint16_t ma = m_cdp1802->get_memory_address();

	switch (offset + 3)
	{
	case 3:
		m_cdp1869->out3_w(space, ma, data);
		break;

	case 4:
		m_cdp1869->out4_w(space, ma, data);
		break;

	case 5:
		m_cdp1869->out5_w(space, ma, data);
		break;

	case 6:
		m_cdp1869->out6_w(space, ma, data);
		break;

	case 7:
		m_cdp1869->out7_w(space, ma, data);
		break;
	}
}

void pecom_state::cdp1869_page_ram(address_map &map)
{
	map(0x000, 0x3ff).mirror(0x400).ram();
}

CDP1869_CHAR_RAM_READ_MEMBER(pecom_state::pecom_char_ram_r )
{
	uint8_t column = pmd & 0x7f;
	uint16_t charaddr = (column << 4) | cma;

	return m_charram[charaddr];
}

CDP1869_CHAR_RAM_WRITE_MEMBER(pecom_state::pecom_char_ram_w )
{
	uint8_t column = pmd & 0x7f;
	uint16_t charaddr = (column << 4) | cma;

	m_charram[charaddr] = data;
}

CDP1869_PCB_READ_MEMBER(pecom_state::pecom_pcb_r )
{
	return BIT(pmd, 7);
}

WRITE_LINE_MEMBER(pecom_state::pecom_prd_w)
{
	// every other PRD triggers a DMAOUT request
	if (m_dma)
	{
		m_cdp1802->set_input_line(COSMAC_INPUT_LINE_DMAOUT, HOLD_LINE);
	}

	m_dma = !m_dma;
}

void pecom_state::video_start()
{
	/* allocate memory */
	m_charram = std::make_unique<uint8_t[]>(PECOM_CHAR_RAM_SIZE);

	/* register for state saving */
	save_item(NAME(m_reset));
	save_item(NAME(m_dma));
	save_pointer(NAME(m_charram), PECOM_CHAR_RAM_SIZE);
}

MACHINE_CONFIG_START(pecom_state::pecom_video)
	MCFG_CDP1869_SCREEN_PAL_ADD(CDP1869_TAG, SCREEN_TAG, cdp1869_device::DOT_CLK_PAL)

	SPEAKER(config, "mono").front_center();

	MCFG_CDP1869_ADD(CDP1869_TAG, cdp1869_device::DOT_CLK_PAL, cdp1869_page_ram)
	MCFG_CDP1869_COLOR_CLOCK(cdp1869_device::COLOR_CLK_PAL)
	MCFG_CDP1869_CHAR_PCB_READ_OWNER(pecom_state, pecom_pcb_r)
	MCFG_CDP1869_CHAR_RAM_READ_OWNER(pecom_state, pecom_char_ram_r)
	MCFG_CDP1869_CHAR_RAM_WRITE_OWNER(pecom_state, pecom_char_ram_w)
	MCFG_CDP1869_PAL_NTSC_CALLBACK(CONSTANT(1))
	MCFG_CDP1869_PRD_CALLBACK(WRITELINE(*this, pecom_state, pecom_prd_w))
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	WAVE(config, "wave", "cassette").add_route(ALL_OUTPUTS, "mono", 0.25);
MACHINE_CONFIG_END
